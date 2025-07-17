#pragma once
#include <cstdint>
#include "CMatrix3x4.h"
#include "../Utils/memory.h"

class RecvTable;

using CreateClientClassFn = void* (*)();
using CreateEventFn = void* (*)();

class ClientClass {
public:
    CreateClientClassFn pCreateFn;
    CreateEventFn pCreateEventFn;
    const char* szNetworkName;
    void* pRecvTable; 
    ClientClass* pNext;
    int iClassID;
};

class CEntity {
public:
    inline int32_t getFlags() noexcept
    {
        return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + 0x430); // m_fFlags
    }
    bool SetupBones(CMatrix3x4* out, std::int32_t max, std::int32_t mask, float currentTime) noexcept
    {
        return mem::Call<bool>(this + 0x4, 16, out, max, mask, currentTime);
    }
    bool IsDormant() noexcept
    {
        return mem::Call<bool>(this + 0x8, 8);
    }
    inline int getHealth() noexcept {
        return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x9C);
    }
    bool isAlive() noexcept {
        return this->getHealth() > 1;
    }
    const CVector& GetAbsOrigin() noexcept
    {

        return mem::Call<const CVector&>(this, 9);
    }

    inline CVector getVecView() noexcept
    {
        return *reinterpret_cast<CVector*>(reinterpret_cast<uintptr_t>(this) + 0xF4);
    }
    CVector GetEyePosition() noexcept
    {
        CVector ViewOffset = this->getVecView();
        return this->GetAbsOrigin() + ViewOffset;
    }
    inline int getTeam() noexcept
    {
        return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x0A4); // m_iTeamNum
    }
    ClientClass* GetClientClass() noexcept
    {
        return mem::Call<ClientClass*>(this + 0x8, 2);
    }
    void SetAbsAngles(const CVector& angles) noexcept
    {
        using SetAbsAnglesFn = void(__thiscall*)(void*, const CVector& angles);
        static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)mem::PatternScan("client.dll", "55 8B EC 83 EC ? A1 ? ? ? ? 33 C5 89 45 ? 56 57 8B 7D ? 8B F1 E8");

        SetAbsAngles(this, angles);
    }

    inline float m_flC4Blow() noexcept
    {
        return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x258C); // m_flC4Blow
    }
    inline float m_flTimerLength() noexcept
    {
        return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x2590); // m_flTimerLength
    }
    inline bool m_bBombDefused() noexcept {
        return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x25A0); // m_bBombDefused
    }

    inline float m_flDefuseCountDown() noexcept {
        return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x259C); // m_flDefuseCountDown
    }
    inline float m_flDefuseLength() noexcept {
        return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x2598); // m_flDefuseLength
    }
    inline bool m_bBombTicking() noexcept {
        return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x2580); // m_bBombTicking
    }
};