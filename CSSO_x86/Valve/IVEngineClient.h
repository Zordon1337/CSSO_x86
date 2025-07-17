#pragma once
#include "../Utils/memory.h"
#include "CVector.h"
class IVEngineClient {
public:
    inline std::int32_t GetLocalPlayerIndex() noexcept
    {
        return mem::Call<std::int32_t>(this, 12);
    }
    inline void SetViewAngles(const CVector& viewAngles) noexcept
    {
        mem::Call<void, const CVector&>(this, 20, viewAngles);
    }
    void GetViewAngles(const CVector& viewAngles) noexcept
    {
        mem::Call<void, const CVector&>(this, 19, viewAngles);
    }
    bool IsInGame() noexcept
    {
        return mem::Call<bool>(this, 27);
    }
    void GetScreenSize(std::int32_t& width, std::int32_t& height) noexcept
    {
        mem::Call<void>(this, 5, std::ref(width), std::ref(height));
    }
    void ClientCmd(const char* cmd) {

        mem::Call<void>(this, 7, cmd);
    }
};