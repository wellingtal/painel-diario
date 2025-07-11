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

    // �б� ������ ��� ����.
    char* rd_ptr() const   { return rd_ptr_; }
    void rd_ptr(size_t n)  { rd_ptr_ += n; }
    void rd_ptr(char* ptr) { rd_ptr_ = ptr; }
    
    // �б� ������ ��� ����.
    char* wr_ptr() const   { return wr_ptr_; }
    void wr_ptr(size_t n)  { wr_ptr_ += n; }
    void wr_ptr(char* ptr) { wr_ptr_ = ptr; }

    // ���� ���� �� �����(past-end) �� ���.
    char* begin() const { return internal_buffer_; }
    char* end() const { return internal_buffer_ + buffer_size_; }
   
    // �����͸� ���۷� ����.
    bool push(const void* ptr, size_t n);

    // �̹� ���� �����͵��� ������ ���� �� ������ ������. �� �����͵��� ��ȿȭ��.
    void pop_read_data();

    size_t length() const   { return wr_ptr_ - rd_ptr_; }     // ������ ����
    size_t capacity() const { return buffer_size_;      }     // ���� ũ��
    size_t remain() const   { return internal_buffer_ + buffer_size_ - wr_ptr_; }  // ���� ������ ����
        
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