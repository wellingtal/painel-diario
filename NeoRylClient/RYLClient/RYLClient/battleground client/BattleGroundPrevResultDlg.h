//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundPrevResultsDlg.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 4. Wednesday
//-----------------------------------------------------------------------------------------------------
#ifndef __RYL_BATTLEGROUND_PREV_RESULT_DLG_H_
#define __RYL_BATTLEGROUND_PREV_RESULT_DLG_H_

#include "RYLDialog.h"

class CRYLLabel;

namespace RYL_BATTLEGROUND_PREV_RESULT_DLG
{
};

//-----------------------------------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------------------------------
typedef struct PrevChannelResult
{
    enum        { MAX_RACE = 2 };

    unsigned char	m_cWinRace;						// 승자 (CClass::MAX_RACE 이면 무승부)
	unsigned short	m_wScore[ MAX_RACE ];	        // 점수

    PrevChannelResult::PrevChannelResult()
    {
        // 초기값은 무승부, 0 : 0
        m_cWinRace = 2;
        ZeroMemory( m_wScore, sizeof( unsigned short ) * MAX_RACE );
    }

    PrevChannelResult::PrevChannelResult( const PrevChannelResult& result )
    {
        m_cWinRace = result.m_cWinRace;

        for( int i = 0 ; i < MAX_RACE ; ++i )
        {
            m_wScore[ i ] = result.m_wScore[ i ];
        }
    }

} PrevChannelResult, *LPPrevChannelResult;

//-----------------------------------------------------------------------------------------------------
// class
//-----------------------------------------------------------------------------------------------------
class CRYLBattleGroundPrevResultDlg : public CRYLDialog
{
private:
    enum        { MAX_CHANNEL_NUMBER = 8 };

                // controls
                CRYLLabel*	m_pDlgNameLabel;				                // '각 채널 승패 내용'

                CRYLLabel*	m_pChannelNumberLabel[ MAX_CHANNEL_NUMBER ];    // 채널 번호
                CRYLLabel*	m_pChannelResultLabel[ MAX_CHANNEL_NUMBER ];    // 채널 결과
private:    
    PrevChannelResult       m_pPrevChannelResult[ MAX_CHANNEL_NUMBER ];
    
	
public:
                    	    CRYLBattleGroundPrevResultDlg();
	virtual                 ~CRYLBattleGroundPrevResultDlg();

public:

	            VOID        InitCtrl();
	            VOID        FinalCtrl();

	virtual     VOID        Show( CGMUIObject* Sender );
	virtual     VOID        Hide( CGMUIObject* Sender );

	virtual     VOID        GMUIPaint( INT xx, INT yy );

	virtual	    HRESULT		Render( LPDIRECT3DDEVICE8 lpD3DDevice )  { return S_OK; };
	virtual     HRESULT		FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) { return S_OK; }
    virtual     HRESULT		Update( BOOL &bClick, BOOL &bEdge );

	static      VOID	    DlgClick( CGMUIObject* Sender, INT X, INT Y );

public:
                VOID        ChangePrevChannelResult( unsigned char ucChannelNum, PrevChannelResult* pChannelResult );
};


#endif //__RYL_BATTLEGROUND_PREV_RESULT_DLG_H_