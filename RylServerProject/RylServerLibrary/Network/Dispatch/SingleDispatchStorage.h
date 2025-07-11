#ifndef _GM_NETWORK_SINGLE_DISPATCH_STORAGE_H_
#define _GM_NETWORK_SINGLE_DISPATCH_STORAGE_H_

#include <Thread/Lock.h>

class CPacketDispatch;

#define GET_SINGLE_DISPATCH(instanceName, type, table) \
	CSingleDispatch::Storage Storage##instanceName(table); \
	type* instanceName = static_cast<type*>(Storage##instanceName.GetDispatch())

class CSingleDispatch
{
public:
	
	class Storage
	{
	public:
		
		Storage(CSingleDispatch& singleDispatch);
		~Storage(); 

		CPacketDispatch* ReloadDispatch();
		CPacketDispatch* GetDispatch() { return m_lpDispatch; }

	private:

		// 힙에 생성 불가.
		static void* operator new(size_t size);
		static void operator delete(void* ptr);

		CPacketDispatch* m_lpDispatch;
		CSingleDispatch& m_SingleDispatch;
	};

	CSingleDispatch();
	~CSingleDispatch();

	void SetDispatch(CPacketDispatch* lpDispatch);
	void RemoveDispatch(CPacketDispatch* lpDispatch);

	size_t GetDispatchNum();
    bool IsEmpty();

private:

	void InternalRemoveDispatch(CPacketDispatch* lpDispatch);
	CPacketDispatch* GetDispatch();

	typedef CCSLock		LockType;

	LockType			m_DispatchLock;
	CACHE_PAD(DispatchLockPad, sizeof(LockType));

	CPacketDispatch*	m_lpDispatch;

	friend class Storage;
};

#endif