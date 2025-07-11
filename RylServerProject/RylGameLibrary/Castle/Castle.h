#ifndef _CASTLE_H_
#define _CASTLE_H_

#pragma once

#include <map>
#include <Creature/CreatureStructure.h>
#include <Creature/Siege/SiegeConstants.h>
#include <Castle/CastleConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>


// ���� ����
class CSiegeObject;
class CSendStream;

namespace Castle
{
	class CCastle
	{
	public:
		~CCastle();

		// Ÿ�� ����
		typedef std::map<unsigned short, unsigned short>	MineralInfoMap;			// <wMineralID, wAmount>
		typedef	std::map<unsigned long, CSiegeObject*>		CastleObjectMap;		// <ObjectCID, lpCastleObject>

		// �߰� ������ ���� �޾ƿ� ���� ���� ����
		bool	SetTaxInfo(const CastleTaxInfo& castleTaxInfo);
		bool	SetMineralInfo(unsigned short wMineralID, unsigned short wAmount, unsigned char cFlag);
		
		void	UpdateTaxMoveInfo(unsigned long* dwMoveTempMoney, unsigned long* dwResultTaxMoney);
		void	UpdateSiegeCountInfo(unsigned char cSiegeCount, unsigned char cInvincibleCount, unsigned short wTotalSiegeCount);

		void	SendTempTaxInfo();												// �߰輭���� �ӽ� ���� ���� ������Ʈ
		bool	SendMineralInfo(CSendStream& SendStream, unsigned char cMineralType=Siege::ACCUMULATED_MINERAL);	// ���� ���� ����

		void	AllRespawn(unsigned char cExceptNation);						// ���ֺ� x���� �ִ� ��뱹�� ĳ���͸� ������ ��Ų��.
		bool	ChangeCastleMaster(unsigned char cNation);						// ���� ���� ����
		void	LoseOwnership();												// �� �߸�ȭ
		void	ChangeCastleObjectNation(unsigned char cNation);				// ���� ������Ʈ���� ������ ����
		void	DestroyAllCastleArms(bool bKillRider);							// ���� ������� ���� ���� NPC �� �����.

		void	AddUpgradeItem(unsigned short wItemID, int iIndex, unsigned char cNum);		// ���׷��̵� ���� ������ �ֱ�
		void	DeleteUpgradeItem(int iIndex, unsigned char cNum);							// ���׷��̵� ���� ������ ����
		void	InitUpgradeItem();															// ���׷��̵� ���� ������ �ʱ�ȭ
		void	AddTempTaxMoney(unsigned char cType, unsigned long dwMoney);				// �ӽ� ���� ����
		bool	AddMineral(unsigned char cFlag, unsigned short wMineralID, unsigned short wAmount);	// ���� ���� ��ô
		void	TakeTaxMoney(unsigned char cType, unsigned long dwTaxMoney);				// ���� ȸ��
		bool	GainMineral(unsigned short wMineralID, unsigned short wAmount);				// ���� ���� ȸ��
		
		bool	InsertCastleObject(CSiegeObject* lpCastleObject);				// �� ������Ʈ �߰�
		bool	DeleteCastleObject(unsigned long dwCID);						// �� ���� ����
		bool	DeleteCastleObject(CSiegeObject* lpCastleObject);				// �� ���� ����

		bool	HasCastleArms(unsigned long dwCID);

		CSiegeObject*	GetCastleEmblem() const;
		CSiegeObject*	GetCastleObject(unsigned long dwCastleObjectID);		// �� ������Ʈ ������

		void	UpgradeByEmblem();
		void	DegradeByEmblem();

		void	Process();

		bool	CheckRight(unsigned char cRightType, unsigned long dwCID, unsigned long dwGID);

		void	SetEnableSiege(bool bEnable);
		void	SetTax(unsigned char cType, unsigned char cTax);
		void	SetRight(CastleRight castleRight);

		bool	IsCastleOfNation(unsigned char cNation);
		bool	IsTaxChangable(unsigned char cType);
		void	EnableTaxChange();
		bool	HasAnotherCastleArms(unsigned long dwCID) const;		

		unsigned long	GetCastleID() const							{ return m_dwCastleID;						}
		unsigned char	GetNation() const							{ return m_cNation;							}
		unsigned char	GetNameID() const							{ return m_cNameID;							}
		unsigned char	GetZone() const								{ return m_cZone;							}
		unsigned char	GetChannel() const							{ return m_cChannel;						}
		const Position& GetBackDoorPos(unsigned long dwPos) const	{ return m_BackDoorPos[dwPos];				}
		unsigned char	GetTax(unsigned char cType) const			{ return m_CastleTax[cType].m_cTax;			}
		unsigned long	GetTaxMoney(unsigned char cType) const		{ return m_CastleTax[cType].m_dwTaxMoney;	}
		unsigned char	GetSiegeCount() const						{ return m_cSiegeCount;						}
		unsigned char	GetInvincibleCount() const					{ return m_cInvincibleCount;				}
		unsigned short	GetTotalSiegeCount() const					{ return m_wTotalSiegeCount;				}
		unsigned short	GetTotalGainTaxCount() const				{ return (m_wTotalSiegeCount / Castle::TEMP_TAX_GAIN_COUNT);	}
		unsigned long	GetTotalTaxMoney() const					{ return m_dwTotalTaxMoney;					}
		unsigned short	GetUpgradeItemID() const					{ return m_wItemID;							}
		unsigned char	GetUpgradeItemNum() const					{ return m_cTotalItemNum;					}
		unsigned char	GetUpgradeItemNum(int iIndex) const			{ return m_cItemNum[iIndex];				}
		unsigned char	GetUpgradeStep() const;
		unsigned short	GetMineralNum(unsigned char cMineralType, unsigned short wMineralID) const;
		
		void			SetBonusRate(float fRate)					{ m_fBonusRate = fRate;						}

	private:
		CCastle(const CastleInfoDB& CastleInfo);

		CastleObjectMap	m_CastleObjectMap;						// �� ���� ������Ʈ��
		
		unsigned long	m_dwCastleID;							// �� Ȥ�� ���� ID
		unsigned char	m_cNation;								// ���� ������ ����
		unsigned char	m_cZone;								// ���� �ִ� �� ��ȣ
		unsigned char	m_cChannel;								// ���� �ִ� ä�� ��ȣ
		unsigned char	m_cNameID;								// �� �̸� ID ( 1����~~ )
		unsigned char	m_cSiegeCount;							// ������ ġ�� Ƚ��
		unsigned char	m_cInvincibleCount;						// ���� Ƚ��
		unsigned short	m_wTotalSiegeCount;						// ���� ���� Ƚ��
		unsigned long	m_dwTotalTaxMoney;						// ���� ���� ȸ����
		unsigned short	m_wItemID;								// ���׷��̵忡 ����� ���� ������ ID
		unsigned char	m_cTotalItemNum;						// ���׷��̵忡 ����� ���� ������ ���� ����
		unsigned char	m_cItemNum[Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM];	// ���׷��̵忡 ����� ���� ���� ������ ����

		CastleRight		m_CastleRight;							// �� ���� ����
		Position		m_BackDoorPos[2];						// �޹� ���� ��/����ġ

		CastleTaxInfo	m_CastleTax[Castle::MAX_TAX_TYPE];		// ���� �� ���� ����

		MineralInfoMap	m_AccumulatedMineralMap;				// ���� ���� ����
		MineralInfoMap	m_TemporaryMineralMap;					// �ӽ� ���� ����

		bool			m_bEnableSiege;							// ���� ������ �����Ѱ�?
		float			m_fBonusRate;							// ��¡�� ���׷��̵忡 ���� �� ���� ���� ���ʽ� (%)
		unsigned char	m_cTempTaxUpdateCount;					// �ӽ� ������ DB �߰� ������ �������� ī����
		unsigned long	m_dwTemporayTempTaxMoney;				// DB �߰�� ������Ʈ���� ����� �ӽ� ����

		friend	class	CCastleMgr;
	};
}

#endif	// _CASTLE_H_