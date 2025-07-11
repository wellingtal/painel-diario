//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLCreatureEffect.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: single Creature's effect management class
//
// Date: 2004. 4. 8. ( Thu )
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __RYL_CREATURE_EFFECT_H__
#define __RYL_CREATURE_EFFECT_H__
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "SceneManager.h"
#include <Creature/CreatureStructure.h>
#include <string>
#include <vector>
#include <list>
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// struct RYLBodyEffect : Creature ���� �ٴ� Effect
struct RYLBodyEffect
{	
    CEffScript*			m_pBodyEffScript;

    bool				m_bPivotRot;
    int					m_nPivotIndex;
    string				m_strPivotName;
    string              m_strEffName;
    bool				m_bIsUseEff;
    unsigned long		m_ulEffID;

    D3DXVECTOR3			m_D3DVecEffPos;

    RYLBodyEffect() 
    {
        m_pBodyEffScript= NULL;
        m_bPivotRot     = false;
        m_nPivotIndex   = -1;
        m_strPivotName.reserve( 128 );
        m_strEffName.reserve( 128 );
        m_bIsUseEff     = false;
        m_ulEffID       = 0;

        m_D3DVecEffPos.x = m_D3DVecEffPos.y = m_D3DVecEffPos.z = 0.0f;
    }

    ~RYLBodyEffect() 
    {
        m_strPivotName.clear();
        m_strEffName.clear();

        if( m_pBodyEffScript ) 
        {     
            if( !CSceneManager::m_EffectManager.CheckNullScript( m_pBodyEffScript ) ) 
            {
                m_pBodyEffScript->SetAllLoopOff();
                m_pBodyEffScript->SetVisibility( false );
                m_pBodyEffScript = NULL;
            }
        }
    }
};

//--------------------------------------------------------------------------------------------------------------------------
// struct RYLArrowEffect
struct RYLArrowEffect
{
    CEffScript*         m_pArrowEffScript;
	string              m_strPivotName;

	RYLArrowEffect() 
    {
		m_pArrowEffScript = NULL;
        m_strPivotName.clear();
	}

	~RYLArrowEffect() 
    {
        m_strPivotName.clear();

		if( m_pArrowEffScript ) 
        {
			if( !CSceneManager::m_EffectManager.CheckNullScript( m_pArrowEffScript ) )
            {
				m_pArrowEffScript->SetAllLoopOff();
				m_pArrowEffScript->SetVisibility( false );
				m_pArrowEffScript = NULL;
			}
		}
	}
};

//--------------------------------------------------------------------------------------------------------------------------
// struct RYLWeaponEffect : Weapon�� �ٴ� effect
struct RYLWeaponEffect
{
    CEffScript*			m_pWeaponEffScript;
    string              m_strEffectName;

    int					m_nWeaponEffectNum;		//effect�� ���� 
    int					m_nWeaponPosNum;		//effect�� �����߿� �� effect ����� ��ġ

    RYLWeaponEffect() 
    {
        m_pWeaponEffScript  = NULL;
        m_strEffectName.clear();

        m_nWeaponEffectNum  = 0;		 
        m_nWeaponPosNum     = 0;		
    }

    ~RYLWeaponEffect() 
    {
        if( m_pWeaponEffScript ) 
        {
            if( !CSceneManager::m_EffectManager.CheckNullScript( m_pWeaponEffScript ) )
            {
                m_pWeaponEffScript->SetAllLoopOff();
                m_pWeaponEffScript->SetVisibility( false );
                m_pWeaponEffScript = NULL;
            }
        }
    }
};

//--------------------------------------------------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// pre-defined class
class RYLCreature;

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLCreatureEffect
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class RYLCreatureEffect
{
private:
            //--------------------------------------------------------------------------------------------------------------------------
            // Effect ��ü�� ���� �ִ� mother creature ��ü�� pointer( �Ҵ�/���� ����! )
            RYLCreature*            m_pMotherCreature;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with chant effect
            EnchantInfo		        m_EnchantInfo;              // current chant status

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Body Effect
    list<RYLBodyEffect*>            m_lstBodyEff;
            bool                    m_bBodyEffExist;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Arrow Effect
    list<RYLArrowEffect*>           m_lstArrowEff;
            bool                    m_bArrowEffExist;

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Weapon Effect
    list<RYLWeaponEffect*>          m_lstWeaponEff;
            bool                    m_bWeaponEffExist;

public:
            //--------------------------------------------------------------------------------------------------------------------------
            // Constructor, Destructor
                                    RYLCreatureEffect( RYLCreature* pMotherCreature );
                                    ~RYLCreatureEffect();

            //--------------------------------------------------------------------------------------------------------------------------
            // set effect visibility
            void                    SetEffectVisibility( bool bVisible );

            //--------------------------------------------------------------------------------------------------------------------------
            // concerned with Body Effect
            // ĳ�� pos �� effect ���ϰ��
            void                    AddBodyEffect( string strScriptFile, unsigned long ulEffID = 0, bool bMine = true, bool bVisibility = true );
            // pivot �ϳ��� effect ���� ( pivot name����.. )
            void                    AddBodyEffect( string strScriptFile, string strPivot, bool bRot = false, unsigned long ulEffID = 0 );
            // pivot �ϳ��� effect ���� ( pivot id��.. )
            void                    AddBodyEffect( string strScriptFile, int nPivotID, bool bRot = false, unsigned long ulEffID = 0 );
           
            void                    DeleteBodyEffect();                                                             // �� effect delete
            void                    DeleteBodyEffect( string strPivot );                                            // �� pivot�� ���� effect delete
            void                    DeleteBodyEffect( const unsigned long ulEffID );                                // effect id�� delete
            void                    DeleteBodyEffectName( string strScriptFile );                                   // effect script �̸��� ���� delete

            // all creature update
            void                    BodyEffectUpdate();
            // single creature update
            void                    BodyEffectUpdate( list<RYLBodyEffect*>::iterator& iterEffect, bool bAllUpdate = false );
            
            void                    SetEffect();                                                                    // monster ���� GCMDS���� effect �ҷ� ���̴� �ڵ�

            // Monster
            void                    MonsterEffectProcess( const char* pstrFileName, int nEffectValue );

            // chant
    inline  EnchantInfo&            GetChantStatus()                                    { return m_EnchantInfo; }
            bool                    GetChantStatus( unsigned long ulChantKind );
            void                    SetChantEffect( bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow );
            void                    SetChantEffect( const EnchantInfo& Info, bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow );
            void                    SetChantEffect( unsigned long ulChant, bool bToggle, bool bSelf, bool bGoodChantEffectShow, bool bBadChantEffectShow );

            //--------------------------------------------------------------------------------------------------------------------------
            // support body effect 
private:
            bool                    CheckInBodyEffect( string strScriptFile );                                      // ���� body effect list�ȿ� �� ����Ʈ�� ���� �ϴ��� �˻�
            void                    RenderSnap( float fBeforex, float fBeforey, float fBeforez,     
                                                float fNowx, float fNowy, float fNowz, 
                                                float fUnitsize, const char *pstrEffName );

public:
    //--------------------------------------------------------------------------------------------------------------------------
    // concerned with Arrow Effect
            // pivot�� arrow ���̱�
		    void                    AddArrow( const char *pstrPivotName, D3DXQUATERNION Quat, int nDiv = 0 );

            // all arrow update
            void                    ArrowUpdate();
            // single arrow update
		    void                    ArrowUpdate( const unsigned int unIndex );

		    void                    DeleteArrow( int nIndex );

    //--------------------------------------------------------------------------------------------------------------------------
    // concerned with Weapon Effect
            void                    AddWeaponEffect( string strEsfName, int nEffectNum, bool bVisible );
		
		    void                    DeleteWeaponEffect();						                                    //�� weapon effect delete
		    void                    DeleteWeaponEffect(string strEsfName, int nEffNum );                            //esfname�� ������ effect delete

		    void                    WeaponEffectUpdate();

    //--------------------------------------------------------------------------------------------------------------------------
    // clear self object
            void                    ClearEffect();
};

#endif //__RYL_CREATURE_EFFECT_H__