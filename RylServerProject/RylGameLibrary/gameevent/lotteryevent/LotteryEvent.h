#ifndef _CLOTTERY_EVENT_H_
#define _CLOTTERY_EVENT_H_

#include <vector>


// 전방 참조
namespace Item
{
	class CItem;
}

class CLotteryEvent
{
public:

	CLotteryEvent();
	virtual ~CLotteryEvent();

	bool Initialize(const char* szFileName = NULL);

// 서버 셋업을 이용하도록 변경되었습니다. (2003-09-05)
/*
	bool IsActive(void)	{ return m_bActive;		}
	void Active(void)	{ m_bActive = true;		}
	void DeActive(void)	{ m_bActive = false;	}
*/

	unsigned short GetLottery(void);
	Item::CItem* PrizeLottery(unsigned short usLotteryID);

protected:

	struct LotteryEventPrize
	{
		unsigned short	m_usPrizeID;
		unsigned char	m_cProbability;
		unsigned char	m_cNum;

		LotteryEventPrize()
		:	m_usPrizeID(0), m_cProbability(0), m_cNum(0)
		{ }

		LotteryEventPrize(unsigned short usPrizeID, unsigned char cProbability, unsigned char cNum)
		:	m_usPrizeID(usPrizeID), m_cProbability(cProbability), m_cNum(cNum)
		{ }
	};

	struct LotteryEventItem
	{
		unsigned short	m_usItemID;
		unsigned char	m_cProbability;
		unsigned short	m_usPrizeSumProb;

		std::vector<LotteryEventPrize>	m_aryPrize;

		LotteryEventItem()
		:	m_usItemID(0), m_cProbability(0), m_usPrizeSumProb(0)
		{ }

		LotteryEventItem(unsigned short usItemID, unsigned char cProbability)
		:	m_usItemID(usItemID), m_cProbability(cProbability), m_usPrizeSumProb(0)
		{ }
	};
	
	
	bool	m_bActive;

	std::vector<LotteryEventItem>	m_aryItem;

	static const char*  ms_szEventScriptFileName;
};


#endif