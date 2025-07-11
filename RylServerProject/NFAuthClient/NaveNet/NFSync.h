/** 
 *  @file		NFSync.h
 *  @brief		Sync Ŭ����
 *  @remarks	
 *  @author		������(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once


namespace NaveNet { 

	/** 
	 *  @class        NFSync
	 *  @brief        Sync Ŭ����
	 *  @remarks      
	 *                NFSync Sync;				\r\n
	 *                NFSyncLock CL(&Sync);
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFSync
	{
	public:
		NFSync(VOID)
		{
			InitializeCriticalSection(&m_Sync);
		}

		~NFSync(VOID)
		{
			DeleteCriticalSection(&m_Sync);
		}

		inline VOID Enter(VOID)
		{
			EnterCriticalSection(&m_Sync);
		}

		inline VOID Leave(VOID)
		{
			LeaveCriticalSection(&m_Sync);
		}

	private:
		CRITICAL_SECTION	m_Sync;
	};

	/** 
	 *  @class        NFSyncLock
	 *  @brief        �̱۽������ ��ũ
	 *  @remarks      
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFSyncLock
	{
	public:
		NFSyncLock(LPVOID lpVoid)
		{
			m_pThis = (NFSync*)lpVoid;
			m_pThis->Enter();
		}

		~NFSyncLock(VOID)
		{
			if(m_pThis)
				m_pThis->Leave();
		}

	private:
		NFSync *m_pThis;
	};

}