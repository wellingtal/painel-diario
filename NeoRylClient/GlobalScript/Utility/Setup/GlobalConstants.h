#ifndef _GLOBAL_CONSTANTS_H_
#define _GLOBAL_CONSTANTS_H_

namespace GameRYL
{
	enum GameContents
	{
		LEVEL_LIMIT_80		= (1 << 0),											// 80���� ����
		LEVEL_LIMIT_90		= (1 << 1) | LEVEL_LIMIT_80,						// 90���� ����
		LEVEL_LIMIT_100		= (1 << 2) | LEVEL_LIMIT_90 | LEVEL_LIMIT_80,		// 100���� ����

		CAMP				= (1 << 3),											// ���
		CAMP_UPGRADE		= (1 << 4) | CAMP,									// ��� ��ȭ
		SIEGE				= (1 << 5) | CAMP_UPGRADE | CAMP,					// ������

		JUDGE_PROTOTYPE		= (1 << 6),											// ���Ʊ� �ĺ� (�ʱ� ����)

		ACCESSORY			= (1 << 7),											// �Ǽ��縮
		RUNE				= (1 << 8),											// ��
		BATTLE_LOHAN		= (1 << 9),											// ��Ʋ ����
		STONE_BATTLE		= (1 << 10),										// ������
		LOTTO				= (1 << 11),										// �븸 �ζ�.
		SHOW_ENEMYNAME		= (1 << 12),										// ���� ĳ���͸� ǥ��
		NEWZONE_ZONE9		= (1 << 13),										// �ű���
		REBALANCE_OVERITEM	= (1 << 14),										// ���������� ����
		FIFTHSKILL_LOCKITEM	= (1 << 15)											// 5�ܰ� ��ų �� ������
	};

	enum ServiceNation
	{
		KOREA				= 0,		// �ѱ� (����Ʈ)
		GLOBAL				= 1,		// �۷ι�
		CHINA				= 2,		// �߱�
		THAILAND			= 3,		// �±� IME������ ����
		JAPAN				= 4,		// �Ϻ� ��¿붧���� ����
	
		MAX_NATION_TYPE		= 5
	};

	// Ŭ���̾�Ʈ�� (�� �������� �˸°� Ŭ���̾�Ʈ ��ƾ �и��� ����...)
	enum ServerType
	{
		SERVER_TEST				= 0,	// �׽�Ʈ ����
		SERVER_REGULAR			= 1,	// ���� ����
		SERVER_BATTLE_LOHAN		= 2,	// ��Ʋ ���� ����

		MAX_SERVER_TYPE			= 3
	};
};

#endif