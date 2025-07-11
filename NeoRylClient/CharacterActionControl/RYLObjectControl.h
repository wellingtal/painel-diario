// CharacterControl.h: interface for the CCharacterControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARACTERCONTROL_H__E5DF7481_5D2B_4E51_B47A_951185AFB01D__INCLUDED_)
#define AFX_CHARACTERCONTROL_H__E5DF7481_5D2B_4E51_B47A_951185AFB01D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TARGET_SELF			10001
#define TARGET_SELECTCHAR	10000

#include "Z3DGeneralChrModel.h"
#include "SceneManager.h"
#include "RYLCreatureCommon.h"
#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "CreatureSize.h"
#include "CameraControl.h"
#include "..\\Effect\\CEffscript.h"
#include <Skill/Spell/SpellKind.h>


//===============================================================================
// ������� 
class CRYLCharacterControl ;
class CRYLShortAttackWeaponControl ;
class CRYLLongAttackWeaponControl ;
class CRYLShortDefenceWeaponControl ;
class CRYLLongDefenceWeaponControl ;
class CRYLDropshipControl ;

#define ATTACKABLE_RANGE 150.0f
#define TARGETNEAR_RANGE 100.0f
#define PLAYERTARGETNEAR_RANGE 1000.0f

enum CONTROL_KIND
{
	CHARACTER_CONTROL = 0,				// ĳ���� ��Ʈ��
	LONG_ATTACKWEAPON_CONTROL = 1,		// ���Ÿ� �������� ��Ʈ��
	SHORT_ATTACKWEAPON_CONTROL = 2,		// �ٰŸ� �������� ��Ʈ��
	LONG_DEFENCEWEAPON_CONTROL = 3,		// ���Ÿ� �������� ��Ʈ��
	SHORT_DEFENCEWEAPON_CONTROL = 4,	// �ٰŸ� �������� ��Ʈ��
	DROPSHIP_CONTROL = 5,				// ���ۼ� ��Ʈ��
} ;

extern int MAXCHRLOAD;

unsigned char KeyPressed(unsigned char key);
bool		CylinderOBBIntersection( vector3 vecPos, vector3 vecDir, float fArcAngle,vector3 vecMax, vector3 vecMin, float fRad );

class CCharacterControl  
{
public :
	class CAttackedNode
	{
	public :
		DWORD	m_dwSrcChrID ;
		DWORD	m_dwTarChrID ;
		DWORD	m_dwWeaponType ;
	};

	class ChrCashNode
	{
	public :
		CZ3DGeneralChrModel* m_lpChrModel ;	
		CHAR	m_strGCMDSName[ MAX_PATH ] ;		
		INT		m_nUsedChrID;
	};

	BOOL					m_bSkillExecuted ;					// ��ų ���� ����
	DWORD					m_dwStartSkillTick ;				// ��ų �ߵ�( ���� ) Tick
	DWORD					m_dwCastingSkillTick ;				// ��ų �ɽ��� Tick
	DWORD					m_dwCastingSkillGrade ;				// ��ų �׷��̵�

	// ------------------------------------------------
    // ĳ���� ����Ʈ( �����丵 ���� ���� ��� )

    vector3					m_vecPrePosition ;					// �ڽ��� ĳ���� ���� ��ġ(NPC �Ÿ� ����)
	BOOL					m_bAttackable ;						// ���� ����
	DWORD					m_dwMaxCombo ;						// �ڽ��� �� �޺���

	// ------------------------------------------------
	// ��ȯ�� ���� ���

	BOOL					m_bSummon ;							// ��ȯ���� �ƴ���
	BOOL					m_bSummonCmd ;						// ��ȯ���� ����� ������ ������ �ƴ���
	unsigned char			m_cSummonCmd ;						// ��ȯ���� ��� Ŀ�ǵ�

	CEffScript*				m_lpDuelReady ;						// Duel �����϶� Ready Effect		
	BOOL					m_bComboSuccess ;					// ��ų ���� ����

	map< unsigned long, unsigned char >	m_lstPreDuelTargetID ;	// �������� Fight�޽����� �ö����� ���⿡ ����� �Ǿ� �ִٰ�, (ID, VSType)
	map< unsigned long, unsigned char >	m_lstDuelTargetID ;		// Fight�޽����� ������ PreDuelTargetID�� ���⿡ ����Ѵ�. (ID, VSType)

	DWORD					m_dwRunFactorTimer ;				// Run Ÿ��. m_dwRunFactorTimer = CFrameTimer::Regist(200.0f);
	DWORD					m_dwNormalTargetMonsterID ;			// Target�� MONSTER ID
	DWORD					m_dwNormalTargetID ;				// Target�� PC ID
	DWORD					m_dwSpecialTargetID ;				// SpecialTarget ID
	BOOL					m_bAttackResult ;					// ���� ���� ����
	BOOL					m_bAutoFollowMode ;					// AutoFollowMode ����
	DWORD					m_dwAutoFollowChrID ;				// ��� ĳ���� ���󰡱� ����

	float					m_fRunSpeed ;						// Run Speed
	float					m_fWalkSpeed ;						// Walk Speed
	float					m_fSkillLength ;					// Skill Length
	float					m_fCameraSpeed ;					// Camera Speed

	BOOL					m_bFreelook ;						// /Neo ������� Freelook ����
	vector3					m_vecCameraPos ;					// ī�޶� ��ġ
	float					m_fCameraDir ;						// ī�޶� ����

	int						m_nCharMovingPacketTimerID ;		// UDP�� ���������� �����ӿ����� ��Ŷ�� �߼۽ð�
	int						m_SkillAttackValue ;				// Skill �� ���� ������ Ÿ�� ����Ʈ�� �ٲ��� �����ϴ� �÷��� 

	unsigned long			m_dwSlideTick ;						// �����̵� ƽ
	int						m_MouseClickMode ;					// ���콺 Ŭ�� ���( �������� )
	int						m_nCameraAnimation ;				// ī�޶� �ִϸ��̼�
	BOOL					m_bCharacterTargetNear ;			// Character Target Near			
	DWORD					m_nSelectItem ;						// ���õ� Item
	bool					m_bSkillAttack ;					// ��ų ���� ����

	bool					m_bGoodChantEffectShow[4] ;			// GooodChant Effect Show ����		��Ƽ, �Ʊ�, �߸�, ����
	bool					m_bBadChantEffectShow[4] ;			// BadChant Effect Show ����		��Ƽ, �Ʊ�, �߸�, ����

	long					m_lAutoRunCounter ;					// AutoRun()�� Ȱ����Ű�� ���� ī����
	unsigned long			m_dwAutoTargetID ;					// Auto Target ID
	bool					m_bAutoTargetAttack ;				// Auto Target ����

	vector3					m_vecClickPos ;						// click pos
	bool					m_bClickMove ;						// Ŭ�� �̵� ����
	bool					m_bClickAttack ;					// Ŭ�� ���� ����
	DWORD					m_dwClickModeAttackTargetID ;		// ���콺��忡�� ���� Ÿ�� ID
	std::vector<CAttackedNode>		m_AttackedList ;					// ���ݳ�� ����Ʈ

	bool					m_bClientUsed ;						// Ŭ���̾�Ʈ ��� ����
	CCameraControl			m_Camera ;							// ī�޶� ��Ʈ��
	CCreatureSize			m_CreatureSize ;					// ĳ���� ������

	CTexture*				m_SimpleHumanTexture[ 4 ] ;


public:
	
	CCharacterControl();
	virtual ~CCharacterControl();
	
	// ------------------------------------------------
	// DuelTarget
	
	void			SetPreDuelTarget( unsigned long dwDuelTargetID, unsigned char cVSType ) ;
	void			SetDuelTarget( void ) ;
	void			DeleteDuelTarget( unsigned long dwDuelTargetID ) ;
	void			InitDuelTarget( void ) ;
	unsigned char	GetIsDuelTarget( unsigned long dwChrID ) ;


	char*		GetCreatureName( unsigned long dwChrID ) ;
	void		InitGlobalAction( void ) ;
	BOOL		GetAttacking( void ) ;
	BOOL		CheckMonsterForward( void ) ;
	void		MakeVisualAttackInfo() ;
	void		MakeAttackInfo( DWORD dwEnemyChrID ) ;
	void		PlaySound( int nMethod,vector3 vecPos ) ;
	
	void		PlayerProcessOperation( BOOL bKeyAble ) ;
	void		SetDirectionforMouse( vector3 &vecDirection ) ;

	void		CheckCameraShakeTiming() ;
	void		CheckAttackedTiming() ;
	
	void		RegistAttacked( DWORD dwSrcChrID,DWORD dwTarChrID, unsigned long dwWeaponType = 0 ) ;
	void		MakeAttacked( DWORD dwChrID, unsigned long dwWeaponType ) ;
	bool		MakeSkillAttackInfo( unsigned long dwTargetID, unsigned short wSkill = 0, 
		                             unsigned char cSkillLock = 0, unsigned char cSkillLevel = 0, 
									 unsigned char cAtCount = 1, BOOL bGod = TRUE, BOOL bDead = FALSE ) ;
	vector3*	GetFindChrPos( unsigned long dwChrID ) ;
	void		MouseModeChange() ;
	void		SetCameraToCharDir() ;

	////////////// Network Bind Function ///////////////
	static vector3	( *GetMonsterMinBox )			( unsigned long dwChrID ) ;
	static vector3	( *GetMonsterMaxBox )			( unsigned long dwChrID ) ;
	static bool		( *IsExistToList )				( unsigned short List_In, DWORD CharID_In ) ;
	static bool		( *CharAddressRequireInfo )		( DWORD SenderID_In, DWORD CharID_In ) ;
	static BOOL		( *SendMovingPacket )			( unsigned long dwUpperAni, unsigned long dwLowerAni, float fDir, vector3 &vecMove ) ;
	static BOOL		( *SendCharRespawn )			( unsigned long dwChrID ) ;
	static BOOL		( *SendMoveUpdatePacket )		( float fDir, vector3 &vecPosition ) ;
	static BOOL		( *SendCharAttack )				( vector3 &vecPos, float fDir, unsigned short wType, bool bCasting, 
		                                              unsigned char cSkillLock, unsigned char cSkillLevel, 
													  unsigned char cAtCount, unsigned short wDefenserNum, 
													  unsigned long dwDefenser[ 10 ],
													  bool bMissileAttack, vector3 &vecDstPos ) ;
	static bool		( *PickItem )					( DWORD nItemID ) ;
	static BOOL		( *CheckAttackable )			( void ) ;
	static BOOL		( *SendSummonAttack )			( unsigned long dwTargetID ) ;
	static void		( *SetChangeWeapon )			( BOOL bChangeWeapon ) ;
	static void		( *SetAutoRun )					( BOOL bAutoRun ) ;
	static BOOL		( *GetAutoRun )					( void ) ;
	static char*	( *GetMotionSheet )				( char *strWeaponName, char *strShieldName, unsigned short wClass ) ;
	static unsigned long ( *GetWeaponType )			( char *strWeaponName ) ;
	static void		( *CheckTargetLength )			( unsigned long &dwTargetID ) ;
	static void		( *CheckTargetforSkill )		( unsigned long &dwTargetID, BOOL bCheckLength ) ;
	static BOOL		( *CheckSkillStart )			( BOOL bDown ) ;
	static BOOL		( *CheckStillCasting )			( unsigned long &dwFunction ) ;
	static void		( *ChangeWeapon )				( void ) ;
	static void		( *SetKillCounter )				( long lKillCounter ) ;
	static void		( *SetClientExitCounter )		( long lExitCounter ) ;
	static void		( *SetLogoutCounter )			( long lLogoutCounter ) ;
	static void		( *SetPortalCounter )			( long lPortalCounter ) ;
	static void		( *SetSkillUseCounter )			( long lSkillUseCounter ) ;
	static unsigned long ( *GetClientIdentity )		( unsigned long dwChrID ) ;
	static unsigned long ( *GetClientEnemyType )	( unsigned long dwChrID) ;
	static unsigned short ( *GetEnchantLevel )		( unsigned long lID );
	static unsigned long ( *GetEnchantTime )		( unsigned long lID );
	static unsigned short ( *GetSkillIDbyChantID )	( unsigned char cChant );
	static void		( *UpdateCamp )					( RYLCreature* pCreature );

	void		WrapSendCharAttack( vector3 &vecPos, float fDir, unsigned short wAtType, bool bCasting, 
									unsigned char cSkillLock, unsigned char cSkillLevel, unsigned char cAtCount, 
									unsigned short wDefenserNum, unsigned long  lpAtNode[ 10 ],
									bool bMissileAttack = false, vector3 &vecDstPos = vector3( 0.0f, 0.0f, 0.0f ) ) ;
	void		WrapSendMoveUpdatePacket( float fDir,vector3 vecPos ) ;
	BOOL		WrapSendCharRespawn( DWORD dwChrID ) ;
	void		WrapSendMovingPacket( DWORD dwUpperAction,DWORD dwLowerAction,float fDir,vector3 vecPos ) ;
	
	unsigned long	GetClientSummon( unsigned long dwChrID, BOOL bNation = FALSE, BOOL bForSkill = FALSE ) ;


private:

	void        UpdateMonster( RYLCreature* pCreature, float fUpdateTime );
    void        UpdatePlayer( RYLCreature* pCreature, float fUpdateTime );
    void		UpdateMonster( unsigned long cChr, float fUpdateTime ) ;
	void		UpdatePlayer( unsigned long cChr, float fUpdateTime ) ;


public:

	BOOL		GetIsExistChar( unsigned long dwChrID ) ;
	void		AddActionData( unsigned long dwChrID, unsigned long dwUpperAni, 
							   unsigned long dwLowerAni, float fDirection, 
							   vector3 &vecNextPosition, float fVec = 0.0f, 
							   unsigned short wAniNum = 0, unsigned long dwFrame = 0 ) ;
	void		Create( void ) ;
	void		DeleteAllCharacter( void ) ;

	void		SetCamera( int dx = 0, int dy = 0, int dz = 0 ) ;
	void		MakeShake( int nChrID ) ;
	bool		MakeAttackInfo( unsigned short wSkill = 0, unsigned char cSkillLock = 0, 
		                        unsigned char cSkillLevel = 0, unsigned char cAtCount = 1, 
								float fAttackAngle = 0.0f, float fAttackRate = 0.0f, 
								BOOL bAngle = TRUE, BOOL bGod = TRUE ) ;

	void		MouseMove( int MouseX, int MouseY ) ;
	unsigned long GetScreenPosChr( POINT &Point, int nMode = 0, float fLength = 0 ) ;

	void		SelfCharacterUpdate() ;

public :

	// ------------------------------------------------
	// ��������

	CRYLCharacterControl*			m_pRYLCharacterControl ;
	CRYLShortAttackWeaponControl*	m_pRYLShortAttackWeaponControl ;
	CRYLLongAttackWeaponControl*	m_pRYLLongAttackWeaponControl ;
	CRYLShortDefenceWeaponControl*	m_pRYLShortDefenceWeaponControl ;
	CRYLLongDefenceWeaponControl*	m_pRYLLongDefenceWeaponControl ;
	CRYLDropshipControl*			m_pRYLDropshipControl ;	

	// ���� ������Ʈ�� ����
	CONTROL_KIND					m_WeaponControlKind ;
 
	VOID		    SetWeaponControlKind( CONTROL_KIND Control ) ;
	CONTROL_KIND    GetWeaponControlKind()									{ return m_WeaponControlKind ; }
	VOID		    ChangeSiegeModel( unsigned long dwChrID, const char *strName, vector3 vecChrPos, float fDirection, const char* szType ) ;
	VOID		    UpdateCreatureControl( BOOL bKeyAble = TRUE, BOOL bEdge = FALSE, int MouseX = 0,int MouseY = 0 ) ;

	void		    Update( float fUpdateTime ) ;
	void		    Render( LPDIRECT3DDEVICE8 pd3dDevice ) ;

	void		    SwapElement( RYLCreature* pNode1, RYLCreature* pNode2 ) ;

	HRESULT		    SwitchModel( unsigned long dwCID,
							     unsigned short wObjectType,
							     unsigned char cState,
							     unsigned char cUpgradeType,
							     unsigned char cUpgradeStep ) ;

	VOID		    SetNationToCastleNPC( unsigned long dwCastleID, unsigned char cNation ) ;
	VOID		    CharRespawn() ;
} ;

#endif // !defined(AFX_CHARACTERCONTROL_H__E5DF7481_5D2B_4E51_B47A_951185AFB01D__INCLUDED_)
