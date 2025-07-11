#ifndef _CASTLE_H_
#define _CASTLE_H_

#pragma once

#include <map>
#include <Creature/CreatureStructure.h>
#include <Creature/Siege/SiegeConstants.h>
#include <Castle/CastleConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>


// 전방 참조
class CSiegeObject;
class CSendStream;

namespace Castle
{
	class CCastle
	{
	public:
		~CCastle();

		// 타입 정의
		typedef std::map<unsigned short, unsigned short>	MineralInfoMap;			// <wMineralID, wAmount>
		typedef	std::map<unsigned long, CSiegeObject*>		CastleObjectMap;		// <ObjectCID, lpCastleObject>

		// 중계 서버로 부터 받아온 성의 정보 셋팅
		bool	SetTaxInfo(const CastleTaxInfo& castleTaxInfo);
		bool	SetMineralInfo(unsigned short wMineralID, unsigned short wAmount, unsigned char cFlag);
		
		void	UpdateTaxMoveInfo(unsigned long* dwMoveTempMoney, unsigned long* dwResultTaxMoney);
		void	UpdateSiegeCountInfo(unsigned char cSiegeCount, unsigned char cInvincibleCount, unsigned short wTotalSiegeCount);

		void	SendTempTaxInfo();												// 중계서버로 임시 세금 정보 업데이트
		bool	SendMineralInfo(CSendStream& SendStream, unsigned char cMineralType=Siege::ACCUMULATED_MINERAL);	// 광물 정보 전송

		void	AllRespawn(unsigned char cExceptNation);						// 성주변 x셀에 있는 상대국적 캐릭터를 리스폰 시킨다.
		bool	ChangeCastleMaster(unsigned char cNation);						// 소유 국가 변경
		void	LoseOwnership();												// 성 중립화
		void	ChangeCastleObjectNation(unsigned char cNation);				// 수성 오브젝트들의 국적을 변경
		void	DestroyAllCastleArms(bool bKillRider);							// 수성 병기들을 병기 관리 NPC 로 만든다.

		void	AddUpgradeItem(unsigned short wItemID, int iIndex, unsigned char cNum);		// 업그레이드 보석 아이템 넣기
		void	DeleteUpgradeItem(int iIndex, unsigned char cNum);							// 업그레이드 보석 아이템 빼기
		void	InitUpgradeItem();															// 업그레이드 보석 아이템 초기화
		void	AddTempTaxMoney(unsigned char cType, unsigned long dwMoney);				// 임시 세금 축적
		bool	AddMineral(unsigned char cFlag, unsigned short wMineralID, unsigned short wAmount);	// 광물 세금 축척
		void	TakeTaxMoney(unsigned char cType, unsigned long dwTaxMoney);				// 세금 회수
		bool	GainMineral(unsigned short wMineralID, unsigned short wAmount);				// 광물 세금 회수
		
		bool	InsertCastleObject(CSiegeObject* lpCastleObject);				// 성 오브젝트 추가
		bool	DeleteCastleObject(unsigned long dwCID);						// 성 병기 삭제
		bool	DeleteCastleObject(CSiegeObject* lpCastleObject);				// 성 병기 삭제

		bool	HasCastleArms(unsigned long dwCID);

		CSiegeObject*	GetCastleEmblem() const;
		CSiegeObject*	GetCastleObject(unsigned long dwCastleObjectID);		// 성 오브젝트 얻어오기

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

		CastleObjectMap	m_CastleObjectMap;						// 성 관련 오브젝트맵
		
		unsigned long	m_dwCastleID;							// 성 혹은 마을 ID
		unsigned char	m_cNation;								// 성을 소유한 국가
		unsigned char	m_cZone;								// 성이 있는 존 번호
		unsigned char	m_cChannel;								// 성이 있는 채널 번호
		unsigned char	m_cNameID;								// 성 이름 ID ( 1부터~~ )
		unsigned char	m_cSiegeCount;							// 공성을 치른 횟수
		unsigned char	m_cInvincibleCount;						// 무적 횟수
		unsigned short	m_wTotalSiegeCount;						// 누적 공성 횟수
		unsigned long	m_dwTotalTaxMoney;						// 누적 세금 회수량
		unsigned short	m_wItemID;								// 업그레이드에 사용할 보석 아이템 ID
		unsigned char	m_cTotalItemNum;						// 업그레이드에 사용할 보석 아이템 갯수 총합
		unsigned char	m_cItemNum[Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM];	// 업그레이드에 사용할 보석 개별 아이템 갯수

		CastleRight		m_CastleRight;							// 성 관리 권한
		Position		m_BackDoorPos[2];						// 뒷문 사용시 안/밖위치

		CastleTaxInfo	m_CastleTax[Castle::MAX_TAX_TYPE];		// 세율 및 세금 정보

		MineralInfoMap	m_AccumulatedMineralMap;				// 누적 광물 세금
		MineralInfoMap	m_TemporaryMineralMap;					// 임시 광물 세금

		bool			m_bEnableSiege;							// 현재 공성이 가능한가?
		float			m_fBonusRate;							// 상징물 업그레이드에 의한 총 세금 수입 보너스 (%)
		unsigned char	m_cTempTaxUpdateCount;					// 임시 세금을 DB 중계 서버로 보내기전 카운터
		unsigned long	m_dwTemporayTempTaxMoney;				// DB 중계로 업데이트전에 모아진 임시 세금

		friend	class	CCastleMgr;
	};
}

#endif	// _CASTLE_H_