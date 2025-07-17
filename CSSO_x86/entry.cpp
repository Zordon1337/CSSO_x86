#include "MinHook/MinHook.h"

#include <mutex>
#include <vector>
#include <optional>
#include <iostream>

#include "Valve/CEntity.h"
#include "Valve/CUserCmd.h"

#include "Interfaces.hpp"
#include "Valve/IEngineTrace.h"
#include "Valve/IVPanel.h"
#include "Menu.h"

#define DEG2RAD(x) ((x) * (3.14159265358979323846f / 180.f))

#include <cmath>
struct Entity {
    CEntity* player;
    float fov;
    int health;
    CMatrix3x4 bones[128];
    float trace;
    const char* type;
};
#include <random>

float RandomFloat(float min, float max) {
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}
CVector aaangle{};
inline std::vector<Entity> g_Entitylist;
inline std::mutex g_entityMutex;
enum client_frame_stage_t {
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END
};
class CViewSetup
{
public:
    void* vmt;

    uint32_t x; //0x0000 
    uint32_t m_nUnscaledX; //0x0004 
    uint32_t y; //0x0008 
    uint32_t m_nUnscaledY; //0x000C 
    uint32_t width; //0x0010 
    uint32_t m_nUnscaledWidth; //0x0014 
    uint32_t height; //0x0018 
    uint32_t m_eStereoEye; //0x001C 
    uint32_t m_nUnscaledHeight; //0x0020 
    bool m_bOrtho; //0x0024 
    float m_OrthoLeft; //0x0028 
    float m_OrthoTop; //0x002C 
    float m_OrthoRight; //0x0030 
    float m_OrthoBottom; //0x0034 
    float m_fov; //0x0038 
    float m_viewmodel_fov; //0x003C 
    CVector origin; //0x0040 
    CVector angles; //0x004C 
    float zNear; //0x0058 
    float zFar; //0x005C 
    float viewmodel_zNear; //0x0060 
    float viewmodel_zFar; //0x0064 
    bool m_bRenderToSubrectOfLargerScreen; //0x0068 
    float m_flAspectRatio; //0x006C 
    bool m_bOffCenter; //0x0070 
    float m_flOffCenterTop; //0x0074 
    float m_flOffCenterBottom; //0x0078 
    float m_flOffCenterLeft; //0x007C 
    float m_flOffCenterRight; //0x0080 
    bool m_bDoBloomAndToneMapping; //0x0084 
    bool m_bCacheFullSceneState; //0x0088 
    bool m_bViewToProjectionOverride; //0x008C 
    CMatrix3x4 m_ViewToProjection; //0x0090 
};
using framestagehook = void(__thiscall*)(void*, client_frame_stage_t);
inline framestagehook FrameStageoriginal;
void __stdcall FrameStage(client_frame_stage_t frame_stage);

void __stdcall FrameStage(client_frame_stage_t stage) {
    if (I::engine && I::engine->IsInGame() && I::entitylist) {
        CVector vec = aaangle;
        if (Config::antiAim) {

            uintptr_t base = (uintptr_t)GetModuleHandleA("engine.dll");
            uintptr_t* ptr = (uintptr_t*)(base + 0x744C58);
            *ptr = 1;
            I::engine->ClientCmd("thirdperson");
        }
        else {
            uintptr_t base = (uintptr_t)GetModuleHandleA("engine.dll");
            uintptr_t* ptr = (uintptr_t*)(base + 0x744C58);
            *ptr = 0;
            I::engine->ClientCmd("firstperson");
        }
        switch (stage) {

            case FRAME_RENDER_START:
            {
                I::pred->SetLocalViewAngles(vec);
                

                break;
            }
        }
    }
    FrameStageoriginal(I::client, stage);
}

using CreateMoveFn = bool(__thiscall*)(void*, float, void*) noexcept;
inline CreateMoveFn CreateMoveOriginal = nullptr;
bool __stdcall CreateMove(float frameTime, CUserCmd* cmd) noexcept;

using PaintTraverseFn = void(__thiscall*)(IVPanel*, std::uintptr_t, bool, bool) noexcept;
inline PaintTraverseFn PaintTraverseOriginal = nullptr;
void __stdcall PaintTraverse(std::uintptr_t vGuiPanel, bool forceRepaint, bool allowForce);

bool __stdcall CreateMove(float frameTime, CUserCmd* cmd) noexcept
{

    if (!cmd->command_number)
        return CreateMoveOriginal(I::clientMode, frameTime, cmd);

    CreateMoveOriginal(I::clientMode, frameTime, cmd);

    CVector oldAngles = cmd->viewangles;
        

    CEntity* pLocal = I::entitylist->GetEntityFromIndex(I::engine->GetLocalPlayerIndex());
    if (pLocal)
    {
        if (!Config::antiAim) {
            
        }
        std::vector<Entity> visibleEnemies;
        if (!(pLocal->getFlags() & (1 << 0)) && Config::bhop) {
            cmd->buttons &= ~(1 << 1);
        }
        //if (!(cmd->buttons & (1 << 0)))
          //  return false;
        if ((cmd->buttons & (1 << 5))) // IN_USE
            return false;
        float bestFov = Config::aimbotFov;
        CVector bestAngle{};
        CEntity* bestTarget = nullptr;
        CGameTrace tr{};
        for (int i = 1; i < I::entitylist->GetHighestEntityIndex(); i++) {
            auto player = I::entitylist->GetEntityFromIndex(i);
            if (!player) continue;

            auto name = player->GetClientClass()->szNetworkName;
            
            if (std::strcmp(name, "CCSPlayer") == 0)
            {
                if (player == pLocal || player->getTeam() == pLocal->getTeam() || !player->isAlive())
                    continue;

                if (player->IsDormant()) continue;

                CMatrix3x4 bones[128];
                if (!player->SetupBones(bones, 128, 0x00000100, I::globals->curtime)) continue;

                CVector eyePos = pLocal->GetEyePosition();
                CVector targetPos = bones[Config::Hitbox].Origin();

                CVector delta = (targetPos - eyePos).ToAngle() - cmd->viewangles;

                delta.Normalize();

                float fov = std::hypot(delta.x, delta.y);
                Entity ent{};
                ent.player = player;
                ent.fov = fov;
                ent.health = player->getHealth();
                ent.type = name;
                std::memcpy(ent.bones, bones, sizeof(bones));


                CVector screen;
                int w, h;
                I::engine->GetScreenSize(w, h);

                ent.trace = tr.fraction;
                visibleEnemies.push_back(ent);
                if (!I::debugoverlay->ScreenPosition(targetPos, screen)) {
                    float dx = screen.x - w / 2.0f;
                    float dy = screen.y - h / 2.0f;
                    float pixelDistance = sqrtf(dx * dx + dy * dy);

                    if (pixelDistance < bestFov) {
                        bestFov = pixelDistance;
                        bestTarget = player;
                        float smooth = Config::aimbotSmooth == 0 ? 1.0f : Config::aimbotSmooth;
                        bestAngle.x = delta.x / smooth;
                        bestAngle.y = delta.y / smooth;
                    }
                }
            }
            else if (strncmp(name, "CWeapon", 7) == 0) {
                Entity ent{};
                ent.player = player;
                ent.type = name;
                visibleEnemies.push_back(ent);
            }
            else if (strcmp(name, "CC4") == 0) {
                Entity ent{};
                ent.player = player;
                ent.type = name;
                visibleEnemies.push_back(ent);
            }
            else if (strcmp(name, "CPlantedC4") == 0) {
                Entity ent{};
                ent.player = player;
                ent.type = name;
                visibleEnemies.push_back(ent);
            }
        }

        if (bestTarget && GetAsyncKeyState(VK_XBUTTON2) && Config::aimbot) {
            if (Config::silentAim) {
                cmd->viewangles += bestAngle;
                I::engine->SetViewAngles(cmd->viewangles);
            }
            else {
                cmd->viewangles += bestAngle;
                I::engine->SetViewAngles(cmd->viewangles);
            }
        }

        {
            std::scoped_lock lock(g_entityMutex);
            g_Entitylist = std::move(visibleEnemies);
        }
        {
            if (!(cmd->buttons & (1 << 0)) && Config::antiAim) {
                enum AntiAimYawType { STATIC, YAWJITTER, SPIN, LEFT, RIGHT };
                enum AntiAimPitchType {DOWN, UP, ZERO, PITCHJITTER, EMOTION};
                AntiAimYawType aaYawType = (AntiAimYawType)Config::AAYawMode;
                AntiAimPitchType aaPitchType = (AntiAimPitchType)Config::AAPitchMode;
                switch (aaYawType) {
                    case AntiAimYawType::STATIC:
                    {
                        cmd->viewangles.y += 180.f;
                        break;
                    }
                    case AntiAimYawType::YAWJITTER:
                    {
                        cmd->viewangles.y = oldAngles.y + 180.f + RandomFloat(-15.f, 45.f);
                        break;
                    }
                    case AntiAimYawType::SPIN:
                    {
                        static float spin = 0.f;
                        spin += 45.f;
                        cmd->viewangles.y = fmod(spin, 360.f);
                        break;
                    }
                    case AntiAimYawType::LEFT:
                    {
                        cmd->viewangles.y -= 60;
                        break;
                    }
                    case AntiAimYawType::RIGHT:
                    {
                        cmd->viewangles.y += 60;
                        break;
                    }
                }

                switch (aaPitchType) {

                    case AntiAimPitchType::PITCHJITTER:
                    {
                        cmd->viewangles.x = RandomFloat(0, 1) > 0.5 ? -89 : 89;
                        break;
                    }
                    case AntiAimPitchType::DOWN:
                    {
                        cmd->viewangles.x = 89.f;
                        break;
                    }
                    case AntiAimPitchType::UP:
                    {
                        cmd->viewangles.x = -89.f;
                        break;
                    }
                    case AntiAimPitchType::ZERO:
                    {
                        cmd->viewangles.x = 0.f;
                        break;
                    }
                    case AntiAimPitchType::EMOTION: {
                        cmd->viewangles.x = 91.f;
                        break;
                    }
                }
                float deltaYaw = DEG2RAD(cmd->viewangles.y - oldAngles.y);

                float sinYaw = std::sin(deltaYaw);
                float cosYaw = std::cos(deltaYaw);

                float oldForward = cmd->forwardmove;
                float oldSide = cmd->sidemove;

                cmd->forwardmove = cosYaw * oldForward - sinYaw * oldSide;
                cmd->sidemove = sinYaw * oldForward + cosYaw * oldSide;

                I::pred->SetLocalViewAngles(cmd->viewangles);
            }
            cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.f, 89.f);
            cmd->viewangles.y = std::fmod(cmd->viewangles.y, 360.f);
            aaangle = cmd->viewangles;

            I::pred->SetLocalViewAngles(cmd->viewangles);
        }

        
    }

    return false;
}

void DrawFovCircle(int centerX, int centerY, float radius, const Color& color, int segments = 128) {
    if (!I::surface || radius <= 0.0f)
        return;

    float angleStep = (2.0f * static_cast<float>(3.14)) / segments;

    for (int i = 0; i < segments; ++i) {
        float theta1 = i * angleStep;
        float theta2 = (i + 1) * angleStep;

        int x1 = static_cast<int>(centerX + radius * std::cos(theta1));
        int y1 = static_cast<int>(centerY + radius * std::sin(theta1));

        int x2 = static_cast<int>(centerX + radius * std::cos(theta2));
        int y2 = static_cast<int>(centerY + radius * std::sin(theta2));

        I::surface->DrawSetColor(color.r, color.g, color.b, color.a); 
        I::surface->DrawLine(x1, y1, x2, y2);
    }
}

void __stdcall PaintTraverse(std::uintptr_t vGuiPanel, bool forceRepaint, bool allowForce)
{
    if (!I::surface)
        return PaintTraverseOriginal(I::panel, vGuiPanel, forceRepaint, allowForce);
    static bool initalized = false;
    static unsigned int testfnt = 0;
    if (!initalized) {
        testfnt = I::surface->CreateFontVolvo();
        I::surface->SetFontGlyphSet(testfnt, "Arial", 17, 100, 0, 0, 0x010);
        initalized = true;
    }
    if (vGuiPanel == I::engineVgui->GetPanel(PANEL_TOOLS))
    {
        if (CMenu::menuOpen) {
            I::surface->DrawSetColor(Config::CheatTheme == 0 ? 0 : 255, Config::CheatTheme == 0 ? 0 : 255, Config::CheatTheme == 0 ? 0 : 255, 240);
            I::surface->DrawFilledRect(0, 280, 200, 700);
        }
        I::surface->DrawSetTextFont(testfnt);
        I::surface->DrawSetTextPos(5, 280);
        I::surface->DrawSetTextColor(Color{ 255,255,255,255 });
        I::surface->DrawPrintText(L"CSSO x86 | github.com/Zordon1337", wcslen(L"CSSO x86 | github.com/Zordon1337"), 0);


        std::scoped_lock lock(g_entityMutex);

        if (I::engine && I::engine->IsInGame()) {
            for (int i = 0; i < g_Entitylist.size(); i++) {
                auto player = g_Entitylist[i].player;
                auto name = g_Entitylist[i].type;
                if (player->IsDormant()) continue;
                if (std::strcmp(name, "CCSPlayer") == 0)
                {
                    auto bones = g_Entitylist[i].bones;
                    int health = g_Entitylist[i].health;


                    if (!player->isAlive())
                        continue;

                    if (player->IsDormant()) continue;


                    CVector top;
                    if (I::debugoverlay->ScreenPosition(bones[10].Origin() + CVector{ 0.f, 0.f, 16.f }, top)) continue;

                    CVector bottom;
                    if (I::debugoverlay->ScreenPosition(player->GetAbsOrigin(), bottom)) continue;

                    float height = bottom.y - top.y;
                    float width = height * 0.3f;

                    int boxX = static_cast<int>(top.x - width);
                    int boxY = static_cast<int>(top.y);
                    int boxW = static_cast<int>(width * 2);
                    int boxH = static_cast<int>(height);
                    if (Config::box) {
                        I::surface->DrawSetColor(Config::CheatTheme == 0 ? 0 : 255, Config::CheatTheme == 0 ? 0 : 255, Config::CheatTheme == 0 ? 0 : 255, 255);
                        I::surface->DrawOutlinedRect(boxX, boxY, boxX + boxW, boxY + boxH);
                    }
                    if (Config::healthbar) {
                        int barW = 1;
                        int barX = boxX - barW - 1;
                        int barY = boxY;
                        int barH = boxH;

                        I::surface->DrawSetColor(60, 60, 60, 255);
                        I::surface->DrawFilledRect(barX, barY, barX + barW, barY + barH);

                        int healthH = static_cast<int>(barH * (health / 100.f));
                        int healthY = barY + (barH - healthH);

                        Color healthColor = Color{
                            static_cast<byte>(255 - (health * 2.55f)),
                            static_cast<byte>(health * 2.55f),
                            0,
                            255
                        };

                        I::surface->DrawSetColor(healthColor.r, healthColor.g, healthColor.b);
                        I::surface->DrawFilledRect(barX, healthY, barX + barW, barY + barH);
                    }
                }
                else if (strncmp(name, "CWeapon", 7) == 0) {
                    CVector tpos = player->GetAbsOrigin();
                    CVector pos;
                    bool offscreen = I::debugoverlay->ScreenPosition(tpos, pos);

                    uint32_t ownerHandle = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(player) + 0x5B8);
                    bool isOnGround = (ownerHandle == 0 || ownerHandle == 0xFFFFFFFF);

                    if (Config::drawDroppedWeapons && isOnGround && !offscreen) { // TODO, later use it for shows player's weapons maybe?
                        CMenu::DrawTxt(pos.x, pos.y, testfnt, name + 7);
                    }

                }
                else if (strcmp(name, "CC4") == 0) {
                    CVector tpos = player->GetAbsOrigin();
                    CVector pos;
                    bool offscreen = I::debugoverlay->ScreenPosition(tpos, pos);

                    if (Config::drawC4 && !offscreen) {
                        CMenu::DrawTxt(pos.x, pos.y, testfnt, "C4");
                    }

                }
                else if (strcmp(name, "CPlantedC4") == 0) {
                    if (!player->m_bBombTicking() || player->m_bBombDefused()) break;
                    CVector tpos = player->GetAbsOrigin();
                    CVector pos;
                    bool offscreen = I::debugoverlay->ScreenPosition(tpos, pos);

                    auto m_flDefuseCountDown = player->m_flDefuseCountDown();
                    auto m_flC4Blow = player->m_flC4Blow();

                    if (Config::drawPlantedC4 && !offscreen) {
                        char buf3[100];
                        std::snprintf(buf3, sizeof(buf3), "Planted bomb | %ds", (int)(m_flC4Blow - I::globals->curtime));
                        CMenu::DrawTxt(pos.x, pos.y, testfnt, buf3); // TODO, show duration until explosion
                    }

                    int w, h;
                    I::engine->GetScreenSize(w, h);
                    if (!w || !h) break;

                    if (Config::drawC4Status) {
                        I::surface->DrawSetColor(Config::CheatTheme == 0 ? 0 : 255, Config::CheatTheme == 0 ? 0 : 255, Config::CheatTheme == 0 ? 0 : 255, 255);
                        I::surface->DrawFilledRect(-120 + w / 2, 0, w / 2 + 120, 70 + (m_flDefuseCountDown != 0 ? 30 : 0));
                        CMenu::DrawTxt(w / 2, 20, testfnt, "Bomb planted");

                        char buf2[100];
                        std::snprintf(buf2, sizeof(buf2), "Exploding in %ds", (int)(m_flC4Blow - I::globals->curtime));
                        CMenu::DrawTxt(w / 2, 40, testfnt, buf2);
                        if (m_flDefuseCountDown != 0) {
                            char buf[100];
                            std::snprintf(buf, sizeof(buf), "Defusing (%ds)", (int)(m_flDefuseCountDown - I::globals->curtime));

                            CMenu::DrawTxt(w / 2, 60, testfnt, buf);
                        }
                    }

                    if (Config::drawC4Bar) {
                        if (m_flDefuseCountDown != 0) {
                            auto m_flDefuseLength = player->m_flDefuseLength();
                            auto multipler = (m_flDefuseCountDown - I::globals->curtime) / m_flDefuseLength;
                            Color clr;
                            clr.a = 200;
                            if (multipler > 0.70) {
                                clr.r = 255;
                                clr.g = 0;
                                clr.b = 0;
                            }
                            else if (multipler > 0.30) {
                                clr.r = 255;
                                clr.g = 215;
                                clr.b = 0;
                            }
                            else {

                                clr.r = 0;
                                clr.g = 255;
                                clr.b = 0;
                            }

                            I::surface->DrawSetColor(clr.r, clr.g, clr.b, clr.a);
                            I::surface->DrawFilledRect(0, h - 10, w* multipler, h);
                        }
                        else {

                            auto m_flTimerLength = player->m_flTimerLength();
                            auto multipler = (m_flC4Blow - I::globals->curtime) / m_flTimerLength;
                            Color clr;
                            clr.a = 200;
                            if (multipler < 0.30) {
                                clr.r = 255;
                                clr.g = 0;
                                clr.b = 0;
                            } else if (multipler < 0.70) {
                                clr.r = 255;
                                clr.g = 215;
                                clr.b = 0;
                            }
                            else {

                                clr.r = 0;
                                clr.g = 255;
                                clr.b = 0;
                            }

                            I::surface->DrawSetColor(clr.r,clr.g,clr.b,clr.a);
                            I::surface->DrawFilledRect(0, h - 10, w * multipler, h);

                        }
                    }
                }

                if (Config::ViewmodelChanger) {
                    const char* vars[] = { "viewmodel_offset_x", "viewmodel_offset_y", "viewmodel_offset_z" };
                    float values[] = { Config::xViewModel, Config::yViewModel, Config::zViewModel };

                    for (int i = 0; i < 3; i++) {
                        auto var = I::cvar->FindVar(vars[i]);
                        if (var) {
                            var->m_fnChangeCallback = nullptr;
                            var->SetValue(values[i]);
                        }
                    }
                }


                if (Config::fullbright) {
                    auto f = I::cvar->FindVar("mat_fullbright");
                    if (!f) break;
                    f->m_fnChangeCallback = nullptr;
                    f->SetValue(1);
                }
                else {
                    auto f = I::cvar->FindVar("mat_fullbright");
                    if (!f) break;
                    f->m_fnChangeCallback = nullptr;
                    f->SetValue(0);
                }

            }
            CMenu::HandleMenuInput();
            if (CMenu::menuOpen) {
                CMenu::DrawMenu(5, 300, testfnt);
            }
            {
                int screenWidth, screenHeight;
                I::engine->GetScreenSize(screenWidth, screenHeight);

                int centerX = screenWidth / 2;
                int centerY = screenHeight / 2;

                float aimbotPixelRadius = static_cast<float>(Config::aimbotFov);

                DrawFovCircle(centerX, centerY, aimbotPixelRadius, Color{ 255, 255, 255, 255 });

            }
        }
    }
    return PaintTraverseOriginal(I::panel, vGuiPanel, forceRepaint, allowForce);
}


DWORD WINAPI Setup(LPVOID idk) {

    AllocConsole();
    FILE* nahuyidi;
    freopen_s(&nahuyidi, "CONIN$", "r", stdin);
    freopen_s(&nahuyidi, "CONOUT$", "w", stderr);
    freopen_s(&nahuyidi, "CONOUT$", "w", stdout);

    I::Init();

    MH_Initialize();

    MH_CreateHook(
        (*static_cast<void***>(I::clientMode))[21],
        &CreateMove,
        reinterpret_cast<void**>(&CreateMoveOriginal)
    );
    MH_CreateHook(
        (*static_cast<void***>(I::client))[36],
        &FrameStage,
        reinterpret_cast<void**>(&FrameStageoriginal)
    );
    MH_CreateHook(
        (*static_cast<void***>((void*)I::panel))[41],
        &PaintTraverse,
        reinterpret_cast<void**>(&PaintTraverseOriginal)
    );

    MH_EnableHook(MH_ALL_HOOKS);




    while (true);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, NULL, Setup, hModule, NULL, nullptr);
        break;
    }
    }
    return TRUE;
}

