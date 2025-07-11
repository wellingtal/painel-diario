#ifndef _CHARACTER_STRUCTURE_H_
#define _CHARACTER_STRUCTURE_H_

#include <Creature/CreatureStructure.h>


// ���� ����
class CCharacter;

// -----------------------------------------------------------------------
// DB ���� ó���� �о� ���� �����͵�. �߰��߰� �����ؼ� DB�� ������.
// �ǵ����̸� ���� ó���߿��� �ǵ帮�� �ʴ´�. ( ������ ����� �����ϰ� ... )

struct CharacterDBData
{
	CHAR_INFOST		m_Info;			// �ܸ�, �̸�, �ɷ�ġ, hp,mp �� �⺻ ����
	CHAR_POS		m_Pos;			// ����� ��ġ ����,  ����� ��ġ ���� ( Respawn ��� )
	SKILL			m_Skill;
	QUICK			m_Quick;
	SPELL			m_Spell;
	unsigned char   m_cAdminLevel;
};
typedef CharacterDBData* LPCharacterDBData;


// ���� ���� ����. ������ IP�� ����.
struct ConnectInfo
{
	SOCKADDR_IN	m_siAgentHost;		// agent UDP address of host
	SOCKADDR_IN	m_siPrivateHost;	// private UDP address of host
	SOCKADDR_IN	m_siPublicHost;		// public UDP address of host
};
typedef ConnectInfo* LPConnectInfo;


// ĳ���ͳ����� ���� ���� (���, ����)
struct CharacterFightInfo
{
	CCharacter*		m_pDuelOpponent;

	POS				m_Pos;
	unsigned long	m_dwCellID;
	__int64			m_nRestoreExp;	// ��ų ���������� ���� ��Ȱ�� ��� �������� ����ġ�� ������ �� �ִ�.

	CharacterFightInfo();
};


#endif