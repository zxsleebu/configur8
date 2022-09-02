#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ext/minhook/MinHook.h"
#include <stdio.h>
#include <string>
#include "types.h"
#include "misc.h"
#include "interfaces.h"
#include "interfaces/engine.h"
#include "interfaces/entitylist.h"
#include "interfaces/cvar.h"

class Onetap {
public:
	uintptr_t allocBase = 0;
	CColor* background;
	CColor* elementsOutline;
	CColor* mainAccent;
	CColor* groupShadows;
	CColor* checkboxBackground;
	CColor* checkboxText;
	CColor* checkboxOutline;
	CColor* checkboxActiveColor;

	CColor* accentColor1;
	CColor* accentColor2;

	static uintptr_t findAllocBase() {
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		MEMORY_BASIC_INFORMATION mbi;
		HANDLE proc = GetCurrentProcess();
		auto addr = (uintptr_t)si.lpMinimumApplicationAddress;
		while (addr < (uintptr_t)si.lpMaximumApplicationAddress) {
			VirtualQueryEx(proc, (void*)addr, &mbi, sizeof(mbi));
			if (mbi.AllocationProtect == PAGE_EXECUTE_READWRITE && mbi.Protect == PAGE_EXECUTE_READWRITE
				&& mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE && mbi.RegionSize == 0x010BB000) {
				return (uintptr_t)mbi.BaseAddress;
			}
			addr += mbi.RegionSize;
		}
		return 0;
	}
	Onetap() {
	}
	bool init() noexcept {
		Console::misc("Detecting otc3 address...");
		auto address = findAllocBase();
		if (address == 0) {
			Console::error("Failed to find address");
			return false;
		}
		Console::success("AllocBase: 0x%p", (void*)address);
		allocBase = address;

		background = (CColor*)(allocBase + 0x84F830);
		elementsOutline = (CColor*)((uintptr_t)background + 4);
		mainAccent = (CColor*)((uintptr_t)elementsOutline + 4);
		groupShadows = (CColor*)(allocBase + 0x84FAE8);

		checkboxBackground = (CColor*)(allocBase + 0x84F768);
		checkboxText = (CColor*)(allocBase + 0x84F770);
		checkboxOutline = (CColor*)((uintptr_t)checkboxText + 4);
		checkboxActiveColor = (CColor*)((uintptr_t)checkboxOutline + 4);

		accentColor1 = (CColor*)(allocBase + 0x37201);
		accentColor2 = (CColor*)(allocBase + 0x108A524);

		return true;
	}
};

Onetap g_Onetap = Onetap();

namespace OnetapHooks {
	namespace Addresses {
		const uintptr_t slideWalk = 0x85C90;
		const uintptr_t slideWalkOpcodes = 0x85D35;
		const uintptr_t slideWalkEnabled = 0xBC30F4;
		const uintptr_t animFix = 0x33180;
		const uintptr_t printColor = 0x143180;
	}
	namespace Originals {
		using SlideWalk_t = void(__fastcall*)(UserCmd* cmd);
		SlideWalk_t slideWalk = nullptr;
		using AnimFix_t = char(__fastcall*)(int);
		AnimFix_t animFix = nullptr;
		using vsprintf_s_t = int(__cdecl*)(unsigned long long, char*, size_t, const char*, _locale_t, va_list);
		vsprintf_s_t vsprintf_s_fn = nullptr;
		using PrintColor_t = void(__cdecl*)(void*, const CColor&, const char*, ...);
		PrintColor_t PrintColor = nullptr;
	}

	void __fastcall slideWalk(UserCmd* cmd) {
		auto netchan = engine->GetNetworkChannel();
		if (!netchan)
			return Originals::slideWalk(cmd);

		static BYTE slide[] = { 0x81, 0xE1, 0xE7, 0xF9, 0xFF, 0xFF };
		static BYTE walk[] = { 0x81, 0xC9, 0x18, 0x06, 0x00, 0x00 };

		auto patch = walk;
		if (netchan->chokedPackets == 0)
			patch = slide;
		memcpy((void*)(g_Onetap.allocBase + Addresses::slideWalkOpcodes), patch, 6);

		Originals::slideWalk(cmd);
	}
	char __fastcall animFix(int edi) {
		auto result = Originals::animFix(edi);
		auto lp = entityList->GetLocalPlayer();
		if (!lp) return result;
		if (!lp->IsAlive()) return result;

		auto animlayers = lp->GetAnimLayers();
		if (!animlayers)
			return result;

		auto lean = animlayers[LEAN];

		lean.m_flWeight = 1;
		lean.m_flCycle = 1;
		lean.m_flPlaybackRate = 1;

		auto jump = animlayers[MOVEMENT_JUMP_OR_FALL];
		jump.m_flCycle = 1;

		auto move = animlayers[MOVEMENT_MOVE];
		move.m_flWeight = 0;

		return result;
	}
	int __cdecl vsprintf_s_fn(unsigned long long _options, char* _buffer, size_t _buffer_count, const char* _format, _locale_t _locale, va_list va) {
		const char* f = _format;
		if (strcmp(_format, "otc3") == 0) {
			f = "configur8";
		}
		auto result = Originals::vsprintf_s_fn(_options, _buffer, _buffer_count, f, _locale, va);
		
		return result;
	}
	void __cdecl PrintColor(void* thisptr, const CColor& color, const char* format, va_list va) {
		if (color == CColor(12, 34, 56, 255)) {
			using namespace std;
			string str = format;
			auto first_delim = str.find(":");
			auto cmd = str.substr(0, first_delim);
			auto payload = str.substr(first_delim + 1);
			if (cmd == "c") {
				auto second_delim = payload.find(":");
				auto id = payload.substr(0, second_delim);
				auto col_s = payload.substr(second_delim + 1);

				auto col = CColor(0, 0, 0);
				col.r = (BYTE)stoi(col_s.substr(0, 2), nullptr, 16);
				col.g = (BYTE)stoi(col_s.substr(2, 2), nullptr, 16);
				col.b = (BYTE)stoi(col_s.substr(4, 2), nullptr, 16);
				col.a = (BYTE)stoi(col_s.substr(6, 2), nullptr, 16);
				
				switch (str2int(id.c_str()))
				{
					case str2int("Background"):
						*g_Onetap.background = col;
						break;
					case str2int("Elements Outline"):
						*g_Onetap.elementsOutline = col;
						break;
					case str2int("Accent 1"):
						*g_Onetap.mainAccent = col;
						break;
					case str2int("Accent 2"):
						*g_Onetap.accentColor1 = col;
						break;
					case str2int("Accent 3"):
						*g_Onetap.accentColor2 = col;
						break;
					case str2int("Scroll Shadows"):
						*g_Onetap.groupShadows = col;
						break;
					case str2int("Checkbox Background"):
						*g_Onetap.checkboxBackground = col;
						break;
					/*case str2int("Checkbox Text"):
						*g_Onetap.checkboxText = col;
						break;*/
					case str2int("Checkbox Outline"):
						*g_Onetap.checkboxOutline = col;
						break;
					case str2int("Checkbox Active Color"):
						*g_Onetap.checkboxActiveColor = col;
						break;
				default:
					break;
				}
			}
			return;
		}
		return Originals::PrintColor(thisptr, color, format, va);
	}
	void processHooks() {
		MH_Initialize();
		Memory::Hook(
			(LPVOID)(g_Onetap.allocBase + OnetapHooks::Addresses::slideWalk),
			&OnetapHooks::slideWalk,
			(LPVOID*)&OnetapHooks::Originals::slideWalk,
			"Slide Walk"
		);
		Memory::Hook(
			(LPVOID)(g_Onetap.allocBase + OnetapHooks::Addresses::animFix),
			&OnetapHooks::animFix,
			(LPVOID*)&OnetapHooks::Originals::animFix,
			"Local Player AnimFix"
		);
		if (auto ucrtbase = GetModuleHandle("ucrtbase.dll"); ucrtbase) {
			Memory::Hook(
				(LPVOID)(GetProcAddress(ucrtbase, "__stdio_common_vsprintf_s")),
				&OnetapHooks::vsprintf_s_fn,
				(LPVOID*)&OnetapHooks::Originals::vsprintf_s_fn,
				"__stdio_common_vsprintf_s"
			);
		}
		Memory::Hook(
			Memory::GetFunction<LPVOID>(cvar, 25),
			&OnetapHooks::PrintColor,
			(LPVOID*)&OnetapHooks::Originals::PrintColor,
			"Print Color"
		);

		MH_EnableHook(MH_ALL_HOOKS);

		Console::success("Successfuly hooked all functions!");

		cvar->PrintColor(CColor(0, 255, 0), "[+] configur8 initialized!\n");
	}
	void unhook() {
		cvar->PrintColor(CColor(255, 0, 0), "[-] configur8 unloaded!\n");
		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);

		MH_Uninitialize();
		Console::info("Unloaded!");
	}
}