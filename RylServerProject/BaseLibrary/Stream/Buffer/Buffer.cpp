#include "stdafx.h"
#include "Buffer.h"
#include "BufferFactory.h"

CBuffer::CBuffer(CBufferFactory& bufferfactory)
:   internal_buffer_(0), rd_ptr_(0), wr_ptr_(0), buffer_size_(0),
	bufferfactory_(bufferfactory), next_(0), prev_(0)
{

}

CBuffer::~CBuffer()
{

}


void CBuffer::init(char* internal_buffer, unsigned long buffer_size)
{    
    rd_ptr_ = wr_ptr_ = internal_buffer_ = internal_buffer;
    buffer_size_ = buffer_size;
	
	next_ = 0;
	prev_ = 0;

	address_.clear();
}


bool CBuffer::push(const void* ptr, size_t n)
{
    if(NULL != wr_ptr_ && n <= capacity() - length())
    {
        memcpy(wr_ptr_, ptr, n);
        wr_ptr_ += n;
        return true;
    }

    return false;
}

void CBuffer::pop_read_data()
{
    if(NULL != internal_buffer_)
    {        
        size_t n = length();
        memmove(internal_buffer_, rd_ptr_, n);
        wr_ptr_ = internal_buffer_ + n;
        rd_ptr_ = internal_buffer_;
    }
}
