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

    unsigned    char    m_ucChannelNum;                     // ä�� ��ȣ

                bool    m_bPlaying;                         // ���� ������ ����
    unsigned    char	m_cMapType;							// ���� ���( ���ھ� ����: 0, ������: 1 )
	unsigned    char	m_cMaxCharNumOfNation;				// �� ���� �ִ� �ο�
	unsigned    char	m_cRemainMin;						// ���� �ð� (��)
	unsigned    short	m_wTargetScore;						// ��ǥ ����
	unsigned    char	m_cCurrentCharNum[ MAX_RACE ];		// ���� �ο�
	unsigned    short	m_wScore[ MAX_RACE ];			    // ����

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
                CRYLLabel*      m_pSortFormLabel;               // '���Ĺ��'
                CRYLLabel*      m_pSortFormValue;               // ���Ĺ�� ��

                // channel list frame
                CRYLLabel*      m_pCHANNEL;                     // 'ä��'
                CRYLLabel*      m_pGAMETYPE;                    // '���ӹ��'
                CRYLLabel*      m_pMAXPEOPLE;                   // '�ִ��ο�'
                CRYLLabel*      m_pREMAINTIME;                  // '�����ð�'
                CRYLStringGrid* m_pChannelListStringGrid;       // ä�� ����Ʈ

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