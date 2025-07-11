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

    // ������ �����ؼ� �����͸� ��� �´�.
    void* GetPointer(DWORD dwLowPosition = 0, 
        DWORD dwHighPosition = 0, DWORD dwMappingSize = 0);
    
    void Flush(DWORD dwBytesToFlush = 0);   // ����� ���� Flush�Ѵ�. (IPC���� �ʿ�)
    void Close();                           // ������ �ݴ´�.

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

    // �̹� �����ϴ� ������ ����.
    bool OpenFile(const char* szFileName, OpenMode eOpenMode);

    // �޸𸮿� ������ �����ϰų� ����.
    bool CreateSharedMemory(DWORD dwLowSize, DWORD dwHighSize = 0, const char* szFileName = 0);
    bool OpenSharedMemory(const char* szFileName, OpenMode eOpenMode);
    
    bool IsOpen();  // �̹� ���� �ִ��� ���ɴ�.
    void Close();   // ���� �ִ� ������ �ݴ´�.

    // ���� �ִ� ���� �����̸�, ������ ũ�⸦ �ش�.
    bool GetFileSize(unsigned long& dwLowSize, unsigned long& dwHighSize);

private:

    // ���� �並 �����Ѵ�.
    void* GetFileView(DWORD dwLowPosition, 
        DWORD dwHighPosition = 0, DWORD dwMappingSize = 0);

    static bool FlushFileView(void* lpView, DWORD dwBytesToFlush);  // ���� �並 Flush�Ѵ�.
    static bool CloseFileView(void* lpView);                        // ���� �並 �ݴ´�.

    HANDLE  m_hFile;
    HANDLE  m_hMap;
    DWORD   m_dwFileViewMode;
    
    friend class CFileMappedView;
};

#endif