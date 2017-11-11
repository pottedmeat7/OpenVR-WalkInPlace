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


struct Request_VirtualDevices_GenericClientMessage {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
};


struct Request_VirtualDevices_GenericDeviceIdMessage {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
};


struct Request_VirtualDevices_AddDevice {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	VirtualDeviceType deviceType;
	char deviceSerial[256];
};


struct Request_VirtualDevices_SetDeviceProperty {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::ETrackedDeviceProperty deviceProperty;
	DevicePropertyValueType valueType;
	union {
		int32_t int32Value;
		uint64_t uint64Value;
		float floatValue; 
		bool boolValue; 
		char stringValue[256];
		vr::HmdMatrix34_t matrix34Value;
		vr::HmdMatrix44_t matrix44Value;
		vr::HmdVector3_t vector3Value;
		vr::HmdVector4_t vector4Value;
	} value;
};

struct Request_VirtualDevices_RemoveDeviceProperty {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::ETrackedDeviceProperty deviceProperty;
};

struct Request_VirtualDevices_SetDevicePose {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::DriverPose_t pose;
};

struct Request_VirtualDevices_SetControllerState {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::VRControllerState_t controllerState;
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
	float stepAcceleration;
	float stepSpeed;
	float stepIntSec;
	int gameStepType;
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
		Request_VirtualDevices_GenericClientMessage vd_GenericClientMessage;
		Request_VirtualDevices_GenericDeviceIdMessage vd_GenericDeviceIdMessage;
		Request_VirtualDevices_AddDevice vd_AddDevice;
		Request_VirtualDevices_SetDeviceProperty vd_SetDeviceProperty;
		Request_VirtualDevices_RemoveDeviceProperty vd_RemoveDeviceProperty;
		Request_VirtualDevices_SetDevicePose vd_SetDevicePose;
		Request_VirtualDevices_SetControllerState vd_SetControllerState;
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

struct Reply_VirtualDevices_GetDeviceCount {
	uint32_t deviceCount;
};

struct Reply_VirtualDevices_GetDeviceInfo {
	uint32_t virtualDeviceId;
	uint32_t openvrDeviceId;
	VirtualDeviceType deviceType;
	char deviceSerial[128];
};

struct Reply_VirtualDevices_GetDevicePose {
	vr::DriverPose_t pose;
};

struct Reply_VirtualDevices_GetControllerState {
	vr::VRControllerState_t controllerState;
};

struct Reply_VirtualDevices_AddDevice {
	uint32_t virtualDeviceId;
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
		Reply_VirtualDevices_GetDeviceCount vd_GetDeviceCount;
		Reply_VirtualDevices_GetDeviceInfo vd_GetDeviceInfo;
		Reply_VirtualDevices_GetDevicePose vd_GetDevicePose;
		Reply_VirtualDevices_GetControllerState vd_GetControllerState;
		Reply_VirtualDevices_AddDevice vd_AddDevice;
		Reply_WalkInPlace_GetDeviceInfo dm_deviceInfo;
	} msg;
};


} // end namespace ipc
} // end namespace vrwalkinplace
