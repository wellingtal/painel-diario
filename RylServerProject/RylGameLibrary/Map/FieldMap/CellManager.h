#ifndef _CELL_MANAGER_H_
#define _CELL_MANAGER_H_

#pragma once

#include <vector>
#include <Creature/CreatureStructure.h>
#include <Stream/Buffer/BufferFactory.h>

class CCell;
namespace VirtualArea
{
	class CBGServerMgr;
}


class CBuffer;
class CCharacter;

class CCellManager
{
public:

	static CCellManager& GetInstance();

    void Destroy();

	bool CheckPositionInZone(Position Pos);

	void Load(void);
	bool LoadComplete(void)		{ return m_bLoadComplete; }

	bool SummonMonster(int nKID, Position Pos, CCharacter* lpMaster);
	bool AdminSummonMonster(int nKID, Position Pos);

	// Normal 상태에서 wMoving의 확률로 Moving. (디폴트는 20%)
	// 이동을 20프로에서 80프로로 변경 2008.07.10
	void SetMoving(bool bMoving, unsigned short wMoving = 20) { m_bMoving = bMoving; m_wNumMoving = wMoving; }
	void SetAvoid(bool bAvoid)	{ m_bAvoid = bAvoid;	}

	bool IsMoving(void) const	{ return m_bMoving;		}		
	bool IsAvoid(void) const	{ return m_bAvoid;		}
	unsigned short GetMovingNum(void) const	{ return m_wNumMoving; }

	// 아이템 아이디로 셀 포인터 얻기
	CCell* GetCell(unsigned __int64 nItemID);

	// 셀 번호로 셀 포인터 얻기
	CCell* GetCell(unsigned short wMapIndex, unsigned char cCellX, unsigned char cCellZ);	

	// 월드 좌표로 셀 포인터 얻기
	CCell* GetCell(unsigned short wMapIndex, unsigned long dwPosX, unsigned long dwPosY, unsigned long dwPosZ);	
	CCell* GetCell(unsigned short wMapIndex, POS& Pos);

	void LowerResolution(int nHighX, int nHighZ, int *nLowX, int *nLowZ);
	void HigherResolution(int nLowX, int nLowZ, int *nHighX, int *nHighZ);

	bool IsCampCreateArea(Position Pos);
	bool IsSafetyZone(Position Pos);
	
    CBufferFactory& GetBufferFactory() { return m_BufferFactory; }

	template<typename FnRefCell>
		inline bool ProcessAllCell(FnRefCell fnRefCell)
    {
		if (0 != m_CellData) 
		{ 
            CCell* lpCellPos = m_CellData;
            CCell* lpCellEnd = m_CellData + CCell::ms_CellSize * CCell::ms_CellSize;

		    for (; lpCellPos != lpCellEnd; ++lpCellPos)
		    {
                fnRefCell(*lpCellPos);
		    }

		    return true;
        }

        return false;
    }

	template<typename FnRefCell, typename Arg1>
		inline bool ProcessAllCell(FnRefCell fnRefCell, Arg1 arg1)
    {
		if (0 != m_CellData) 
		{ 
            CCell* lpCellPos = m_CellData;
            CCell* lpCellEnd = m_CellData + CCell::ms_CellSize * CCell::ms_CellSize;

		    for (; lpCellPos != lpCellEnd; ++lpCellPos)
		    {
                fnRefCell(*lpCellPos, arg1);
		    }

		    return true;
        }

        return false;
    }

	// --------------------------------------------------------------------------------------------
	// 셀 통계 내는 함수들

	bool CheckCellAggresiveCreatures(void);		// 셀 내에 겹치는 CID가 있는지 검사
	bool CheckCellStatus(void);					// 셀 내에 사용자 분포 출력


private:

	enum
	{
		BGM_TEXTURE_SIZE	= 128,
		SECTOR_SIZE			= 31508
	};

	// 클라이언트에서 사용중인 BGM트리거를 수정하였습니다. (같은 리소스 사용을 위해...)
	struct SafetyZoneInfo
	{
		enum EventKey
		{
			EK_BGM_ONCE_SAFE		= 0,  
			EK_BGM_LOOP_SAFE		= 1,
			EK_BGM_LOOP_UNSAFE		= 2,
			EK_BGM_ONCE_UNSAFE		= 3,
			EK_NOTBGM_SAFE			= 4,
			EK_BGM_TURN_AMB_SAFE	= 5,
			EK_BGM_TURN_AMB_UNSAFE	= 6,
			EK_CAMP_UNCREATE		= 9,
			EK_ESF_SAFE				= 10,
			EK_ESF_UNSAFE			= 11
		};

		unsigned long	m_dwSectorX; 
		unsigned long	m_dwSectorY;

		char m_szFilename[MAX_PATH];

		unsigned long	m_dwEventNum; 
		
		std::vector<unsigned long> m_vecEventKey; 
		std::vector<unsigned long> m_vecBGMColorKey; 
		
		unsigned long m_aryColorTable[BGM_TEXTURE_SIZE][BGM_TEXTURE_SIZE]; 
	};

	CCellManager();
	~CCellManager();

	bool InitAI(void);
	bool CreateCell(void);	
	bool SetWeather(const char* szFileName);
	bool LoginMonster(const char* szFileName, unsigned short wMapIndex = 0);
	bool LoadSafetyZone(const char* szFileName);


private:

	CCell*			m_CellData;
	unsigned char*	m_WeatherRate;

	unsigned short	m_wNumMoving;
    unsigned short	m_usSummonCount;

	CPoolBufferFactory	m_BufferFactory;

	std::vector<SafetyZoneInfo *>	m_vecSafetyZone;

	bool 			m_bMoving;
	bool 			m_bAvoid;
	bool 			m_bLoadComplete;
    bool 			m_bPadding[1];

	friend class VirtualArea::CBGServerMgr;		// LoginMonster() 호출을 위해...
};

#endif