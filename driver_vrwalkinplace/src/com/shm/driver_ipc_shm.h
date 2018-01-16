#pragma once

#include <thread>
#include <string>
#include <map>
#include <mutex>
#include <memory>
#include <boost/interprocess/ipc/message_queue.hpp>


// driver namespace
namespace vrwalkinplace {

namespace ipc { struct Reply; }

namespace driver {

// forward declarations
class ServerDriver;


class IpcShmCommunicator {
public:
	void init(ServerDriver* driver);
	void shutdown();

private:
	static void _ipcThreadFunc(IpcShmCommunicator* _this, ServerDriver* driver);

	void sendReply(uint32_t clientId, const ipc::Reply& reply);

	std::mutex _sendMutex;
	ServerDriver* _driver = nullptr;
	std::thread _ipcThread;
	volatile bool _ipcThreadRunning = false;
	volatile bool _ipcThreadStopFlag = false;
	std::string _ipcQueueName = "driver_vrwalkinplace.server_queue";
	uint32_t _ipcClientIdNext = 1;
	std::map<uint32_t, std::shared_ptr<boost::interprocess::message_queue>> _ipcEndpoints;
};


} // end namespace driver
} // end namespace vrwalkinplace
