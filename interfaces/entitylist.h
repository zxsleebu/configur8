#pragma once
#include "../types.h"
#include "engine.h"
#include "../interfaces.h"
class IClientEntityList
{
public:
	constexpr CEntity* GetEntityFromIndex(const std::int32_t index) noexcept
	{
		return Memory::Call<CEntity*>(this, 3, index);
	}
	CEntity* GetLocalPlayer() noexcept
	{
		return GetEntityFromIndex(engine->GetLocalPlayerIndex());
	}
};