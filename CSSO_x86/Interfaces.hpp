#pragma once
#include "Valve/IVEngineClient.h"
#include "Valve/IEntityList.h"
#include "Valve/IGlobalVars.h"
#include "Valve/IEngineTrace.h"
#include "Valve/IEngineVGui.h"
#include "Valve/IVPanel.h"
#include "Valve/ISurface.h"
#include "Valve/IVDebugOverlay.h"
#include "Valve/IPrediction.h"
#include "Valve/ICvar.h"
#include <iostream>
namespace I {
    void* client;
    void* clientMode;
    IVEngineClient* engine = nullptr;
    IEntityList* entitylist = nullptr;
    IGlobalVars* globals = nullptr;
    IEngineTrace* engineTrace = nullptr;
    IVPanel* panel = nullptr;
    IEngineVGui* engineVgui = nullptr;
    ISurface* surface = nullptr;
    IVDebugOverlay* debugoverlay = nullptr;
    IPrediction* pred = nullptr;
    ICvar* cvar = nullptr;
    template <typename i>
    i* GetInterface(const char* module, const char* interfaceN)
    {
        const auto handle = GetModuleHandleA(module);

        if (!handle) return nullptr;

        using CreateInterfaceFn = i * (__cdecl*)(const char*, int*);
        const CreateInterfaceFn ci = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(handle, "CreateInterface"));

        return ci(interfaceN, nullptr);
    }

    void Init() {
        I::client = I::GetInterface<void*>("client.dll", "VClient017");
        I::clientMode = **reinterpret_cast<void***>((*reinterpret_cast<unsigned int**>(client))[10] + 5);
        I::engine = I::GetInterface<IVEngineClient>("engine.dll", "VEngineClient013");
        I::entitylist = I::GetInterface<IEntityList>("client.dll", "VClientEntityList003");
        I::engineTrace = I::GetInterface<IEngineTrace>("engine.dll", "EngineTraceClient003");
        I::globals = *(IGlobalVars**)(mem::PatternScan("engine.dll", "A1 ? ? ? ? 8B 11 68") + 8);
        I::panel = GetInterface<IVPanel>("vgui2.dll", "VGUI_Panel009");
        I::engineVgui = GetInterface<IEngineVGui>("engine.dll", "VEngineVGui001");
        I::surface = GetInterface<ISurface>("vguimatsurface.dll", "VGUI_Surface030");
        I::debugoverlay = GetInterface<IVDebugOverlay>("engine.dll", "VDebugOverlay004");
        I::pred = GetInterface<IPrediction>("client.dll", "VClientPrediction001");
        I::cvar = GetInterface<ICvar>("vstdlib.dll", "VEngineCvar004");
        std::cout << "I::client: " << I::client << std::endl;
        std::cout << "I::clientMode: " << I::clientMode << std::endl;
        std::cout << "I::engine: " << I::engine << std::endl;
        std::cout << "I::entitylist: " << I::entitylist << std::endl;
        std::cout << "I::globals: " << I::globals << std::endl;
        std::cout << "I::engineTrace: " << I::engineTrace << std::endl;
        std::cout << "I::panel: " << I::panel << std::endl;
        std::cout << "I::engineVgui: " << I::engineVgui << std::endl;
        std::cout << "I::debugoverlay: " << I::debugoverlay << std::endl;
        std::cout << "I::pred: " << I::pred << std::endl;
        std::cout << "I::cvar: " << I::cvar << std::endl;
    }
}