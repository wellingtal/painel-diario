// FSMState.cpp: implementation of the CFSMState2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <algorithm>

#include "FSMState.h"


CFSMState::CFSMState(int StateID, int Transitions)
:	m_dwStateID(StateID), m_dwNumberOfTransistions((Transitions) ? Transitions : 1)
{	
	m_pdwInputs = new int[m_dwNumberOfTransistions];
	if (NULL == m_pdwInputs)
	{
		ERRLOG1(g_Log, "m_pdwInputs의 메모리 할당에 실패하였습니다. 할당 개수는 %d개 입니다.",
			m_dwNumberOfTransistions);		
	}
    else
    {
	    std::fill_n(m_pdwInputs, m_dwNumberOfTransistions, 0);

	    m_pdwOutputState = new int[m_dwNumberOfTransistions];
	    if (NULL == m_pdwOutputState)
	    {
		    ERRLOG1(g_Log, "m_pdwOutputState의 메모리 할당에 실패하였습니다. 할당 개수는 %d개 입니다.",
			    m_dwNumberOfTransistions);		
	    }
        else
        {
            std::fill_n(m_pdwOutputState, m_dwNumberOfTransistions, 0);
        }
    }
}

CFSMState::~CFSMState()
{
	if(NULL != m_pdwInputs) 
    { 
		delete m_pdwInputs;
        m_pdwInputs = NULL;
	}

	if(NULL != m_pdwOutputState)
    {
		delete m_pdwOutputState;
        m_pdwOutputState = NULL;
	}
}


void CFSMState::AddTransition(int Input, int OutputID)
{
	int nTransition = 0;
	for(; nTransition < m_dwNumberOfTransistions; ++nTransition)
	{
		if(0 == m_pdwOutputState[nTransition])
        {
			break;
		}
	}

	if(nTransition >= m_dwNumberOfTransistions) 
    {
		return;
	}

	m_pdwInputs[nTransition] = Input;
	m_pdwOutputState[nTransition] = OutputID;	
}


int CFSMState::GetOutput(int Input)
{
	int OutputID = m_dwStateID;	// output state to be returned

	for(int nTransition = 0;
		nTransition < m_dwNumberOfTransistions; ++nTransition)
	{
		if(0 == m_pdwOutputState[nTransition]) 
        {
			break;
		}

		if(Input == m_pdwInputs[nTransition]) 
        {
			OutputID = m_pdwOutputState[nTransition];	// output state id
			break;
		}
	}

	return OutputID;
}


void CFSMState::DeleteTransition(int OutputID)
{
	int nTransition = 0;
	for(;nTransition < m_dwNumberOfTransistions; ++nTransition)
	{
		if(OutputID == m_pdwOutputState[nTransition]) 
        {
			break;
		}
	}

	if(nTransition >= m_dwNumberOfTransistions) 
    {
		return;
	}

	m_pdwInputs[nTransition] = 0; 
	m_pdwOutputState[nTransition] = 0;

	for(;nTransition < (m_dwNumberOfTransistions - 1); ++nTransition)
	{
		if (!m_pdwOutputState[nTransition]) 
        {
			break;
		}

		m_pdwInputs[nTransition] = m_pdwInputs[nTransition+1];
		m_pdwOutputState[nTransition] = m_pdwOutputState[nTransition+1];
	}

	m_pdwInputs[nTransition] = 0; 
	m_pdwOutputState[nTransition] = 0;
}