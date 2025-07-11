#include "stdafx.h"
#include "Pulse.h"

#include <mmsystem.h>
#include <BaseLibrary/Log/ServerLog.h>


CPulse& CPulse::GetInstance()
{
	static CPulse Pulse; 
	return Pulse;
}


CPulse::CPulse()
:   m_dwPulse(0), 
    m_dwLastTick(timeGetTime()), 
    m_dwHeavyTrafficCount(0), 
    m_bTPPOverTwoTime(FALSE),
    m_nTicksPerPulse(0),
    m_nTicksPerSec(0)
{    
    SetTicksPerPulse(DEFAULT_TICKS_PER_PULSE);
}

CPulse::~CPulse()
{


}

void CPulse::SetTicksPerPulse(long nTicksPerPulse)
{
    if(nTicksPerPulse <= 0)
    {
        nTicksPerPulse = DEFAULT_TICKS_PER_PULSE;
    }

    m_nTicksPerPulse    = nTicksPerPulse;    
    m_nTicksPerSec      = 1000 / nTicksPerPulse;
}

unsigned long CPulse::GetRemainTime() const
{
    long nElapsed = timeGetTime() - m_dwLastTick;

    if (nElapsed < m_nTicksPerPulse)
    {
        return (m_nTicksPerPulse - nElapsed);
    }

    return 0;
}

unsigned long CPulse::CheckSleep()
{	
	long nElapsed = timeGetTime() - m_dwLastTick;

	if (nElapsed < m_nTicksPerPulse)
    {
		// �帥�ð��� TPP���� ���� ��� Sleep
        unsigned long dwSleep = (m_nTicksPerPulse - nElapsed) - 1;
        unsigned long dwTicksPerPulse = m_nTicksPerPulse;

        if(m_nTicksPerPulse < 0)
        {
            dwTicksPerPulse = 0;
        }

        if(dwTicksPerPulse < dwSleep)
        {
            dwSleep = 0;
        }

        Sleep(dwSleep);

		m_dwHeavyTrafficCount = 0;
		m_bTPPOverTwoTime = FALSE;
	}
    else if(((++m_dwHeavyTrafficCount) > 5) || (nElapsed > (m_nTicksPerPulse * 2)))
    {
		// �帥 �ð��� TPP�� �ι躸�� ū ��� �Ǵ� ���������� Ʈ������ �ʹ� ���� ���
		DETLOG3(g_Log, "Time Overflow : Elapsed time is %u, (TPP is %d), HeavyTrafficCount is %u"
			, nElapsed, m_nTicksPerPulse, m_dwHeavyTrafficCount);

		m_bTPPOverTwoTime = TRUE;
	}
    else 
    {
		DETLOG1(g_Log, "Traffic is %u", m_dwHeavyTrafficCount);
		m_bTPPOverTwoTime = FALSE;
	}

    m_dwLastTick = timeGetTime();

    ++m_dwPulse;

    if(0 == m_dwPulse)
    {
        // ���� �޽��� �ѹ��� ���Ƽ� �ٽ� 0���� ���ƿԴٸ�, 
        // �ѹ� �� ���ؼ� 0�� �� �� ���� �Ѵ�.
        ++m_dwPulse;
    }

	return m_dwPulse;
}


/*
#include "../stdafx.h"
#include "Pulse.h"


#define DEFAULT_TPP		100


CPulse	g_Pulse;


// constructor and destructor
CPulse::CPulse() : m_dwPulse(0), m_dwExtraTick(GetTickCount()), m_dwLastTick(GetTickCount()), m_dwHeavyTrafficCount(0), m_TPPOverTwoTime(false)
//, m_SleepOperation(NULL)
{
	SetTicksPerPulse(DEFAULT_TPP);

	for (int i=0; i<PULSETABLE_NUM; ++i)
		::ZeroMemory(&m_PulseTable[i], sizeof(PULSE_TIMER));

	DETLOG1(g_Log, "this:0x%08x.", this);
}

CPulse::~CPulse()
{
	DETLOG1(g_Log, "this:0x%08x.", this);	
}

void CPulse::SetTicksPerPulse(unsigned long dwTicksPerPulse)
{
	m_nTicksPerPulse = dwTicksPerPulse;
}

//void CPulse::SetSleepOperation(Operation pOperation)
//{
//	m_SleepOperation	= pOperation;
//}

unsigned long CPulse::CheckSleep(void)
{
	m_dwExtraTick = GetTickCount();
	unsigned long nElapsed = m_dwExtraTick - m_dwLastTick;

	if (nElapsed <= m_nTicksPerPulse)  {
		// �帥�ð��� TPP���� ���� ��� Sleep
		if (nElapsed < m_nTicksPerPulse) {
			//if (m_SleepOperation) {
			//	// ���� ���۷��̼��� ����
			//	m_SleepOperation();

			//	// ���ν����� ������ ���Ŀ� ����� �ð��� Ȯ���ؼ� TPP���� ���� ��쿡�� Sleep
			//	m_dwExtraTick = GetTickCount();
			//	nElapsed = m_dwExtraTick - m_dwLastTick;
			//	if (nElapsed < m_nTicksPerPulse) {
			//		Sleep((m_nTicksPerPulse - nElapsed) - 1);
			//	}

			//} else {
				Sleep((m_nTicksPerPulse - nElapsed) - 1);
			//}
		}
		m_dwLastTick = GetTickCount();
		m_dwHeavyTrafficCount = 0;

		m_TPPOverTwoTime = false;

	} else if(((++m_dwHeavyTrafficCount) > 5) || (nElapsed > (m_nTicksPerPulse * 2))) {
		// �帥 �ð��� TPP�� �ι躸�� ū ��� �Ǵ� ���������� Ʈ������ �ʹ� ���� ���
		DETLOG2(g_Log, "Time Overflow : �帥 �ð��� %d �� TPP �� �ι躸�� ũ�ų�, �������� Ʈ������ %d �Դϴ�.", nElapsed, m_dwHeavyTrafficCount);
		m_dwLastTick = GetTickCount();

		m_TPPOverTwoTime = true;
	} else {
		DETLOG1(g_Log, "Ʈ������ %d �Դϴ�.", m_dwHeavyTrafficCount);
		// m_nTicksPerPulse < nElapsed <= m_nTicksPerPulse * 2
		m_dwLastTick = GetTickCount() - (nElapsed - m_nTicksPerPulse);

		m_TPPOverTwoTime = false;
	}

	return ++m_dwPulse;
}

bool CPulse::InstallTimer(int nOperation, unsigned long dwPulse, Operation pOperation)
{
	if (nOperation <= 0 || nOperation >= PULSETABLE_NUM) {
		return false;
	}

	for (int i=0; i < PULSETABLE_NUM; ++i) {
		if (0 == m_PulseTable[i].m_nOperation) {
			m_PulseTable[i].m_nOperation	= nOperation;
			m_PulseTable[i].m_dwPulse		= dwPulse;
			m_PulseTable[i].m_pOperation	= pOperation;

			return true;
		}
	}

	// TODO : �����α�
	return false;	
}

bool CPulse::UninstallTimer(int nOperation)
{
	if(nOperation <= 0 || nOperation >= PULSETABLE_NUM) 
	{
		return false;
	}

	for (int i=0; i<PULSETABLE_NUM; ++i) {
		if (nOperation == m_PulseTable[i].m_nOperation) {
			m_PulseTable[i].m_nOperation	= 0;
			m_PulseTable[i].m_dwPulse		= 0;
			m_PulseTable[i].m_pOperation	= NULL;

			// TODO : ���̺��� shift up
			ShiftupTable(i + 1);

			return true;
		}
	}

	// TODO : �����α�
	return false;
}

void CPulse::Operate(void)
{
	int i	=	0;
	while (m_PulseTable[i].m_nOperation) {
		if (0 == (GetCurrentPulse() % m_PulseTable[i].m_dwPulse)) {
			if (m_PulseTable[i].m_nOperation) {
				m_PulseTable[i].m_pOperation();
			}
		}

		++i;
	}
}

void CPulse::ShiftupTable(int nStartIndex)
{
	for (int i=nStartIndex; i<(PULSETABLE_NUM - 1); ++i) {
		// TODO : ���̺��� shift up
		m_PulseTable[i].m_nOperation	= m_PulseTable[i+1].m_nOperation;
		m_PulseTable[i].m_dwPulse		= m_PulseTable[i+1].m_dwPulse;
		m_PulseTable[i].m_pOperation	= m_PulseTable[i+1].m_pOperation;
	}

	m_PulseTable[PULSETABLE_NUM].m_nOperation	= 0;
	m_PulseTable[PULSETABLE_NUM].m_dwPulse		= 0;
	m_PulseTable[PULSETABLE_NUM].m_pOperation	= NULL;
}

*/