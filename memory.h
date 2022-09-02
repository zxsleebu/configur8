#pragma once
#include <cstdint>
#include <iostream>
#include "console.h"
namespace Memory
{
	template <typename Return, typename ... Arguments>
	constexpr Return Call(void* vmt, const std::uint32_t index, Arguments ... args) noexcept
	{
		using Function = Return(__thiscall*)(void*, decltype(args)...);
		return (*static_cast<Function**>(vmt))[index](vmt, args...);
	}
	void Hook(LPVOID target, LPVOID detour, LPVOID* original, const char* name) noexcept {
		MH_CreateHook(
			target,
			detour,
			original
		);
		
		Console::info("Hooking %s at 0x%p", name, target);
	}
	template <class T>
	static T GetFunction(void* instance, int index)
	{
		const auto vtable = *static_cast<void***>(instance);
		return reinterpret_cast<T>(vtable[index]);
	}
}