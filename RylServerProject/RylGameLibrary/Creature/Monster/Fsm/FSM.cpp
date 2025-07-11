// FSM.cpp: implementation of the CFSM2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <algorithm>
#include "FSM.h"
#include "FSMState.h"

CFSM CFSM::ms_this;

CFSM::CFSM(void)
:	m_nStateNum(0)
{
	std::fill_n(m_lpFSMState, int(MAX_STATE), reinterpret_cast<CFSMState*>(0));
}

CFSM::~CFSM()
{
    CFSMState** lppFSMPastEnd = m_lpFSMState + MAX_STATE;
	for(CFSMState** lppFSM = m_lpFSMState; lppFSM != lppFSMPastEnd; ++lppFSM)
    {
        delete *lppFSM;
        *lppFSM = NULL;
    }
}


int CFSM::StateTransition(int nCrurrentState, int Input)
{
	if (!nCrurrentState) 
    {
		return nCrurrentState;
	}

	CFSMState* lpState = GetState(nCrurrentState);
	if (NULL == lpState) 
    {
		return nCrurrentState;
	}

	nCrurrentState = lpState->GetOutput(Input);
	return nCrurrentState;
}


CFSMState* CFSM::GetState(int StateID)
{
    CFSMState** lppFSMPastEnd = m_lpFSMState + MAX_STATE;
	for(CFSMState** lppFSM = m_lpFSMState; lppFSM != lppFSMPastEnd; ++lppFSM)
	{
		if(NULL == *lppFSM)
        {
			return NULL;
		}

		if(StateID == (*lppFSM)->GetID())
        {
			return *lppFSM;
		}
	}

	return NULL;
}


bool CFSM::AddState(CFSMState* lpNewState)
{
	if(m_nStateNum >= MAX_STATE)
    {
		return false;
	}

	m_lpFSMState[m_nStateNum++] = lpNewState;
	return true;
}
