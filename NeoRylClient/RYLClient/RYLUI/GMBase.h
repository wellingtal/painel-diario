//================================================================
//
//	Name : GMBase.h 
//	Desc : Gama Soft Utility Function
//  Date : 2003. 09. 09
//
//================================================================
#ifndef __GMBASE_H__
#define __GMBASE_H__

#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <crtdbg.h>

//================================================================
//
//	inline
//
//================================================================

#ifndef GMINLINE
#ifdef _MSC_VER
  #if (_MSC_VER >= 1200)
  #define GMINLINE __forceinline
  #else		// (_MSC_VER < 1200)
  #define GMINLINE __inline
  #endif	// (_MSC_VER >= 1200)
#else
  #ifdef __cplusplus
  #define GMINLINE inline
  #else		// !defined __cplusplus
  #define GMINLINE
  #endif	//	__cplusplus
#endif
#endif


//================================================================
//
//	Memory Delete/Release
//
//================================================================
#ifndef GM_DELETE
  #define GM_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#endif

#ifndef GM_DELETE_ARRAY
  #define GM_DELETE_ARRAY(p)	{ if (p) { delete [] (p); (p) = NULL; } }
#endif

#ifndef GM_RELEASE
  #define GM_RELEASE(p)			{ if (p) { (p)->Release(); (p) = NULL; }}
#endif


//================================================================
//
//	4바이트 예외값. (GM Const Unused 32byte)
//
//================================================================

#ifndef GMCUNUSED32
  #define GMCUNUSED32	(0xffffffff)
#endif

//================================================================
//
//	디버그모드시 exp_if(조건)이 거짓일 경우, assert()를 호출한다.
//
//================================================================

#ifndef GMASSERT
#if defined (_DEBUG)
	#define GMASSERT(exp_if, desc)	 _ASSERT(exp_if && desc)
#else
	#define GMASSERT(exp_if, desc)
#endif
#endif

//===============================================================
//
// FadeIn, FadeOut
//
//===============================================================
#define FADE_IN				0
#define FADE_OUT			1
#define FADE_NONE			0xFFFFFFFF

#define FADE_START			0
#define FADE_END			200

#endif //__GMBASE_H__