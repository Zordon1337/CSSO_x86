#pragma once
#include "Valve/CVector.h"
class IVDebugOverlay {
public:
	bool ScreenPosition(const CVector& point, CVector& screen) noexcept
	{
		return mem::Call<bool, const CVector&, CVector&>(this, 11, point, screen);
	}
};