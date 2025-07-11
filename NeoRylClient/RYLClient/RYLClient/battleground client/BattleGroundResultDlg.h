//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundResultsDlg.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 18. Wednesday
//-----------------------------------------------------------------------------------------------------
#ifndef __RYL_BATTLEGROUND_RESULT_DLG_H_
#define __RYL_BATTLEGROUND_RESULT_DLG_H_

#include "RYLDialog.h"

class CRYLImage;

namespace RYL_BATTLEGROUND_RESULT_DLG
{
};

//-----------------------------------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------------------------------
typedef struct GameResult
{
    enum        { MAX_RACE = 2 };

    unsigned char	m_cWinRace;						// 승자 (CClass::MAX_RACE 이면 무승부)
    unsigned char	m_cPlayMin;						// 실제 플레이한 시간
    unsigned char	m_cKill;						// 상대방을 죽인 수 
	unsigned char	m_cKilled;						// 죽은 수
    unsigned short	m_wAward;						// 보상으로 받은 마일리지
	unsigned short	m_wScore[ MAX_RACE ];	        // 점수

    GameResult::GameResult()
    {
        // 초기값은 무승부, 0 : 0
        m_cWinRace	= 2;
        m_cPlayMin  = 0;
        m_cKill     = 0;
        m_cKilled   = 0;
        m_wAward    = 0;
        ZeroMemory( m_wScore, sizeof( unsigned short ) * MAX_RACE );
    }

    GameResult::GameResult( const GameResult& result )
    {
        m_cWinRace	= result.m_cWinRace;
        m_cPlayMin  = result.m_cPlayMin;
        m_cKill     = result.m_cKill;
        m_cKilled   = result.m_cKilled;
        m_wAward    = result.m_wAward;

        for( int i = 0 ; i < MAX_RACE ; ++i )
        {
            m_wScore[ i ] = result.m_wScore[ i ];
        }
    }

} GameResult, *LPGameResult;

//-----------------------------------------------------------------------------------------------------
// class
//-----------------------------------------------------------------------------------------------------
class CRYLBattleGroundResultDlg : public CRYLDialog
{
private:
                // number images
                CRYLImage*      m_pBackGroundImage;
                CRYLImage*      m_pWhiteNumberImage[10];

                // image positions
                POINT           m_ptFinalScore1;
                POINT           m_ptFinalScore2;
                POINT           m_ptPlayTime;
                POINT           m_ptMerits;
                POINT           m_ptKill;
                POINT           m_ptKilled;

                // game result info
                GameResult      m_GameResult;
	
public:
                    	    CRYLBattleGroundResultDlg();
	virtual                 ~CRYLBattleGroundResultDlg();

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
                VOID        SetGameResult( GameResult* pGameResult )            { m_GameResult = *pGameResult; }

private:
                VOID        DrawNumber( INT n100, INT n10, INT n1, POINT ptPos );
};


#endif //__RYL_BATTLEGROUND_RESULT_DLG_H_