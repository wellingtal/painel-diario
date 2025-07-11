// FSM.h: interface for the CFSM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSM_H__5BDFA770_7869_4B08_B0BC_D7C6AC789C9D__INCLUDED_)
#define AFX_FSM_H__5BDFA770_7869_4B08_B0BC_D7C6AC789C9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <Pattern/Singleton.h>
#include "FSMState.h"


enum MONSTER_STATE_OR_INPUT
{
	STATE_ID_NULL = 0,
	STATE_ID_NORMAL,
	STATE_ID_ATTACK,
	STATE_ID_RETURN,
	STATE_ID_ESCAPE,
	STATE_ID_DIE,

	INPUT_ID_SEEN_PLAYER = 100,
	INPUT_ID_LOW_HP,
	INPUT_ID_ZERO_HP,
	INPUT_ID_LEAVE_PLAYER,
	INPUT_ID_ARRIVAL_SITE,
	INPUT_ID_ATTACKED_PLAYER,
	INPUT_ID_CMD_ATTACK,
	INPUT_ID_CMD_ESCAPE
};


class CFSM : public CSingleton<CFSM>
{
public:
	
	enum { MAX_STATE = 8 };
	
	~CFSM(void);							// clean up memory usage

	bool AddState(CFSMState* lpNewState);	// add a FSMstate object pointer to the map	
	CFSMState* GetState(int StateID);		// return the FSMstate object pointer

	int StateTransition(int nCrurrentState, int Input);	// perform a state transition based on input and current state

protected:

    CFSM(void);								// set initial state of the FSM

	int				m_nStateNum;
	CFSMState*		m_lpFSMState[MAX_STATE];	// map containing all states of this FSM

    static CFSM     ms_this;
};


#endif // !defined(AFX_FSM_H__5BDFA770_7869_4B08_B0BC_D7C6AC789C9D__INCLUDED_)