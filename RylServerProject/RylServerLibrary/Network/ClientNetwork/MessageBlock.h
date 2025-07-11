#ifndef _CLIENTSOCKET_MESSAGE_BLOCK_H_
#define _CLIENTSOCKET_MESSAGE_BLOCK_H_

class CMessageBlock
{
public:

    explicit CMessageBlock();
    explicit CMessageBlock(size_t n);
    virtual ~CMessageBlock();

    bool is_valid() { return 0 != internal_buffer_; }

    // �б� - ���� ������ ��� ����.
    char* rd_ptr() const   { return rd_ptr_; }
    void rd_ptr(size_t n)  { rd_ptr_ += n; }
    void rd_ptr(char* ptr) { rd_ptr_ = ptr; }
    
    char* wr_ptr() const   { return wr_ptr_; }
    void wr_ptr(size_t n)  { wr_ptr_ += n; }
    void wr_ptr(char* ptr) { wr_ptr_ = ptr; }

    // ���� ���� �� �����(past-end) �� ���.
    char* begin() const { return internal_buffer_; }
    char* end() const { return internal_buffer_ + buffer_size_; }
   
    // �����͸� ���۷� ����.
    bool push(const void* ptr, size_t n);

    template<typename T>
    bool push(const T& t)
    {
        if(0 != wr_ptr_ && sizeof(T) <= capacity() - length())
        {
            memcpy(wr_ptr_, &t, sizeof(T));
            wr_ptr_ += sizeof(T);
            return true;
        }

        return false;
    }

    template<typename T>
    bool pop(T& t)
    {
        if(0 != rd_ptr_ && sizeof(T) <= length())
        {   
            memcpy(&t, rd_ptr_, sizeof(T));
            rd_ptr_ += sizeof(T);
            return true;
        }

        return false;
    }    

    // �̹� ���� �����͵��� ������ ���� �� ������ ������. �� �����͵��� ��ȿȭ��.
    void pop_read_data();

    size_t length() const { return wr_ptr_ - rd_ptr_; }     // ������ ����
    size_t capacity() const { return buffer_size_; }        // ���� ũ��
    size_t remain() const { return internal_buffer_ + buffer_size_ - wr_ptr_; }  // ���� ������ ����
    
    // ���� �ڽ��� ������ �����Ѵ�.
    static CMessageBlock* clone(CMessageBlock& msg_block);

    // ���� ũ�⸦ �ø���.
    bool resize(size_t n);

    // �ʱ�ȭ
    bool init(size_t n);

private:

    void destroy();

    char*   internal_buffer_;
    char*   rd_ptr_;
    char*   wr_ptr_;
    size_t  buffer_size_;
};

#endif