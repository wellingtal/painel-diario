#include "RYLSceneobject.h"
#include "GMMemory.h"

#ifdef _DEBUG
	#define new DEBUG_NORMALBLOCK
#endif 

CRYLSceneObject::CRYLSceneObject( INT iID ) 
{
	m_iSceneID = iID ;
	m_lScreenFade = 0L ;
	m_dwFadeMode  = 0xffffffff ;
}

CRYLSceneObject::~CRYLSceneObject()
{

}

void CRYLSceneObject::FadeIn(void)
{	
	m_dwFadeMode	= FADE_IN ;
	m_lScreenFade	= FADE_START ;
	m_lItemFade		= FADE_START ;
}
void CRYLSceneObject::FadeOut(void)
{	
	m_dwFadeMode = FADE_OUT;
	m_lScreenFade = FADE_END;
	m_lItemFade = FADE_END;

}