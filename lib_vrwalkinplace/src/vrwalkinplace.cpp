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
			strncpy(message.msg.ipc_ClientConnect.queueName, _ipcClientQueueName.c_str(), 127);
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

	void VRWalkInPlace::openvrHMDAdded(uint32_t deviceId) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_HMDAdded);
			message.msg.ipc_HMDAdded.deviceId = deviceId;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::openvrFollowDevice(uint32_t deviceId) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_FollowDevice);
			message.msg.ipc_FollowDevice.deviceId = deviceId;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::openvrUpdatePose(uint32_t deviceId, vr::DriverPose_t pose) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_PoseUpdate);
			message.msg.ipc_PoseUpdate.deviceId = deviceId;
			message.msg.ipc_PoseUpdate.pose = pose;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::openvrUpdateState(uint32_t deviceId, vr::VRControllerState_t new_state) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_StateUpdate);
			message.msg.ipc_StateUpdate.deviceId = deviceId;
			message.msg.ipc_StateUpdate.new_state = new_state;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::openvrButtonEvent(ButtonEventType eventType, uint32_t deviceId, vr::EVRButtonId buttonId, double timeOffset) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_ButtonEvent);
			message.msg.ipc_ButtonEvent.eventType = eventType;
			message.msg.ipc_ButtonEvent.deviceId = deviceId;
			message.msg.ipc_ButtonEvent.buttonId = buttonId;
			message.msg.ipc_ButtonEvent.timeOffset = timeOffset;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::openvrAxisEvent(uint32_t deviceId, uint32_t axisId, const vr::VRControllerAxis_t & axisState) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_AxisEvent);
			message.msg.ipc_AxisEvent.deviceId = deviceId;
			message.msg.ipc_AxisEvent.axisId = axisId;
			message.msg.ipc_AxisEvent.axisState = axisState;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

	void VRWalkInPlace::openvrEnableDriver(bool val) {
		if (_ipcServerQueue) {
			ipc::Request message(ipc::RequestType::OpenVR_EnableDriver);
			message.msg.ipc_EnableDriver.enable = val;
			_ipcServerQueue->send(&message, sizeof(ipc::Request), 0);
		}
		else {
			throw vrwalkinplace_connectionerror("No active connection.");
		}
	}

} // end namespace vrwalkinplace