#pragma once

#include <stdint.h>


namespace vrwalkinplace {

	static const char* const vrsettings_SectionName = "driver_00vrwalkinplace";


	enum class DevicePropertyValueType : uint32_t {
		None = 0,
		FLOAT = 1,
		INT32 = 2,
		UINT64 = 3,
		BOOL = 4,
		STRING = 5,
		MATRIX34 = 20,
		MATRIX44 = 21,
		VECTOR3 = 22,
		VECTOR4 = 23
	};


	struct DeviceInfo {
		uint32_t deviceId;
		vr::ETrackedDeviceClass deviceClass;
		int deviceMode;
		bool stepDetectionEnabled;
	};

} // end namespace vrwalkinplace
