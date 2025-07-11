#ifndef _GAME_TIME_CONSTANTS_H_
#define _GAME_TIME_CONSTANTS_H_

namespace GameTime
{
	// ���� �ð� Ÿ��
	enum TimeType
	{
		TYPE_NONE	= 0,
		GUILD		= 1,			// ����� �ð�
		REALM		= 2,			// ������ �ð�
		SIEGE		= 3,			// ���� �ð�
		MINE		= 4,			// ���� �ֱ������� �ӽ� ä��
		NEWZONE		= 5,			// �׼��� �ű��� ��������

		EVENT		= 6,			// ���Ӱ��� �̺�Ʈ

		MAX_GAME_TIME_TYPE	= 7
	};


	// �ð� �ֱ�
	enum Cycle
	{
		CYCLE_NONE		= 0,
		WEEKLY			= 1,		// ���� Ư�� ���� Ư�� �ð���
		DAILY			= 2,		// ���� Ư�� �ð���
		MONTHLY			= 3,		// �ſ� Ư�� ��¥ Ư�� �ð���
		DATE			= 4,		// Ư�� ��¥ Ư�� �ð���

		MAX_TIME_CYCLE_TYPE	= 5
	};


	// ����
	enum Week
	{
		WEEK_NONE	= 0,
		SUNDAY		= 1,
		MONDAY		= 2,
		TUESDAY		= 3,
		WEDNESDAY	= 4,
		THURSDAY	= 5,
		FRIDAY		= 6,
		SATURDAY	= 7,
	};


	// ����� �ð� ����
	enum GuildTime
	{
		GT_NORMAL				= 0,	// �Ϲ�
		GT_PREPARE_START_30		= 1,	// ����� ���� 30����
		GT_PREPARE_START_5		= 2,	// ����� ���� 5����
		GT_GUILD_WAR			= 3,	// ����� �ð�
		GT_PREPARE_END_30		= 4,	// ����� ���� 30����
	};


	// ������ �ð� ����
	enum RealmTime
	{
		RT_NORMAL				= 0,	// �Ϲ�
		RT_PREPARE_START_30		= 1,	// ������ ���� 30����
		RT_PREPARE_START_15		= 2,	// ������ ���� 15����
		RT_PREPARE_START_10		= 3,	// ������ ���� 10����
		RT_PREPARE_START_5		= 4,	// ������ ���� 5����
		RT_REALM_WAR			= 5,	// ������ �ð�
		RT_REALM_WAR_END_5		= 6,	// ������ ���� 5����. 
	};


	// ������ �ð� ����
	enum SiegeTime
	{
		ST_NORMAL				= 0,	// �Ϲ�
		ST_PREPARE_START_5		= 1,	// ������ ���� 5����
		ST_SIEGE_WAR			= 2,	// ���� �ð�
		ST_PREPARE_END_5		= 3,	// ������ ���� 5����
	};

	// �ű��� �ð� ����
	enum NewZoneTime
	{
		ET_LIMIT				= 0,	// ���� �Ұ�
		ET_PREPARE_START_5		= 1,	// ���� 5����
		ET_ALLOWENTER			= 2,	// ���� ����
		ET_PREPARE_END_5		= 3,	// ���� 5����
	};

	// �ӻ� ä�� �ð� ����
	enum MiningTime
	{
		MT_NORMAL				= 0,	// �Ϲ�
		MT_MINING				= 1,	// ä������ �ð� (MT_MINING -> MT_NORMAL �� �ٲ� �ӽ� ä����)
	};

	// ���� �̺�Ʈ ����
	enum EventTime
	{
		EVENT_NORMAL				= 0,	// �̺�Ʈ �ð��� �ƴϴ�.
		EVENT_All20					= 1,	// Value1 �� ���� �����Ǵ� ������ Value1 ���� �̺�Ʈ ���̵ȴ�.
		EVENT_All50					= 2,	// Value1 �� ���� �����Ǵ� ������ Value1 ���� �̺�Ʈ ���̵ȴ�.
	};

	// GameTimeInfo ��Ŷ ����
	enum GameTimeInfoType
	{
		TYPE_GAMESERVER_LOGIN		= 0,		// ���Ӽ����� �߰輭���� ����ɶ� ���� �ð� ���� ����
		TYPE_CHARACTER_LOGIN		= 1,		// ĳ���Ͱ� ���Ӽ����� �α��ν� ���� �ð� ���� ����
		TYPE_CHANGE_GAME_TIME		= 2,		// ���� �ð��� ����Ǿ ������Ʈ��
		TYPE_ENABLE_CASTLE_SIEGE	= 3,		// Ư�� ���� ���� ���� ���� �����
		TYPE_UPDATE_TIME			= 4,		// Ŭ���̾�Ʈ���� �ֱ������� �ð� ���� ������Ʈ��
	};
}

#endif	//_GAME_TIME_CONSTANTS_H_