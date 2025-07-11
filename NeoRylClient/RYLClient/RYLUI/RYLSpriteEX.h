//================================================================
//
//	Name : RYLSpriteEx.h 
//	Desc : RYL Ȯ�� ��������Ʈ
//  Date : 2003. 12. 23
//
//================================================================
#ifndef __RYLSPRITEEX_H__
#define __RYLSPRITEEX_H__

#include "GMUILabel.h"
#include <d3d8.h>

class CRYLSprite ;
class CTexture ;
class CGMUICustomDlg ;

class CRYLSpriteEx : public CGMUILabel
{
private:
	BOOL				m_bAlpha;				//�ؽ����� ���� ���� ����
public :

	CRYLSprite*			m_pSprite ;
	CTexture*			m_pTexture ;
	LPDIRECT3DDEVICE8	m_lpD3DDevice ;

public:
	CRYLSpriteEx( CGMUICustomDlg *pParentDlg ) ;
	virtual ~CRYLSpriteEx() ;

	virtual VOID	DoDraw( CGMUIObject* Sender, RECT r, INT State ) ;
	virtual VOID	AttachImageTexture( CRYLSprite* pSprite ) ;
	virtual VOID	DetachImageTexture() ;

	VOID			SetTextureAlpha( BOOL bAlpha ){ m_bAlpha = bAlpha; }; //�ؽ����� ���� ���� ����
	BOOL			GetTextureAlpha(){ return m_bAlpha; };
} ;

#endif // __RYLSPRITEEX_H__