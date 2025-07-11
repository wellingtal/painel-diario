#ifndef _GAMA_SERVER_NETWORK_DISPATCH_H_
#define _GAMA_SERVER_NETWORK_DISPATCH_H_

// ���� ����
class CSession;

// Session���� ����ϴ� DispatchŬ������ ���� �������̽�.
class CPacketDispatch
{
public:

    CPacketDispatch(CSession& Session) : m_Session(Session) { }
    virtual ~CPacketDispatch() { }

    // In  : char* const lpStream_In             ���� ��Ʈ��
    //       unsigned long* dwStreamSize_InOut   ���� ��Ʈ�� ����

    // out : Return                              ��Ŷ �Ľ� ���� / ����(���н� ���� ����)
    //       unsigned long* dwStreamSize_InOut   �Ľ��� ����
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
//  �׽�Ʈ�� Sample Dispatchers....


//  ���� ��Ŷ�� ���� ������. ó���� ���� �ʴ´�.
class CNullDispatch : public CPacketDispatch
{
public:

    CNullDispatch(CSession& Session) : CPacketDispatch(Session) { }
    virtual ~CNullDispatch() { }

    virtual bool ParsePacket (char* const lpStream_In, unsigned long* dwStreamSIze_InOut) { return true; }
    virtual bool Dispatch() { return true; }
};


//  ���� ��Ŷ�� �״�� �ٽ� ���� �ش�.
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