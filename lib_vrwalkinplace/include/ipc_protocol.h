#pragma once

#include "vrwalkinplace_types.h"
#include <utility>


#define IPC_PROTOCOL_VERSION 1

namespace vrwalkinplace {
namespace ipc {


enum class RequestType : uint32_t {
	None,

	// IPC connection handling
	IPC_ClientConnect,
	IPC_ClientDisconnect,
	IPC_Ping,

	WalkInPlace_GetDeviceInfo,
	WalkInPlace_DefaultMode,
	WalkInPlace_StepDetectionMode
};


enum class ReplyType : uint32_t {
	None,
	
	IPC_ClientConnect,
	IPC_Ping,

	GenericReply,

	WalkInPlace_GetDeviceInfo
};


enum class ReplyStatus : uint32_t {
	None,
	Ok,
	UnknownError,
	InvalidId,
	AlreadyInUse,
	InvalidType,
	NotFound,
	TooManyDevices,
	InvalidVersion,
	MissingProperty,
	InvalidOperation
};

struct Request_IPC_ClientConnect {
	uint32_t messageId;
	uint32_t ipcProcotolVersion;
	char queueName[128];
};


struct Request_IPC_ClientDisconnect {
	uint32_t clientId;
	uint32_t messageId;
};


struct Request_IPC_Ping {
	uint32_t clientId;
	uint32_t messageId;
	uint64_t nonce;
};


struct Request_OpenVR_VendorSpecificEvent {
	uint32_t deviceId;
	vr::EVREventType eventType;
	vr::VREvent_Data_t eventData;
	double timeOffset;
};

struct Request_WalkInPlace_StepDetectionMode {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t enableStepDetect;
	uint32_t useEulerForStep;
	uint32_t stepDetectOperation;
	vr::HmdVector3d_t hmdThreshold;
	float handJogThreshold;
	float handRunThreshold;
	float walkTouch;
	float jogTouch;
	float runTouch;
	float stepAcceleration;
	float stepSpeed;
	float stepIntSec;
	int gameStepType;
	int useAccuracyButton;
};


struct Request {
	Request() {}
	Request(RequestType type) : type(type) {
		timestamp = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	Request(RequestType type, uint64_t timestamp) : type(type), timestamp(timestamp) {}

	void refreshTimestamp() {
		timestamp = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	RequestType type = RequestType::None;
	int64_t timestamp = 0; // milliseconds since epoch
	union {
		Request_IPC_ClientConnect ipc_ClientConnect;
		Request_IPC_ClientDisconnect ipc_ClientDisconnect;
		Request_IPC_Ping ipc_Ping;
		Request_WalkInPlace_StepDetectionMode dm_StepDetectionMode;
	} msg;
};



struct Reply_IPC_ClientConnect {
	uint32_t clientId;
	uint32_t ipcProcotolVersion;
};

struct Reply_IPC_Ping {
	uint64_t nonce;
};

struct Reply_WalkInPlace_GetDeviceInfo {
	uint32_t deviceId;
	vr::ETrackedDeviceClass deviceClass;
	int deviceMode;
	bool stepDetectionEnabled;
};


struct Reply {
	Reply() {}
	Reply(ReplyType type) : type(type) {
		timestamp = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	Reply(ReplyType type, uint64_t timestamp) : type(type), timestamp(timestamp) {}

	ReplyType type = ReplyType::None;
	uint64_t timestamp = 0; // milliseconds since epoch
	uint32_t messageId;
	ReplyStatus status;
	union {
		Reply_IPC_ClientConnect ipc_ClientConnect;
		Reply_IPC_Ping ipc_Ping;
		Reply_WalkInPlace_GetDeviceInfo dm_deviceInfo;
	} msg;
};


} // end namespace ipc
} // end namespace vrwalkinplace
