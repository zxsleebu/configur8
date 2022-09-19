#pragma once
#include "../interfaces.h"
#include "../types.h"

class ConVar {
public:
    constexpr float GetFloat() {
        return Memory::Call<float>(this, 12);
    }
    constexpr int GetInt() {
        return Memory::Call<int>(this, 13);
    }
    constexpr void SetValue(const char* value) {
        return Memory::Call<void>(this, 14, value);
    }
    constexpr void SetFloat(float value) {
        return Memory::Call<void>(this, 15, value);
    }
    constexpr void SetInt(int value) {
        return Memory::Call<void>(this, 16, value);
    }
    /*VIRTUAL_METHOD(void, setValue, WIN32_LINUX(15, 18), (float value), (this, value))
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(16, 19), (int value), (this, value))*/
};

class IVEngineCvar {
public:
	template <typename... Values>
	constexpr void PrintColor(const CColor& color, const char* text, Values... params){
		return Memory::GetFunction<void(__cdecl*)(void*, const CColor&, const char*, ...)>(this, 25)(this, color, text, params...);
	}
	constexpr ConVar* FindVar(const char* name) {
		return Memory::Call<ConVar*>(this, 15, name);
	}
    ConVar* CheckAndGetVar(const char* name) {
        auto var = this->FindVar(name);
        if (var == nullptr) {
            static char buffer[25];
            sprintf(buffer, "setinfo %s 1", name);
            Console::info("Initialized CVar %s", name);
            engine->ExecuteCmd(buffer);
        }
        return var;
    };
};