#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "types.h"
#include "memory.h"
#include <cstdint>

#include "interfaces/engine.h"
IVEngineClient* engine = nullptr;

#include "interfaces/entitylist.h"
IClientEntityList* entityList = nullptr;

#include "interfaces/cvar.h"
IVEngineCvar* cvar = nullptr;

namespace Interfaces {
	template <typename T>
	T* GetInterface(const char* moduleName, const char* interfaceName) noexcept
	{
		const HINSTANCE handle = GetModuleHandle(moduleName);
		if (!handle) return nullptr;
		using Fn = T*(__cdecl*)(const char*, int*);
		const Fn createInterface =
			reinterpret_cast<Fn>(GetProcAddress(handle, "CreateInterface"));
		return createInterface(interfaceName, nullptr);
	}
	void init() {
		engine = GetInterface<IVEngineClient>("engine.dll", "VEngineClient014");
		entityList = GetInterface<IClientEntityList>("client.dll", "VClientEntityList003");
		cvar = GetInterface<IVEngineCvar>("vstdlib.dll", "VEngineCvar007");
	}
}

