//=================================================================
// Name : RYLCharacterSelectScene.h
// Data : 2003. 11. 13
// Desc : ĳ���� ���� ȭ��
//			- public : CRYLSceneObject
//=================================================================

#ifndef __RYL_CHARACTERSELECTSCENE_H__
#define __RYL_CHARACTERSELECTSCENE_H__

#include "RYLSceneObject.h"

class CRYLDialog ;
class CRYLLabel ;

class CRYLCharacterSelectScene : public CRYLSceneObject
{
private :
	CRYLDialog*		m_pCharacterCreation ;
	CRYLDialog*		m_pCharacterInfoDlg ;
	CRYLDialog*		m_pCharacterDeleteDlg ;

	// �߱���
	CRYLDialog*		m_pChannelDlg ;

	// WORK_LIST 2.3 ���� ���� ���� UI �߰�
	CRYLDialog*		m_pNationDlg ;
	CRYLDialog*		m_pNationSelectDlg ;

	// ���� ����
	CRYLLabel*		m_pDescription ;

public :
	CRYLCharacterSelectScene( INT iID ) ;
	virtual ~CRYLCharacterSelectScene() ;

	virtual HRESULT			SetRYLRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual	HRESULT			Render( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	virtual HRESULT			Update() ;
	
	virtual HRESULT			InitResourceObject() ;
	virtual HRESULT			DeleteResourceObject() ;

	virtual HRESULT			FinalCleanup() ;
	virtual HRESULT			MsgProc( HWND hWnd, UINT msg, WPARAM, LPARAM lParam ) ;

	virtual VOID			SceneShow() ;
	virtual VOID			SceneHide() ;

//	WORK_LIST 1.2 ĳ���� ����â���� ���� �� ������
//	VOID			CreateCharacterSelectDlg() ;
	VOID			CreateCharacterSelectDlg( unsigned long dwRace ) ;

	CRYLDialog*		GetCharacterCreation()	{ return m_pCharacterCreation ; }
	CRYLDialog*		GetCharacterInfoDlg()	{ return m_pCharacterInfoDlg ; }
	CRYLDialog*		GetCharacterDeleteDlg()	{ return m_pCharacterDeleteDlg ; }
	CRYLDialog*		GetChannelDlg()			{ return m_pChannelDlg ; }

	// WORK_LIST 2.3 ���� ���� ���� UI �߰�
	CRYLDialog*		GetNationDlg()			{ return m_pNationDlg ; }
	CRYLDialog*		GetNationSelectDlg()	{ return m_pNationSelectDlg ; }

	VOID			SetCurrentNation( unsigned long dwNation ) ;
	VOID			ShowNationDlg()	;
	VOID			HideNationDlg()	;

	void			DeleteCharacterCreation();
} ;

#endif //__RYL_CHARACTERSELECTSCENE_H__