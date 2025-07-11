#ifndef _SOCKET_FAMILY_H_
#define _SOCKET_FAMILY_H_

class CSocketFactory
{
public:

    virtual ~CSocketFactory() { }

  	SOCKET CreateConnectedSocket(const char* lpConnAddr, unsigned short usPort);
	SOCKET CreateBindedSocket(const char* lpBindAddr, unsigned short usPort);
	SOCKET CreateListenSocket(const char* lpListenAddr, unsigned short usPort, int nBackLog = MAX_BACKLOG);

    virtual SOCKET CreateSocket();
    virtual bool GetNetworkInfo(char* Address_Out, int nMaxBufferSize) = 0;
    virtual bool SetAddr(LPSOCKADDR lpSA, const char* szAddr, unsigned short usPort) = 0;

    inline int GetAddressLen() const { return m_nAddressLen; }

protected:

    CSocketFactory(int nSocketFamily, int nSocketType, 
        int nSocketProtocol, int nAddressLen);

    enum { MAX_BACKLOG = 63 };

    int m_nSockFamily;
    int m_nSockType;
    int m_nSockProtocol;
    int m_nAddressLen;
};


class CINETFamilyFactory : public CSocketFactory
{
public:

    virtual bool GetNetworkInfo(char* Address_Out, int nMaxBufferSize);
    virtual bool SetAddr(LPSOCKADDR lpSA, const char* szAddr, unsigned short usPort);
    
protected:

    CINETFamilyFactory(int nSocketType, int nSocketProtocol);    
    virtual ~CINETFamilyFactory() { }
};


class CTCPFactory : public CINETFamilyFactory
{
public:

    CTCPFactory();
    virtual ~CTCPFactory() { }
    static bool SetLinger(SOCKET hSocket, bool bOn, unsigned short usTimeOut);
};


class CUDPFactory : public CINETFamilyFactory
{
public:

    CUDPFactory();
	virtual ~CUDPFactory() { }
};

/*
//-----------------------------------------------------------------------------------------


class CIPXFamilyFactory : public CSocketFactory
{
public:

    virtual bool GetNetworkInfo(char* Address_Out, int nMaxBufferSize);
    virtual bool SetAddr(LPSOCKADDR lpSA, char* szAddr, unsigned short usPort);

protected:

    CIPXFamilyFactory(int nSocketType, int nSocketProtocol);
    virtual ~CIPXFamilyFactory() { }
};


class CSPXFactory : public CIPXFamilyFactory
{
public:

    CSPXFactory();
    virtual ~CSPXFactory() { }    
};


class CIPXFactory : public CIPXFamilyFactory
{
public:

    CIPXFactory();
    virtual ~CIPXFactory() { }    
};


//-----------------------------------------------------------------------------------------
*/

#endif