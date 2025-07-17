#pragma once
#include <string>
#include <Windows.h>
#include "Interfaces.hpp"
#include <vector>
#include "Config.h"
#include <unordered_map>
namespace CMenu {
    static int selectedIndex = 0;
    static bool menuOpen = true;

    struct MenuItem {
        std::string name;
        enum Type { CHECKBOX, SLIDER, DROP, SECTION } type;
        bool* checkboxValue = nullptr;
        float* sliderValue = nullptr;
        float sliderMin = 0;
        float sliderMax = 100;
        int* selectedList = 0;
        std::unordered_map<int, std::string> list = {};
    };
    static std::vector<MenuItem> menuItems = {
        
        { "== AIMBOT ==", MenuItem::SECTION },
        { "Enable Aim", MenuItem::CHECKBOX, &Config::aimbot },
        { "FOV", MenuItem::SLIDER, nullptr, &Config::aimbotFov, 0, 300 },
        { "Smoothing", MenuItem::SLIDER, nullptr, &Config::aimbotSmooth, 0, 50 },
        { "Silent Aim", MenuItem::CHECKBOX, &Config::silentAim },
        { "Bone", MenuItem::DROP, nullptr, nullptr, 0, 2, &Config::Hitbox,
        {
            {8,  "Head"},
            {7,  "Neck"},
            {2,  "Upper Chest"}
        }},
        { "== Anti Aim ==", MenuItem::SECTION },
        { "Enable Anti Aim", MenuItem::CHECKBOX, &Config::antiAim },
        { "Yaw", MenuItem::DROP, nullptr, nullptr, 0, 2, &Config::AAYawMode,
        {
            {0, "Static"},
            {1, "Jitter"},
            {2, "Spin"},
            {3, "Left"},
            {4, "Right"},
            {5, "None"},
        }},
        { "Pitch", MenuItem::DROP, nullptr, nullptr, 0, 2, &Config::AAPitchMode,
        {
            {0, "Down"},
            {1, "Up"},
            {2, "Zero"},
            {3, "Jitter"},
            {4, "Emotion"},
            {5, "None"},
        }},
        { "== ESP ==", MenuItem::SECTION },
        { "Enable Box", MenuItem::CHECKBOX, &Config::box },
        { "Enable Healthbar", MenuItem::CHECKBOX, &Config::healthbar },
        { "Enable Planted C4", MenuItem::CHECKBOX, &Config::drawPlantedC4 },
        { "Enable C4", MenuItem::CHECKBOX, &Config::drawC4 },
        { "Enable Dropped Weapons", MenuItem::CHECKBOX, &Config::drawDroppedWeapons },
        { "Enable C4 Status", MenuItem::CHECKBOX, &Config::drawC4Status },
        { "Enable C4 Defuse & Explosion Bar", MenuItem::CHECKBOX, &Config::drawC4Bar },
        { "== MISC ==", MenuItem::SECTION },
        { "Enable Bhop", MenuItem::CHECKBOX, &Config::bhop },
        { "Enable Viewmodel Changer", MenuItem::CHECKBOX, &Config::ViewmodelChanger },
        { "Viewmodel X", MenuItem::SLIDER, nullptr, &Config::xViewModel, -20, 20 },
        { "Viewmodel Y", MenuItem::SLIDER, nullptr, &Config::yViewModel, -20, 20 },
        { "Viewmodel Z", MenuItem::SLIDER, nullptr, &Config::zViewModel, -20, 20 },
        { "Enable Full Bright", MenuItem::CHECKBOX, &Config::fullbright },
        { "== THEME ==", MenuItem::SECTION },
        { "Style", MenuItem::DROP, nullptr, nullptr, 0, 2, &Config::CheatTheme,
        {
            {0,  "Dark"},
            {1,  "Light"},
        }},
    };
    int GetListIndexFromKey(const std::unordered_map<int, std::string>& list, int key) {
        int index = 0;
        for (const auto& [k, v] : list) {
            if (k == key)
                return index;
            ++index;
        }
        return 0;
    }

    int GetKeyFromListIndex(const std::unordered_map<int, std::string>& list, int index) {
        int i = 0;
        for (const auto& [k, v] : list) {
            if (i == index)
                return k;
            ++i;
        }
        return 0;
    }

    void HandleMenuInput() {
        if (menuOpen) {
            if (GetAsyncKeyState(VK_DOWN) & 1) selectedIndex = (selectedIndex + 1) % menuItems.size();
            if (GetAsyncKeyState(VK_UP) & 1) selectedIndex = (selectedIndex - 1 + menuItems.size()) % menuItems.size();

            MenuItem& item = menuItems[selectedIndex];
            if (item.type == MenuItem::CHECKBOX && (GetAsyncKeyState(VK_RETURN) & 1)) {
                *item.checkboxValue = !*item.checkboxValue;
            }
            if (item.type == MenuItem::SLIDER) {
                if (GetAsyncKeyState(VK_LEFT) & 1) *item.sliderValue = max(*item.sliderValue - 1, item.sliderMin);
                if (GetAsyncKeyState(VK_RIGHT) & 1) *item.sliderValue = min(*item.sliderValue + 1, item.sliderMax);
            }
            if (item.type == MenuItem::DROP) {
                if (!item.selectedList) return;

                int listSize = static_cast<int>(item.list.size());
                int currentIndex = GetListIndexFromKey(item.list, *item.selectedList);

                if (GetAsyncKeyState(VK_LEFT) & 1)
                    currentIndex = (currentIndex - 1 + listSize) % listSize;
                if (GetAsyncKeyState(VK_RIGHT) & 1)
                    currentIndex = (currentIndex + 1) % listSize;

                *item.selectedList = GetKeyFromListIndex(item.list, currentIndex);
            }


        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            menuOpen = !menuOpen;
        }
    }
    
    void DrawMenu(int x, int y, unsigned long fnt) {
        I::surface->DrawSetTextFont(fnt);
        I::surface->DrawSetTextColor(Color{ Config::CheatTheme == 0 ? 255 : 0, Config::CheatTheme == 0 ? 255 : 0, Config::CheatTheme == 0 ? 255 : 0, 255 });

        for (size_t i = 0; i < menuItems.size(); ++i) {
            std::string label = (i == selectedIndex ? "> " : "  ") + menuItems[i].name + " ";
            if (menuItems[i].type == MenuItem::CHECKBOX)
                label += *menuItems[i].checkboxValue ? "[x]" : "[ ]";
            else if (menuItems[i].type == MenuItem::SLIDER)
                label += std::to_string(*menuItems[i].sliderValue);
            else if (menuItems[i].type == MenuItem::DROP) {
                int idx = menuItems[i].selectedList ? *menuItems[i].selectedList : 0;
                auto it = menuItems[i].list.find(idx);
                if (it != menuItems[i].list.end())
                    label += it->second;
                else
                    label += "shit code broke itself xdddddd";
            }


            std::wstring wLabel(label.begin(), label.end());
            I::surface->DrawSetTextPos(x, y + (i * 15));
            I::surface->DrawPrintText(wLabel.c_str(), wLabel.size(), 0);
        }
    }
    void DrawTxt(int x, int y, unsigned long fnt, std::string text, bool forcetheme = false) {

        I::surface->DrawSetTextFont(fnt);

        I::surface->DrawSetTextColor(Color{ 
            forcetheme ? (Config::CheatTheme == 0 ? 0 : 255) : (Config::CheatTheme == 0 ? 255 : 0),
            forcetheme ? (Config::CheatTheme == 0 ? 0 : 255) : (Config::CheatTheme == 0 ? 255 : 0),
            forcetheme ? (Config::CheatTheme == 0 ? 0 : 255) : (Config::CheatTheme == 0 ? 255 : 0),
            255 });
        std::wstring wLabel(text.begin(), text.end());
        int w, h;
        I::surface->GetTextSize(fnt, wLabel.c_str(), &w, &h);
        I::surface->DrawSetTextPos(x - (w/2), y - (h/2));
        I::surface->DrawPrintText(wLabel.c_str(), wLabel.size(), 0);
    }
}