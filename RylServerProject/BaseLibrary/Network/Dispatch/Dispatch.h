#ifndef _GAMA_SERVER_NETWORK_DISPATCH_H_
#define _GAMA_SERVER_NETWORK_DISPATCH_H_

// 전방 참조
class CSession;

// Session에서 사용하는 Dispatch클래스를 위한 인터페이스.
class CPacketDispatch
{
public:

    CPacketDispatch(CSession& Session) : m_Session(Session) { }
    virtual ~CPacketDispatch() { }

    // In  : char* const lpStream_In             버퍼 스트림
    //       unsigned long* dwStreamSize_InOut   버퍼 스트림 길이

    // out : Return                              패킷 파싱 성공 / 실패(실패시 접속 해제)
    //       unsigned long* dwStreamSize_InOut   파싱한 길이
	virtual bool ParsePacket(char* const lpStream_In, unsigned long* dwStreamSize_InOut) = 0;
    virtual bool Dispatch() = 0;
  
    virtual void Connected() { }
    virtual void Disconnected() { }
    
    CSession& GetSession() { return m_Session; }
    
protected:

    CSession&   m_Session;
};


class CDispatchFactory
{
public:

    virtual ~CDispatchFactory() = 0;

    virtual CPacketDispatch* CreateDispatch(CSession& Session) = 0;
    virtual void DeleteDispatch(CPacketDispatch* lpDispatch) = 0;
};

inline CDispatchFactory::~CDispatchFactory() { }


template<typename DispatchType>
class CDefaultDispatchFactory : public CDispatchFactory
{
public:

    virtual ~CDefaultDispatchFactory() { }

    virtual CPacketDispatch* CreateDispatch(CSession& Session) { return new DispatchType(Session); }
    virtual void DeleteDispatch(CPacketDispatch* lpDispatch) { delete lpDispatch; }
};
  

// -----------------------------------------------------------------------
//  테스트용 Sample Dispatchers....


//  받은 패킷을 전부 버린다. 처리도 하지 않는다.
class CNullDispatch : public CPacketDispatch
{
public:

    CNullDispatch(CSession& Session) : CPacketDispatch(Session) { }
    virtual ~CNullDispatch() { }

    virtual bool ParsePacket (char* const lpStream_In, unsigned long* dwStreamSIze_InOut) { return true; }
    virtual bool Dispatch() { return true; }
};


//  받은 패킷을 그대로 다시 돌려 준다.
class CEchoDispatch : public CPacketDispatch
{
public:

    CEchoDispatch(CSession& Session) : CPacketDispatch(Session) { }
    virtual ~CEchoDispatch() { }

    virtual void Connected();
    virtual void Disconnected();

    virtual bool ParsePacket (char* const lpStream_In, unsigned long* dwStreamSIze_InOut);
    virtual bool Dispatch() { return true; }
};

#endif