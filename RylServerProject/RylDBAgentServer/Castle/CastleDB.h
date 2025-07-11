#ifndef _RYL_DBAGENT_CASTLE_DB_H_
#define _RYL_DBAGENT_CASTLE_DB_H_

#pragma once

#include <map>
#include <Castle/CastleConstants.h>
#include <Creature/CreatureStructure.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

// ���� ����
class CSiegeObjectDB;
class CDBComponent;

namespace Castle
{
	class CCastleDB
	{
	public:

		~CCastleDB();

		// Ÿ�� ����
		typedef std::map<unsigned short, unsigned short>		MineralInfoMap;			// <wMineralID, wAmount>
		typedef	std::map<unsigned long, CSiegeObjectDB*>		CastleObjectMap;		// <ObjectCID, lpCastleObject>

		bool	ChangeCastleMaster(unsigned char cNation);						// ���� ����
		void	LoseOwnership();												// �� ������ �߸�ȭ
		
		void	CloseCastleGate();												// ������ �ݴ´�.
		void	OpenCastleGate();												// ������ ����. (�μ��� ��� ����)
		
		void	ChangeCastleObjectNation(unsigned char cNation);				// ���� ������Ʈ���� ������ ����
		void	DestroyAllCastleArms();											// ���� ������� ���� ���� NPC �� �����.
		
		bool	InsertCastleObject(CSiegeObjectDB* lpCastleObject);				// �� ������Ʈ �߰�
		bool	DeleteCastleObject(unsigned long dwCID);						// ���� ���� ����
		bool	DeleteCastleObject(CSiegeObjectDB* lpCastleObject);				// ���� ���� ����
		
		CSiegeObjectDB*	GetCastleEmblem();
		CSiegeObjectDB* GetCastleGate();
		CSiegeObjectDB*	GetCastleObject(unsigned long dwCastleObjectID);

		void	CheckEnableSiege(bool bIsSiegeTime);
		
		// ���� �ð��� ���� �� ���ŵ� �� ���� ����
		void	UpdateSiegeCount();
		
		void	SetTax(unsigned char cType, unsigned char cTax);					// ���� ����
		void	TakeTaxMoney(unsigned char cType, unsigned long dwTakeTaxMoney);	// ���� ȸ��
		bool	GainMineral(unsigned short wMineralID, unsigned short wAmount);		// ���� ���� ȸ��
		bool	SetTempTaxMoney(unsigned long* dwTempTaxMoney, unsigned long* dwTaxMoney);	// ���� �����κ��� �ӽ� �������� ������Ʈ
		bool	SetMineralInfo(unsigned char cMineralType, unsigned char cNum, CastleMineral* lpMineralInfo);	// ���� �����κ��� ���� �������� ������Ʈ
		bool	InsertMineralInfo(unsigned char cMineralType, const CastleMineral& mineralInfo);				// Mineral �߰�
		void	SetRight(CastleRight castleRight);									// ���� ���� ����
		void	SetUpgradeItemInfo(unsigned short wItemID, unsigned char cItemNum);
		
		bool	SetTaxInfo(const CastleTaxInfoDB& taxInfo);						// DB ���� �о���� Tax ���� ����
		bool	SetMineralInfo(const CastleMineralInfoDB& mineralInfo);			// DB ���� �о���� Mineral ���� ����
		        
		unsigned long	GetCastleID() const						{ return m_dwCastleID;			}
		unsigned long	GetNation() const						{ return m_cNation;				}
		unsigned char	GetZone() const							{ return m_cZone;				}
		unsigned char	GetChannel() const						{ return m_cChannel;			}

		unsigned char	GetTax(unsigned char cType) const		{ return m_CastleTax[cType].m_cTax;			}
		unsigned long	GetTaxMoney(unsigned char cType) const	{ return m_CastleTax[cType].m_dwTaxMoney;	}
		unsigned char	GetSiegeCount() const					{ return m_cSiegeCount;						}
		unsigned char	GetInvincibleCount() const				{ return m_cInvincibleCount;				}
		unsigned short	GetTotalSiegeCount() const				{ return m_wTotalSiegeCount;				}
		unsigned long	GetTotalTaxMoney() const				{ return m_dwTotalTaxMoney;					}
		bool			GetEnableSiege() const					{ return m_bEnableSiege;					}

	private:

		CCastleDB(CDBComponent& DBComponent, const CastleInfoDB& CastleInfo);

		CastleObjectMap	m_CastleObjectMap;						// �� ���� ������Ʈ��
		
		unsigned long	m_dwCastleID;							// �� Ȥ�� ���� ID
		unsigned char	m_cNation;								// ���� ������ ����
		unsigned char	m_cZone;								// ���� �ִ� �� ��ȣ
		unsigned char	m_cChannel;								// ���� �ִ� ä�� ��ȣ
		unsigned char	m_cNameID;								// ���� �̸� ID ( 1����~~ )
		unsigned char	m_cSiegeCount;							// ���� Ƚ��
		unsigned char	m_cInvincibleCount;						// ���� Ƚ��
		unsigned short	m_wTotalSiegeCount;						// ���� ���� Ƚ��
		unsigned long	m_dwTotalTaxMoney;						// ���� ���� ȸ����
		unsigned short	m_wItemID;								// ���׷��̵忡 ����� ���� ������ ID
		unsigned char	m_cItemNum;								// ���׷��̵忡 ����� ���� ������ ����
		
		CastleRight		m_CastleRight;							// �� ���� ����
		Position		m_BackDoorPos[2];						// �޹� ���� ��/����ġ
        		
		CastleTaxInfo	m_CastleTax[Castle::MAX_TAX_TYPE];		// ���� �� ���� ����

		MineralInfoMap	m_AccumulatedMineralMap;				// ���� ���� ����
		MineralInfoMap	m_TemporaryMineralMap;					// �ӽ� ���� ����

		bool			m_bEnableSiege;							// ���� ������ �����Ѱ�?
		
        CDBComponent&   m_DBComponent;

		friend	class	CCastleDBMgr;
	};
}

#endif	// _RYL_DBAGENT_CASTLE_DB_H_