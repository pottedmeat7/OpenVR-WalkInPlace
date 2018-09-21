#pragma once

#include <stdint.h>


namespace vrwalkinplace {

	static const char* const vrsettings_SectionName = "driver_00vrwalkinplace";

	enum class ButtonEventType : uint32_t {
		None = 0,
		ButtonPressed = 1,
		ButtonUnpressed = 2,
		ButtonTouched = 3,
		ButtonUntouched = 4,
		TrackpadX = 5,
		TrackpadY = 6,
		JoystickX = 7,
		JoystickY = 8
	};

} // end namespace vrwalkinplace
