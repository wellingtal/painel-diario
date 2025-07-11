#ifndef _DBAGENT_CASTLE_PACKET_PARSE_H_
#define _DBAGENT_CASTLE_PACKET_PARSE_H_

// ���� ����
struct PktBase;
class CDBAgentDispatch;
class CCharacter;

namespace DBAgentPacketParse
{
	// ���� ������Ʈ
	bool ParseCastleInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// �� ������ ������Ʈ
	bool ParseCampInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� ��� ������ ������Ʈ
	bool ParseFertilityInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ���� ������ ������Ʈ
	bool ParseWorldWeaponInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ���� ���� ������Ʈ

	bool ParseCreateCamp(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ���� ������ ��� ���
	bool ParseCreateSiegeArms(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ���� ������ ���� ����

	bool ParseCastleCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// �� ���� ��� ��Ŷ
	bool ParseCampCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� ��� ��� ��Ŷ
	bool ParseSiegeArmsCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ���� ���� ��� ��Ŷ

	bool ParseCastleRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// �� ���� ���� ����
	bool ParseCampRight(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// ��� ��� ���� ���� ����

	bool ParseGameTimeInfo(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ���� �ð� ���� ��Ŷ
	bool ParseCastleTaxMove(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// �� �ӽ� ���� -> ���� ���� �̵� ���� ������Ʈ
	bool ParseCastleSiegeCount(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);			// ���� �ð� �� ���� ���� ���� Ƚ�� ���� ������Ʈ

	bool ParseCampMessage(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ��� ��� ���� �޼��� ó��
	bool ParseProcessMining(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// ä���� ���μ��� ����

	bool ParseWarOnOff(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// �����, ������ �÷��� ó��

	bool ParseRealmPoint(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);				// �������� �������� ����Ʈ.

	void RealmInchantAdd(CCharacter* lpCharacter);												// �������� �������� ����Ʈ ��æƮ ���.
	void RealmInchantRemove(CCharacter* lpCharacter);											// �������� �������� ����Ʈ ��æƮ ����.

	bool ParseStatueCmd(CDBAgentDispatch& DBAgentDispatch, PktBase* lpPktBase);					// �������� ���� Ŀ�ǵ� ��Ŷ ó��

	void SiegeMovePos(CCharacter* pCharacter);													// ������ ĳ���� ������ ó��.	
};

#endif
