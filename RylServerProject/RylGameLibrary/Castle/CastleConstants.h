#ifndef _CASTLE_CONSTANTS_H_
#define _CASTLE_CONSTANTS_H_

namespace Castle
{
	enum Type
	{
		NONE			= 0,
		CASTLE			= 1,
		CAMP			= 2
	};

	enum IndentifyBit
	{
		STATUE_BIT		= 0x80000000,	// ��ũ ī���� ���� (���� ��Ʈ�� ����.)
		CASTLE_BIT		= 0x40000000,	// ��
		CAMP_BIT		= 0x20000000,	// ��� ���
		TOWN_BIT		= 0x10000000,	// ����

		ABTOWN_BIT		= 0x0F000000,	// �����Ƽ�� �߰� Ÿ���Ʈ

		SIEGE_BIT		= 0x00FF0000,	// �����ð��� ������ ������ ����ϴ� �ӽ� ������ ����Ʈ ��Ʈ
										// CASTLE_BIT | SIEGE_BIT �� ���!!
										// ex) 0x40FF1001 : CastleNameID �� 1 �̰� �ӽ� ������ ����Ʈ 2��

		CASTLE_NAME_BIT_SHIFT	= 12	// �ٸ� ä�ο� ���� �����Ҽ� �ֱ� ������ ��ũ��Ʈ�� ���� �� ID ���
										// CASTLE_BIT | (CastleNameID << CASTLE_NAME_BIT_SHIFT) �� ���� ����Ѵ�.
										// ex) 0x40001000 : CastleNameID �� 1 �̴�.
	};

	enum TaxType
	{
		TRADE_TAX			= 0,	// ���� ����/�Ǹ�
		REPAIR_TAX			= 1,	// ���� ����
		RESMELT_TAX			= 2,	// �������� ���
		CAMP_GOLD_TAX		= 3,	// ��� Gold
		CAMP_MINERAL_TAX	= 4,	// ��� ����

		MAX_TAX_TYPE		= 5
	};

	enum TaxChangable
	{
		TAX_DISABLE		= 0,		// ���� ���� �Ұ���
		TAX_ENABLE		= 1			// ���� ���� ����
	};

	enum MineralType
	{
		ACCUMULATED_MINERAL					= 1,		// ������ ���� ������
		TEMPORARY_MINERAL					= 2,		// �ӽ� ������ ���� ������
	};

	enum CastleName
	{
		TEMP_CASTLE_NAME_0					= 0,
		TEMP_CASTLE_NAME_1					= 1,
		TEMP_CASTLE_NAME_2					= 2,
		TEMP_CASTLE_NAME_3					= 3,
		TEMP_CASTLE_NAME_4					= 4,
		
		MAX_CASTLE_NAME_NUM					= 5,		
		MAX_CASTLE_NAME_LEN					= 32
	};

	enum CastleTitle
	{
		TITLE_NEW							= 0,	// Īȣ '���ο�'
		TITLE_SETTLED						= 1,	// Īȣ '������'
		TITLE_FLOURISHED					= 2,	// Īȣ '��â��'
		TITLE_HONORABLE						= 3,	// Īȣ '���ο�'
		TITLE_GLORIOUS						= 4,	// Īȣ '������'

		MAX_TITLE_NUM						= 5
	};

	enum Const
	{
		CASTLE_EXP_BONUS_RADIUS				= 300,		// ���� ����ġ ���� �ݰ�

		MAX_CASTLE_NUM						= 10,
		MAX_CASTLE_MINERAL_NUM				= 200,
		MAX_CASTLE_OBJECT_NUM				= 250,

		MAX_TEMP_TAX_UPDATE_COUNT			= 10,
		MAX_TEMP_TAX_AMOUNT					= 50000,

		EMBLEM_UPGRADE_JEWEL_NUM			= 90,
		EMBLEM_UPGRADE_JEWEL_NUM_PER_POS	= 10,
		EMBLEM_UPGRADE_JEWEL_POS_NUM		= 9,

		FAME_FOR_CAPTURE_CASTLE				= 10000,	// �������� ���� ������ �������� ��� ��ġ
		FAME_FOR_FRIENDLY_GUILD				= 5000,		// �������� ���� ������ ��ȣ����� �������� ��� ��ġ

		INVINCIBLE_COUNT					= 0,		// ���� ����� �־����� ���� ���� Ƚ�� (0ȸ)
		TEMP_TAX_GAIN_COUNT					= 1,		// �ӽ� ������ ���� �������� �ű�� �ʿ��� ���� Ƚ�� (1ȸ)

		DAY_END_TIME						= 0			// �Ϸ簡 ������ �ð�	(�� 12��)
	};
}

#endif	// _CASTLE_CONSTANTS_H_
