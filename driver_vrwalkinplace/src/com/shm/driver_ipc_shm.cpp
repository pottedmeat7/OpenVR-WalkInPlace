#include "driver_ipc_shm.h"
#include "../../stdafx.h"
#include "../../driver_vrwalkinplace.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <ipc_protocol.h>
#include <openvr_math.h>

namespace vrwalkinplace {
	namespace driver {


		void IpcShmCommunicator::init(CServerDriver* driver) {
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

		void IpcShmCommunicator::_ipcThreadFunc(IpcShmCommunicator* _this, CServerDriver * driver) {
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
											_this->_ipcEndpoints.insert({ clientId, queue });
											reply.msg.ipc_ClientConnect.clientId = clientId;
											reply.status = ipc::ReplyStatus::Ok;
											LOG(INFO) << "New client connected: endpoint \"" << message.msg.ipc_ClientConnect.queueName << "\", cliendId " << clientId;
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
										LOG(INFO) << "Client disconnected: clientId " << message.msg.ipc_ClientDisconnect.clientId;
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

								case ipc::RequestType::WalkInPlace_StepDetectionMode:
								{
									ipc::Reply resp(ipc::ReplyType::GenericReply);
									resp.messageId = message.msg.dm_StepDetectionMode.messageId;
									auto serverDriver = CServerDriver::getInstance();
									if (serverDriver) {
										switch (message.msg.dm_StepDetectionMode.stepDetectOperation) {
										case 1:
											if (message.msg.dm_StepDetectionMode.stepDetectOperation == 1) {
												if (message.msg.dm_StepDetectionMode.enableStepDetect == 1) {
													serverDriver->enableStepDetection(true);
												}
												else {
													serverDriver->enableStepDetection(false);
												}
											}
											break;
										case 2:
											serverDriver->setGameStepType(message.msg.dm_StepDetectionMode.gameStepType);
											break;
										case 3:
											break;
										case 4:
											serverDriver->setStepIntSec(message.msg.dm_StepDetectionMode.stepIntSec);
											break;
										case 5:
											serverDriver->setHMDThreshold(message.msg.dm_StepDetectionMode.hmdThreshold);
											break;
										case 6:
											serverDriver->setHandJogThreshold(message.msg.dm_StepDetectionMode.handJogThreshold);
											break;
										case 7:
											serverDriver->setHandRunThreshold(message.msg.dm_StepDetectionMode.handRunThreshold);
											break;
										case 8:
											serverDriver->setWalkTouch(message.msg.dm_StepDetectionMode.walkTouch);
											break;
										case 9:
											serverDriver->setJogTouch(message.msg.dm_StepDetectionMode.jogTouch);
											break;
										case 10:
											serverDriver->setRunTouch(message.msg.dm_StepDetectionMode.runTouch);
											break;
										case 11:
											serverDriver->setAccuracyButton(message.msg.dm_StepDetectionMode.useAccuracyButton);
											break;
										case 12:
											serverDriver->setHMDPitchDown(message.msg.dm_StepDetectionMode.hmdPitchDown);
											break;
										case 13:
											serverDriver->setHMDPitchUp(message.msg.dm_StepDetectionMode.hmdPitchUp);
											break;
										default:
											break;
										}
										resp.status = ipc::ReplyStatus::Ok;
									}
									else {
										resp.status = ipc::ReplyStatus::UnknownError;
									}
									if (resp.status != ipc::ReplyStatus::Ok) {
										LOG(ERROR) << "Error while updating step detect: Error code " << (int)resp.status;
									}
									if (resp.messageId != 0) {
										auto i = _this->_ipcEndpoints.find(message.msg.dm_StepDetectionMode.clientId);
										if (i != _this->_ipcEndpoints.end()) {
											i->second->send(&resp, sizeof(ipc::Reply), 0);
										}
										else {
											LOG(ERROR) << "Error while updating step detect: Unknown clientId " << message.msg.dm_StepDetectionMode.clientId;
										}
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


	} // end namespace driver
} // end namespace vrwalkinplace
