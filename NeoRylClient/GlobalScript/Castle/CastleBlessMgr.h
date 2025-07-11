#ifndef _CASTLE_BLESS_MANAGER_H_
#define _CASTLE_BLESS_MANAGER_H_

#pragma once

#define g_CastleBlessMgr	Castle::CCastleBlessMgr::GetInstance()

#include <map>

namespace Castle
{
	class CCastleBlessMgr
	{
	private:

		enum Const
		{
			MAX_BLESS_INFO_NUM	= 6
		};

		struct sCastleBlessInfo
		{
			unsigned short	m_wGainTaxCount;	// ���� ȹ�� Ƚ��(�ӽ� -> ����) = �� ���� Ƚ�� / Castle::TEMP_TAX_GAIN_COUNT
			unsigned char	m_cUpgradeStep;		// �ʿ� ���׷��̵�
			unsigned char	m_cBonusPercent;	// ���� ��Ȯ��
			float			m_fArea;			// ����(�ݰ� m ����)
			unsigned char	m_cTitle;			// Īȣ

			sCastleBlessInfo(unsigned short wGainTaxCount, unsigned char cUpgradeStep, unsigned char cBonusPercent,
							 float fArea, unsigned char cTitle)
			: m_wGainTaxCount(wGainTaxCount), m_cUpgradeStep(cUpgradeStep), m_cBonusPercent(cBonusPercent),
			  m_fArea(fArea), m_cTitle(cTitle)
			{
			}
		};

	public:
		~CCastleBlessMgr();
		static CCastleBlessMgr& GetInstance();

		unsigned char	GetTitle(unsigned short wGainTaxCount, unsigned char cUpgradeStep) const;
		unsigned char	GetBonusPercent(unsigned short wGainTaxCount, unsigned char cUpgradeStep) const;
		float			GetBlessArea(unsigned short wGainTaxCount, unsigned char cUpgradeStep) const;
				
	private:
		CCastleBlessMgr();

		int				FindProperIndex(unsigned short wGainTaxCount, unsigned char cUpgradeStep) const;

		bool			Initialize();
		void			Destroy();

		static sCastleBlessInfo		m_CastleBlessInfo[ MAX_BLESS_INFO_NUM ] ;
	};
}

#endif	// _CASTLE_BLESS_MANAGER_H_