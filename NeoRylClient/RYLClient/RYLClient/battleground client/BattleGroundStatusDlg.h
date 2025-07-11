//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundStatusDlg.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 3. Tuesday
//-----------------------------------------------------------------------------------------------------
#ifndef __RYL_BATTLEGROUND_STATUS_DLG_H_
#define __RYL_BATTLEGROUND_STATUS_DLG_H_

#include "RYLDialog.h"
#include <list>

class CRYLLabel;
class CRYLButton;
class CRYLStringGrid;

namespace RYL_BATTLEGROUND_STATUS_DLG
{
    const DWORD     CHANNEL_STRINGGRID          = 0x00000000;    
    const DWORD     CHANNEL_JOIN_PLAYER         = 0x00000001;
    const DWORD     CHANNEL_JOIN_SPECTATOR      = 0x00000002;
};

//-----------------------------------------------------------------------------------------------------
// structor
//-----------------------------------------------------------------------------------------------------
typedef struct ChannelInfo
{
    enum        { MAX_RACE = 2 };

    unsigned    char    m_ucChannelNum;                     // 채널 번호

                bool    m_bPlaying;                         // 게임 중인지 여부
    unsigned    char	m_cMapType;							// 게임 방식( 스코어 경쟁: 0, 점령전: 1 )
	unsigned    char	m_cMaxCharNumOfNation;				// 한 진영 최대 인원
	unsigned    char	m_cRemainMin;						// 남은 시간 (분)
	unsigned    short	m_wTargetScore;						// 목표 점수
	unsigned    char	m_cCurrentCharNum[ MAX_RACE ];		// 현재 인원
	unsigned    short	m_wScore[ MAX_RACE ];			    // 점수

    ChannelInfo::ChannelInfo()
    {
        m_ucChannelNum          = 0;

        m_bPlaying              = false;
        m_cMapType              = 0;	
        m_cMaxCharNumOfNation   = 0;
        m_cRemainMin            = 0;			
        m_wTargetScore          = 0;
        ZeroMemory( m_cCurrentCharNum, sizeof( unsigned char ) * MAX_RACE );
        ZeroMemory( m_wScore, sizeof( unsigned short ) * MAX_RACE );
    }

    ChannelInfo::ChannelInfo( const ChannelInfo& info )
    {
        m_ucChannelNum          = info.m_ucChannelNum;

        m_bPlaying              = info.m_bPlaying;
        m_cMapType              = info.m_cMapType;	
        m_cMaxCharNumOfNation   = info.m_cMaxCharNumOfNation;
        m_cRemainMin            = info.m_cRemainMin;			
        m_wTargetScore          = info.m_wTargetScore;
        memcpy( m_cCurrentCharNum, info.m_cCurrentCharNum, sizeof( unsigned char ) * MAX_RACE );
        memcpy( m_wScore, info.m_wScore, sizeof( unsigned short ) * MAX_RACE );
    }

} ChannelInfo, *LPChannelInfo;

//-----------------------------------------------------------------------------------------------------
// class CRYLBattleGroundStatusDlg
//-----------------------------------------------------------------------------------------------------
class CRYLBattleGroundStatusDlg : public CRYLDialog
{
private:
                // controls
                CRYLLabel*      m_pSortFormLabel;               // '정렬방식'
                CRYLLabel*      m_pSortFormValue;               // 정렬방식 값

                // channel list frame
                CRYLLabel*      m_pCHANNEL;                     // '채널'
                CRYLLabel*      m_pGAMETYPE;                    // '게임방식'
                CRYLLabel*      m_pMAXPEOPLE;                   // '최대인원'
                CRYLLabel*      m_pREMAINTIME;                  // '남은시간'
                CRYLStringGrid* m_pChannelListStringGrid;       // 채널 리스트

                // channel info frame
                CRYLLabel*      m_pGameTypeLabel;               
                CRYLLabel*      m_pGameTypeValue;

                CRYLLabel*      m_pTimeInfoLabel;
                CRYLLabel*      m_pTimeInfoValue;

                CRYLLabel*      m_pMaxPeopleLabel;
                CRYLLabel*      m_pMaxPeopleValue;

                CRYLLabel*      m_pTargetLabel;
                CRYLLabel*      m_pTargetValue;

                CRYLLabel*      m_pCurrentPeopleLabel;
                CRYLLabel*      m_pCurrentPeopleValue;

                CRYLLabel*      m_pScoreLabel;
                CRYLLabel*      m_pScoreValue;

                // join buttons
                CRYLButton*     m_pJoinPlayerButton;
                CRYLButton*     m_pJoinSpectatorButton;

private:
                // member variables
                DWORD           m_dwJoinType;
                DWORD           m_dwJoinChNumber;
    std::list<ChannelInfo*>     m_ltChannelInfo;

                BOOL            m_bIsPlayer;

public:
                // constructor, destructor
                          	    CRYLBattleGroundStatusDlg();
	virtual                     ~CRYLBattleGroundStatusDlg();

public:

                // concerned with interface control
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
                VOID            ChangeChennelStatus( int nIndex, ChannelInfo* pChannelInfo );

public:
                VOID            SelectChannel();
                VOID            ClearChannelInfo();
                VOID            CheckChannelStart( int nIndex, bool bPlaying );

                BOOL            GetPlayerState()                                { return m_bIsPlayer; }

private:
                VOID            UpdateChannelListStringGrid();
                VOID            UpdateSelectedChannelInfo( int nIndex );

                VOID            JoinChannel( bool bIsPlayer );
};

#endif //__RYL_BATTLEGROUND_STATUS_DLG_H_