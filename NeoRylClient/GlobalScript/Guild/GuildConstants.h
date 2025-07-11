
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

		MAX_MEMBER_NUM				= 50,				// ��� ���� ���� �� �ִ� �ִ� ũ��(ms_aryMaxMemberNum[MAX_LEVEL - 1] ���� �����ؾ� �Ѵ�.)
		MAX_RELATION_NUM			= 20,				// ��ȣ/���� ���� ����Ʈ�� �ִ� ũ��

		MAX_LEVEL					= 5,				// ����� �ִ� ����
		CREATE_LEVEL				= 30,				// ��� ������ �䱸�Ǵ� �ּ� ĳ���� ����
		SET_MARK_GOLD				= 5000000,			// ��ũ ����� �ʿ��� �ݾ�

		MAX_SMALL_NODE_NUM			= 500,				// �ѹ��� ���� ������ ��� ���� ������ �ִ� ����
		MEMBER_INFO_UPDATE_COUNT	= 10,				// ���� ������ �ٸ� ������ ������Ʈ�ϴ� Ÿ�̹�
		DELETE_GUILD_MEMBER_NUM		= 4,				// ��尡 ������ �� �ִ� �ּ� �ο�

		DELETE_GUILD_LOGOUT_TIME	= 30,				// ������ �α������� �ʾ� ��尡 ��ü�� �������� �Ⱓ (���� : ��)
		MEMBER_NUM_DELETE_TIME		= 1,				// ������ DELETE_GUILD_MEMBER_NUM �̸��� �� �� ��� ��ü������ �Ⱓ (���� : ��)
		TRANFER_MASTER_TIME			= 14,				// �������� �������� ���� ��帶������ ���� �絵 �Ⱓ (���� : ��)
		LEAVE_WAIT_TIME				= 24,				// Ż�� ��� �Ⱓ (���� : �ð�)
		
		MEMBER_WAIT_BIT				= 0x80000000		// ����/Ż�� ����ڿ��� ��ũ�� �������ϰ� �ϴ� �÷���
	};

	enum Title
	{
		NONE			= 0, 
		MASTER			= 1,					// ��� ������
		MIDDLE_ADMIN	= 2,					// �߰� ������
		COMMON			= 3,					// �Ϲ� ����
		LEAVE_WAIT		= 4,					// Ż�� �����
		JOIN_WAIT		= 5,					// ���� �����

		MAX_TITLE		= 6
	};

	enum Relation
	{
		NEUTRALITY		= 0,					// �߸� ����
		FRIENDLY		= 1,					// ��ȣ ����
		HOSTILITY		= 2,					// ���� ����
		MINE			= 3						// �ڽ��� ���
	};

	// �߰� ���� ����
	enum TimeUpdateType
	{
		// �����ÿ� CDBObject::UpdateGuildTime �Լ��� ����

		LAST_LOGOUT		= 0,					// ������ ���������� �α׾ƿ��� �ð�
		MEMBER_NUM_DOWN	= 1,					// ���� ���� 3�� ���ϰ� �� �ð�	
		MEMBER_NUM_UP	= 2,					// ���� ���� 4�� �̻��� �� �ð�	
		MASTER_LOGOUT	= 3						// �����Ͱ� ���������� �α׾ƿ��� �ð�
	};

	// ���� ���� ����
	enum MemberInfoUpdateType
	{
		// CGuild::UpdateMemberInfo �Լ� ���� 

		TYPE_LEVEL		= 0,
		TYPE_CLASS		= 1,
		TYPE_FAME		= 2,
		TYPE_GOLD		= 3
	};

	enum Inclination
	{
		// ����
		RACE_HUMAN			= 0x20,		// �ΰ�
		RACE_AKHAN			= 0x40,		// ��ĭ
		RACE_BOTH			= 0x60,		// ���� ���

		// ����
		NATION_KARTERANT	= 0x04,		// ī���׶�Ʈ
		NATION_MERKADIA		= 0x08,		// �޸�ī���
		NATION_THIRD		= 0x10,		// ��3����
		NATION_STATELESS	= 0x00,		// ������

		// ����/��ȭ ���
		MODE_WAR			= 0x01,		// ���� ���
		MODE_PEACE			= 0x02,		// ��ȭ ���

		// �� ������ ���� ��Ʈ��
		RACE_BIT			= 0x60,		// ����
		NATION_BIT			= 0x1C,		// ����
		MODE_BIT			= 0x03		// ����/��ȭ ���
	};
	
	inline unsigned char	Inclination2Race( unsigned char inclination ){ return (inclination&RACE_BIT); };
	inline unsigned char	Inclination2Nation( unsigned char inclination ){ return (inclination&NATION_BIT); };
	inline unsigned char	Inclination2PeaceMode( unsigned char inclination ){ return (inclination&MODE_BIT); };


	const unsigned char ms_aryMaxMemberNum[MAX_LEVEL] = { 10, 20, 30, 40, 50 };
	const unsigned long ms_arySetLevelFame[MAX_LEVEL] = { 0, 2000, 10000, 25000, 50000 };
	const unsigned long ms_arySetLevelGold[MAX_LEVEL] = { 1000000, 5000000, 10000000, 50000000, 100000000 };
};

#endif