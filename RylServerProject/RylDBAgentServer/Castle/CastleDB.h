#ifndef _RYL_DBAGENT_CASTLE_DB_H_
#define _RYL_DBAGENT_CASTLE_DB_H_

#pragma once

#include <map>
#include <Castle/CastleConstants.h>
#include <Creature/CreatureStructure.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

// 전방 참조
class CSiegeObjectDB;
class CDBComponent;

namespace Castle
{
	class CCastleDB
	{
	public:

		~CCastleDB();

		// 타입 정의
		typedef std::map<unsigned short, unsigned short>		MineralInfoMap;			// <wMineralID, wAmount>
		typedef	std::map<unsigned long, CSiegeObjectDB*>		CastleObjectMap;		// <ObjectCID, lpCastleObject>

		bool	ChangeCastleMaster(unsigned char cNation);						// 성주 변경
		void	LoseOwnership();												// 성 소유를 중립화
		
		void	CloseCastleGate();												// 성문을 닫는다.
		void	OpenCastleGate();												// 성문을 연다. (부셔진 경우 복구)
		
		void	ChangeCastleObjectNation(unsigned char cNation);				// 수성 오브젝트들의 국적을 변경
		void	DestroyAllCastleArms();											// 수성 병기들을 병기 관리 NPC 로 만든다.
		
		bool	InsertCastleObject(CSiegeObjectDB* lpCastleObject);				// 성 오브젝트 추가
		bool	DeleteCastleObject(unsigned long dwCID);						// 공성 병기 삭제
		bool	DeleteCastleObject(CSiegeObjectDB* lpCastleObject);				// 공성 병기 삭제
		
		CSiegeObjectDB*	GetCastleEmblem();
		CSiegeObjectDB* GetCastleGate();
		CSiegeObjectDB*	GetCastleObject(unsigned long dwCastleObjectID);

		void	CheckEnableSiege(bool bIsSiegeTime);
		
		// 공성 시간이 끝난 후 갱신된 성 정보 전송
		void	UpdateSiegeCount();
		
		void	SetTax(unsigned char cType, unsigned char cTax);					// 세율 변경
		void	TakeTaxMoney(unsigned char cType, unsigned long dwTakeTaxMoney);	// 세금 회수
		bool	GainMineral(unsigned short wMineralID, unsigned short wAmount);		// 광물 세금 회수
		bool	SetTempTaxMoney(unsigned long* dwTempTaxMoney, unsigned long* dwTaxMoney);	// 게임 서버로부터 임시 세금정보 업데이트
		bool	SetMineralInfo(unsigned char cMineralType, unsigned char cNum, CastleMineral* lpMineralInfo);	// 게임 서버로부터 광물 세금정보 업데이트
		bool	InsertMineralInfo(unsigned char cMineralType, const CastleMineral& mineralInfo);				// Mineral 추가
		void	SetRight(CastleRight castleRight);									// 관리 권한 변경
		void	SetUpgradeItemInfo(unsigned short wItemID, unsigned char cItemNum);
		
		bool	SetTaxInfo(const CastleTaxInfoDB& taxInfo);						// DB 에서 읽어들인 Tax 정보 설정
		bool	SetMineralInfo(const CastleMineralInfoDB& mineralInfo);			// DB 에서 읽어들인 Mineral 정보 설정
		        
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

		CastleObjectMap	m_CastleObjectMap;						// 성 관련 오브젝트맵
		
		unsigned long	m_dwCastleID;							// 성 혹은 마을 ID
		unsigned char	m_cNation;								// 성을 차지한 국가
		unsigned char	m_cZone;								// 성이 있는 존 번호
		unsigned char	m_cChannel;								// 성이 있는 채널 번호
		unsigned char	m_cNameID;								// 성의 이름 ID ( 1부터~~ )
		unsigned char	m_cSiegeCount;							// 공성 횟수
		unsigned char	m_cInvincibleCount;						// 무적 횟수
		unsigned short	m_wTotalSiegeCount;						// 누적 공성 횟수
		unsigned long	m_dwTotalTaxMoney;						// 누적 세금 회수량
		unsigned short	m_wItemID;								// 업그레이드에 사용할 보석 아이템 ID
		unsigned char	m_cItemNum;								// 업그레이드에 사용할 보석 아이템 갯수
		
		CastleRight		m_CastleRight;							// 성 관리 권한
		Position		m_BackDoorPos[2];						// 뒷문 사용시 안/밖위치
        		
		CastleTaxInfo	m_CastleTax[Castle::MAX_TAX_TYPE];		// 세율 및 세금 정보

		MineralInfoMap	m_AccumulatedMineralMap;				// 누적 광물 세금
		MineralInfoMap	m_TemporaryMineralMap;					// 임시 광물 세금

		bool			m_bEnableSiege;							// 현재 공성이 가능한가?
		
        CDBComponent&   m_DBComponent;

		friend	class	CCastleDBMgr;
	};
}

#endif	// _RYL_DBAGENT_CASTLE_DB_H_