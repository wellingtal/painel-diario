#ifndef _GM_NETWORK_MULTI_DISPATCH_STORAGE_H_
#define _GM_NETWORK_MULTI_DISPATCH_STORAGE_H_

#include <map>
#include <Thread/Lock.h>

#define GET_MULTI_DISPATCH(instanceName, keyValue, type, table) \
	CMultiDispatch::Storage Storage##instanceName(table, keyValue); \
	type* instanceName = static_cast<type*>(Storage##instanceName.GetDispatch())

class CPacketDispatch;

class CMultiDispatch
{
public:

	typedef std::map<unsigned long, CPacketDispatch*> DispatchMap;

	class Storage
	{
	public:
		
		Storage(CMultiDispatch& multiDispatch, unsigned long dispatchKey);
		~Storage();
	
		CPacketDispatch* ReloadDispatch(unsigned long dispatchKey);
		CPacketDispatch* GetDispatch() { return m_lpDispatch; }

	private:

		// 힙에 생성 불가.
		static void* operator new(size_t size);
		static void operator delete(void* ptr);

		CPacketDispatch*	m_lpDispatch;
		CMultiDispatch&		m_MultiDispatch;
	};

	CMultiDispatch();
	~CMultiDispatch();

	bool SetDispatch(unsigned long dispatchKey, CPacketDispatch* lpDispatch);
	void ForceSetDispatch(unsigned long dispatchKey, CPacketDispatch* lpDispatch);
	void RemoveDispatch(unsigned long dispatchKey);

	template<typename FnProcess>
	void Process(FnProcess fnProcess)
	{
        LockType::Syncronize sync(m_DispatchLock); 
				
		DispatchMap::iterator pos = m_DispatchMap.begin();
		DispatchMap::iterator end = m_DispatchMap.end();

		for(;pos != end; ++pos)
		{			
			fnProcess(pos->first, *pos->second);
		}
	}

	size_t GetDispatchNum();
    bool IsEmpty();

private:

	void InternalRemoveDispatch(unsigned long dispatchKey);
	CPacketDispatch* GetDispatch(unsigned long dispatchKey);

	typedef CCSLock	LockType;

	LockType		m_DispatchLock;
	CACHE_PAD(DispatchLockPad, sizeof(LockType));
	DispatchMap		m_DispatchMap;
	
	friend class Storage;
};

#endif