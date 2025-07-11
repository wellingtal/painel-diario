// FastMath.h: interface for the CFastMath class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CUSTOM_MATH_FUNCTIONS_H_
#define _CUSTOM_MATH_FUNCTIONS_H_

#include <winsock2.h>
#include <windows.h>
#include <cmath>


namespace Math
{
    namespace Convert
    {
        extern const unsigned short m_FastHeToBi[0x100];

	    // SPX, IPX �ּ� ����� ���� ����ϴ� �Լ�
	    inline TCHAR	BiToHe(TCHAR cBin);	
	    inline void		AcToHe(char *szDst, char *szSrc, int iCount);

	    // String�� ������ �ٲ�(String�� 0x�� ���� Hex string�̴�.)
	    inline BYTE	    StrToHex08(const TCHAR *szSrc);
	    inline WORD     StrToHex16(const TCHAR *szSrc);
	    inline unsigned long    StrToHex32(const TCHAR *szSrc);
	    inline DWORD64  StrToHex64(const TCHAR *szSrc);

	    // String�� ������ �ٲ�(0x�� �ٴ���, ���� �ʴ������� ���� �ٲ� �� �ִ�)
	    inline BYTE 	Atoc(const TCHAR *szSrc);
	    inline WORD     Atos(const TCHAR *szSrc);
	    inline unsigned long    Atoi(const TCHAR *szSrc);
	    inline DWORD64  Atol64(const TCHAR *szSrc);

	    // ���� Hex String���� �ٲ�
	    inline void     Hex08ToStr(TCHAR *szDest, BYTE hex);
	    inline void     Hex16ToStr(TCHAR *szDest, WORD hex);
	    inline void     Hex32ToStr(TCHAR *szDest, unsigned long hex);
	    inline void     Hex64ToStr(TCHAR *szDest, DWORD64 hex);
    };

    namespace Random
    {
        // ���ϰ��� nBeginExtent ~ nEndExtent - 1�� ������ ���´�.
	    unsigned long ComplexRandom(int nEndExtent, int nBeginExtent = 0);	// ���� ������ ����.
		int SimpleRandom(unsigned long dwSeed, int nEndExtent, int nBeginExtent = 0);
    };

    namespace HashFunc
    {
	    // -------------------------------------------------------------------------------------------------
	    // String-to-Hash �Լ��� : http://www.cs.yorku.ca/~oz/hash.html ���� ����

	    inline unsigned long djb2Hash(const unsigned char *str);	// first reported by dan bernstein 
	    inline unsigned long sdbmHash(const unsigned char *str);	// this is one of the algorithms used in berkeley db        
	    inline unsigned long looseHash(const unsigned char *str);	// ���� ������ �ؽ�. �׳� ��� ����.
    };

	namespace Const
	{
		static float PI = 3.14159f;
	}
};

#include "Convert.inl"



inline unsigned long Math::HashFunc::djb2Hash(const unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

	while (c = *str++) { hash = ((hash << 5) + hash) + c; /* hash * 33 + c */ }
    return hash;
}


inline unsigned long Math::HashFunc::sdbmHash(const unsigned char *str)
{
    unsigned long hash = 0;
    int c;

	while (c = *str++) { hash = c + (hash << 6) + (hash << 16) - hash; }
    return hash;
}


inline unsigned long Math::HashFunc::looseHash(const unsigned char *str)
{
	unsigned int hash = 0;
	int c;

	while (c = *str++) { hash += c; }
	return hash;
}


#endif // !defined(AFX_FASTMATH_H__ED69578B_18C1_42EA_9C5E_888DC38101C2__INCLUDED_)
