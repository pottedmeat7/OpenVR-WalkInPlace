// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <windows.h>

#include <cstdlib>
#include <iostream>

#define LOG(text) std::cerr << text << std::endl;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 ) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			LOG("VRWalkInPlace dll loaded...");
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

#endif
