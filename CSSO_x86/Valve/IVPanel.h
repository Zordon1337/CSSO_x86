#pragma once
#include "../Utils/memory.h"

class IVPanel
{
public:
	const char* GetName(std::uint32_t panel) noexcept
	{
		return mem::Call<const char*>(this, 36, panel);
	}
};