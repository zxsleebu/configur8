#pragma once
#include "../interfaces.h"
#include "../types.h"

class IVEngineCvar {
public:
	template <typename... Values>
	constexpr void PrintColor(const CColor& color, const char* text, Values... params){
		return Memory::GetFunction<void(__cdecl*)(void*, const CColor&, const char*, ...)>(this, 25)(this, color, text, params...);
	}
};