#include <vrwalkinplace.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <config.h>


#if VRWALKINPLACE_EASYLOGGING == 1
#include "logging.h";
#define WRITELOG(level, txt) LOG(level) << txt;
#else
#define WRITELOG(level, txt) std::cerr << txt;
#endif



namespace vrwalkinplace {

	// Receives and dispatches ipc messages
	void VRWalkInPlace::_ipcThreadFunc(VRWalkInPlace * _this) {
		_this->_ipcThreadRunning = true;
		while (!_this->_ipcThreadStop) {
			try {
				ipc::Reply message;
				uint64_t recv_size;
				unsigned priority;
				boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(50);
				if (_this->_ipcClientQueue->timed_receive(&message, sizeof(ipc::Reply), recv_size, priority, timeout)) {
					if (recv_size == sizeof(ipc::Reply)) {
						std::lock_guard<std::recursive_mutex> lock(_this->_mutex);
						auto i = _this->_ipcPromiseMap.find(message.messageId);
						if (i != _this->_ipcPromiseMap.end()) {
							if (i->second.isValid) {
								i->second.promise.set_value(message);
							}
							else {
								_this->_ipcPromiseMap.erase(i); // nobody wants it, so we delete it
							}
						}
					}
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
			catch (std::exception& ex) {
				WRITELOG(ERROR, "Exception in ipc receive loop: " << ex.what() << std::endl);
			}
		}
		_this->_ipcThreadRunning = false;
	}


	VRWalkInPlace::VRWalkInPlace(const std::string& serverQueue, const std::string& clientQueue) : _ipcServerQueueName(serverQueue), _ipcClientQueueName(clientQueue) {}

	VRWalkInPlace::~VRWalkInPlace() {
		disconnect();
	}

	bool VRWalkInPlace::isConnected() const {
		return _ipcServerQueue != nullptr;
	}

	void VRWalkInPlace::connect() {
		if (!_ipcServerQueue) {
			// Open server-side message queue
			try {
				_ipcServerQueue = new boost::interprocess::message_queue(boost::interprocess::open_only, _ipcServerQueueName.c_str());
			}
			catch (std::exception& e) {
				_ipcServerQueue = nullptr;
				std::stringstream ss;
				ss << "Could not open server-side message queue: " << e.what() << " ";
				ss << _ipcServerQueueName.c_str();
				throw vrwalkinplace_connectionerror(ss.str());
			}
			// Append random number to client queue name (and hopefully no other client uses the same random number)
			_ipcClientQueueName += std::to_string(_ipcRandomDist(_ipcRandomDevice));
			// Open client-side message queue
			try {
				boost::interprocess::message_queue::remove(_ipcClientQueueName.c_str());
				_ipcClientQueue = new boost::interprocess::message_queue(
					boost::interprocess::create_only,
					_ipcClientQueueName.c_str(),
					100,					//max message number
					sizeof(ipc::Reply)    //max message size
				);
			}
			catch (std::exception& e) {
				delete _ipcServerQueue;
				_ipcServerQueue = nullptr;
				_ipcClientQueue = nullptr;
				std::stringstream ss;
				ss << "Could not open client-side message queue: " << e.what();
				throw vrwalkinplace_connectionerror(ss.str());
			}
			// Start ipc thread
			_ipcThreadStop = false;
			_ipcThread = std::thread(_ipcThreadFunc, this);
			// Send ClientConnect message to server
			ipc::Request message(ipc::RequestType::IPC_ClientConnect);
			auto messageId = _ipcRandomDist(_ipcRandomDevice);
			message.msg.ipc_ClientConnect.messageId = messageId;
			message.msg.ipc_ClientConnect.ipcProcotolVersion = IPC_PROTOCOL_VERSION;
			strncpy_s(message.msg.ipc_ClientConnect.queueName, _ipcClientQueueName.c_str(), 127);
			message.msg.ipc_ClientConnect.queueName[127] = '\0';
			std::promise<ipc::Reply> respPromise;
			auto respFuture = respPromise.get_future();
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
			}
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			// Wait for response
			auto resp = respFuture.get();
			m_clientId = resp.msg.ipc_ClientConnect.clientId;
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_ipcPromiseMap.erase(messageId);
			}
			if (resp.status != ipc::ReplyStatus::Ok) {
				delete _ipcServerQueue;
				_ipcServerQueue = nullptr;
				delete _ipcClientQueue;
				_ipcClientQueue = nullptr;
				std::stringstream ss;
				ss << "Connection rejected by server: ";
				if (resp.status == ipc::ReplyStatus::InvalidVersion) {
					ss << "Incompatible ipc protocol versions (server: " << resp.msg.ipc_ClientConnect.ipcProcotolVersion << ", client: " << IPC_PROTOCOL_VERSION << ")";
					throw vrwalkinplace_invalidversion(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_connectionerror(ss.str());
				}
			}
		}
	}

	void VRWalkInPlace::disconnect() {
		if (_ipcServerQueue) {
			// Send disconnect message (so the server can free resources)
			ipc::Request message(ipc::RequestType::IPC_ClientDisconnect);
			auto messageId = _ipcRandomDist(_ipcRandomDevice);
			message.msg.ipc_ClientDisconnect.clientId = m_clientId;
			message.msg.ipc_ClientDisconnect.messageId = messageId;
			std::promise<ipc::Reply> respPromise;
			auto respFuture = respPromise.get_future();
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
			}
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			auto resp = respFuture.get();
			m_clientId = resp.msg.ipc_ClientConnect.clientId;
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_ipcPromiseMap.erase(messageId);
			}
			// Stop ipc thread
			if (_ipcThreadRunning) {
				_ipcThreadStop = true;
				_ipcThread.join();
			}
			// delete message queues
			if (_ipcServerQueue) {
				delete _ipcServerQueue;
				_ipcServerQueue = nullptr;
			}
			if (_ipcClientQueue) {
				delete _ipcClientQueue;
				_ipcClientQueue = nullptr;
			}
		}
	}

	void VRWalkInPlace::ping(bool modal, bool enableReply) {
		if (_ipcServerQueue) {
			uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
			uint64_t nonce = _ipcRandomDist(_ipcRandomDevice);
			ipc::Request message(ipc::RequestType::IPC_Ping);
			message.msg.ipc_Ping.clientId = m_clientId;
			message.msg.ipc_Ping.messageId = messageId;
			message.msg.ipc_Ping.nonce = nonce;
			if (modal) {
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				if (resp.status != ipc::ReplyStatus::Ok) {
					std::stringstream ss;
					ss << "Error while pinging server: Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				if (enableReply) {
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					message.msg.ipc_Ping.messageId = messageId;
					_ipcPromiseMap.insert({ messageId, _ipcPromiseMapEntry() });
				}
				else {
					message.msg.ipc_Ping.messageId = 0;
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::enableStepDetection(bool enable, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.enableStepDetect = enable ? 1 : 2;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 1;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step detection: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				message.msg.dm_StepDetectionMode.messageId = 0;
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setGameStepType(int gameType, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.gameStepType = gameType;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 2;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				message.msg.dm_StepDetectionMode.messageId = 0;
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setControlSelect(int control, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.controlSelect = control;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 14;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setStepIntSec(float value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.stepIntSec = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 4;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step speed: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setHMDThreshold(const vr::HmdVector3d_t & value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.hmdThreshold = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 5;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setAccuracyButton(int buttonId, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.useAccuracyButton = buttonId;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 11;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setHMDPitchDown(int value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.hmdPitchDown = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 12;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setHMDPitchUp(int value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.hmdPitchUp = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 13;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setHandJogThreshold(float value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.handJogThreshold = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 6;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setHandRunThreshold(float value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.handRunThreshold = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 7;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setWalkTouch(float value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.walkTouch = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 8;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setJogTouch(float value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.jogTouch = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 9;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::setRunTouch(float value, bool modal) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetectionMode);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			message.msg.dm_StepDetectionMode.messageId = 0;
			message.msg.dm_StepDetectionMode.runTouch = value;
			message.msg.dm_StepDetectionMode.stepDetectOperation = 10;
			if (modal) {
				uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
				message.msg.dm_StepDetectionMode.messageId = messageId;
				std::promise<ipc::Reply> respPromise;
				auto respFuture = respPromise.get_future();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
				}
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
				auto resp = respFuture.get();
				{
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ipcPromiseMap.erase(messageId);
				}
				std::stringstream ss;
				ss << "Error while enabling step accleration: ";
				if (resp.status == ipc::ReplyStatus::InvalidId) {
					ss << "Invalid device id";
					throw vrwalkinplace_invalidid(ss.str());
				}
				else if (resp.status == ipc::ReplyStatus::NotFound) {
					ss << "Device not found";
					throw vrwalkinplace_notfound(ss.str());
				}
				else if (resp.status != ipc::ReplyStatus::Ok) {
					ss << "Error code " << (int)resp.status;
					throw vrwalkinplace_exception(ss.str());
				}
			}
			else {
				_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	bool VRWalkInPlace::getStepDetected() {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::WalkInPlace_StepDetect);
			memset(&message.msg, 0, sizeof(message.msg));
			message.msg.dm_StepDetectionMode.clientId = m_clientId;
			uint32_t messageId = _ipcRandomDist(_ipcRandomDevice);
			message.msg.dm_StepDetectionMode.messageId = messageId;
			std::promise<ipc::Reply> respPromise;
			auto respFuture = respPromise.get_future();
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_ipcPromiseMap.insert({ messageId, std::move(respPromise) });
			}
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
			auto resp = respFuture.get();
			{
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_ipcPromiseMap.erase(messageId);
			}
			std::stringstream ss;
			ss << "Error while getting device info: ";
			if (resp.status == ipc::ReplyStatus::Ok) {
				return (bool)resp.msg.dm_stepDetect.stepDetected;
			}
			else if (resp.status == ipc::ReplyStatus::InvalidId) {
				ss << "Invalid device id";
				throw vrwalkinplace_invalidid(ss.str());
			}
			else if (resp.status == ipc::ReplyStatus::NotFound) {
				ss << "Device not found";
				throw vrwalkinplace_invalidid(ss.str());
			}
			else if (resp.status != ipc::ReplyStatus::Ok) {
				ss << "Error code " << (int)resp.status;
				throw vrwalkinplace_invalidid(ss.str());
			}
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
		return false;
	}
} // end namespace vrwalkinplace
