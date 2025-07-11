#ifndef _RYL_DBAGENT_CAMP_SHOP_H_
#define _RYL_DBAGENT_CAMP_SHOP_H_

#pragma once

#include "CampDB.h"

using namespace Siege;

class CCampShopDB : public CCampDB
{
public:

	virtual ~CCampShopDB();

	// Process 함수
	virtual Siege::eReturn	Process();

	void SetItemContainer(const char* aryContainer, unsigned long dwContainerSize, unsigned char cItemNum)		
	{ 
		std::copy(aryContainer, aryContainer + dwContainerSize + sizeof(unsigned long) * cItemNum, m_aryItemContainer);
		m_dwItemContainerSize = dwContainerSize;
		m_cItemNum = cItemNum;
	}

	void SetTempSafe(unsigned long dwTempSafe)	{ m_dwTempSafe = dwTempSafe;			}
	void SetTax(unsigned char cTax)				{ m_cTax = cTax;						}
	
	bool UpdateCampInfo(const char* aryItemBuffer, unsigned long dwItemBufferSize, unsigned char cItemNum, 
		unsigned long dwTempSafe, unsigned char cTax);

	bool SerializeOut(char* szBuffer_Out, unsigned short& usBufferSize_Out, unsigned long& dwCID_Out);


private:

	CCampShopDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo);

	unsigned char	m_aryItemContainer[CampShopInfoDB::MAX_CONTAINER_SIZE];		// 판매중인 아이템 목록
	unsigned long	m_dwItemContainerSize;										// 아이템 목록 사이즈
	unsigned char	m_cItemNum;													// 아이템 갯수

	unsigned long	m_dwTempSafe;		// 임시 금고
	unsigned char	m_cTax;				// 세율

	friend class CSiegeObjectDBMgr;
};

#endif
