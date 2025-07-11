//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundChangeMoneysDlg.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 12. Thursday
//-----------------------------------------------------------------------------------------------------
#ifndef __RYL_BATTLEGROUND_CHANGE_MONEY_DLG_H_
#define __RYL_BATTLEGROUND_CHANGE_MONEY_DLG_H_

#include "RYLDialog.h"

class CRYLLabel;
class CRYLEdit;
class CRYLButton;
class CRYLStringGrid;

//-----------------------------------------------------------------------------------------------------
// namespace for define control's ID
//-----------------------------------------------------------------------------------------------------
namespace RYL_BATTLEGROUND_CHANGE_MONEY_DLG
{
    const   DWORD       SERVER_LIST_STRING_GRID     =   0x00000000;
    const   DWORD       INPUT_MEDAL_VALUE_EDIT      =   0x00000001;
    const   DWORD       CHANGE_MONEY_ACCEPT_BUTTON  =   0x00000002;
    const   DWORD       CHANGE_MONEY_CANCEL_BUTTON  =   0x00000003;
};

//-----------------------------------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
// class
//-----------------------------------------------------------------------------------------------------
class CRYLBattleGroundChangeMoneyDlg : public CRYLDialog
{
private:
                // controls
                CRYLLabel*      m_pDialogNameLabel;             // 'ȯ����'
                CRYLLabel*      m_pServerListLabel;             // '��������'
                CRYLLabel*      m_pSelectedServerValue;         // ���õ� ����
                CRYLLabel*      m_pSelectedServerLabel;         // '���� ����â��ݾ�'
                CRYLLabel*      m_pCurrentMoneyValue;           // ���õ� ���� â�� �ݾ� ��
                CRYLLabel*      m_pCurrentMedalLabel;           // '���� �޴�'
                CRYLLabel*      m_pCurrentMedalValue;           // ���� �޴� ��
                CRYLLabel*      m_pSelectMedalLabel;            // 'ȯ���� �޴�'
                CRYLLabel*      m_pChangedMoneyLabel;           // '�߰� �ݾ�(ȯ���޴� * 10Gold)'
                CRYLLabel*      m_pChangedMoneyValue;           // �߰� �ݾ�

                CRYLStringGrid* m_pServerListStringGrid;        // ���� ����Ʈ

                CRYLButton*     m_pAcceptChangeButton;          // 'ȯ���ϱ�' ��ư
                CRYLButton*     m_pCancelChangeButton;          // '���' ��ư

public:
                CRYLEdit*       m_pInputMedalValueEdit;         // ȯ���� �޴� �� �Է�

private:
                unsigned char   m_cGroup;
                unsigned long   m_dwGold;
                unsigned long   m_dwMileage;

				unsigned char	m_cRequestCount;				// ���� ���� ��û ī��Ʈ
	
public:
                // constructor, destructor
                    	        CRYLBattleGroundChangeMoneyDlg();
	virtual                     ~CRYLBattleGroundChangeMoneyDlg();

public:
                // concerned with interface control
	            VOID            InitCtrl();
	            VOID            FinalCtrl();

	virtual     VOID            Show( CGMUIObject* Sender );
	virtual     VOID            Hide( CGMUIObject* Sender );

	virtual     VOID            GMUIPaint( INT nPosX, INT nPosY );

	virtual	    HRESULT		    Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK; };
	virtual     HRESULT		    FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK; }
    virtual     HRESULT		    Update( BOOL &bClick, BOOL &bEdge );

	static      VOID	        DlgClick( CGMUIObject* Sender, INT X, INT Y );
    static      VOID            DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift );

public:
                VOID            SelectServer();
                VOID            SetValue( unsigned char cGroup, unsigned long dwGold, unsigned long dwMileage );
                VOID            SetChangeMoneyValue();
                VOID            AcceptMileage();

                VOID            SetAcceptButtonValidate( bool bValidate );
};      

#endif //__RYL_BATTLEGROUND_CHANGE_MONEY_DLG_H_