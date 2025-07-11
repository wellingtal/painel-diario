#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

namespace Nave { namespace Sync {
/*
	class CTest : public CMTSync<CTest>
	{
	public:
		void Test()
		{
			CTSync Sync;
		}
	};
*/
	class CSync
	{
	public:
		CSync(VOID)
		{
			InitializeCriticalSection(&mSync);
		}

		~CSync(VOID)
		{
			DeleteCriticalSection(&mSync);
		}

		inline VOID Enter(VOID)
		{
			EnterCriticalSection(&mSync);
		}

		inline VOID Leave(VOID)
		{
			LeaveCriticalSection(&mSync);
		}

	private:
		CRITICAL_SECTION	mSync;
	};

	class CSSync
	{
	public:
		CSSync(LPVOID lpVoid)
		{
			m_pThis = (CSync*)lpVoid;
			m_pThis->Enter();
		}

		~CSSync(VOID)
		{
			if(m_pThis)
				m_pThis->Leave();
		}

	protected:
		CSync *m_pThis;
	};

	template <class T>
	class CMTSync
	{
	friend class CTSync;
	public:
		class CTSync
		{
		public:
			CTSync(VOID)
			{
				T::mSync.Enter();
			}

			~CTSync(VOID)
			{
				T::mSync.Leave();
			}
		};

	private:
		static CSync mSync;
	};

	template <class T>
	CSync CMTSync<T>::mSync;

}}