#include "stdafx.h"
#include "MappedFile.h"


CMemoryMappedFile::CMemoryMappedFile()
:   m_hFile(INVALID_HANDLE_VALUE), m_hMap(0)
{


}


CMemoryMappedFile::~CMemoryMappedFile()
{
    Close();
}


bool CMemoryMappedFile::OpenFile(const char* szFileName, OpenMode eOpenMode)
{
    DWORD dwOpenMode    = 0;
    DWORD dwShareMode   = 0;
    DWORD dwProtect     = 0;
    
    switch(eOpenMode)
    {
    case READ:
        dwOpenMode          = GENERIC_READ;
        dwShareMode         = FILE_SHARE_READ;
        dwProtect           = PAGE_READONLY;
        m_dwFileViewMode    = FILE_MAP_READ;
        break;

    case READ_WRITE:
        dwOpenMode          = GENERIC_READ | GENERIC_WRITE;
        dwShareMode         = FILE_SHARE_READ | FILE_SHARE_WRITE;
        dwProtect           = PAGE_READWRITE;
        m_dwFileViewMode    = FILE_MAP_WRITE;
        break;
    };

    Close();

    m_hFile = CreateFile(szFileName, dwOpenMode, 
        dwShareMode, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if(INVALID_HANDLE_VALUE != m_hFile)
    {
        m_hMap = CreateFileMapping(m_hFile, 0, dwProtect, 0, 0, 0);
        if(NULL != m_hMap)
        {
            return true;
        }
        else
        {
            CloseHandle(m_hFile);
            m_hFile = INVALID_HANDLE_VALUE;
        }
    }

    return false;
}

// 메모리에 파일을 생성하거나 연다.
bool CMemoryMappedFile::CreateSharedMemory(DWORD dwLowSize, DWORD dwHighSize, const char* szFileName)
{
    Close();

    m_hMap = CreateFileMapping(INVALID_HANDLE_VALUE, 0, 
        PAGE_READWRITE, dwHighSize, dwLowSize, szFileName);

    return (0 != m_hMap);
}

bool CMemoryMappedFile::OpenSharedMemory(const char* szFileName, OpenMode eOpenMode)
{
    Close();

    switch(eOpenMode)
    {
    case READ:
        m_dwFileViewMode    = FILE_MAP_READ;
        break;

    case READ_WRITE:
        m_dwFileViewMode    = FILE_MAP_WRITE;
        break;
    };

    m_hMap = OpenFileMapping(m_dwFileViewMode, FALSE, szFileName);

    return (0 != m_hMap);
}




void CMemoryMappedFile::Close()
{
    if(0 != m_hMap)
    {
        CloseHandle(m_hMap);
        m_hMap = 0;
    }

    if(INVALID_HANDLE_VALUE != m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

bool CMemoryMappedFile::IsOpen()
{
    return (0 != m_hMap);
}


void* CMemoryMappedFile::GetFileView(DWORD dwLowPosition, 
                                        DWORD dwHighPosition,
                                        DWORD dwMappingSize)
{
    return MapViewOfFileEx(m_hMap, m_dwFileViewMode, 
        dwHighPosition, dwLowPosition, dwMappingSize, 0);
}


inline bool CMemoryMappedFile::CloseFileView(void* lpView)
{
    if(0 != lpView)
    {
        return (TRUE == UnmapViewOfFile(lpView));
    }

    return false;
}

inline bool CMemoryMappedFile::FlushFileView(void* lpView, DWORD dwBytesToFlush)
{
    return (TRUE == FlushViewOfFile(lpView, dwBytesToFlush));
}


bool CMemoryMappedFile::GetFileSize(unsigned long& dwLowSize, unsigned long& dwHighSize)
{
    if(INVALID_HANDLE_VALUE != m_hFile)
    {   
        dwLowSize = ::GetFileSize(m_hFile, &dwHighSize); 
        return true;
    }

    return false;
}



CFileMappedView::CFileMappedView(CMemoryMappedFile& mappedFile)
:   m_MappedFile(mappedFile), m_FilePointer(0)
{

}


CFileMappedView::~CFileMappedView()
{
    Close();
}

void* CFileMappedView::GetPointer(DWORD dwLowPosition, 
                            DWORD dwHighPosition,
                            DWORD dwMappingSize)
{
    Close();

    m_FilePointer = m_MappedFile.GetFileView(
        dwLowPosition, dwHighPosition, dwMappingSize);

    return m_FilePointer;
}

void CFileMappedView::Flush(DWORD dwBytesToFlush)
{
    m_MappedFile.FlushFileView(m_FilePointer, dwBytesToFlush);
}

void CFileMappedView::Close()
{
    m_MappedFile.CloseFileView(m_FilePointer);
    m_FilePointer = 0;
}


