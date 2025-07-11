#ifndef _SIEGE_CONSTANTS_H_
#define _SIEGE_CONSTANTS_H_

namespace Siege
{
	enum SiegeObjectType
	{
		// ���� Kind ID �� ����Ѵ�.
		EMBLEM					= 5000,		// �� ��¡��					( DESTORYING, CHANGING, DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		GATE					= 5324,		// ����							( DESTORYING, CHANGING, DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		BACKDOOR				= 5378,		// �޹�							( Only COMPLETE ���¸� ���� )
		CAMP					= 5379,		// ��� ��� (ķ��)				( DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )//was 5379 hz guild war fix
		CASTLE_ARMS_NPC			= 5433,		// ���� ���� NPC				( Only COMPLETE ���¸� ���� )
		GUARD					= 5434,		// ����							( DESTORYING, CHANGING, DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		SHORT_RANGE_CASTLE_ARMS	= 5488,		// �ٰŸ� ���� ����				( DESTORYING, CHANGING, DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		LONG_RANGE_CASTLE_ARMS	= 5542,		// ���Ÿ� ���� ����				( DESTORYING, CHANGING, DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		SHORT_RANGE_SIEGE_ARMS	= 5596,		// �ٰŸ� ���� ����				( CHANGING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		LONG_RANGE_SIEGE_ARMS	= 5650,		// ���Ÿ� ���� ����				( CHANGING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		AIRSHIP					= 5704,		// �����						( CHANGING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		MINING_CAMP				= 5758,		// ä���� (ķ��)				( DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		CAMP_SHOP				= 5812,		// ����Ÿ�Կ�� (ķ��)			( DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		KARTERANT_WEAPON		= 5866,		// ī���׶�Ʈ ���� ���� (ķ��)	( DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		MERKADIA_WEAPON			= 5920,		// �޸�ī��� ���� ���� (ķ��)	( DEVELOPING, CANCELING, RESTORING, DESTROYED 1���� ���� )
		
		DEFAULT_TYPE			= 0			// ����Ʈ �������ڿ�
	};

	enum State
	{
		// KID             => SiegeObjectType
		//					  + (State * UPGRADE_KIND_NUM)
		//					  + UpgradeStep

		// KID (�� ��¡��) => SiegeObjectType
		//					  + (State * JEWEL_TYPE_NUM * UPGRADE_KIND_NUM)
		//					  +	(JewelType * UPGRADE_KIND_NUM)
		//					  + UpgradeStep

		COMPLETE				= 0,		// �� ��¡�� ��ȯ �Ϸ�, ��� ��� ���� �Ϸ�, ����, ���, �ٰŸ�, ���Ÿ�, ����� ���� �Ϸ�
		UPGRADING				= 1,		// �� ��¡��, ��� ���, ����, ����, �ٰŸ�/���Ÿ� �������� ���׷��̵� ���� ����
		REPARING				= 2,		// ��� ���, ����, �ٰŸ�/���Ÿ�, ����� ���� ����
		DESTROYING				= 3,		// ��� ���, �ٰŸ�/���Ÿ�, ����� �ı��ؼ� ��ŸƮ Ŷ���� ������ ����
		CHANGING				= 4,		// ��� ��� ������
		DEVELOPING				= 5,		// �� ��¡�� ��ȯ��, ��� ��� ������, ����, ���, �ٰŸ�, ���Ÿ�, ����� ������
		CANCELING				= 6,		// ��� ���, �ٰŸ�/���Ÿ�, ����� ������ ������� ���� (��ŸƮŶ ���������� ���ư���.)
		RESTORING				= 7,		// ���� ���� ����
		DESTROYED				= 8,		// ���� �ı� ����

		// SubState
		NONE					= 0,

        MINE					= 1,		// �Ʊ� �� ��¡��
		ENEMY					= 2,		// ���� �� ��¡��

		OPENED					= 1,		// ���� ���� ����
		CLOSED					= 2,		// ���� ���� ����

		MINING_OFF				= 0,		// ä���� ���� ����
		MINING_ON				= 1,		// ä���� ���� ����
		MINING_READY			= 2,		// ä���� ���� �غ� ����

		WEAPON_EMPTY			= 0,		// ������� ���� ������ ����
		WEAPON_CHARGE			= 1,		// ���� ������
		WEAPON_READY			= 2,		// ���� ������ ����
		WEAPON_FIRE				= 3,		// ���� �߻���
	};

	enum JewelType
	{
		// ���� ������ �ٲ�� �ȵ�!!
		// ������ ��ũ��Ʈ�� ��������̴�.
		NO_JEWEL				= 0,		// ���� ����
		RUBY					= 1,		// ���
		EMERALD					= 2,		// ���޶���
		SAPPHIRE				= 3,		// �����̾�
		DIAMOND					= 4,		// ���̾Ƹ��
		BLACKMOON				= 5,		// ����
				
		JEWEL_PROTOTYPE_ID		= 1900,
		JEWEL_KIND				= 5,
		
		JEWEL_TYPE_NUM			= 6,		// ��¡�� ���׷���Ʈ ���� Ÿ�� ������ (���� ����)
	};


	enum CampConst
	{
		CAMP_BUILDING_RADIUS				= 315,		// ķ�� ���� �ݰ� (m)

		CAMP_REPAIR_GOLD_PER_HP				= 10,		// HP ������ 10 ���

		CAMP_UPGRADE_MATERIAL_NUM			= 30,		// ��� ��� �Ѵܰ� ���׷��̵�� �ʿ��� �����
		MINING_CAMP_UPGRADE_MATERIAL_NUM	= 50,		// ä���� �Ѵܰ� ���׷��̵�� �ʿ��� �����
		CAMP_SHOP_UPGRADE_MATERIAL_NUM		= 40,		// ��� ��� ���� �Ѵܰ� ���׷��̵�� �ʿ��� �����
		WEAPON_CHARGE_MATERIAL_NUM			= 50,		// ���� ���� ���� ������ �ʿ��� �����
		CAMP_REPAIR_HP_PER_MATERIAL			= 5000,		// ���� �ϳ��� �����Ǵ� HP

		FAME_FOR_CAMP_BUILDING_TEST			= 0,//1000,		// ��� ��� ����� ����� �ʿ� ��ġ (�׼���)
		FAME_FOR_CAMP_BUILDING				= 0,//2000,		// ��� ��� ����� ����� �ʿ� ��ġ (������)
		FAME_FOR_WEAPON_BUILDING_TEST		= 1000,		// ���� ���� ����� ����� �ʿ� ��ġ (�׼���)
		FAME_FOR_WEAPON_BUILDING			= 2000,		// ���� ���� ����� ����� �ʿ� ��ġ (������)
		FAME_FOR_SELF_DESTROY_CAMP_FOR_USA	= -50,		// ��� ��� �ı��� ���̴� ��ġ (�ڽ��� �ı�(�̱�))
		FAME_FOR_SELF_DESTROY_CAMP			= -50,		// ��� ��� �ı��� ���̴� ��ġ (�ڽ��� �ı�)
		FAME_FOR_DESTROYED_CAMP				= -100,		// ��� ��� �ı��� ���̴� ��ġ (�ٸ� ����� �ı�)

		GOLD_FOR_SELF_DESTROY_CAMP			= 500000,	// ��� ��� �ı��� �����ִ� �� (�ڽ��� �ı�)
		GOLD_FOR_CANCEL_CAMP				= 1000000,	// ��� ��� ���� ������ �����ִ� ��

		CAMP_ATTACK_RANGE					= 45,		// ��� ���� ���� (�ݰ� 45m)

		// �ð� ������Ʈ Ÿ��
		TYPE_REMAIN_TIME					= 0,		// �ð� ���� ������Ʈ
		TYPE_LAST_USE_TIME					= 1,		// ������ ��� �ð� ������Ʈ

		// ä�� ������ Ÿ�� ����
		ACCUMULATED_MINERAL					= 1,		// ������ ���� ������
		TEMPORARY_MINERAL					= 2,		// �ӽ� ������ ���� ������

		MAX_MINERAL_NUM						= 1000,		// ���� �ְ� ���� ����

		// ���� Ÿ�� ����
		MAX_TAX								= 100,		// �ִ� ����

		// ���� ���� ����
		WEAPON_FIRE_WARNING_COUNT			= 10,		// ���� ���� �߻� ��� ǥ�� Ƚ�� (10�� : 10��)
		WEAPON_REBUILD_SIEGE_TIME_COUNT		= 3,		// ���� �ð��� 3�� ������ �ٽ� ������ ���� (�μ��� ������ ���)
		WEAPON_DAMAGE_RANGE					= 256,		// ���� ���� ������ �ݰ� (256m)
		WEAPON_DAMAGE						= 10000		// ���� ���� ���� ������
	};

	enum EmblemConst
	{
		MAX_EMBLEM_UPGRADE					= 3,		// �� ��¡�� ���׷��̵� �ִ�ġ
														// TODO : ���� ���� 2������ �߰��Ǹ� Const::MAX_UPGRADE_NUM �� ����Ѵ�.

		EMBLEM_ATTACK_HEIGHT_ERROR			= 5,		// �������� �ּ� 5m �ȿ� �ִ� ���� ������ ����
	};

	enum GateConst
	{
		GATE_REPAIR_MIN_PER_HP				= 1,		// HP 1�� 1���� ���� �ð�
	};

	enum CastleArmsConst
	{
		CASTLE_ARMS_REPAIR_TIME				= 1,		// ���� ���� ���� �ð� (1��)
		CASTLE_ARMS_REPAIR_GOLD_PER_UNIT	= 10000,	// ���� ��� ������ 10000 ��� ( ������ 10%�� )

		FAME_FOR_DESTROY_CASTLE_ARMS		= -500,		// ���� ���⸦ NPC �� �����Ҷ� ���̴� ��ġ
	};

	enum SiegeArmsConst
	{
		SIEGE_ARMS_REPAIR_TIME				= 1,		// ���� ���� ���� �ð� (1��).

		SIEGE_ARMS_UPGRADE_MATERIAL_NUM		= 10,		// ���� ���� �Ѵܰ� ���׷��̵�� �ʿ��� �����
		SIEGE_ARMS_REPAIR_HP_PER_MATERIAL	= 10,		// ���� �ϳ��� 10% �� HP ����

		AIRSHIP_RIDER_NUM					= 10,		// ���ۼ� �ִ� ž�� �ο�
		AIRSHIP_PASSENGER_NUM				= 9,		// ���ۼ� �°� �ִ� �ο�
	};

	enum Const
	{
		INSIDE								= 0,		// �޹� ���( ������ )
		OUTSIDE								= 1,		// �޹� ���( ������ )

		MAX_UPGRADE_NUM						= 5,		// ���� ������Ʈ ���׷��̵� �ִ�ġ
		UPGRADE_KIND_NUM					= 6,		// ���׷���Ʈ ���� �� (0 ~ 5)

		VIRTUAL_CID_START_NUM				= 1000,		// ��� ���, ���� ������ ���� CID �ε��� ���� ��ȣ

		BROADCAST_CELL_SIZE					= 5,		// ��ε�ĳ��Ʈ �ݰ� (5��)
		BROADCAST_RADIUS					= 150,		// ��ε�ĳ��Ʈ �ݰ� (150m)
		BROADCAST_SQUARED_RADIUS			= 22500,	// ��ε�ĳ��Ʈ �ݰ� ������ (150 x 150)

		MAX_HP_UPDATE_COUNT					= 5,		// 5 ���� �ѹ��� DB �߰輭���� HP ���� ����

		MAX_REPAIR_GOLD						= 99999999,	// ������ �Ҽ� ���� ��ü�� �����Ҷ� ��� ����Ʈ ��� (�������� �ϱ�����)

		MAX_SIEGE_OBJECT					= 5,

		CREATE_LEVEL_LIMIT					= 80
	};

	enum RiderValue
	{
		NOT_RIDER							= 0,		// ž������ �ʾ���
		RIDER_FOR_OWNER						= 1,		// �������� ž���� ����
		RIDER_FOR_PASSENGER					= 2			// �°����� ž���� ����
	};

	enum TimeValue
	{
/*
		// Test : ��� ��� �׽�Ʈ�� �ð�
		DEFAULT_TIME_VALUE			= 1,		//     1 ��
		DEFAULT_REPAIR_TIME			= 30,		//    30 ��

		CAMP_ENCAMPING_TIME			= 1,		//     1 ��
		CAMP_CANCELING_TIME			= 1,		//     1 ��
		CAMP_REPAIR_TIME			= 30,		//    30 ��
		CAMP_TO_STARTKIT_TIME		= 1,		//     1 ��
		CAMP_CHANGING_TIME			= 1,		//     1 ��
		CAMP_LEAST_USE_TIME			= 7,		//     7 ��
		CAMP_ENCAMPING_INTERVAL		= 10,		//    10 �� �������� ������ �޼��� ����
		CAMP_ATTACKED_INTERVAL		= 60000,	// 60000 ms (1��)
		MINING_CAMP_GAIN_COUNT		= 1,		//	   1 ȸ
		CAMP_SHOP_TRANSFER_COUNT	= 1,		//     1 ȸ
		WEAPON_CHARGE_TIME			= 1,		//	   1 ��
*/

		DEFAULT_TIME_VALUE			= 10,		//    10 ��
		DEFAULT_REPAIR_TIME			= 30,		//    30 ��

		CAMP_ENCAMPING_TIME			= 30,		//    30 ��
		CAMP_CANCELING_TIME			= 20,		//    20 ��
		CAMP_REPAIR_TIME			= 30,		//    30 ��
		CAMP_TO_STARTKIT_TIME		= 10,		//    10 ��
		CAMP_CHANGING_TIME			= 10,		//    10 ��
		CAMP_LEAST_USE_TIME			= 7,		//     7 ��
		CAMP_ENCAMPING_INTERVAL		= 10,		//    10 �� �������� ������ �޼��� ����
		CAMP_ATTACKED_INTERVAL		= 60000,	// 60000 ms (1��)
		MINING_CAMP_GAIN_COUNT		= 1,		//     1 ȸ
		CAMP_SHOP_TRANSFER_COUNT	= 1,		//     1 ȸ
		WEAPON_CHARGE_TIME			= 10,		//	  10 ��

		EMBLEM_SUMMON_TIME			= 1,		//	   1 ��
		EMBLEM_ATTACKED_INTERVAL	= 60000,	// 60000 ms (1��)

		SIEGE_ARMS_BUILDING_TIME	= 3,		//     3 ��
		SIEGE_ARMS_TO_STARTKIT_TIME	= 3,		//     1 ��
		SIEGE_ARMS_LEAST_USE_TIME	= 10,		//    10 ��		

		CASTLE_ARMS_LEAST_USE_TIME	= 10,		//	  10 ��
	};

	enum eReturn
	{
		RET_OK					= 0,
		RET_DESTROY_CAMP		= 1,			// ��� ��� ��ü ����
		RET_DESTROY_SIEGE_ARMS	= 2,			// ���� ���� ��ü ����
		RET_CHANGE_TYPE			= 3,			// ��� ��� Ÿ�� ����		
	};

	enum eUpdateHPType
	{
		NOW_HP					= 0,			// ���� HP �״��
		UPGRADE_HP				= 1,			// ���׷��̵�� HP ��
		REPAIR_HP				= 2,			// ������ HP ��
		FULL_HP					= 3,			// Full HP ��
	};


	static int GetKID(unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeType, unsigned char cUpgradeStep)
	{
		int nKID = 0;

		if (Siege::EMBLEM == wObjectType)
		{
			nKID = wObjectType
				   + (cState * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM)
				   + (cUpgradeType * Siege::UPGRADE_KIND_NUM)
				   + cUpgradeStep;
		}
		else
		{
			nKID = wObjectType
				+ (cState * Siege::UPGRADE_KIND_NUM)
				+ cUpgradeStep;
		}

		return nKID;
	}

	static int GetUpgradeMaterialNum(unsigned short wObjectType)
	{
		int nNum = 0;

		switch (wObjectType)
		{
		case Siege::MINING_CAMP:
			nNum = Siege::MINING_CAMP_UPGRADE_MATERIAL_NUM;
			break;

		case Siege::CAMP_SHOP:
			nNum = Siege::CAMP_SHOP_UPGRADE_MATERIAL_NUM;
			break;

		case Siege::CAMP:
		case Siege::KARTERANT_WEAPON:
		case Siege::MERKADIA_WEAPON:
			nNum = Siege::CAMP_UPGRADE_MATERIAL_NUM;
			break;
		}

		return nNum;
	}

	static int GetChangingTypeMaterialNum(unsigned short wBeforeType, unsigned short wAfterType, unsigned char cUpgradeStep)
	{
		int nBeforeNum = GetUpgradeMaterialNum(wBeforeType);
		int nAfterNum = GetUpgradeMaterialNum(wAfterType);

		if (nBeforeNum < nAfterNum)
		{
			return ( (nAfterNum - nBeforeNum) * cUpgradeStep );
		}

		return 0;
	}
}

#endif	// _SIEGE_CONSTANTS_H_