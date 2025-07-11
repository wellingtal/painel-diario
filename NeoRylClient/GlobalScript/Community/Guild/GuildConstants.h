
#ifndef _GUILD_CONSTANTS_H_
#define _GUILD_CONSTANTS_H_

namespace Guild
{
	enum Const
	{
		MAX_GUILD_NAME_LEN			= 11,				// ��� �̸��� �ִ� ���� (���� ����)
		MAX_GUILD_NAME_FOR_DB		= 20,				// ��� �̸��� �ִ� ���� (DB �����)
		MAX_MEMBER_NAME_LEN			= 16,				// ���� �̸��� �ִ� ���� (�Ϲ����� ĳ���� �̸��� �ִ� ���̿� ����)

		MAX_MARK_SIZE				= 18 * 12 * 2 + 1,	// ��� ��ũ ũ�� (18 * 12 ũ���� 16��Ʈ Į���� BMP ����, ù 1����Ʈ�� ��ũ ���� ����)
		MAX_RIGHT_SIZE				= 50,				// ��� ���� ����ü ũ�� 

		MAX_MEMBER_NUM				= 100,				// ��� ���� ���� �� �ִ� �ִ� ũ�� (���Դ���� ������ �������� �� �д�.)
		MAX_RELATION_NUM			= 255,				// ��ȣ/���� ���� ����Ʈ�� �ִ� ũ�� (ī���� ���� ��û������ �������� �� �д�.)
		MAX_HOSTILITY_APPLY_NUM		= 49,				// ������� ������ �ִ� �ִ� ���� ���� ��
		MAX_HOSTILITY_NUM			= 1,				// �ڽ��� ���� ������ �� �ִ� �ִ� ��
		MAX_ALERT_NUM				= 1,				// �ڽ��� ��� ���� ������ �� �ִ� �ִ� ��

		MAX_LEVEL					= 5,				// ����� �ִ� ����
		CREATE_LEVEL				= 30,				// ��� ������ �䱸�Ǵ� �ּ� ĳ���� ����
		CREATE_LEVEL_FOR_CHINA		= 15,				// (�߱���) ��� ������ �䱸�Ǵ� �ּ� ĳ���� ����
		SET_MARK_GOLD				= 5000000,			// ��ũ ����� �ʿ��� �ݾ�
		FAME_LEAVE_PENALTY			= 1000,				// ��帶������ ���� ���� Ż��� ���̴� ��

		MAX_SMALL_NODE_NUM			= 500,				// �ѹ��� ���� ������ ��� ���� ������ �ִ� ����
		MEMBER_INFO_UPDATE_COUNT	= 10,				// ���� ������ �ٸ� ������ ������Ʈ�ϴ� Ÿ�̹�
		DELETE_GUILD_MEMBER_NUM		= 4,				// ��尡 ������ �� �ִ� �ּ� �ο�

		MINUTES_PER_HOUR			= 60,				

		// �ؿ��� ��û�� �����Ͽ� Ż�� ��� �ð��� ���������ϴ�. (2005-06-13 by �ε�)
		LEAVE_WAIT_TIME				= 0,				// Ż�� ��� �Ⱓ (���� : ��)
		LEAVE_WAIT_TIME_FOR_CHINA	= 10080,			// (�߱���) Ż�� ��� �Ⱓ (���� : ��)
		DELETE_GUILD_LOGOUT_TIME	= 43200,			// ������ �α������� �ʾ� ��尡 ��ü�� �������� �Ⱓ (���� : ��)
		MEMBER_NUM_DELETE_TIME		= 1440,				// ������ DELETE_GUILD_MEMBER_NUM �̸��� �� �� ��� ��ü������ �Ⱓ (���� : ��)
		TRANFER_MASTER_TIME			= 20160,			// �������� �������� ���� ��帶������ ���� �絵 �Ⱓ (���� : ��)
		CHANGE_INCLINATION_TIME		= 1440,				// ��� ���� ���� �� �ٽ� ������ �� ���� �������� �Ⱓ (���� : ��)
		RELATION_WAIT_TIME			= 1440,				// ���� ���� ��� �ð� (���� : ��)

/*
		// Test : ��� �׽�Ʈ�� ���� �ð�
		LEAVE_WAIT_TIME_FOR_CHINA	= 10,				// (�߱���) Ż�� ��� �Ⱓ (���� : ��)
		DELETE_GUILD_LOGOUT_TIME	= 30,				// ������ �α������� �ʾ� ��尡 ��ü�� �������� �Ⱓ (���� : ��)
		MEMBER_NUM_DELETE_TIME		= 15,				// ������ DELETE_GUILD_MEMBER_NUM �̸��� �� �� ��� ��ü������ �Ⱓ (���� : ��)
		TRANFER_MASTER_TIME			= 20,				// �������� �������� ���� ��帶������ ���� �絵 �Ⱓ (���� : ��)
		CHANGE_INCLINATION_TIME		= 10,				// ��� ���� ���� �� �ٽ� ������ �� ���� �������� �Ⱓ (���� : ��)
		RELATION_WAIT_TIME			= 1,				// ���� ���� ��� �ð� (���� : ��)
*/

		MEMBER_WAIT_BIT				= 0x80000000,		// ���� ����ڿ��� ��ũ�� �������ϰ� �ϴ� �÷���

		TIME_GUILD_POINT			= 30,				// ����� ������ �ð�(�д���).

		TACTICE_JON_LEVEL			= 1,				// �뺴 ���� ����.

		TACTICS						= 1,			// �뺴.
		TACTICS_WAIT				= 2,			// �뺴 ���� �����.
	};

	enum Title
	{
		NONE			= 0, 
		MASTER			= 1,			// ��� ������
		MIDDLE_ADMIN	= 2,			// �߰� ������
		COMMON			= 3,			// �Ϲ� ����
		LEAVE_WAIT		= 4,			// Ż�� �����
		JOIN_WAIT		= 5,			// ���� �����		

		MAX_TITLE		= 6
	};

	enum MemberType
	{
		TYPE_MEMBER		= 0,			// ���
		TYPE_TACTICS	= 1				// �뺴
	};

	enum GuildSort
	{
		GUILD_FAME			= 0,		// ����(��)
		GUILD_NAME			= 1,		// ����
		GUILD_HOSTILITY		= 2,		// ������
		GUILD_NEUTRALITY	= 3,		// ��������
		GUILD_MINE			= 4,		// �ڽ��� ���

		GUILD_REQUEST		= 5			// ��� ����Ʈ ��û (�� �α� � ���� ������ ���� ��û)
	};

	enum MemberSort
	{
		MEMBER_TITLE		= 0,		// ������
		MEMBER_CLASS		= 1,		// Ŭ������ ��Ʈ
		MEMBER_LEVEL		= 2,		// ������ ��Ʈ
		MEMBER_FAME			= 3,		// ���� ��Ʈ
		MEMBER_NAME			= 4,		// �̸��� ��Ʈ
		MEMBER_GOLD			= 5,		// �����ݾ׺� ��Ʈ
		MEMBER_POSITION		= 6,		// ��ġ�� ��Ʈ
		MEMBER_JOIN_WAIT	= 7,		// ��û���� �÷��̾�
		MEMBER_WAR_ON		= 8,		// ��� ���� �������� ��� (�̸������� ��Ʈ)
		MEMBER_WAR_OFF		= 9,		// ��� ���� �������� ��� (�̸������� ��Ʈ)
		TACTICS_JOIN_WAIT	= 10,		// ��û���� �뺴 (���� ���� ��)
		TACTICS_ACTIVE		= 11		// Ȱ������ �뺴 (���� ���� ��)
	};

	enum Relation
	{
        NEUTRALITY			= 0,			// �߸� ����
		HOSTILITY			= 1,			// ����
		COUNTER_HOSTILITY	= 2,			// ī���� ����
		ALERT_HOSTILITY		= 3,			// ��� ����
		MINE				= 4,			// �ڽ��� ���
		TARGET_NEUTRALITY	= 5,			// A->B���븦 �ϸ� DB���� 1�� ����ǰ� �̻��¿��� �ǽ��� �����ϸ�
											// �ش簪�� 5�� �ٲ��. 5�� �ٲ�ڿ��� B��尡 ���뼱������� ���ָ�
											// �̶� DB���� ����� ������ ��ȭ���谡 �ȴ�.

		ALL_HOSTILITY		= 6				// ��� ���� ����
	};

	enum RelationActor
	{
		BY_MINE				= 0,			// �ڽ��� ���� ��� ����
		BY_TARGET			= 1,			// ������κ��� ���� ��� ����

		MAX_RELATION_ACTOR	= 2
	};

	// �߰� ���� ����
	enum TimeUpdateType
	{
		// �����ÿ� CDBObject::UpdateGuildTime �Լ��� ����

		LAST_LOGOUT		= 0,			// ������ ���������� �α׾ƿ��� �ð�
		MEMBER_NUM_DOWN	= 1,			// ���� ���� 3�� ���ϰ� �� �ð�	
		MASTER_LOGOUT	= 2				// �����Ͱ� ���������� �α׾ƿ��� �ð�
	};

	enum ErrType
	{
		S_SUCCESS			= 0,

		// CGuild::SetTitle �Լ���
		E_NOT_MEMBER		= 1,		// ������ �ƴ�
		E_DISSOLVE_GUILD	= 2,		// ��� ��ü
		E_NOT_CHANGE		= 3,		// ����� �� ����
		E_DB_ERROR			= 4,		// DB ����
		E_NOT_TEMP_MASTER	= 5			// �ӽ� �����Ͱ� �� ������ ����
	};

	enum AddFameType
	{
		// CGuildDB::AddAllMemberFame, AddFriendGuildMemberFame, AddMemberFame �Լ���

		TYPE_VALUE			= 0,		// Fame ���� �״�� �����ش�.
		TYPE_HALF			= 1,		// ��ġ�� 1/2�� �����.
		TYPE_QUARTER		= 2			// ��ġ�� 1/4�� �����.
	};

	// ���� ���� ����
	enum MemberInfoUpdateType
	{
		// CGuild::UpdateMemberInfo �Լ� ���� 

		TYPE_LEVEL		= 0,
		TYPE_CLASS		= 1,
		TYPE_FAME		= 2,
		TYPE_GOLD		= 3,
		TYPE_WARFLAG	= 4
	};

	const unsigned char ms_aryMaxMemberNum[MAX_LEVEL] 			= { 10,			20,			30,			40,			50			};
	const unsigned long ms_arySetLevelFame[MAX_LEVEL] 			= { 0,			1000,		3000,		5000,		10000		};
	const unsigned long ms_arySetLevelFameForUSA[MAX_LEVEL] 	= { 0,			400,		2000,		5000,		10000		};
	const unsigned long ms_arySetLevelGold[MAX_LEVEL] 			= { 1000,	5000,	20000,	100000,	250000	};
	const unsigned long ms_arySetLevelGoldForChina[MAX_LEVEL]	= { 1000,	5000,	20000,	100000,	250000	};
};

#endif