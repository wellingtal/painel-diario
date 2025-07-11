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
                CRYLLabel*      m_pDialogNameLabel;             // '환전소'
                CRYLLabel*      m_pServerListLabel;             // '서버군명'
                CRYLLabel*      m_pSelectedServerValue;         // 선택된 서버
                CRYLLabel*      m_pSelectedServerLabel;         // '서버 현재창고금액'
                CRYLLabel*      m_pCurrentMoneyValue;           // 선택된 서버 창고 금액 값
                CRYLLabel*      m_pCurrentMedalLabel;           // '현재 메달'
                CRYLLabel*      m_pCurrentMedalValue;           // 현재 메달 값
                CRYLLabel*      m_pSelectMedalLabel;            // '환전할 메달'
                CRYLLabel*      m_pChangedMoneyLabel;           // '추가 금액(환전메달 * 10Gold)'
                CRYLLabel*      m_pChangedMoneyValue;           // 추가 금액

                CRYLStringGrid* m_pServerListStringGrid;        // 서버 리스트

                CRYLButton*     m_pAcceptChangeButton;          // '환전하기' 버튼
                CRYLButton*     m_pCancelChangeButton;          // '취소' 버튼

public:
                CRYLEdit*       m_pInputMedalValueEdit;         // 환전할 메달 값 입력

private:
                unsigned char   m_cGroup;
                unsigned long   m_dwGold;
                unsigned long   m_dwMileage;

				unsigned char	m_cRequestCount;				// 정섭 정보 요청 카운트
	
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