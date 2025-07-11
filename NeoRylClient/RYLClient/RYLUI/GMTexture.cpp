#include "GMTexture.h"
#include "GMMemory.h"

CGMTexture::CGMTexture () : CGMObject ()
{
	m_dwColor	= 0xFFFFFFFF;
	m_iWidth	= 0;
	m_iHeight	= 0;
	m_Alpha		= 255;
	m_boVisible = TRUE;
}

CGMTexture::~CGMTexture ()
{

}

VOID CGMTexture::Clear ()
{
	m_dwColor	= 0xFFFFFFFF ;
	m_iWidth	= 0 ;
	m_iHeight	= 0 ;
	m_Alpha		= 255 ;
}