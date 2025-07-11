#include "stdafx.h"
#include "PacketStatistics.h"

#include <algorithm>
#include <functional>
#include <Utility/Debug/DebugUtils.h>

void MakeBytesToString(char* szString, unsigned long dwStringLen, double fBytes)
{
    _snprintf(szString, dwStringLen - 1, "%16.4fMb/%16.4fKb/%16.4fBytes", 
        fBytes / 1024 / 1024, fBytes / 1024, fBytes);

    szString[dwStringLen - 1] = 0;
}

void LogPacketStatData(FILE* logFile, const char* szTotalSummeryHeader, time_t tDiffTime,
                       CPacketStatistics::PacketData* lpPos, CPacketStatistics::PacketData* lpEnd) 


{
    const int MAX_BUFFER = 128;
    char szTotalBytes[MAX_BUFFER];
    CPacketStatistics::PacketData totalData;
    
    for(unsigned char cCmd = 0; lpPos != lpEnd; ++lpPos, ++cCmd)
    {
        if (0.0f != lpPos->m_Count)
        {
            MakeBytesToString(szTotalBytes, MAX_BUFFER, lpPos->m_Bytes);

            fprintf(logFile, "\t %3d(0x%02X) : %16.4f : (%s) : %16.4fBytes/Sec\n",
                cCmd, cCmd, lpPos->m_Count, szTotalBytes,
                lpPos->m_Bytes / tDiffTime);

            totalData.m_Bytes += lpPos->m_Bytes;
            totalData.m_Count += lpPos->m_Count;
        }
    }

    MakeBytesToString(szTotalBytes, MAX_BUFFER, totalData.m_Bytes);

    fprintf(logFile, 
        "\n %s"
        "\n Total Data  : %s/%16.4fBytes/Sec"
        "\n Total Count : %16.4f/%16.4f(Count/Sec)\n\n",
        szTotalSummeryHeader, szTotalBytes, totalData.m_Bytes / tDiffTime,
        totalData.m_Count, totalData.m_Count / tDiffTime);
}


CPacketStatistics::PacketData::PacketData()
: m_Bytes(0.0f)
, m_Count(0.0f)
{

}
void CPacketStatistics::PacketData::Clear() 
{
    m_Bytes = m_Count = 0.0f; 
}

CPacketStatistics::CompressData::CompressData()
: m_CompressCount(0.0f)
, m_FailedCount(0.0f)
, m_TotalBytes(0.0f)
, m_TotalCompressed(0.0f) 
{

}

void CPacketStatistics::CompressData::Clear() 
{ 
    m_CompressCount = m_FailedCount = 
        m_TotalBytes = m_TotalCompressed = 0.0f; 
}


CPacketStatistics::CPacketStatistics()
:   m_StartTime(time(0)), m_fnPrefix(0), m_fnPostfix(0)
{
    

}

CPacketStatistics::~CPacketStatistics()
{
    Log();
}


CPacketStatistics& CPacketStatistics::GetInstance()
{
    static CPacketStatistics packetStatistics;
    return packetStatistics;
}


void CPacketStatistics::Recv(unsigned char cCmd, unsigned long dwPacketSize)
{
    PacketData& packetData = m_RecvData[cCmd];

    packetData.m_Bytes += dwPacketSize;
    ++packetData.m_Count;
}

void CPacketStatistics::Send(unsigned char cCmd, unsigned long dwPacketSize)
{
    PacketData& packetData = m_SendData[cCmd];

    packetData.m_Bytes += dwPacketSize;
    ++packetData.m_Count;    
}

// 압축관련 통계 작성
void CPacketStatistics::CheckCompression(unsigned char cCmd, 
                                         unsigned long dwSrcSize, unsigned long dwDstSize)
{
    CompressData& compressData = m_CompressionData[cCmd];
    
    compressData.m_TotalBytes += dwSrcSize;
    compressData.m_TotalCompressed += dwDstSize;

    ++compressData.m_CompressCount;
    if (dwSrcSize < dwDstSize) { ++compressData.m_FailedCount; }
}

void CPacketStatistics::Clear()
{
    std::for_each(m_RecvData, m_RecvData + UCHAR_MAX, std::mem_fun_ref(&PacketData::Clear));
    std::for_each(m_SendData, m_SendData + UCHAR_MAX, std::mem_fun_ref(&PacketData::Clear));
    std::for_each(m_CompressionData, m_CompressionData + UCHAR_MAX, std::mem_fun_ref(&CompressData::Clear));

    m_StartTime = time(0);
}

void CPacketStatistics::SetUserMessageFunc(FnPrintUserMessage fnPreFix, FnPrintUserMessage fnPostFix)
{
    m_fnPrefix  = fnPreFix;
    m_fnPostfix = fnPostFix;
}

bool CPacketStatistics::Log()
{
	char strLogPath[MAX_PATH];
    char strLogFileName[MAX_PATH];

	DbgUtils::SetProgramName(strLogPath, MAX_PATH, 0);

	// create the directory if it doesn't exist
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strLogPath) && 
        !CreateDirectory(strLogPath, 0))
	{
		return false;
	}

    unsigned long dwSpinCount   = 0;
    unsigned long dwMaxFileSize = 50 * 1024 * 1024;

	while (true) 
	{
		_sntprintf(strLogFileName, MAX_PATH, "%s/Statistics-%s-%05d.log", 
			strLogPath, strLogPath, dwSpinCount);
	
		HANDLE hLogFile = CreateFile(strLogFileName, GENERIC_WRITE, 
			FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

		if(INVALID_HANDLE_VALUE == hLogFile)
		{
			break;
		}
		else
		{
			unsigned long dwHighValue = 0;
			unsigned long dwFileSize = GetFileSize(hLogFile, &dwHighValue);

			CloseHandle(hLogFile);

			if(dwHighValue == 0 && dwFileSize < dwMaxFileSize)
			{
				break;
			}
		}

		++dwSpinCount;
	}

	FILE* logFile = fopen(strLogFileName, "at");

    if(0 != logFile)
    {
        // Prefix
        if(0 != m_fnPrefix)
        {
            m_fnPrefix(logFile);
        }

        // 실제 로그
        time_t tCurrent = time(0);
        time_t tDiffTime = tCurrent - m_StartTime;

        time_t tDiffHour   = tDiffTime / 3600;
        time_t tDiffMinute = (tDiffTime % 3600) / 60;
        time_t tDiffSecond = (tDiffTime % 3600) % 60;

        struct tm tmServerStart = *localtime(&m_StartTime);
        struct tm tmCurrent     = *localtime(&tCurrent);

        fprintf(logFile, "\t-- Server Packet Statistics -- \n"
            "\tLog Start Time    : %04dyear %02dmon %02dday %02dhour %02dmin %02dsec\n"
            "\tCurrent Log Time  : %04dyear %02dmon %02dday %02dhour %02dmin %02dsec\n"
            "\tLog Duration      : %02dhour %02dmin %02dsec\n\n",

            tmServerStart.tm_year + 1900, tmServerStart.tm_mon + 1, tmServerStart.tm_mday, 
            tmServerStart.tm_hour, tmServerStart.tm_min, tmServerStart.tm_sec,

            tmCurrent.tm_year + 1900, tmCurrent.tm_mon + 1, tmCurrent.tm_mday, 
            tmCurrent.tm_hour, tmCurrent.tm_min, tmCurrent.tm_sec,

            tDiffHour, tDiffMinute, tDiffSecond);

        const int MAX_BUFFER = 128;
        char szTotalBytes[MAX_BUFFER];        
        char szCompressedBytes[MAX_BUFFER];

        // 받은 데이터 로그
        LogPacketStatData(logFile, "Recv Packet", tDiffTime, m_RecvData, m_RecvData + UCHAR_MAX);

        // 보낸 데이터 로그
        LogPacketStatData(logFile, "Send Packet", tDiffTime, m_SendData, m_SendData + UCHAR_MAX);

        // 압축 데이터 로그
        CompressData* lpCompressPos = m_CompressionData;
        CompressData* lpCompressEnd = m_CompressionData + UCHAR_MAX;

        CompressData totalCount;
        for(unsigned char cCmd = 0; lpCompressPos != lpCompressEnd; ++lpCompressPos, ++cCmd)
        {
            if (0.0f != lpCompressPos->m_CompressCount)
            {
                MakeBytesToString(szTotalBytes, MAX_BUFFER, lpCompressPos->m_TotalBytes);
                MakeBytesToString(szCompressedBytes, MAX_BUFFER, lpCompressPos->m_TotalCompressed);

                fprintf (logFile, "\t %3d(0x%02X) : %16.4f(Failed %16.4f) : Total(%16s) : Compressed(%16s)\n",
                    cCmd, cCmd, lpCompressPos->m_CompressCount, lpCompressPos->m_FailedCount,
                    szTotalBytes, szCompressedBytes);

                totalCount.m_CompressCount   += lpCompressPos->m_CompressCount;
                totalCount.m_FailedCount     += lpCompressPos->m_FailedCount;
                totalCount.m_TotalBytes      += lpCompressPos->m_TotalBytes;
                totalCount.m_TotalCompressed += lpCompressPos->m_TotalCompressed;
            }
        }

        MakeBytesToString(szTotalBytes, MAX_BUFFER, totalCount.m_TotalBytes);
        MakeBytesToString(szCompressedBytes, MAX_BUFFER, totalCount.m_TotalCompressed);

        fprintf(logFile, 
            "\n Check compression status - not actually sent"
            "\n Total Data            : %s"
            "\n Total Data Compressed : %s"
            "\n Total Count           : %16.4f/Failed(%16.4f)\n\n\n",
            szTotalBytes, szCompressedBytes, totalCount.m_CompressCount, totalCount.m_FailedCount);

        // Postfix
        if(0 != m_fnPostfix)
        {
            m_fnPostfix(logFile);
        }

        fclose(logFile);
    }

    return true;    
}
