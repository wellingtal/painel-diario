#include "GMUITooltip.h"
#include "GMMemory.h"


CGMUITooltip::CGMUITooltip()
{
	m_bShow			= TRUE ;
	m_dwMaxLength	= 0 ;
	m_dwAlign		= DT_CENTER ;
	m_dwCtrlType	= 0L ;
	m_dwKey			= 0L ;
}

CGMUITooltip::~CGMUITooltip()
{
}
