#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "onetap.h"
#include "interfaces.h"
#include <stdio.h>
#include "console.h"

DWORD WINAPI init(LPVOID instance) {
	FreeConsole();
	AllocConsole();
	auto tmp = freopen("CONIN$", "r", stdin);
	tmp = freopen("CONOUT$", "w", stdout);

	Console::info("configur8 sleebu#0090");
	
	if (g_Onetap.init()) {
		Interfaces::init();
		OnetapHooks::processHooks();

		//Sleep(3000);
		//ShowWindow(GetConsoleWindow(), SW_HIDE);
		Sleep(3000);

		while (!GetAsyncKeyState(VK_END)) {
			Sleep(1000);
		}
		OnetapHooks::unhook();
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}
	
	Sleep(3000);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(instance);
		
		const HANDLE thread = CreateThread(
			nullptr,
			NULL,
			init,
			instance,
			NULL,
			nullptr
		);

		if (thread) 
			CloseHandle(instance);
    }
	return TRUE;
}

