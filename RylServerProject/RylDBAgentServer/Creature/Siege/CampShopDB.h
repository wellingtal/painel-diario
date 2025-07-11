#ifndef _RYL_DBAGENT_CAMP_SHOP_H_
#define _RYL_DBAGENT_CAMP_SHOP_H_

#pragma once

#include "CampDB.h"

using namespace Siege;

class CCampShopDB : public CCampDB
{
public:

	virtual ~CCampShopDB();

	// Process �Լ�
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

	unsigned char	m_aryItemContainer[CampShopInfoDB::MAX_CONTAINER_SIZE];		// �Ǹ����� ������ ���
	unsigned long	m_dwItemContainerSize;										// ������ ��� ������
	unsigned char	m_cItemNum;													// ������ ����

	unsigned long	m_dwTempSafe;		// �ӽ� �ݰ�
	unsigned char	m_cTax;				// ����

	friend class CSiegeObjectDBMgr;
};

#endif
