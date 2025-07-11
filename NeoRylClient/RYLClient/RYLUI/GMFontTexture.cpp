#include "GMFontTexture.h"
#include "GMMemory.h"

CGMFontTexture::CGMFontTexture() : CGMTexture()
{
	m_dwClearColor	= 0x00000000 ;
	m_dwFontColor   = 0xFF000000 ;
	m_dwBorderColor = 0xFF000000 ;
	m_dwShadowColor = 0xFF000000 ;
	m_FontDrawType	= CGMFontTexture::FontDrawType_NONE ;
}

CGMFontTexture::~CGMFontTexture()
{   
	m_dwClearColor	= 0x00000000 ;
	m_dwFontColor	= 0xFF000000 ;
	m_dwBorderColor = 0xFF000000 ;
	m_dwShadowColor = 0xFF000000 ;
	m_FontDrawType	= CGMFontTexture::FontDrawType_NONE ;
}
