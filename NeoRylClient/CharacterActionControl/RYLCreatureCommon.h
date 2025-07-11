//--------------------------------------------------------------------------------------------------------------------------
// File Name: ZCreatureCommon.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: const variables which is using commonly in several files that concerned with character control
//
// Date: 2004. 3. 25. ( Thu )
//--------------------------------------------------------------------------------------------------------------------------

#ifndef __ZCREATURE_COMMON_H__
#define __ZCREATURE_COMMON_H__

//--------------------------------------------------------------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// enum CHAR_ACT_ENUM : 캐릭터 액션 목록

// 액션 한계값은 255임.
enum CHAR_ACT_ENUM
{
    CA_WAIT             = 0,    CA_RUN              = 1,    CA_ATTACK           = 2,    CA_MOVEATTACK       = 3,
    CA_WALKBACK         = 4,    CA_WALKLEFT         = 5,    CA_WALKRIGHT        = 6,    CA_BASH             = 7,
    CA_WALK             = 8,    CA_FALLDOWN         = 9,    CA_REST             = 10,    CA_SITDOWN          = 11,
    CA_STANDUP          = 12,    CA_ATTACKADVANCE    = 13,    CA_ATTACKLEFT       = 14,    CA_ATTACKRIGHT      = 15,
    CA_ATTACKRETREAT    = 16,    CA_ATTACK2          = 17,    CA_ATTACK3          = 18,    CA_ATTACK4          = 19,
    CA_RESPAWN          = 20,    CA_JUMP             = 21,    CA_LEFTDASH         = 22,    CA_RIGHTDASH        = 23,
    CA_CASTING          = 24,    CA_FIRE1            = 25,    CA_FIRE2            = 26,    CA_COMBINATIONBLOW  = 27,
    CA_BACKSTAB         = 28,    CA_AIMEDSHOT2       = 29,    CA_ATTACK5          = 30,    CA_ATTACK6          = 31,
    CA_BACKDASH         = 32,    CA_AIMEDSHOT1       = 33,    CA_JUMPATTACK       = 34,    CA_JUMPATTACK2      = 35,
    CA_FRONTDASH        = 36,    CA_LAND             = 37,    CA_FORWARDJUMP      = 38,    CA_BACKJUMP         = 40,
    CA_SHOT             = 41,    CA_JUMPSHOT         = 42,    CA_OVERBASH1        = 43,    CA_OVERBASH2        = 44,
    CA_OVERBASH3        = 45,    CA_OVERBASH4        = 46,    CA_OVERBASH5        = 47,    CA_POWERDRAIN       = 48,
    CA_TURNPUNCH1       = 49,    CA_TURNPUNCH2       = 50,    CA_TURNPUNCH3       = 51,    CA_JUMPATTACK3      = 52,
    CA_AIMEDSHOT3       = 53,    CA_STUN             = 54,    CA_ROUNDSWING       = 55,    CA_SUMMON           = 56,
    CA_HIT1             = 57,    CA_WAITH1           = 58,    CA_HIT2             = 59,    CA_WAITH2           = 60,
	CA_ADVANCE			= 61,    CA_SWIM             = 100,	CA_FORWARD			= 101,	CA_BACKWARD			= 102,
	CA_GATHERSTART		= 111,	CA_GATHER01			= 111,	CA_GATHER02			= 112,	CA_GATHER03			= 113,
	CA_GATHER04			= 114,	CA_GATHER05			= 115,	CA_GATHER06			= 116,	CA_GATHER07			= 117,
	CA_GATHER08			= 118,	CA_GATHER09			= 119,	CA_GATHER10			= 120,	CA_GATHEREND		= 120,
	CA_SOCIALSTART		= 151,	CA_SOCIAL_LAUGH		= 151,	CA_SOCIAL_DESPAIR	= 152,	CA_SOCIAL_SHY		= 153,
	CA_SOCIAL_SURPRISE	= 154,	CA_SOCIAL_ROAR		= 155,	CA_SOCIAL_WAVE		= 156,	CA_SOCIAL_THANK		= 157,
	CA_SOCIAL_SORRY		= 158,	CA_SOCIAL_SALUTE	= 159,	CA_SOCIAL_CHEER		= 160,	CA_SOCIAL_NOD		= 161,
	CA_SOCIAL_NO		= 162,	CA_SOCIAL_CHARGE	= 163,	CA_SOCIAL_COMFORT	= 164,	CA_SOCIAL_TALK		= 165,
	CA_SOCIALEND		= 165,	CA_ACTIONEND		= 165,
	CA_MAX              = 255,
};


//enum CHANT_ENUM
//{    
//    CHANT_BATTLE_SONG           = 1,
//    CHANT_MAINTENANCE_CHANT     = 2,
//    CHANT_ACCELERATION_CHANT    = 3,
//    CHANT_LIFE_AURA             = 4,
//
//    CHANT_STRENGTH              = 12,
//    CHANT_SLOW                  = 13,
//    CHANT_ARMOR_BROKEN          = 14,
//    CHANT_BLAZE                 = 15,
//    CHANT_CHARGING              = 16,
//    CHANT_STEALTH               = 17,
//    CHANT_MANASHELL             = 18,
//    CHANT_ENCOURAGE             = 19,
//    CHANT_ENCHANT_WEAPON        = 20,
//    CHANT_BRIGHT_ARMOR          = 21,
//    CHANT_HARD_SKIN             = 22,
//    CHANT_FLEXIBILITY           = 23,
//    CHANT_GUARD                 = 24,
//    CHANT_HOLD                  = 25,
//    CHANT_STUN                  = 26,
//    CHANT_FROZEN                = 27,
//    CHANT_POISONED              = 28,
//    CHANT_LOWERSTRENGTH         = 29,
//
//    CHANT_INVINCIBLE            = 30,
//
//    CHANT_HIDE                  = 31,
//
//    CHANT_MAX
//};

enum Damage_Type
{
    Type_Front			= 0,	Type_Side		= 1,	Type_Back			= 2,	
    Type_Guard			= 3,	Type_Critical	= 4,	Type_Heal			= 5,	
    Type_ManaHeal		= 6,	Type_Chant		= 7,	Type_Enchant		= 8,	
    Type_Disenchant	    = 9,	Type_Evade		= 10,	Type_Resist		    = 11,	
    Type_ManaShell		= 12,	Type_Poisoned	= 13,	Type_NonShow		= 14,
    Type_Miss           = 15,	Type_Fired		= 20,   Type_MAX			= 21
};

//--------------------------------------------------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------------------------------------------------

#endif //__ZCREATURE_COMMON_H__