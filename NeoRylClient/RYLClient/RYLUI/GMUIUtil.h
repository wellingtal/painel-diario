//================================================================
//
//	Name : GMUIUtil.h 
//	Desc : Gama Soft Util Mecro
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMUIUTIL_H__
#define __GMUIUTIL_H__

#include "GMObjectDef.h"
#include "GMFont.h"

typedef struct tagTImgLibColor 
{
	BYTE Red; 
	BYTE Green; 
	BYTE Blue; 
	bool Used;
} TImgLibColor; 

 
DWORD ColorToRGB (int Color);


int GetMin(const int A, const int B);
int GetMax(const int A, const int B);

int InArea (int aLeft, int aTop, int aWidth, int aHeight, int aX, int aY);

DWORD Color32toColor32 (DWORD Alpha, DWORD Color);
WORD PAnsColorTo16Color (const WORD AlphaValue, WORD Value);
DWORD PAnsColorTo32Color (const WORD AlphaValue, WORD Value);
WORD PAnsColorTo4444Color (const WORD AlphaValue, WORD Value);

DWORD GetReverseAlpha (DWORD dwAlpha, DWORD dwApplyAlpha);

int KeysToShiftState (WORD keys);

int GetGMUITextWidth (char* pstr);
int GetGMUITextHeight (char* pstr);

int GetTextureSize (int Value);
#endif