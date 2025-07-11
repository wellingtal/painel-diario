#ifndef _RYL_GAME_CCOMMANDS_H_
#define _RYL_GAME_CCOMMANDS_H_

#include <RylServerLibrary/Utility/ServerAppFrameWork/ConsoleWindow/ConsoleCMDFactory.h>

// �������� ����߸���.
class CCMDDropItem : public CConsoleCommand
{
protected:

    enum { MAX_NAME = 16 };

    virtual CConsoleCommand* Clone(const char* szCommand, size_t nCommandLength);
	virtual bool Destroy() { delete this; return true; }
    virtual bool DoProcess();

    char            m_szName[MAX_NAME + 1];
    unsigned short  m_usItemProtoTypeID;
};

// �����۸���Ʈ�� ����߸���.
class CCMDDropItemList : public CConsoleCommand
{
protected:

    enum { MAX_NAME = 16 };
	enum { MAX_DROP_NUM = 64 };

    virtual CConsoleCommand* Clone(const char* szCommand, size_t nCommandLength);
	virtual bool Destroy() { delete this; return true; }
    virtual bool DoProcess();

	unsigned short	m_nItemNum;
	char            m_szName[MAX_NAME + 1];
	unsigned short	m_usItemProtoTypeID[MAX_DROP_NUM];
};

// ���� �����鿡�� ������ ���� �� ���
class CCMDNotify : public CConsoleCommand
{
public:

    CCMDNotify() : m_nLength(0) { m_szBuffer[0] = 0; }
	virtual CConsoleCommand* Clone(const char* szCommand, size_t nCommandLength);

protected:

	virtual bool DoProcess();
	virtual bool Destroy() { delete this; return true; }

    enum { MAX_NOTIFY = 2000 };

private:

	char    m_szBuffer[MAX_NOTIFY];
    int     m_nLength;

};

// ����뷱�� on/off
class CCMDAutoBalance : public CConsoleCommand
{
protected:

	virtual CConsoleCommand* Clone(const char* szCommand, size_t nCommandLength);
	virtual bool Destroy() { delete this; return true; }
	virtual bool DoProcess();

private:

	enum AutoBalanceCmd
	{
		ERROR_CMD			= 0,
		NOW_STATE			= 1,
		AUTO_BALANCE_ON		= 2,
		AUTO_BALANCE_OFF	= 3
	};

	unsigned char	m_cCmd;
};

// ���� �̺�Ʈ on/off
class CCMDLotteryEvent : public CConsoleCommand
{
protected:

	virtual CConsoleCommand* Clone(const char* szCommand, size_t nCommandLength);
	virtual bool Destroy() { delete this; return true; }
	virtual bool DoProcess();

private:

	enum LotteryEventCmd
	{
		ERROR_CMD			= 0,
		NOW_STATE			= 1,
		LOTTERY_EVENT_ON	= 2,
		LOTTERY_EVENT_OFF	= 3
	};

	unsigned char	m_cCmd;
};


#endif