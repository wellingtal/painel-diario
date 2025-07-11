//================================================================
//
//	Name : GMUIObject.inl 
//	Desc : 
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMUIOBJECT_INL__
#define __GMUIOBJECT_INL__
/*
GMINLINE VOID		
CGMUIObject::SetColor( DWORD dwColor ) 
{
	m_dwColor = dwColor ;
}

GMINLINE DWORD
CGMUIObject::GetColor() 
{
	return m_dwColor ;
}

GMINLINE VOID
CGMUIObject::SetFontColor( DWORD dwColor ) 
{
	m_dwFontColor = dwColor ;
}

GMINLINE DWORD
CGMUIObject::GetFontColor()
{
	return m_dwFontColor ;
}

GMINLINE VOID	
CGMUIObject::SetBorderFontColor( DWORD dwColor ) 
{
	m_dwBorderFontColor = dwColor ;
}

GMINLINE DWORD	
CGMUIObject::GetBorderFontColor() 
{
	return m_dwBorderFontColor ;
}

GMINLINE VOID		
CGMUIObject::SetLeft( INT X ) 
{
	m_iLeft = X ;
}

GMINLINE INT		
CGMUIObject::GetLeft() 
{
	return m_iLeft;
}

GMINLINE VOID		
CGMUIObject::SetTop( INT Y ) 
{
	m_iTop = Y;
}

GMINLINE INT		
CGMUIObject::GetTop() 
{
	return m_iTop ;
}

GMINLINE VOID		
CGMUIObject::SetWidth( INT Width ) 
{
	if (m_iWidth != Width)
	{
		m_iWidth = Width;
		SetSize(this);
	}
}

GMINLINE INT		
CGMUIObject::GetWidth() 
{
	return m_iWidth ;
}

GMINLINE VOID		
CGMUIObject::SetHeight( INT Height ) 
{
	if (m_iHeight != Height)
	{
		m_iHeight = Height;
		SetSize(this);
	}
}

GMINLINE INT		
CGMUIObject::GetHeight() 
{
	return m_iHeight;
}

GMINLINE VOID		
CGMUIObject::SetVisible( BOOL boVisible ) 
{
	if (boVisible)
	{
		m_bVisible = TRUE;
		Show (this);
	}
	else
	{
		m_bVisible = FALSE;
		Hide (this);
	}
}

GMINLINE BOOL		
CGMUIObject::GetVisible() 
{
	return m_bVisible;
}

GMINLINE VOID		
CGMUIObject::SetEnable( BOOL boEnable ) 
{
	if (boEnable)
	{
		m_bEnable = TRUE;
		if (OnEnableEvent != NULL)
		{
			OnEnableEvent (this);
		}
	}
	else 
	{
		m_bEnable = FALSE;
		if (OnDisibleEvent != NULL)
		{
			OnDisibleEvent (this);
		}
	}
}

GMINLINE BOOL		
CGMUIObject::GetEnable() 
{
	return m_bEnable;
}

GMINLINE BOOL		
CGMUIObject::GetEntered() 
{
	return m_bEntered; 
}

GMINLINE VOID		
CGMUIObject::SetDown( BOOL bDown )		
{
	m_bDown = bDown ; 
}

GMINLINE BOOL		
CGMUIObject::GetDown()					
{
	return m_bDown ; 
}

GMINLINE VOID		
CGMUIObject::SetParentHandle( HWND hWnd ) 
{
	m_hParentHandle = hWnd;
}

GMINLINE HWND		
CGMUIObject::GetParentHandle() 
{
	return m_hParentHandle ;
}
*/
#endif //__GMUIOBJECT_INL__