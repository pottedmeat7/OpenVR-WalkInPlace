#include "driver_ipc_shm.h"
#include "../../driver/ServerDriver.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <ipc_protocol.h>
#include <openvr_math.h>

namespace vrwalkinplace {
	namespace driver {


		void IpcShmCommunicator::init(ServerDriver* driver) {
			_driver = driver;
			_ipcThreadStopFlag = false;
			_ipcThread = std::thread(_ipcThreadFunc, this, driver);
		}

		void IpcShmCommunicator::shutdown() {
			if (_ipcThreadRunning) {
				_ipcThreadStopFlag = true;
				_ipcThread.join();
			}
		}

		void IpcShmCommunicator::_ipcThreadFunc(IpcShmCommunicator* _this, ServerDriver * driver) {
			_this->_ipcThreadRunning = true;
			LOG(DEBUG) << "CServerDriver::_ipcThreadFunc: thread started";
			try {
				// Create message queue
				boost::interprocess::message_queue::remove(_this->_ipcQueueName.c_str());
				boost::interprocess::message_queue messageQueue(
					boost::interprocess::create_only,
					_this->_ipcQueueName.c_str(),
					100,					//max message number
					sizeof(ipc::Request)    //max message size
				);

				while (!_this->_ipcThreadStopFlag) {
					try {
						ipc::Request message;
						uint64_t recv_size;
						unsigned priority;
						boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(50);
						if (messageQueue.timed_receive(&message, sizeof(ipc::Request), recv_size, priority, timeout)) {
							LOG(TRACE) << "CServerDriver::_ipcThreadFunc: IPC request received ( type " << (int)message.type << ")";
							if (recv_size == sizeof(ipc::Request)) {
								switch (message.type) {

								case ipc::RequestType::IPC_ClientConnect:
								{
									try {
										auto queue = std::make_shared<boost::interprocess::message_queue>(boost::interprocess::open_only, message.msg.ipc_ClientConnect.queueName);
										ipc::Reply reply(ipc::ReplyType::IPC_ClientConnect);
										reply.messageId = message.msg.ipc_ClientConnect.messageId;
										reply.msg.ipc_ClientConnect.ipcProcotolVersion = IPC_PROTOCOL_VERSION;
										if (message.msg.ipc_ClientConnect.ipcProcotolVersion == IPC_PROTOCOL_VERSION) {
											auto clientId = _this->_ipcClientIdNext++;
											if (_this->_ipcClientIdNext > 100) {
												_this->_ipcClientIdNext = 1;
												clientId = 1;
												_this->_ipcEndpoints.clear();
											}
											//if (clientId == 7) {
											//	LOG(INFO) << "New client connected: endpoint \"" << message.msg.ipc_ClientConnect.queueName << "\", cliendId " << clientId;
											//}
											_this->_ipcEndpoints.insert({ clientId, queue });
											reply.msg.ipc_ClientConnect.clientId = clientId;
											reply.status = ipc::ReplyStatus::Ok;
											//LOG(INFO) << "New client connected: endpoint \"" << message.msg.ipc_ClientConnect.queueName << "\", cliendId " << clientId;
										}
										else {
											reply.msg.ipc_ClientConnect.clientId = 0;
											reply.status = ipc::ReplyStatus::InvalidVersion;
											LOG(INFO) << "Client (endpoint \"" << message.msg.ipc_ClientConnect.queueName << "\") reports incompatible ipc version "
												<< message.msg.ipc_ClientConnect.ipcProcotolVersion;
										}
										queue->send(&reply, sizeof(ipc::Reply), 0);
									}
									catch (std::exception& e) {
										LOG(ERROR) << "Error during client connect: " << e.what();
									}
								}
								break;

								case ipc::RequestType::IPC_ClientDisconnect:
								{
									ipc::Reply reply(ipc::ReplyType::GenericReply);
									reply.messageId = message.msg.ipc_ClientDisconnect.messageId;
									auto i = _this->_ipcEndpoints.find(message.msg.ipc_ClientDisconnect.clientId);
									if (i != _this->_ipcEndpoints.end()) {
										reply.status = ipc::ReplyStatus::Ok;
										auto msgQueue = i->second;
										_this->_ipcEndpoints.erase(i);
										//LOG(INFO) << "Client disconnected: clientId " << message.msg.ipc_ClientDisconnect.clientId;
										if (reply.messageId != 0) {
											msgQueue->send(&reply, sizeof(ipc::Reply), 0);
										}
									}
									else {
										LOG(ERROR) << "Error during client disconnect: unknown clientID " << message.msg.ipc_ClientDisconnect.clientId;
									}
								}
								break;

								case ipc::RequestType::IPC_Ping:
								{
									LOG(TRACE) << "Ping received: clientId " << message.msg.ipc_Ping.clientId << ", nonce " << message.msg.ipc_Ping.nonce;
									auto i = _this->_ipcEndpoints.find(message.msg.ipc_Ping.clientId);
									if (i != _this->_ipcEndpoints.end()) {
										ipc::Reply reply(ipc::ReplyType::IPC_Ping);
										reply.messageId = message.msg.ipc_Ping.messageId;
										reply.status = ipc::ReplyStatus::Ok;
										reply.msg.ipc_Ping.nonce = message.msg.ipc_Ping.nonce;
										if (reply.messageId != 0) {
											i->second->send(&reply, sizeof(ipc::Reply), 0);
										}
									}
									else {
										LOG(ERROR) << "Error during ping: unknown clientID " << message.msg.ipc_ClientDisconnect.clientId;
									}
								}
								break;

								case ipc::RequestType::OpenVR_DeviceAdded:
								{
									if (vr::VRServerDriverHost()) {
										driver->openvr_deviceAdded(message.msg.ipc_DeviceAdded.deviceId, true);
									}
									break;
								}

								case ipc::RequestType::OpenVR_PoseUpdate:
								{
									if (vr::VRServerDriverHost()) {
										driver->openvr_poseUpdate(message.msg.ipc_PoseUpdate.deviceId, message.msg.ipc_PoseUpdate.pose, message.timestamp);
									}
								}
								break;

								case ipc::RequestType::OpenVR_ButtonEvent:
								{
									try {
										if (vr::VRServerDriverHost()) {
											auto& e = message.msg.ipc_ButtonEvent;
											driver->openvr_buttonEvent(e.deviceId, e.eventType, e.buttonId, e.timeOffset);
										}
									}
									catch (std::exception& e) {
										LOG(ERROR) << "Error in button event ipc thread: " << e.what();
									}
								}
								break;

								case ipc::RequestType::OpenVR_AxisEvent:
								{
									try {
										if (vr::VRServerDriverHost()) {
											auto& e = message.msg.ipc_AxisEvent;
											driver->openvr_axisEvent(e.deviceId, e.axisId, e.axisState);
										}
									}
									catch (std::exception& e) {
										LOG(ERROR) << "Error in axis event ipc thread: " << e.what();
									}
								}
								break;

								default:
									LOG(ERROR) << "Error in ipc server receive loop: Unknown message type (" << (int)message.type << ")";
									break;
								}
							}
							else {
								LOG(ERROR) << "Error in ipc server receive loop: received size is wrong (" << recv_size << " != " << sizeof(ipc::Request) << ")";
							}
						}
					}
					catch (std::exception& ex) {
						LOG(ERROR) << "Exception caught in ipc server receive loop: " << ex.what();
					}
				}
				boost::interprocess::message_queue::remove(_this->_ipcQueueName.c_str());
			}
			catch (std::exception& ex) {
				LOG(ERROR) << "Exception caught in ipc server thread: " << ex.what();
			}
			_this->_ipcThreadRunning = false;
			LOG(DEBUG) << "CServerDriver::_ipcThreadFunc: thread stopped";
		}

		void IpcShmCommunicator::sendReply(uint32_t clientId, const ipc::Reply& reply) {
			std::lock_guard<std::mutex> guard(_sendMutex);
			auto i = _ipcEndpoints.find(clientId);
			if (i != _ipcEndpoints.end()) {
				i->second->send(&reply, sizeof(ipc::Reply), 0);
			}
			else {
				LOG(ERROR) << "Error while sending reply: Unknown clientId " << clientId;
			}
		}

	} // end namespace driver
} // end namespace vrwalkinplace