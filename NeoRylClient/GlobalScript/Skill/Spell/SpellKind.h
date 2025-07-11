#ifndef _SPELL_KIND_H_
#define _SPELL_KIND_H_

namespace Skill
{
    namespace SpellID
    {
        enum Type
        {
            None				= 0,

		    BattleSong	        = 1,		
			MaintenanceChant	= 2,
            AccelerationChant	= 3,	    
			LifeAura	        = 4,	
			SpeedBuff			= 5,		
			Regeneration	    = 6,		
            Slow			    = 7,		
			ArmorBroken		    = 8,
            Blaze			    = 9,		
			Charging		    = 10,
			Stealth			    = 11,		
			ManaShell		    = 12,
            Encourage		    = 13,		
			EnchantWeapon	    = 14,

            BrightArmor		    = 15,		
			HardenSkin		    = 16,
			Flexibility		    = 17,		
			Guard			    = 18,
            Hold			    = 19,		
			Stun			    = 20,
            Frozen			    = 21,		
			Poisoned		    = 22,
			LowerStrength	    = 23,		
			Invincible		    = 24,
            Hide                = 25,		
			StoneForm			= 26,		
			CounterAttack		= 27, 
			Envenom				= 28,		

			BombSet				= 29, 
			Fired				= 30,
			CurseOfBlind		= 31,		
			DeCoolDown			= 32,		
			ExpOrb				= 33,
			LuckyOrb			= 34,

			DemagePotion		= 35,
			ArmorPotion			= 36,		
			HitRatePotion		= 37,		
			EvadePotion			= 38,
			MaxHPPotion			= 39,		
			MaxMPPotion			= 40,
			HPRegenPotion		= 41,
			MPRegenPotion		= 42,		
			CriticalPotion		= 43,
			BlockPotion			= 44,		
			SpeedPotion			= 45,
			DeCoolDownPotion	= 46,		
			MagicPowerPotion	= 47,
			MagicResistPotion	= 48,

			PowerStatue			= 49,		
			IntStatue			= 50,		
			ExpStatue			= 51,		
			WealthStatue		= 52,		
			RealmHP				= 53,		
			RealmMP				= 54,
			Concentration		= 55,		

			StealHand			= 56,	// � �߰���ų 
			MichaelBless		= 57,

			// �� �Ʒ��δ� ����â���� ǥ�ð� �ȵȴ�.
			TasteWine			= 58, 
			WifeCracker			= 59,		
			KarterantWorldBuff	= 60,		
			MerkadiaWorldBuff	= 61,
			KarterantWorldDeBuff= 62,		
			MerkadiaWorldDeBuff	= 63,
			HundredLevel		= 64,

			Drunk				= 65,


			CD_8D12				= 66,
			CD_8D14				= 67,
			CD_8D16				= 68,
			CD_99A1				= 69,
			CD_99A2				= 70,
			CD_99A3				= 71,

			//new aura starting from here
			Clairvoyance		= 72,
			GuardAura			= 73,
			DeEnchant			= 74,
			KillStreakBuff		= 75,//hz added for killstreak buff
			KillStreakBuff1		= 76,//hz added for killstreak buff
			KillStreakBuff2		= 77,//hz added for killstreak buff
			KillStreakBuff3		= 78,//hz added for killstreak buff
			KillStreakBuff4		= 79,//hz added for killstreak buff
			KillStreakBuff5		= 80,//hz added for killstreak buff
			KillStreakBuff6		= 81,//hz added for killstreak buff

			GreenPotion			= 82,
			GreenPotion1		= 83,
			GreenPotion2	    = 84,
			GreenPotion3		= 85,

			YellowPotion		= 86,
			YellowPotion1		= 87,
			YellowPotion2	    = 88,
			YellowPotion3		= 89,

			BlackPotion		    = 90,
			BlackPotion1		= 91,
			BlackPotion2	    = 92,
			BlackPotion3		= 93,

			MAX_SPELL_TYPE		= 94 // was 82
        };
    }

	namespace SpellTarget
    {
        enum Type
        {
            NONE                    = 0,
            CHANT                   = 1,
            FRIEND_TARGET_ENCHANT   = 2,
            ENEMY_TARGET_ENCHANT    = 3,
			ALL_ENCHANT				= 4
        };
    };

	namespace Disenchant
	{
		enum Type
		{
			NONE		= 0,
			LEVEL		= 1,
			DURATION	= 2,
			DEAD		= 3
		};

		enum Const
		{
			INFINITE_NUM	= UCHAR_MAX
		};
	};

	namespace SpellType
	{
		enum Type
		{
			NONE						= 0,
			MAGICAL_SPELL				= 1,			// ������ �ֹ�
			PHYSICAL_SPELL				= 2,			// ������ �ֹ� (ex - ��)
			WORLDWEAPON_SPELL			= 3,			// ���� ���� ��æƮ

			REALM_SPELL					= 4,			// �������� �������� ȿ�� ��þƮ.

			BUFF_SPELL					= 5,			// DB�� ����Ǵ� �������� ��þƮ (������ �����ȴ�)
			PAYBUFF_SPELL				= 6,			// ���� ������ �̰� �׾ �����ȵȴ�.
			STATUE_SPELL				= 7,			// ���� ���� ȿ�� ��þƮ

			ETERNAL_SPELL				= 8,			// ���������� �����Ǿ� �ϴ� ��æƮ (ex - 100�� ����Ʈ)
		};
	};
};

#endif