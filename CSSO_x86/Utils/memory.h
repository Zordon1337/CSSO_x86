#pragma once
#include <cstdint>
#include <vector>
#include "../Valve/CVector.h"
namespace mem {

    const char* lasthandled = "";
    template <typename Return, typename ... Arguments>
    Return Call(void* vmt, const std::uint32_t index, Arguments ... args) noexcept
    {
        __try
        {
            using Function = Return(__thiscall*)(void*, decltype(args)...);
            return (*static_cast<Function**>(vmt))[index](vmt, args...);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if constexpr (std::is_same_v<Return, CVector>)
            {
                return CVector{ 0, 0, 0 };
            }
            else if constexpr (std::is_same_v<Return, int>)
            {
                return 0;
            }
            else if constexpr (std::is_same_v<Return, float>)
            {
                return 0.f;
            }
            else
            {
                return Return{};
            }
        }
    }

    std::uint8_t* PatternScan(const char* moduleName, const char* pattern) noexcept
    {
        static auto patternToByte = [](const char* pattern) noexcept -> const std::vector<std::int32_t>
            {
                std::vector<std::int32_t> bytes = std::vector<std::int32_t>{ };
                char* start = const_cast<char*>(pattern);
                const char* end = const_cast<char*>(pattern) + std::strlen(pattern);

                for (auto current = start; current < end; ++current)
                {
                    if (*current == '?')
                    {
                        ++current;

                        if (*current == '?')
                            ++current;

                        bytes.push_back(-1);
                    }
                    else
                        bytes.push_back(std::strtoul(current, &current, 16));
                }

                return bytes;
            };

        const HINSTANCE handle = GetModuleHandle(moduleName);

        if (!handle)
            return nullptr;

        const PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
        const PIMAGE_NT_HEADERS ntHeaders =
            reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

        const std::uintptr_t size = ntHeaders->OptionalHeader.SizeOfImage;
        const std::vector<std::int32_t> bytes = patternToByte(pattern);
        std::uint8_t* scanBytes = reinterpret_cast<std::uint8_t*>(handle);

        const std::size_t s = bytes.size();
        const std::int32_t* d = bytes.data();

        for (std::size_t i = 0ul; i < size - s; ++i)
        {
            bool found = true;

            for (std::size_t j = 0ul; j < s; ++j)
            {
                if (scanBytes[i + j] != d[j] && d[j] != -1)
                {
                    found = false;
                    break;
                }
            }

            if (found)
                return &scanBytes[i];
        }

        return nullptr;
    }
}