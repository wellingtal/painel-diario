//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreatureManager.h
//
// Programmer: RYLergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: Creature Managerment Class
//
// Date: 2004. 4. 7. ( Wen )
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __RYLCREATURE_MANAGER_H__
#define __RYLCREATURE_MANAGER_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "RYLCreature.h"

class CCharacterControl;
class CZ3DGeneralChrModel;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
struct CharModelNode
{
    CZ3DGeneralChrModel*    m_pCharModel;
    string                  m_strGCMDSName;
    unsigned long           m_ulUsedCharID;
};

//--------------------------------------------------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreatureManager
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class RYLCreatureManager
{
private:

			typedef map<unsigned long, unsigned long>	ArmsMap;

            //--------------------------------------------------------------------------------------------------------------------------
            // creature list
            RYLCreature*            m_pSelfCreature;            // self creature
			map<unsigned long,RYLCreature*>     m_mapCreature;              // creature map
            list<CharModelNode*>    m_lstCharModel;             // model data list

			map<unsigned long,RYLCreature*>::iterator           m_iterBeforeCreature;   // �ܺο��� map ��ȸ�� ���Ҷ� ������ ��ȸ�� ��ġ�� �����ϰ� �ִٰ� ���� ��ġ�� �Ѱ��ش�.	
			list<CharModelNode*>::iterator                  m_iterBeforeModel;      // �ܺο��� map ��ȸ�� ���Ҷ� ������ ��ȸ�� ��ġ�� �����ϰ� �ִٰ� ���� ��ġ�� �Ѱ��ش�.

			

			ArmsMap					m_ArmsMap;			

            unsigned int            m_unMaxCash;                // cash size

            //--------------------------------------------------------------------------------------------------------------------------
            // CCharacterControl
            CCharacterControl*      m_pObjControl;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with effect
            bool                    m_bGoodChantEffectShow;     // good effect show?
            bool                    m_bBadChantEffectShow;      // bad effect show?

public:
            //--------------------------------------------------------------------------------------------------------------------------
            // Constructor, Destructor
                                    RYLCreatureManager( unsigned int unMaxCash = 30 );
                                    ~RYLCreatureManager();

            //--------------------------------------------------------------------------------------------------------------------------
            // Binding functions
    static  char*                   (*GetMotionSheet)( char* pstrWeaponName, char* pstrShieldName, unsigned short usClass );
    static  unsigned long           (*GetWeaponType)( char* strWeaponName );
    static  unsigned long           (*GetClientIdentity)( unsigned long dwChrID ) ;
	static  unsigned long           (*GetClientEnemyType)( unsigned long dwChrID) ;

            //--------------------------------------------------------------------------------------------------------------------------
            // 
            void                    SetObjectControl( CCharacterControl* pObjControl )              { m_pObjControl = pObjControl; }

            //--------------------------------------------------------------------------------------------------------------------------
            // creature managing
            unsigned long           AddCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName,
                                                 vector3 vecPosition, float fDirection, bool bCollision, unsigned long dwNation = 0,
                                                 int iValue = -1, CZ3DGeneralChrModel **ppOld = NULL,bool bHouse = false,int iHouseType = -1, 
												 D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f) ) ;

            unsigned long           AddCreature( unsigned long ulCharID, const char* pstrName, const char* pstrGCMDSName, 
                                                 const char* pstrFaceType, const char* pstrHairType, 
												 const char** pstrShape, const Broadcast2nd::CNetworkEquipGrade& equipGrade, 
                                                 unsigned char ucRace, vector3 vecPosition, float fDirection, bool bCollision,
                                                 int iValue = -1, CZ3DGeneralChrModel **ppOld = NULL,bool bHouse = false,int iHouseType = -1, 
												 D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f) ) ;

            unsigned long           AddCreature( unsigned long ulCharID, vector3 vecPosition, 
                                                 int iValue = -1, CZ3DGeneralChrModel **ppOld = NULL,
                                                 bool bHouse = false, int iHouseType = -1,
                                                 D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f) );

            // Special Case : ���������Ͽ� ���� ����� ���� ĳ���͸� Switching �ϱ� ���� ����.
            RYLCreature*            AddSiegeCreature( RYLCreature* pSiegeCreature, unsigned long ulCharID, const char* pstrName, vector3 vecPosition, float fDirection, const char* pstrType );

            void                    DeleteCreature( unsigned long ulCharID, BOOL bForceDelete = FALSE );
			void					SwitchCreature( RYLCreature* pNode1, RYLCreature* pNode2 ) ; 

            // Creature�� ����Ʈ �󿡼� �����ش�.( Creature�� �״�� ���� ä �ּҸ� ����... )
            void                    AddCreatureList( RYLCreature* pCreature );
            RYLCreature*            DeleteCreatureList( unsigned long ulCharID );

            RYLCreature*            GetCreature( CZ3DGeneralChrModel *pModel );
            RYLCreature*            GetCreature( unsigned long ulCharID );
			RYLCreature*            GetCreature( char* szName );


            RYLCreature*            GetCreatureByAddedOrder( unsigned long ulAddedOrder );
            
    inline  RYLCreature*            GetSelfCreature()                                               { return m_pSelfCreature; }
    inline  void                    SetSelfCreature( RYLCreature* pSelfCreature )                   { m_pSelfCreature = pSelfCreature; }

            void                    SortCreatureMap();

            void                    GetCharIDArray( vector<unsigned long>& vtCharID );              // ĳ���� ID�� ��Ƽ� �迭��...

            const char*             GetCreatureName( unsigned long ulCharID );                      // ĳ������ ���� ������ �Ǵ��� ���� ���( NULL�̸� ��ȿ )

            RYLCreature*            GetCreatureListHead();
            RYLCreature*            GetCreatureListNext();

            CZ3DGeneralChrModel*    GetModelListHead();
            CZ3DGeneralChrModel*    GetModelListNext();

    inline  void                    SetMaxCash( unsigned int unMaxCash )                            { m_unMaxCash = unMaxCash; }
    inline  unsigned int            GetMaxCash()                                                    { return m_unMaxCash; }
            //--------------------------------------------------------------------------------------------------------------------------
            // model memory managing
            CZ3DGeneralChrModel*    AllocModel( RYLCreature* pCreature, bool bHouse = false, int iHouseType = -1, D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f) );
            CZ3DGeneralChrModel*    AllocTransformModel( unsigned long ulCharID, const char* pstrGCMDSName, bool bHouse = false, int iHouseType = -1, D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f) );
            void                    FreeModel( CZ3DGeneralChrModel* pModel, BOOL bForceDelete = FALSE );
            CharModelNode*          CheckUsableModelNode( const char* pstrGCMDSName );
            bool                    CheckExistModel( CZ3DGeneralChrModel* pModel );
			// 2005.06.20, yundi, PC�� 0���ε��� �����ϸ� GMĳ���ͷ� ��ȯ���� �߰�
			CZ3DGeneralChrModel*    ReplaceModel( RYLCreature* pCreature, const char* pstrGCMDSName );

            //----------------------------------------------------------------  ----------------------------------------------------------
            // get character list
            void		            GetSendCharacterChatList( list<unsigned long>& lstChar );
		    void		            GetSendCharacterAroundList( list<unsigned long>& lstChar, unsigned long ulSummonID = 0, bool bSummonAttacked = false );
		    void		            GetSendCharacterAroundList( list<unsigned long>& lstChar, unsigned long ulSummonID[ 10 ] );
		    void		            GetSendCharacterList( list<unsigned long>& lstChar );

            //--------------------------------------------------------------------------------------------------------------------------
            // clear self object
            void                    ClearCreatureManager();

            //--------------------------------------------------------------------------------------------------------------------------
            // add action node to single creature
            void                    AddAction( unsigned long ulCharID, unsigned long ulUpperAni, unsigned long ulLowerAni, 
                                               float fDirection, vector3 &vecNextPosition, 
                                               float fVec = 0.0f, unsigned short usAniNum = 0, unsigned long ulFrame = 0 );
            
            //--------------------------------------------------------------------------------------------------------------------------
            // Transformation
            void                    Transformation( unsigned long ulCharID, const char* pstrGCMDSName = NULL );

			void					AddArmsCreature(unsigned long dwCID, unsigned long dwArmsCID);
			void					DeleteArmsCreature(unsigned long dwCID);
			unsigned long			IsArmsCreature(unsigned long dwCID);
			void					DeleteAllArmsCreature(void);

private:
            //--------------------------------------------------------------------------------------------------------------------------
            // singleton
    static  RYLCreatureManager*     m_pInstance;

public:
    static VOID Release()
	{
        if( m_pInstance )
        {
            delete m_pInstance;
			m_pInstance = NULL;
        }
	}

    static  RYLCreatureManager*     Instance()
    {
        if( !m_pInstance )
        {
            m_pInstance = new RYLCreatureManager;
        }

        return m_pInstance;
    }
};

#endif //__RYLCREATURE_MANAGER_H__