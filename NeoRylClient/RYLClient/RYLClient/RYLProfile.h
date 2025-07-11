/***************************************************************************************************
**
** RYLProfile.h
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

/*
** A node in the Profile Hierarchy Tree
*/
#pragma once

#include <winsock2.h>
#include <windows.h>

#include <map>
#include <string>
#include <vector>

class	CProfileNode {

public:
	CProfileNode( const char * name, CProfileNode * parent );
	~CProfileNode( void );

	CProfileNode * Get_Sub_Node( const char * name );

	CProfileNode * Get_Parent( void )		{ return Parent; }
	CProfileNode * Get_Sibling( void )		{ return Sibling; }
	CProfileNode * Get_Child( void )			{ return Child; }

	void				Reset( void );
	void				Call( void );
	bool				Return( void );

	const char *	Get_Name( void )				{ return Name; }
	int				Get_Total_Calls( void )		{ return TotalCalls; }
	float				Get_Total_Time( void )		{ return TotalTime; }

protected:

	const char *	Name;
	int				TotalCalls;
	float				TotalTime;
	__int64			StartTime;
	int				RecursionCounter;

	CProfileNode *	Parent;
	CProfileNode *	Child;
	CProfileNode *	Sibling;
};

/*
** An iterator to navigate through the tree
*/
class CProfileIterator
{
public:
	// Access all the children of the current parent
	void				First(void);
	void				Next(void);
	bool				Is_Done(void);

	void				Enter_Child( int index );		// Make the given child the new parent
	void				Enter_Largest_Child( void );	// Make the largest child the new parent
	void				Enter_Parent( void );			// Make the current parent's parent the new parent

	// Access the current child
	const char *	Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
	int				Get_Current_Total_Calls( void )	{ return CurrentChild->Get_Total_Calls(); }
	float				Get_Current_Total_Time( void )	{ return CurrentChild->Get_Total_Time(); }

	// Access the current parent
	const char *	Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
	int				Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
	float				Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }
	CProfileNode *	GetParentPtr() {return CurrentParent;}
	CProfileNode *	GetChildPtr() { return CurrentChild;}
protected:

	CProfileNode *	CurrentParent;
	CProfileNode *	CurrentChild;

	CProfileIterator( CProfileNode * start );
	friend	class		CProfileManager;
};


/*
** The Manager for the Profile system
*/
class	CProfileManager {
public:
	static	void						Start_Profile( const char * name );
	static	void						Stop_Profile( void );

	static	void						Reset( void );
	static	void						Increment_Frame_Counter( void );
	static	int						Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
	static	float						Get_Time_Since_Reset( void );

	static	CProfileIterator *	Get_Iterator( void )	{ return new CProfileIterator( &Root ); }
	static	void						Release_Iterator( CProfileIterator * iterator ) { delete iterator; }

private:
	static	CProfileNode			Root;
	static	CProfileNode *			CurrentNode;
	static	int						FrameCounter;
	static	__int64					ResetTime;
};


/*
** ProfileSampleClass is a simple way to profile a function's scope
** Use the PROFILE macro at the start of scope to time
*/
class	CProfileSample {
public:
	CProfileSample( const char * name )
	{ 
		CProfileManager::Start_Profile( name ); 
	}
	
	~CProfileSample( void )					
	{ 
		CProfileManager::Stop_Profile(); 
	}
};

#ifdef DEBUG
#define	PROFILE( name )			CProfileSample __profile( name )
#else
#define	PROFILE( name )
#endif


// --------------------------------------------------------------------------
// 패킷 트래픽 체크용 (by 로딘)

struct PacketProfileNode
{
	unsigned long	m_dwMinTimeInterval;		// 패킷을 주고 받는 간격의 최소 시간
	unsigned long	m_dwMaxTimeInterval;		// 패킷을 주고 받는 간격의 최대 시간
	unsigned long	m_dwAveTimeInterval;		// 패킷을 주고 받는 간격의 평균 시간

	unsigned long	m_dwMinFrame;				// 패킷을 주고 받는 사이에 뿌린 최소 프레임
	unsigned long	m_dwMaxFrame;				// 패킷을 주고 받는 사이에 뿌린 최대 프레임
	unsigned long	m_dwAveFrame;				// 패킷을 주고 받는 사이에 뿌린 평균 프레임

	unsigned long	m_dwNum;					// 패킷을 주고 받은 횟수
	bool			m_bSend;					// 패킷을 보낸 상태인가?

	unsigned long	m_dwSendTick;				// 패킷을 보낸 시각
	unsigned long	m_dwCurrentFrame;			// 패킷을 보낸 후 현재까지 뿌린 프레임

	std::map<unsigned char, unsigned short>	m_mapOtherPacket;

	PacketProfileNode()
	:	m_dwMinTimeInterval(0), m_dwMaxTimeInterval(0), m_dwAveTimeInterval(0), 
		m_dwMinFrame(0), m_dwMaxFrame(0), m_dwAveFrame(0), 
		m_dwNum(0), m_bSend(false), m_dwSendTick(0), m_dwCurrentFrame(0)
	{
	}
};

class CPacketProfileMgr
{
private:

	std::map<std::string, PacketProfileNode *>	m_mapPacket;


public:

	CPacketProfileMgr();
	~CPacketProfileMgr();

    void Send(std::string strPacketName);
	void Recv(std::string strPacketName);

	void UpdateFrame(void);
	void RecvOtherPacket(unsigned char cCmd);

	void Print(const char* strFileName);
};


class CSimpleProfile
{
public:

    struct ProfileStruct
    {
        ProfileStruct(LARGE_INTEGER startTime, LARGE_INTEGER diffTime, const char* szName, unsigned long dwFrames)
            :   m_startTime(startTime), m_diffTime(diffTime), m_szName(szName), m_dwFrames(dwFrames)
        {

        }

        LARGE_INTEGER   m_startTime;
        LARGE_INTEGER   m_diffTime;

        const char*     m_szName;
        unsigned long   m_dwFrames;
    };

    static CSimpleProfile& GetInstance();    
    void AddProfile(const char* szName, LARGE_INTEGER startTime);
    void IncrementFrameCounter() { ++m_dwFrameCounter; }

    void WriteProfile();
    void ClearProfile();

public:
    CSimpleProfile();
    ~CSimpleProfile();

private:

    typedef std::vector<ProfileStruct> ProfileVector;

    ProfileVector   m_profileVector;
    const size_t    m_MAX_PROFILE_NUM;

    unsigned long   m_dwFrameCounter;
};


class CSimpleProfileInstrument
{
public:

    CSimpleProfileInstrument(const char* szName)
        :   m_szName(szName)
    {
        QueryPerformanceCounter(&m_startTime);
    }

    ~CSimpleProfileInstrument()
    {
        CSimpleProfile::GetInstance().AddProfile(m_szName, m_startTime);
    }

private:

    LARGE_INTEGER   m_startTime;
    const char*     m_szName;
};

#define SIMPLE_PROFILE( name )  CSimpleProfileInstrument name(#name);

