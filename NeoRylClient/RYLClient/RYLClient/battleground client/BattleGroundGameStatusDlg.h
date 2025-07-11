//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundGameStatussDlg.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 12. Thursday
//-----------------------------------------------------------------------------------------------------
#ifndef __RYL_BATTLEGROUND_GAME_STATUS_DLG_H_
#define __RYL_BATTLEGROUND_GAME_STATUS_DLG_H_

#include "RYLDialog.h"
#include <string>
using namespace std;

class CRYLImage;

//-----------------------------------------------------------------------------------------------------
// namespace for define control's ID
//-----------------------------------------------------------------------------------------------------
namespace RYL_BATTLEGROUND_GAME_STATUS_DLG
{
};

//-----------------------------------------------------------------------------------------------------
// struct
//-----------------------------------------------------------------------------------------------------
typedef struct GameInfo
{
    enum        { MAX_RACE = 2 };

    unsigned    char	m_cMapType;							// 게임 방식( 스코어 경쟁: 0, 점령전: 1 )
	unsigned    char	m_cRemainMin;						// 남은 시간 (분)
	unsigned    short	m_wTargetScore;						// 목표 점수
	unsigned    char	m_cCurrentCharNum[ MAX_RACE ];		// 현재 인원
	unsigned    short	m_wScore[ MAX_RACE ];			    // 점수

    GameInfo::GameInfo()
    {
        m_cMapType              = 0;
        m_cRemainMin            = 0;			
        m_wTargetScore          = 0;
        ZeroMemory( m_cCurrentCharNum, sizeof( unsigned char ) * MAX_RACE );
        ZeroMemory( m_wScore, sizeof( unsigned short ) * MAX_RACE );
    }

    GameInfo::GameInfo( const GameInfo& info )
    {
        m_cMapType              = info.m_cMapType;	
        m_cRemainMin            = info.m_cRemainMin;			
        m_wTargetScore          = info.m_wTargetScore;
        memcpy( m_cCurrentCharNum, info.m_cCurrentCharNum, sizeof( unsigned char ) * MAX_RACE );
        memcpy( m_wScore, info.m_wScore, sizeof( unsigned short ) * MAX_RACE );
    }

} GameInfo, *LPGameInfo;

//-----------------------------------------------------------------------------------------------------
// class
//-----------------------------------------------------------------------------------------------------
class CRYLBattleGroundGameStatusDlg : public CRYLDialog
{
private:
                // number images
                CRYLImage*      m_pWhiteNumberImage[80];
                CRYLImage*      m_pRedNumberImage[80];

                // image positions
                POINT           m_ptHumanScore;
                POINT           m_ptTime;
                POINT           m_ptAkhanScore;

                // for 'NUMBER' animation
                // 각 자리가 갖고 있는 숫자
                int             m_nTargetHumanScoreNumber[ 3 ];
                int             m_nTargetTimeNumber[ 2 ];
                int             m_nTargetAkhanScoreNumber[ 3 ];

                string          m_queTargetHumanScoreNumber[ 3 ];
                string          m_queTargetTimeNumber[ 2 ];
                string          m_queTargetAkhanScoreNumber[ 3 ];

                int             m_nCurrentHumanScoreNumber[ 3 ];
                int             m_nCurrentTimeNumber[ 2 ];
                int             m_nCurrentAkhanScoreNumber[ 3 ];

private:
                GameInfo        m_GameInfo;

public:
                    	        CRYLBattleGroundGameStatusDlg();
	virtual                     ~CRYLBattleGroundGameStatusDlg();

public:

	            VOID            InitCtrl();
	            VOID            FinalCtrl();

	virtual     VOID            Show( CGMUIObject* Sender );
	virtual     VOID            Hide( CGMUIObject* Sender );

	virtual     VOID            GMUIPaint( INT nPosX, INT nPosY );

	virtual	    HRESULT		    Render( LPDIRECT3DDEVICE8 lpD3DDevice )         { return S_OK; };
	virtual     HRESULT		    FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice )    { return S_OK; }
    virtual     HRESULT		    Update( BOOL &bClick, BOOL &bEdge );

	static      VOID	        DlgClick( CGMUIObject* Sender, INT X, INT Y );

public:
                VOID            ChangeGameStatus( GameInfo* pGameInfo );

//public:
//    unsigned    char            GetGameType()                                   { return m_GameInfo.m_cMapType; }

private:
                // draw number
                VOID            DrawNumber();
};

#endif //__RYL_BATTLEGROUND_GAME_STATUS_DLG_H_