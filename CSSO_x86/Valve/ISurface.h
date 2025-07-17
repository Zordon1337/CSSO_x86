#pragma once
#include "../Utils/memory.h"
#include "../Interfaces.hpp"
enum EFontDrawType : std::int32_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE
};

enum EFontFlag
{
	FONT_FLAG_NONE,
	FONT_FLAG_ITALIC = 0x001,
	FONT_FLAG_UNDERLINE = 0x002,
	FONT_FLAG_STRIKEOUT = 0x004,
	FONT_FLAG_SYMBOL = 0x008,
	FONT_FLAG_ANTIALIAS = 0x010,
	FONT_FLAG_GAUSSIANBLUR = 0x020,
	FONT_FLAG_ROTARY = 0x040,
	FONT_FLAG_DROPSHADOW = 0x080,
	FONT_FLAG_ADDITIVE = 0x100,
	FONT_FLAG_OUTLINE = 0x200,
	FONT_FLAG_CUSTOM = 0x400,
	FONT_FLAG_BITMAP = 0x800,
};
struct Color {
	int r, g, b, a;
};
class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a = 255) noexcept
	{
		mem::Call<void>(this, 11, r, g, b, a);
	}

	void DrawFilledRect(int x, int y, int xx, int yy) noexcept
	{
		mem::Call<void>(this, 12, x, y, xx, yy);
	}

	void DrawOutlinedRect(int x, int y, int xx, int yy) noexcept
	{
		mem::Call<void>(this, 14, x, y, xx, yy);
	}
	void DrawLine(int x, int y, int xx, int yy) noexcept
	{
		mem::Call<void>(this, 15, x, y, xx, yy);
	}
	void DrawOutlinedCircle(int x, int y, int radius, int segments) noexcept
	{
		mem::Call<void>(this, 107, x, y, radius, segments);
	}

	void DrawColoredCircle(int centerx, int centery, float radius, int r, int g, int b, int a) noexcept
	{
		mem::Call<void>(this, 162, centerx, centery, radius, r, g, b, a);
	}
	void DrawSetTextFont(unsigned long FONT) {
		mem::Call<void>(this, 17, FONT);
	}
	void DrawSetTextColor(Color c)
	{
		mem::Call<void>(this, 19, c);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		mem::Call<void>(this, 18, r,g,b,a);
	}
	void DrawSetTextPos(int x, int y)
	{
		mem::Call<void>(this, 20, x, y);
	}
	void DrawPrintText(const wchar_t* text, int textLen, int unknown)
	{
		mem::Call<void>(this, 22, text, textLen, unknown);
	}

	void GetTextSize(unsigned long hFont, const wchar_t* wText, int* iWide, int* iTall)
	{
		mem::Call<void>(this, 75, hFont, wText, iWide, iTall);
	}
	unsigned long CreateFontVolvo()
	{
		return mem::Call<unsigned long>(this, 66);
	}
	bool SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		return mem::Call<bool>(this, 67, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	

};