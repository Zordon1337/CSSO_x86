#pragma once
#include "../Utils/memory.h"
#include "CEntity.h"
class IEntityList {
public:
    inline  CEntity* GetEntityFromIndex(const std::int32_t index) noexcept
    {
        return mem::Call<CEntity*>(this, 3, index);
    }
    inline CEntity* GetEntityFromHandle(const uint32_t handle) noexcept
    {
        return mem::Call<CEntity*>(this, 4, handle);
    }
    inline int GetHighestEntityIndex() noexcept {
        return mem::Call<int>(this, 6);
    }
};

