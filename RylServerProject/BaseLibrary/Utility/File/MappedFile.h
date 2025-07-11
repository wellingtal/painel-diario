#ifndef _GAMA_NETWORKLIB_MEMORY_MAPPED_FILE_
#define _GAMA_NETWORKLIB_MEMORY_MAPPED_FILE_

#include <winsock2.h>
#include <windows.h>


// forward decl
class CMemoryMappedFile;

class CFileMappedView
{
public:

    CFileMappedView(CMemoryMappedFile& mappedFile);
    ~CFileMappedView();

    // 파일을 매핑해서 포인터를 얻어 온다.
    void* GetPointer(DWORD dwLowPosition = 0, 
        DWORD dwHighPosition = 0, DWORD dwMappingSize = 0);
    
    void Flush(DWORD dwBytesToFlush = 0);   // 기록한 것을 Flush한다. (IPC에서 필요)
    void Close();                           // 파일을 닫는다.

private:

    CMemoryMappedFile&  m_MappedFile;
    void*               m_FilePointer;
};


class CMemoryMappedFile
{
public:

    enum OpenMode
    {
        READ,
        READ_WRITE
    };

    CMemoryMappedFile();    
    ~CMemoryMappedFile();

    // 이미 존재하는 파일을 연다.
    bool OpenFile(const char* szFileName, OpenMode eOpenMode);

    // 메모리에 파일을 생성하거나 연다.
    bool CreateSharedMemory(DWORD dwLowSize, DWORD dwHighSize = 0, const char* szFileName = 0);
    bool OpenSharedMemory(const char* szFileName, OpenMode eOpenMode);
    
    bool IsOpen();  // 이미 열려 있는지 살핀다.
    void Close();   // 열려 있는 파일을 닫는다.

    // 열려 있는 것이 파일이면, 파일의 크기를 준다.
    bool GetFileSize(unsigned long& dwLowSize, unsigned long& dwHighSize);

private:

    // 파일 뷰를 생성한다.
    void* GetFileView(DWORD dwLowPosition, 
        DWORD dwHighPosition = 0, DWORD dwMappingSize = 0);

    static bool FlushFileView(void* lpView, DWORD dwBytesToFlush);  // 파일 뷰를 Flush한다.
    static bool CloseFileView(void* lpView);                        // 파일 뷰를 닫는다.

    HANDLE  m_hFile;
    HANDLE  m_hMap;
    DWORD   m_dwFileViewMode;
    
    friend class CFileMappedView;
};

#endif