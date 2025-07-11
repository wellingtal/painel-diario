/***************************************************************************************************
**
** profile.cpp
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

#include "RYLProfile.h"
#include "GMMemory.h"


inline void Profile_Get_Ticks(_int64 * ticks)
{
	__asm
	{
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
		_emit 31h
		mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
	}
}

inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;
	
	if (_CPUFrequency == -1.0f) {
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 
	
	return _CPUFrequency;
}

/***************************************************************************************************
**
** CProfileNode
**
***************************************************************************************************/

/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - pointer to a static string which is the name of this profile node                    *
 * parent - parent pointer                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The name is assumed to be a static pointer, only the pointer is stored and compared for     *
 * efficiency reasons.                                                                         *
 *=============================================================================================*/
CProfileNode::CProfileNode( const char * name, CProfileNode * parent ) :
	Name( name ),
	TotalCalls( 0 ),
	TotalTime( 0 ),
	StartTime( 0 ),
	RecursionCounter( 0 ),
	Parent( parent ),
	Child( NULL ),
	Sibling( NULL )
{
	Reset();
}


CProfileNode::~CProfileNode( void )
{
	delete Child;
	delete Sibling;
}


/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - static string pointer to the name of the node we are searching for                   *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All profile names are assumed to be static strings so this function uses pointer compares   *
 * to find the named node.                                                                     *
 *=============================================================================================*/
CProfileNode * CProfileNode::Get_Sub_Node( const char * name )
{
	// Try to find this sub node
	CProfileNode * child = Child;
	while ( child ) {
		if ( child->Name == name ) {
			return child;
		}
		child = child->Sibling;
	}

	// We didn't find it, so add it
	CProfileNode * node = new CProfileNode( name, this );
	node->Sibling = Child;
	Child = node;
	return node;
}


void	CProfileNode::Reset( void )
{
	TotalCalls = 0;
	TotalTime = 0.0f;

	if ( Child ) {
		Child->Reset();
	}
	if ( Sibling ) {
		Sibling->Reset();
	}
}


void	CProfileNode::Call( void )
{
	TotalCalls++;
	if (RecursionCounter++ == 0) {
		Profile_Get_Ticks(&StartTime);
	}
}


bool	CProfileNode::Return( void )
{
	if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
		__int64 time;
		Profile_Get_Ticks(&time);
		time-=StartTime;
		TotalTime += (float)time / Profile_Get_Tick_Rate();
	}
	return ( RecursionCounter == 0 );
}


/***************************************************************************************************
**
** CProfileIterator
**
***************************************************************************************************/
CProfileIterator::CProfileIterator( CProfileNode * start )
{
	CurrentParent = start;
	CurrentChild = CurrentParent->Get_Child();
}


void	CProfileIterator::First(void)
{
	CurrentChild = CurrentParent->Get_Child();
}


void	CProfileIterator::Next(void)
{
	CurrentChild = CurrentChild->Get_Sibling();
}


bool	CProfileIterator::Is_Done(void)
{
	return CurrentChild == NULL;
}


void	CProfileIterator::Enter_Child( int index )
{
	CurrentChild = CurrentParent->Get_Child();
	while ( (CurrentChild != NULL) && (index != 0) ) {
		index--;
		CurrentChild = CurrentChild->Get_Sibling();
	}

	if ( CurrentChild != NULL ) {
		CurrentParent = CurrentChild;
		CurrentChild = CurrentParent->Get_Child();
	}
}


void	CProfileIterator::Enter_Parent( void )
{
	if ( CurrentParent->Get_Parent() != NULL ) {
		CurrentParent = CurrentParent->Get_Parent();
	}
	CurrentChild = CurrentParent->Get_Child();
}


/***************************************************************************************************
**
** CProfileManager
**
***************************************************************************************************/

CProfileNode	CProfileManager::Root( "Root", NULL );
CProfileNode *	CProfileManager::CurrentNode = &CProfileManager::Root;
int				CProfileManager::FrameCounter = 0;
__int64			CProfileManager::ResetTime = 0;


/***********************************************************************************************
 * CProfileManager::Start_Profile -- Begin a named profile                                    *
 *                                                                                             *
 * Steps one level deeper into the tree, if a child already exists with the specified name     *
 * then it accumulates the profiling; otherwise a new child node is added to the profile tree. *
 *                                                                                             *
 * INPUT:                                                                                      *
 * name - name of this profiling record                                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The string used is assumed to be a static string; pointer compares are used throughout      *
 * the profiling code for efficiency.                                                          *
 *=============================================================================================*/
void	CProfileManager::Start_Profile( const char * name )
{
	if (name != CurrentNode->Get_Name()) {
		CurrentNode = CurrentNode->Get_Sub_Node( name );
	} 
	
	CurrentNode->Call();
}


/***********************************************************************************************
 * CProfileManager::Stop_Profile -- Stop timing and record the results.                       *
 *=============================================================================================*/
void	CProfileManager::Stop_Profile( void )
{
	// Return will indicate whether we should back up to our parent (we may
	// be profiling a recursive function)
	if (CurrentNode->Return()) {
		CurrentNode = CurrentNode->Get_Parent();
	}
}


/***********************************************************************************************
 * CProfileManager::Reset -- Reset the contents of the profiling system                       *
 *                                                                                             *
 *    This resets everything except for the tree structure.  All of the timing data is reset.  *
 *=============================================================================================*/
void	CProfileManager::Reset( void )
{ 
	Root.Reset(); 
	FrameCounter = 0;
	Profile_Get_Ticks(&ResetTime);
}


/***********************************************************************************************
 * CProfileManager::Increment_Frame_Counter -- Increment the frame counter                    *
 *=============================================================================================*/
void CProfileManager::Increment_Frame_Counter( void )
{
	FrameCounter++;
}


/***********************************************************************************************
 * CProfileManager::Get_Time_Since_Reset -- returns the elapsed time since last reset         *
 *=============================================================================================*/
float CProfileManager::Get_Time_Since_Reset( void )
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= ResetTime;
	return (float)time / Profile_Get_Tick_Rate();
}


// --------------------------------------------------------------------------
// 패킷 트래픽 체크용 (by 로딘)

CPacketProfileMgr::CPacketProfileMgr()
{
}

CPacketProfileMgr::~CPacketProfileMgr()
{
	for (std::map<std::string, PacketProfileNode *>::iterator it = m_mapPacket.begin(); it != m_mapPacket.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}

	m_mapPacket.clear();
}

void CPacketProfileMgr::Send(std::string strPacketName)
{
	PacketProfileNode* lpNode = NULL;

	std::map<std::string, PacketProfileNode *>::iterator it = m_mapPacket.find(strPacketName);
	if (it != m_mapPacket.end())
	{
		lpNode = it->second;
	}
	else
	{
		lpNode = new PacketProfileNode;
		m_mapPacket.insert(std::make_pair(strPacketName, lpNode));
	}

	if (NULL != lpNode)
	{
		lpNode->m_dwSendTick = ::GetTickCount();
		lpNode->m_bSend = true;
	}
}

void CPacketProfileMgr::Recv(std::string strPacketName)
{
	std::map<std::string, PacketProfileNode *>::iterator it = m_mapPacket.find(strPacketName);
	if (it != m_mapPacket.end())
	{
		PacketProfileNode* lpNode = it->second;

		unsigned long dwNum = lpNode->m_dwNum + 1;

		unsigned long dwTimeInterval = ::GetTickCount() - lpNode->m_dwSendTick;
		lpNode->m_dwMinTimeInterval = (0 == lpNode->m_dwMinTimeInterval) ? dwTimeInterval : min(lpNode->m_dwMinTimeInterval, dwTimeInterval);
		lpNode->m_dwMaxTimeInterval = max(lpNode->m_dwMaxTimeInterval, dwTimeInterval);
		lpNode->m_dwAveTimeInterval = (lpNode->m_dwAveTimeInterval * lpNode->m_dwNum + dwTimeInterval) / dwNum;

		lpNode->m_dwMinFrame = (0 == lpNode->m_dwMinFrame) ? lpNode->m_dwCurrentFrame : min(lpNode->m_dwMinFrame, lpNode->m_dwCurrentFrame);
		lpNode->m_dwMaxFrame = max(lpNode->m_dwMaxFrame, lpNode->m_dwCurrentFrame);
		lpNode->m_dwAveFrame = (lpNode->m_dwAveFrame * lpNode->m_dwNum + lpNode->m_dwCurrentFrame) / dwNum;

		lpNode->m_dwNum = dwNum;
		lpNode->m_dwCurrentFrame = 0;
		lpNode->m_bSend = false;
	}
}

void CPacketProfileMgr::UpdateFrame(void)
{
	for (std::map<std::string, PacketProfileNode *>::iterator it = m_mapPacket.begin(); it != m_mapPacket.end(); ++it)
	{
		PacketProfileNode* lpNode = it->second;
		if (true == lpNode->m_bSend)
		{
			++lpNode->m_dwCurrentFrame;
		}
	}
}

void CPacketProfileMgr::RecvOtherPacket(unsigned char cCmd)
{
	for (std::map<std::string, PacketProfileNode *>::iterator it = m_mapPacket.begin(); it != m_mapPacket.end(); ++it)
	{
		PacketProfileNode* lpNode = it->second;
		if (true == lpNode->m_bSend)
		{
			std::map<unsigned char, unsigned short>::iterator otherIt = lpNode->m_mapOtherPacket.find(cCmd);
			if (otherIt != lpNode->m_mapOtherPacket.end())
			{
				++otherIt->second;
			}
			else
			{
				lpNode->m_mapOtherPacket.insert(std::make_pair(cCmd, 1));
			}
		}
	}
}

void CPacketProfileMgr::Print(const char* strFileName)
{
	FILE* fp = fopen(strFileName, "wt");
	
	for (std::map<std::string, PacketProfileNode *>::iterator it = m_mapPacket.begin(); it != m_mapPacket.end(); ++it)
	{
		PacketProfileNode* lpNode = it->second;

		fprintf(fp, "\n\n** %s", it->first.c_str());

		fprintf(fp, "\n서버 반응 최소 시간 : %.3f초", lpNode->m_dwMinTimeInterval / 1000.0f);
		fprintf(fp, "\n서버 반응 최대 시간 : %.3f초", lpNode->m_dwMaxTimeInterval / 1000.0f);
		fprintf(fp, "\n서버 반응 평균 시간 : %.3f초", lpNode->m_dwAveTimeInterval / 1000.0f);

		fprintf(fp, "\n패킷을 주고 받는 동안 출력된 최소 프레임 : %d", lpNode->m_dwMinFrame);
		fprintf(fp, "\n패킷을 주고 받는 동안 출력된 최대 프레임 : %d", lpNode->m_dwMaxFrame);
		fprintf(fp, "\n패킷을 주고 받는 동안 출력된 평균 프레임 : %d", lpNode->m_dwAveFrame);

		fprintf(fp, "\n패킷을 주고 받는 동안 받은 다른 패킷(패킷 번호, 횟수)");
		for (std::map<unsigned char, unsigned short>::iterator otherIt = lpNode->m_mapOtherPacket.begin(); 
			otherIt != lpNode->m_mapOtherPacket.end(); ++otherIt)
		{
			fprintf(fp, "\n(0x%02x, %d)", otherIt->first, otherIt->second);
		}
	}

	fclose(fp);
}



CSimpleProfile simpleProfile;

CSimpleProfile::CSimpleProfile()
:   m_MAX_PROFILE_NUM(60 * 60 * 33 + 100)
{
    ClearProfile();
}

CSimpleProfile::~CSimpleProfile()
{

}


CSimpleProfile& CSimpleProfile::GetInstance()
{
    return simpleProfile;    
}

void CSimpleProfile::AddProfile(const char* szName, LARGE_INTEGER startTime)
{
    bool bAdded = false;

    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);

    LARGE_INTEGER diffTime;
    diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;

    ProfileVector::reverse_iterator rpos = m_profileVector.rbegin();
    ProfileVector::reverse_iterator rend = m_profileVector.rend();

    for(; rpos != rend; ++rpos)
    {
        ProfileStruct& profStruct = *rpos;

        if(profStruct.m_dwFrames == m_dwFrameCounter &&
            profStruct.m_szName == szName)
        {
            profStruct.m_diffTime.QuadPart += diffTime.QuadPart;
            bAdded = true;
        }
        else if(profStruct.m_dwFrames < m_dwFrameCounter)
        {
            break;
        }
    }

    if(!bAdded)
    {
        m_profileVector.push_back(
            ProfileStruct(startTime, diffTime, szName, m_dwFrameCounter));
    }

    if(m_MAX_PROFILE_NUM < m_profileVector.size() + 100)
    {
        WriteProfile();
        ClearProfile();
    }
}


void CSimpleProfile::WriteProfile()
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    char szFileName[MAX_PATH];
    _snprintf(szFileName, MAX_PATH - 1, "./RylPerformanceLog%04d%02d%02d-%02d%02d%02d%03d.txt",
        sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

    szFileName[MAX_PATH - 1] = 0;

    FILE* lpFile = fopen(szFileName, "wt");
    if(0 != lpFile)
    {
        LARGE_INTEGER perSecCount;
        QueryPerformanceFrequency(&perSecCount);

        fprintf(lpFile, "CounterPerSec\t%I64u\n", perSecCount.QuadPart);

        fprintf(lpFile, "FrameCounter\tName\tStartCounter\tDiffCounter\n");

        ProfileVector::iterator pos = m_profileVector.begin();
        ProfileVector::iterator end = m_profileVector.end();

        for(; pos != end; ++pos)
        {
            ProfileStruct& propStruct = *pos;

            fprintf(lpFile, "%u\t%s\t%I64u\t%I64u\n",
                propStruct.m_dwFrames, 
                propStruct.m_szName,
                propStruct.m_startTime.QuadPart,
                propStruct.m_diffTime.QuadPart);
        }

        fclose(lpFile);
    }
}

void CSimpleProfile::ClearProfile()
{
    m_dwFrameCounter = 0;
    m_profileVector.clear();
    m_profileVector.reserve(m_MAX_PROFILE_NUM);
}
