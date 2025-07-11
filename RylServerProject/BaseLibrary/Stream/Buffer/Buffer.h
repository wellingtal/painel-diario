#ifndef _CBUFFER_H_
#define _CBUFFER_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>

#include "../../Network/Address/INET_Addr.h"

class CBufferFactory;

class CBuffer
{
public:

	CBuffer(CBufferFactory& bufferfactory);
	~CBuffer();

	void init(char* internal_buffer, unsigned long buffer_size);

    bool is_valid() { return 0 != internal_buffer_; }

    // 읽기 포인터 얻어 오기.
    char* rd_ptr() const   { return rd_ptr_; }
    void rd_ptr(size_t n)  { rd_ptr_ += n; }
    void rd_ptr(char* ptr) { rd_ptr_ = ptr; }
    
    // 읽기 포인터 얻어 오기.
    char* wr_ptr() const   { return wr_ptr_; }
    void wr_ptr(size_t n)  { wr_ptr_ += n; }
    void wr_ptr(char* ptr) { wr_ptr_ = ptr; }

    // 버퍼 시작 및 끝경과(past-end) 값 얻기.
    char* begin() const { return internal_buffer_; }
    char* end() const { return internal_buffer_ + buffer_size_; }
   
    // 데이터를 버퍼로 복사.
    bool push(const void* ptr, size_t n);

    // 이미 읽은 데이터들을 버리고 버퍼 및 내용을 재조정. 각 포인터들이 무효화됨.
    void pop_read_data();

    size_t length() const   { return wr_ptr_ - rd_ptr_; }     // 데이터 길이
    size_t capacity() const { return buffer_size_;      }     // 버퍼 크기
    size_t remain() const   { return internal_buffer_ + buffer_size_ - wr_ptr_; }  // 남은 데이터 공간
        
    CBuffer* next() { return next_; }
    void next(CBuffer* next) { next_ = next; }

	CBuffer* prev() { return prev_; }
	void prev(CBuffer* prev) { prev_ = prev; }

    INET_Addr& get_addr() { return address_; }

	CBufferFactory& GetBufferFactory() { return bufferfactory_; }

private:
	
    // -----------------------------------------------------------
	// variable

    char*       internal_buffer_;
    char*       rd_ptr_;
    char*       wr_ptr_;
    size_t      buffer_size_;

    CBuffer*    next_;
	CBuffer*    prev_;
    INET_Addr   address_;

	CBufferFactory& bufferfactory_;    
};


#endif