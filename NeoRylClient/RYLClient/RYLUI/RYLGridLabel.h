//================================================================
//
//	Name : RYLGridLabel.h 
//	Desc : RYL Grid Label
//  Date : 2003. 09. 24
//
//================================================================
#ifndef __RYLGRIDLABEL_H__
#define __RYLGRIDLABEL_H__

#include "GMObject.h"

class CGMFontTexture ;

typedef struct tagSGridLabelData 
{
	int	iPosX ;
	CGMFontTexture* FontTexture ;
	CGMString Str ;
	DWORD	m_dwFontColor;			// 셋팅만 되어있는 폰트 칼라
									// FontTexture 의 m_dwFontColor 에 설정하기 전에는 이색으로 출력되지 않는다.
	tagSGridLabelData()
	{
		iPosX			= -1 ;
		FontTexture		= NULL ;
		m_dwFontColor	= 0xffffffff ;
	}
}SGridLabelData ;

class CRYLGridLabel : public CGMObject
{
private :
	INT				m_iDataCount ;
	INT				m_iWidth ;
	SGridLabelData* m_ArrGridLabelData ;
	
public:
	CRYLGridLabel( INT DataCount ) ;
	virtual ~CRYLGridLabel() ;

	VOID	Clear() ;
	VOID	SetDataCount( INT DataCount ) ;
	INT		GetDataCount() ;
	LPSTR	GetData( INT Index ) ;
	VOID	SetFontColor( INT Index, DWORD dwFontColor ) ;
	VOID	SetFontColor( DWORD dwFontColor ) ;
	DWORD	GetFontColor( INT Index ) ;
	VOID	SetViewFontColor() ;
	VOID	SetViewFontColor( DWORD dwFontColor ) ;		// 화면상에 출력되는 색 설정
	VOID	SetData( INT Index, LPSTR pStr ) ;
	VOID	SetSize( INT Index, INT Width, int iPosX ) ;
	VOID	Paint( INT xx, INT yy ) ;
};

#endif //__RYLGRIDLABEL_H__