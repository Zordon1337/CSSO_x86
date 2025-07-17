#pragma once
#include "CVector.h"
class CMatrix3x4
{
public:
    constexpr float* operator[](size_t index) noexcept
    {
        return data[index];
    }

    constexpr CVector Origin() noexcept
    {
        return { data[0][3], data[1][3], data[2][3] };
    }
    constexpr bool IsValidBone() {

        return data[0][0] != 0.0f || data[0][1] != 0.0f || data[0][2] != 0.0f || data[0][3] != 0.0f;
    }
    float data[3][4];
};