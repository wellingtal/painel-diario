#include "stdafx.h"
#include "minilzo.h"
#include "miniLZOWrapper.h"

#if _MSC_VER >= 1300
#define THREAD_LOCAL_STORAGE __declspec(thread) static 
#else
#define THREAD_LOCAL_STORAGE 
#endif

bool CMiniLZO::CheckLZOUsable()
{
	return LZO_E_OK == lzo_init();
}

bool CMiniLZO::Compress(const char* in, unsigned long in_len, 
								char* out, unsigned long* lp_out_len)
{
    THREAD_LOCAL_STORAGE lzo_voidp wrkmem[LZO1X_1_MEM_COMPRESS];
	return (LZO_E_OK == lzo1x_1_compress(reinterpret_cast<const lzo_byte*>(in), in_len, 
        reinterpret_cast<lzo_byte*>(out), reinterpret_cast<lzo_uintp>(lp_out_len), wrkmem));
}

bool CMiniLZO::Decompress(const char* in, unsigned long in_len, 
								  char* out, unsigned long* buffersize_in_out_len)
{
	return (LZO_E_OK == lzo1x_decompress_safe(reinterpret_cast<const lzo_byte*>(in), in_len,
		reinterpret_cast<lzo_byte*>(out), reinterpret_cast<lzo_uintp>(buffersize_in_out_len), 0));
}

