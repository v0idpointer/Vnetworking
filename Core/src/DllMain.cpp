#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#include <Vnetworking/Platform.h>

#pragma comment (lib, "WS2_32.lib")

static bool Initialize(void);
static void Uninitialize(void);

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {

	switch (dwReason) {

	case DLL_PROCESS_ATTACH:
		if (!Initialize()) return FALSE;
		break;

	case DLL_PROCESS_DETACH:
		if (lpvReserved == nullptr) Uninitialize();
		break;

	}

	return TRUE;
}

static bool Initialize() { 

	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);

	return (result == 0);
}

static void Uninitialize() { 
	WSACleanup();
}