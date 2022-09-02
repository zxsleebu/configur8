#pragma once
#include "../interfaces.h"
class IVEngineClient {
public:
	constexpr NetworkChannel* GetNetworkChannel() noexcept  {
		return Memory::Call<NetworkChannel*>(this, 78);
	}
	constexpr std::int32_t GetLocalPlayerIndex() noexcept
	{
		return Memory::Call<std::int32_t>(this, 12);
	}
};