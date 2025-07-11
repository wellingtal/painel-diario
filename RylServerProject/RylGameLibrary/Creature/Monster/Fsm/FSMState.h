// FSMState.h: interface for the CFSMState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMSTATE_H__4D5ECC39_C7A5_47E7_A774_9E79707B18D8__INCLUDED_)
#define AFX_FSMSTATE_H__4D5ECC39_C7A5_47E7_A774_9E79707B18D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFSMState
{
private:
	
	int m_dwNumberOfTransistions;			// maximum number of states supported by this state
	int *m_pdwInputs;						// input array for tranistions
	int *m_pdwOutputState;					// output state array
	int m_dwStateID;						// the unique ID of this state

	int FindTransitionIndex(int nID);

public:
	
	explicit CFSMState(int StateID, int Transitions);
	~CFSMState(void);

	void AddTransition(int Input, int OutputID);
	void DeleteTransition(int OutputID);
	
	int GetOutput(int Input);

	inline int GetID(void) { return m_dwStateID; }
};


#endif // !defined(AFX_FSMSTATE_H__4D5ECC39_C7A5_47E7_A774_9E79707B18D8__INCLUDED_)
