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

			OpenVR_PoseUpdate,
			OpenVR_StateUpdate,
			OpenVR_ButtonEvent,
			OpenVR_AxisEvent,
			OpenVR_HMDAdded,
			OpenVR_FollowDevice,

			OpenVR_EnableDriver
		};


		enum class ReplyType : uint32_t {
			None,

			IPC_ClientConnect,
			IPC_Ping,

			GenericReply
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

		struct Request_EnableLeapMotion {
			bool enable;
		};

		struct Request_OpenVR_PoseUpdate {
			uint32_t deviceId;
			vr::DriverPose_t pose;
		};

		struct Request_OpenVR_StateUpdate {
			uint32_t deviceId;
			vr::VRControllerState_t new_state;
		};

#define REQUEST_OPENVR_BUTTONEVENT_MAXCOUNT 12

		struct Request_OpenVR_ButtonEvent {
			ButtonEventType eventType;
			uint32_t deviceId;
			vr::EVRButtonId buttonId;
			double timeOffset;
		};


#define REQUEST_OPENVR_AXISEVENT_MAXCOUNT 12

		struct Request_OpenVR_AxisEvent {
			uint32_t deviceId;
			uint32_t axisId;
			vr::VRControllerAxis_t axisState;
		};

		struct Request_OpenVR_HMDAdded {
			uint32_t deviceId;
		};

		struct Request_OpenVR_FollowDevice {
			uint32_t deviceId;
		};

		struct Request_OpenVR_EnableDriver {
			bool enable;
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
				Request_EnableLeapMotion ipc_LeapMotion;
				Request_OpenVR_PoseUpdate ipc_PoseUpdate;
				Request_OpenVR_StateUpdate ipc_StateUpdate;
				Request_OpenVR_ButtonEvent ipc_ButtonEvent;
				Request_OpenVR_AxisEvent ipc_AxisEvent;
				Request_OpenVR_HMDAdded ipc_HMDAdded;
				Request_OpenVR_FollowDevice ipc_FollowDevice;
				Request_OpenVR_EnableDriver ipc_EnableDriver;
			} msg;
		};



		struct Reply_IPC_ClientConnect {
			uint32_t clientId;
			uint32_t ipcProcotolVersion;
		};

		struct Reply_IPC_Ping {
			uint64_t nonce;
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
			} msg;
		};


	} // end namespace ipc
} // end namespace vrwalkinplace