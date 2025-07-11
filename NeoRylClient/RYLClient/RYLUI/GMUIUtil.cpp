#include "GMUIUtil.h"
#include "GMMemory.h"

DWORD ColorToRGB (int Color)
{
	if (Color < 0)
	{
		return GetSysColor (Color & 0x000000FF);
	}
	else return Color;
}
 
int GetMin(const int A, const int B)
{
	if (A < B)
	{
		return A;
	}
	else
	{
		return B;
	}
}

int GetMax(const int A, const int B)
{
	if (A < B)
	{
		return B;
	}
	else
	{
		return A;
	}
}

int InArea (int aLeft, int aTop, int aWidth, int aHeight, int aX, int aY)
{
	if (aLeft > aX) {return -1;}
	if (aTop > aY) {return -1;}
	if (aLeft+aWidth < aX) {return -1;}
	if (aTop+aHeight < aY) {return -1;}
	return 0;
}


DWORD Color32toColor32 (DWORD Alpha, DWORD Color)
{
	DWORD dwR, dwG, dwB;
	dwR = Color & 0xFF;
	dwG = (Color & 0xFF00) >> 8;
	dwB = (Color & 0xFF0000) >> 16;
	return (Alpha <<= 24) | (dwR <<= 16) | (dwG <<= 8) | dwB;
}

WORD PAnsColorTo16Color (const WORD AlphaValue, WORD Value)
{
	WORD a, r, g, b;

	a = AlphaValue;
	b = Value & 0x1F;
	g = (Value >>= 5) & 0x1F;
	r = (Value >>= 5) & 0x1F;
	return (a <<= 15) | (r <<= 10) | (g <<= 5) | b;
}

DWORD PAnsColorTo32Color (const WORD AlphaValue, WORD Value)
{
	WORD a, r, g, b;

	a = AlphaValue;
	b = Value & 0x1F;
	g = (Value >>= 5) & 0x1F;
	r = (Value >>= 5) & 0x1F;

    r = r * 8;
    g = g * 8;
    b = b * 8;
	return (a <<= 24) | (r <<= 16) | (g <<= 8) | b;
}

WORD PAnsColorTo4444Color (const WORD AlphaValue, WORD Value)
{
	WORD a, r, g, b;

	a = AlphaValue;
	b = Value & 0x1F;
	g = (Value >>= 5) & 0x1F;
	r = (Value >>= 5) & 0x1F;
	return (a <<= 12) | (r <<= 8) | (g <<= 4) | b;
}

int KeysToShiftState (WORD keys)
{
	int n;
	n = KEY_NONE;
	if ((keys & MK_SHIFT) != 0) {n = KEY_SHIFT;} 
	if ((keys & MK_CONTROL) != 0) {n = KEY_CTRL;}
    if ((keys & MK_LBUTTON) != 0) {n = KEY_LEFT;}
    if ((keys & MK_RBUTTON) != 0) {n = KEY_RIGHT;}
    if ((keys & MK_MBUTTON) != 0) {n = KEY_MIDDLE;}
	if (GetKeyState (VK_MENU) < 0) {n = KEY_ALT;}
	return n;
}

DWORD GetReverseAlpha (DWORD dwAlpha, DWORD dwApplyAlpha)
{
	DWORD dwA;
	dwAlpha = 255 - dwAlpha;
	dwA = dwApplyAlpha - (dwAlpha); 
	if (dwApplyAlpha < dwAlpha)
		dwA = 40;
	return dwA;
}

int GetGMUITextWidth (char* pstr)
{
	return CGMFont::Instance()->TextWidth (NULL, pstr);
}

int GetGMUITextHeight (char* pstr)
{
	return CGMFont::Instance()->TextHeight (NULL, pstr);
}

int GetTextureSize (int Value)
{
//	if ((Value > 0) && (Value <= 4)) return 4;
//	if ((Value > 4) && (Value <= 8)) return 8;
	if ((Value > 0) && (Value <= 4)) return 16;
	if ((Value > 4) && (Value <= 8)) return 16;
	if ((Value > 8) && (Value <= 16)) return 16;
	if ((Value > 16) && (Value <= 32)) return 32;
	if ((Value > 32) && (Value <= 64)) return 64;
	if ((Value > 64) && (Value <= 128)) return 128;
	if ((Value > 128) && (Value <= 256)) return 256;
	if ((Value > 256) && (Value <= 512)) return 512;
	if ((Value > 512) && (Value <= 1024)) return 1024;
	return 0;
}
