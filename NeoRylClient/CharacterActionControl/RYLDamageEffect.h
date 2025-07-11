//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLDamageEffect.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: single creature's damage management
//
// Date: 2004. 3. 10. ( Sat )
//--------------------------------------------------------------------------------------------------------------------------
#ifndef __RYL_DAMAGE_EFFECT_H__
#define __RYL_DAMAGE_EFFECT_H__

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include "Z3DGeneralChrModel.h"
#include <list>
using namespace std;

class CTexture;
class CRYLGameData;

//--------------------------------------------------------------------------------------------------------------------------
// Enums, Structs
//--------------------------------------------------------------------------------------------------------------------------
struct RYLDamageNode
{ 
    short           sDamageLifeTime;
	long            lDamageValue;
	unsigned short  usDamageType;
	float           fScale;
	long			dwAlign;		// 0 : Center, 1 : Left, 2 : Right

	enum
	{
		DN_Center = 0,
		DN_Left,
		DN_Right
	};

	RYLDamageNode()
    {
        sDamageLifeTime = 0;
        lDamageValue    = 0;
        usDamageType    = 0;
        fScale          = 0.0f;
		dwAlign			= DN_Center;
    }
};

//--------------------------------------------------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RYLDamageEffect
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
class RYLDamageEffect
{
private:
    CTexture*				        m_pNumberTexture;
    list<RYLDamageNode*>            m_lstDamageEffect;

public:
            //--------------------------------------------------------------------------------------------------------------------------
            // Constructor, Destructor
                                    RYLDamageEffect();
                                    ~RYLDamageEffect();

            //--------------------------------------------------------------------------------------------------------------------------
            // add damage effect
    inline  void                    AddDamage( RYLDamageNode* pNode )           { m_lstDamageEffect.push_back( pNode ); }

            //--------------------------------------------------------------------------------------------------------------------------
            // damage rendering
			enum					DAMAGE_TYPE { FRIENDLY_DAMAGE = 0, ENEMY_DAMAGE = 1 };
            void                    RenderDamage( LPDIRECT3DDEVICE8 lpD3DDevice, bool bMovieVersion, vector3 vecPos, DAMAGE_TYPE type );
private:
            void                    RenderDamageNumber( LPDIRECT3DDEVICE8 lpD3DDevice, short sPosX, short sPosY, unsigned char cAlpha, unsigned short wMode, long lValue, float fRate, unsigned long dwAlign );

public:

            //--------------------------------------------------------------------------------------------------------------------------
            // clear self object
            void                    ClearDamageEffect();
};


#endif // __RYL_DAMAGE_EFFECT_H__