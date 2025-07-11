//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreature.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: base class for Creature
//
// Date: 2004. 3. 25. ( Tue )
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __RYL_CREATURE_H__
#define __RYL_CREATURE_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------

#include <string>   

#include <Network/Broadcast/BroadcastCharacterData.h>
#include <Creature/Siege/SiegeConstants.h>
#include <Item/ItemConstants.h>

#include "Z3DGeneralChrModel.h"
#include "Z3DBoundingVolumeObject.h"
#include "SceneManager.h"

#include "RYLCreatureEffect.h"
#include "RYLDamageEffect.h"

//#include "RYLCreatureCommon.h"

using namespace std;

class CRYLLabel ;
class CRYLListBox ;
class CRYLPicture ;
class CCharacterControl ;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
typedef struct CharActionNode
{
	unsigned long   m_ulLowerChrAction;
	unsigned long   m_ulUpperChrAction;
	float           m_fDirection;
	vector3         m_vecNextPosition;
	float           m_fVec;
	unsigned short  m_usAniNum;
	unsigned short  m_usAniNumCount;
	unsigned long   m_ulFrame;

    CharActionNode()
    {
        m_ulLowerChrAction  = 0;
        m_ulUpperChrAction  = 0;
        m_fDirection        = 0;

        m_vecNextPosition.x = 0;
        m_vecNextPosition.y = 0;
        m_vecNextPosition.z = 0;

        m_fVec              = 0;
        m_usAniNum          = 0;
        m_usAniNumCount     = 0;
        m_ulFrame           = 0;
    }

    void operator=( const CharActionNode& node )
    {
        m_ulLowerChrAction  = node.m_ulLowerChrAction;
        m_ulUpperChrAction  = node.m_ulUpperChrAction;
        m_fDirection        = node.m_fDirection;
        m_vecNextPosition   = node.m_vecNextPosition;
        m_fVec              = node.m_fVec;
        m_usAniNum          = node.m_usAniNum;
        m_usAniNumCount     = node.m_usAniNumCount;
        m_ulFrame           = node.m_ulFrame;
    }

} CharActionNode;

//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreature
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class RYLCreature
{
            //--------------------------------------------------------------------------------------------------------------------------
            // friend class
    friend  class                   RYLCreatureEffect;
    friend  class                   RYLCreatureManager;

//--------------------------------------------------------------------------------------------------------------------------
// member variables
//--------------------------------------------------------------------------------------------------------------------------
protected:
            //--------------------------------------------------------------------------------------------------------------------------
            // unique creature ID
            unsigned long           m_ulCharID;                 // special identifier allocated this creature

            static unsigned long    m_ulAddedOrder;             //
            unsigned long           m_ulAddedID;                //

            //--------------------------------------------------------------------------------------------------------------------------
            // creature type
            unsigned long           m_ulCreatureType;           // creature type    

            //--------------------------------------------------------------------------------------------------------------------------
            // is used?
            bool                    m_bUsed;
            bool                    m_bFirstPositionSend;
            bool                    m_bMemoryReusable;

            //--------------------------------------------------------------------------------------------------------------------------
            // data for basic rendering 
            string                  m_strCharName;					// creature name
			CRYLLabel*				m_pCharStreetStallNameLabel ;	// creature street stall name label
			CRYLLabel*				m_pCharNameLabel ;				// creature name label
			CRYLLabel*				m_pCharTempGuildNameLabel;
			CRYLLabel*				m_pCharJobNameLabel ;
			CRYLLabel*				m_pCharChatMsgLabel ; 
			CRYLPicture*			m_pSignpostImg ;

            CZ3DGeneralChrModel*    m_pCharModel;               // creature model data
            
            string                  m_strGCMDSName;             // modeling data file name
            string                  m_strFaceType;              // model face type
            string                  m_strHairType;              // model hair type

            unsigned short          m_usShape[Item::EquipmentPos::MAX_EQUPMENT_POS];	// save the creature shape data ( for identifier )
			const char*				m_strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];	// save the creature shape data ( for name )

			Broadcast2nd::CNetworkEquipGrade	m_EquipGrade;	// 장비 그레이드별 이펙트용 

            vector3                 m_vecPosition;              // position
			vector3                 m_vecNamePosition;          // name position
            float                   m_fDirection;               // direction

            unsigned long           m_dwLastUpdateActionTimer;  //

            unsigned long           m_ulWeaponPos;              // 
			unsigned char           m_ucRideFlag;               //

			unsigned char			m_ucEnableRide;				// 라이드를 탄상태다

            unsigned char           m_ucRace;                   // 0 : Human, 1 : Akhan
            unsigned short          m_usClass;                  // character Class

            float                   m_fScale;                   // Creature scale
            float                   m_fFixAngle;                // 

			unsigned char			m_cGMModelFlag;				// GM 모델 변경 여부.

			unsigned char			m_cRealmPoint;				// 국가전쟁 공헌훈장 포인트.

			unsigned char			m_cTactics;					// 용병 정보.

public:
			//--------------------------------------------------------------------------------------------------------------------------
			// concerned with skill action : 얼마 안쓰이기땜에 그냥 public으로 선언해서 편하게 쓰는 것이 -_-;;
			unsigned long           m_ulAttackCombo;
			unsigned long           m_ulSkillComboCount;        
			bool                    m_bNoneComboAttackMode;     
			bool                    m_bSkillCombo;

			//--------------------------------------------------------------------------------------------------------------------------
			// chatting timer : 이것도 역시...(-_-)
			float                   m_fChatTimer;

    static  unsigned long           m_ulRunFactorTimer;         // Run 타임. m_dwRunFactorTimer = CFrameTimer::Regist(200.0f);

private:
            float                   m_fRunFactor;               // 
            vector3                 m_vecJumpAttackVector;      // 

            float                   m_fSelfCharLens;            // Lens            
	        bool                    m_bViewName;				// 게임 화면 내의 캐릭터 이름 감추기 옵션( 개별 Creature )

            //--------------------------------------------------------------------------------------------------------------------------
            // creature action
            unsigned long           m_ulUpperCharActionID;      // upper action identifier
            unsigned long           m_ulLowerCharActionID;      // lower action identifier
            bool                    m_bUpperActing;             // is upper part acting?
            bool                    m_bLowerActing;             // is lower part acting?
            bool                    m_bUpperAble;               // is upper action available?
            bool                    m_bLowerAble;               // is lower action available?

            CharActionNode			m_Action;

			list<CharActionNode*>::iterator m_iterBeforeAction;	// 외부에서 list 순회를 원할때 직전에 순회한 위치를 저장하고 있다가 다음 위치를 넘겨준다.

            queue<CharActionNode*>  m_queActionData;            // Action Queue

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with "DEAD" process
            float                   m_fDeadTimer;               // 
            bool                    m_bFirstDead;               // True일 시 Dead Action 한번 취해줌. 그 이후 모든 SetAction에 대해서 처리 안함. 
            bool                    m_bCharDead;                // died?

            bool                    m_bStillCasting;            

            //--------------------------------------------------------------------------------------------------------------------------
            // flags for current creature state
            bool                    m_bFlying;                  // 점프중인가?
			DWORD					m_dwFlyingTick;

            bool                    m_bSitMode;                 // not sitting down?
            bool                    m_bFlyFirstCheck;

			//-----------------------------------------
			// 어택 상태 설정
			bool					m_bSkillAttacked;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with slide character
            CEffScript::CEffExt6    m_effSlideValue;            //
		    bool                    m_bSlide;                   //

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with collision         
            unsigned char           m_ucCDTType;                // collision check type

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with creature effect
            RYLCreatureEffect*      m_pCreatureEffect;          //

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with damage effect
            RYLDamageEffect*        m_pDamageEffect;            //

			//--------------------------------------------------------------------------------------------------------------------------
		    // 공성 관련
			unsigned long			m_dwCampOrCastleID;			// 요새 = 요새(캠프) ID, 성 = 성ID
		    unsigned long			m_dwOwnerID;				// 공성병기 = 주인 CID, 수성 병기 = 주인 CID
		    unsigned short			m_wObjectType;				// Siege::SiegeObjectType 참고
		    unsigned char			m_cState;					// Siege::State
		    unsigned char			m_cSubState;				// Siege::State
		    unsigned char			m_cUpgradeStep;	    		// 업그레이드 단계
		    unsigned char			m_cUpgradeType;	    		// 상징물만 해당( 보석에의한 업그레이드 상태 )
		    unsigned long			m_dwDoorBlockrate;			// 성문에만 해당( 블럭율 )
    		
		    bool					m_bOBBEnable;				// OBB 여부
		    CZ3DOBB*				m_pZ3DOBB;					// OBB

			DWORD					m_dwRiderID[Siege::AIRSHIP_RIDER_NUM];		// 병기 탄 사람 ID ( 0 번째는 주인, 1-9 는 손님 )
		    bool					m_bRideEnable;								// 캐릭터가 병기에 탑승하고 있는지 없는지의 여부

			BOOL					m_bRepairFullHP;			// 수성 병기 수리시에 전부 수리인지 여부

			//--------------------------------------------------------------------------------------------------------------------------
			// 길드전, 국가전 전쟁 플래그
			unsigned char			m_cRealmWarFlag;					// 국가 전쟁 플래그
			unsigned char			m_cGuildWarFlag;					// 길드 전쟁 플래그

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
public:
            //--------------------------------------------------------------------------------------------------------------------------
            // Constructor, Destructor
                                    RYLCreature();
                                    RYLCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName, 
                                                 vector3 vecCharPos, float fDirection );
                                    RYLCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName, 
                                                 const char* pstrFaceType, const char* pstrHairType, const char** pstrShape, 
                                                 unsigned char ucRace, vector3 vecCharPos, float fDirection );
                                    RYLCreature( unsigned long ulCharID, vector3 vecPosition );
    virtual                         ~RYLCreature();

            //--------------------------------------------------------------------------------------------------------------------------
            // Init Creature
            void                    InitCreature( bool bRespawn = false );

            //--------------------------------------------------------------------------------------------------------------------------
            // get creature id
            unsigned long           GetCharID( void )                                       { return m_ulCharID; }
			string					GetCharacterName( void )								{ return m_strCharName; } //hayhozar killspree buff system

            //--------------------------------------------------------------------------------------------------------------------------
            // get creature type
            unsigned long           GetCreatureType( void )                                 { return m_ulCreatureType; }

            //--------------------------------------------------------------------------------------------------------------------------
            // set creature used or not( Manager클래스에서 creature pool 관리를 위해 사용 )
    inline  void                    SetIsUsed( bool bUsed )                                 { m_bUsed = bUsed; }
    inline  bool                    GetIsUsed( void )                                       { return m_bUsed; }

    inline  void                    SetMemoryReusable( bool bReusable )                     { m_bMemoryReusable = bReusable; }
    inline  bool                    GetMemoryReusable( void )                               { return m_bMemoryReusable; }

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with rendering data
    inline  void                    SetCharModel( CZ3DGeneralChrModel* pCharModel )         { m_pCharModel = pCharModel; }
    inline  CZ3DGeneralChrModel*    GetCharModel( void )                                    { return m_pCharModel; }
    inline  void                    SetAlphaLevel( float fAlphaFactor )                     { m_pCharModel->SetAlphaLevel( fAlphaFactor ); }

            //void                    SetPosition( float fPosX, float fPosY, float fPosZ );
            void                    SetPosition( vector3 &vecPos );
            vector3*                GetPosition( void );
			vector3*				GetNamePosition( void );

            void                    SetDirection( float fDirection );
            float                   GetDirection( void );

    inline  void                    SetWeaponPos( unsigned long ulWeaponPos )               { m_ulWeaponPos = ulWeaponPos; }
    inline  unsigned long           GetWeaponPos( void )                                    { return m_ulWeaponPos; }

    inline  void                    SetRideFlag( unsigned char cRideFlag )                 { m_ucRideFlag = cRideFlag; }
    inline  unsigned char           GetRideFlag( void )                                    { return m_ucRideFlag; }
	
    
    inline  void                    SetFixAngle( float fFixAngle )                          { m_fFixAngle = fFixAngle; }
    inline  float                   GetFixAngle( void )                                     { return m_fFixAngle; }

    inline  void                    SetScale( float fScale )                                { m_fScale = fScale; }
    inline  float                   GetScale( void )                                        { return m_fScale; }

    inline  void                    SetRunFactor( float fRunFactor )                        { m_fRunFactor = fRunFactor; }
    inline  float                   GetRunFactor( void )                                    { return m_fRunFactor; }

    inline  void                    SetJumpAttackVector( vector3 vecJumpAttackVector )      { m_vecJumpAttackVector = vecJumpAttackVector; }
    inline  vector3                 GetJumpAttackVector( void )                             { return m_vecJumpAttackVector; }

    inline  void                    SetSelfLens( float fCharLens)                           { m_fSelfCharLens = fCharLens; }
    inline  float                   GetSelfLens( void )                                     { return m_fSelfCharLens; }

    inline  void                    SetViewName( bool bViewName )                           { m_bViewName = bViewName; }
    inline  bool                    GetViewName( void )                                     { return m_bViewName; }

    inline  void                    SetModelFileName( const char* pstrGCMDSName )           { m_strGCMDSName = pstrGCMDSName; }
    inline  string*                 GetModelFileName( void )                                { return &m_strGCMDSName; }
    inline  void                    SetFaceType( const char* pstrFaceType )                 { m_strFaceType = pstrFaceType; }
    inline  string*                 GetFaceType( void )                                     { return &m_strFaceType; }
    inline  void                    SetHairType( const char* pstrHairType )                 { m_strHairType = pstrHairType; }
    inline  string*                 GetHairType( void )                                     { return &m_strHairType; }

            //--------------------------------------------------------------------------------------------------------------------------
            // setting action 
            void                    SetActionID( unsigned long* pulActionID, const char* pstrAction );
            void                    SetActionString( const unsigned long ulActionID, char* pstrAction );
            bool                    SetAction( unsigned long ulUpperAction, unsigned long ulLowerAction );
            bool                    SetAction( const char* pstrUpperAction, const char* pstrLowerAction );

    inline  void                    SetCurrentAction( CharActionNode pAction )              { m_Action = pAction; }
    inline  CharActionNode*         GetCurrentAction( void )                                { return &m_Action; }

            void                    UpdatePlayerAction();
            void                    UpdateMonsterAction();

	inline	void					SetGMModelFlag(unsigned char cGMModelFlag)				{ m_cGMModelFlag = cGMModelFlag;	}
	inline	unsigned char			GetGMModelFlag(void)									{ return m_cGMModelFlag;			}

	inline	void					SetRealmPoint(unsigned char cRealmPoint)				{ m_cRealmPoint = cRealmPoint;		}
	inline	unsigned char			GetRealmPoint(void)										{ return m_cRealmPoint;				}

	inline	void					SetTactics(unsigned char cTactics)						{ m_cTactics = cTactics;			}
	inline	unsigned char			GetTactics(void)										{ return m_cTactics;				}

public:
            void                    AddActionData( CharActionNode* pAction );

private:
            bool                    SetActionSub( unsigned long ulUpperAction, const char* pstrUpperAction, unsigned long ulLowerAction, const char* pstrLowerAction );
public:
    inline  unsigned long           GetLowerActionID( void )                                { return m_ulLowerCharActionID; 	}
    inline  unsigned long           GetUpperActionID( void )                                { return m_ulUpperCharActionID; 	}

    inline  void                    SetUpperAble( bool bAble )                              { m_bUpperAble = bAble;		}
    inline  bool                    GetUpperAble( void )                                    { return m_bUpperAble;		}
    inline  void                    SetLowerAble( bool bAble )                              { m_bLowerAble = bAble;		}
    inline  bool                    GetLowerAble( void )                                    { return m_bLowerAble;		}

    inline  void                    SetUpperActing( bool bActing )                          { m_bUpperActing = bActing; }
    inline  bool                    GetUpperActing( void )                                  { return m_bUpperActing;	}
    inline  void                    SetLowerActing( bool bActing )                          { m_bLowerActing = bActing; }
    inline  bool                    GetLowerActing( void )                                  { return m_bLowerActing;	}

public:
    inline  void                    SetShape( bool bForceChange = false )                   { SetShape( m_strShape, m_EquipGrade, bForceChange );	}
            void                    SetShape( const char** strShape, const Broadcast2nd::CNetworkEquipGrade& equipGrade, bool bForceChange = false );
    inline  const char**			GetShapeString( void )                                  { return m_strShape; }

			Broadcast2nd::CNetworkEquipGrade	GetEquipGrade(void);
            
            void                    InitCharAction( void );                                         // initialize action
            void                    InitWeapon( unsigned long ulEquip );                            // initialize character weapon(?)
            bool                    IsCancelAction( unsigned long ulPart );

            void                    Respawn( void );
			bool					Landing( CCharacterControl* lpObjControl );
    
            //--------------------------------------------------------------------------------------------------------------------------
            // do action funcions
            enum                    { MOVE_FOWARD = 0, MOVE_RIGHT, MOVE_BACK, MOVE_LEFT };          // Direction Identifier

            void                    Walk( unsigned char ucDirection, float fRunFactor = 0.0f );     // move forward, back, left, right
            void                    Dead( void );                                                   // set dead motion
            void                    Wait( void );
            void                    ToggleCrouch( void );                                           // Toggle "Crouch" <-> "Stand up"

            bool                    Dash( unsigned char ucDirection );
		    bool                    Jump( void );

            void                    BeginCast( const char* pstrMotion );
            bool                    Casting( unsigned long ulUpperAct = 0xFFFFFFFF, unsigned long ulLowerAct = 0xFFFFFFFF );

            void                    CancelSkill( void );
            void                    EndSkill( const char* pstrMotion );
            void                    SkillAttack( unsigned long ulUpperAct, unsigned long unLowerAct );

            bool                    Attack( unsigned long ulUpperAct = 0xFFFFFFFF, unsigned long ulLowerAct = 0xFFFFFFFF );

            //--------------------------------------------------------------------------------------------------------------------------
            // slide
    inline  void                    SetSlideValue( CEffScript::CEffExt6 effValue )          { m_effSlideValue = effValue; m_bSlide = true;}
            void                    PlaySlide( void );
            void                    RenderSlide( float fBx, float fBy, float fBz, float fNx, float fNy, float fNz, float fUnitSize );

            //--------------------------------------------------------------------------------------------------------------------------
            // get dead state
    inline  void                    SetIsDead( bool bCharDead )                             { m_bCharDead = bCharDead; }
    inline  bool                    GetIsDead( void )                                       { return m_bCharDead; }
            
    inline  void                    SetDeadTimer( float fDeadTimer )                        { m_fDeadTimer = fDeadTimer; }
    inline  float                   GetDeadTimer( void )                                    { return m_fDeadTimer; }

            //--------------------------------------------------------------------------------------------------------------------------
            // 스킬 관련
    inline  bool                    GetStillCasting( void )                                 { return m_bStillCasting; }
    inline  void                    SetStillCasting( bool bStillCasting )                   { m_bStillCasting = bStillCasting; }

            //--------------------------------------------------------------------------------------------------------------------------
            // creature current state
    inline  void                    SetFlying( bool bFlying )                               
	{ 
		m_bFlying = bFlying; 
		m_dwFlyingTick = timeGetTime();
	}

    inline  bool                    GetFlying( bool bTimeCheck = false )                                       
	{ 
		// edtih 2009.08.12 점프중 스킬 사용 불가 테스트 하는 루틴.
		if(bTimeCheck && m_bFlying)
		{
			if(abs((long)timeGetTime()-(long)m_dwFlyingTick) > 3000)
			{
				m_bFlying = false;
			}
		}
		return m_bFlying; 
	}

    inline  void                    SetSitMode( bool bSitMode )                             { m_bSitMode = bSitMode; }
    inline  bool                    GetSitMode( void )                                      { return m_bSitMode; }

    inline  void                    SetFlyFirstCheck( bool bFlyFirstCheck )                 { m_bFlyFirstCheck = bFlyFirstCheck; }
    inline  bool                    GetFlyFirstCheck( void )                                { return m_bFlyFirstCheck; }

            bool                    IsInSafetyZone( void );

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Collision type
    inline  void                    SetCDTType( unsigned char ucCDTType )                   { m_ucCDTType = ucCDTType; }
    inline  unsigned char           GetCDTType( void )                                      { return m_ucCDTType; }

            //--------------------------------------------------------------------------------------------------------------------------
            // clear self object
            void                    ClearCreature( void );

            //--------------------------------------------------------------------------------------------------------------------------
            // get creature effect
    inline  RYLCreatureEffect*      GetEffect( void )                                       { return m_pCreatureEffect; }

            //--------------------------------------------------------------------------------------------------------------------------
            // get damage effect
    inline  RYLDamageEffect*        GetDamageEffect( void )                                 { return m_pDamageEffect; }
        
            //--------------------------------------------------------------------------------------------------------------------------
            // 공성 관련
            float                   AttackWeaponRun( float fRunFactor );

            //--------------------------------------------------------------------------------------------------------------------------
		    // 공성관련
			unsigned long			GetCampID( void )										{ return m_dwCampOrCastleID ; }
			unsigned long			GetCastleID( void )										{ return m_dwCampOrCastleID ; }
			void					SetCampID( unsigned long dwCampID )						{ m_dwCampOrCastleID = dwCampID ; }
			void					SetCastleID( unsigned long dwCastleID )					{ m_dwCampOrCastleID = dwCastleID ; }
			void					SetCampOrCastleID( unsigned long dwCampOrCastleID )		{ m_dwCampOrCastleID = dwCampOrCastleID ; }

		    unsigned long	        GetOwnerID( void )										{ return m_dwOwnerID ; }
		    void			        SetOwnerID( unsigned long dwOwnerID )					{ m_dwOwnerID = dwOwnerID ; }

		    unsigned short	        GetObjectType( void )									{ return m_wObjectType ; }
		    void			        SetObjectType( unsigned short wObjectType )				{ m_wObjectType = wObjectType ; }

            unsigned char	        GetState( void )										{ return m_cState ; }	
		    void			        SetState( unsigned char cState )						{ m_cState = cState ; }	

		    unsigned char	        GetSubState( void )										{ return m_cSubState ; }	
		    void			        SetSubState( unsigned char cSubState )					{ m_cSubState = cSubState ; }	

		    unsigned char	        GetUpgradeStep( void )									{ return m_cUpgradeStep ; }
		    void			        SetUpgradeStep( unsigned char cUpgradeStep )			{ m_cUpgradeStep = cUpgradeStep ; }

		    unsigned char	        GetUpgradeType( void )									{ return m_cUpgradeType ; }
		    void			        SetUpgradeType( unsigned char cUpgradeType )			{ m_cUpgradeType = cUpgradeType ; }

            CZ3DOBB*                GetOBB( void )                                          { return m_pZ3DOBB; }
            void                    SetOBB( CZ3DOBB* pZ3DOBB )                              { m_pZ3DOBB = pZ3DOBB; }

		    bool			        GetOBBEnable( void )									{ return m_bOBBEnable ; }
		    void			        SetOBBEnable( bool bOBBEnable )							{ m_bOBBEnable = bOBBEnable ; }		

		    unsigned long	        GetDoorBlockrate( void )								{ return m_dwDoorBlockrate ; }
		    void			        SetDoorBlockrate( unsigned long dwDoorBlockrate )		{ m_dwDoorBlockrate = dwDoorBlockrate ; }

		    bool			        GetIsCastleObject( void ) ;
		    bool			        GetIsCastleNPC( void ) ;
		    bool			        GetIsSiegeArms( void ) ;

		    // 병기 ID
		    DWORD		            GetRiderID( int iIndex )								{ return m_dwRiderID[ iIndex ] ; }
		    void		            AddRiderID( DWORD dwRiderID ) ;
		    void		            DeleteRiderID( DWORD dwRiderID ) ;
			void			        SetRiders( DWORD* pdwRiders ) ;

		    bool		            GetRideEnable( void )									{ return m_bRideEnable ;  }
		    void		            SetRideEnable( bool bRide )								{ m_bRideEnable = bRide ; }

		    //------------------------------------------------------------------------
		    // 성문
		    unsigned long	        GetCastleDoorUpgradePrice( void )	;
		    unsigned long	        GetCastleDoorUpgradeTime( void ) ;
		    unsigned long	        GetCastleDoorRepairPrice( void ) ;
		    unsigned long	        GetCastleDoorRecoveryPrice( void ) ;
		    unsigned long	        GetCastleDoorRecoveryTime( void ) ;
		    unsigned short          GetRealObjType( bool bEmblemEnable = false ) ;

			unsigned long			GetCastleArmsDevelopePrice( DWORD dwArmsType ) ;
			unsigned long			GetCastleArmsDevelopeTime( DWORD dwArmsType ) ;
			unsigned long			GetCastleArmsRepairPrice() ;
			unsigned long			GetCastleArmsRepairTime() ;

			BOOL					GetRepairFullHP() const 								{ return m_bRepairFullHP ;	}
			void					SetRepairFullHP( BOOL bFull )							{ m_bRepairFullHP = bFull ;	}	

			CRYLLabel*				GetCharStreetStallNameLabel()		{ return m_pCharStreetStallNameLabel ;	}
			CRYLLabel*				GetCharNameLabel()					{ return m_pCharNameLabel ;				}
			CRYLLabel*				GetCharTeampGuildNameLabel()		{ return m_pCharTempGuildNameLabel;		}
			CRYLLabel*				GetCharJobNameLabel()				{ return m_pCharJobNameLabel ;			}
			CRYLLabel*				GetCharChatMsgLabel()				{ return m_pCharChatMsgLabel ;			}
			void					MakeSignpostImg( char* pszText, bool bNPC = true ) ;
			CRYLPicture*			GetSignpostImg()					{ return m_pSignpostImg ;				}
			void					DeleteSignpostImg()	;

			//--------------------------------------------------------------------------------------------------------------------------
			// 길드전, 국가전 전쟁 플래그
			unsigned char			GetGuildWarFlag( void ) const			{ return m_cGuildWarFlag ;			}
			unsigned char			GetRealmWarFlag( void ) const			{ return m_cRealmWarFlag ;			}
			void					SetGuildWarFlag( unsigned char cFlag )	{ m_cGuildWarFlag = cFlag ;			}
			void					SetRealmWarFlag( unsigned char cFlag )	{ m_cRealmWarFlag = cFlag ;			}
};

#endif // __RYL_CREATURE_H__                
