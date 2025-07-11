#ifndef _MATH_CONVERT_INLINE_H_
#define _MATH_CONVERT_INLINE_H_

#include <cstdlib>
#include <tchar.h>

inline void Math::Convert::Hex08ToStr( TCHAR *szDest, BYTE hex )
{
	*((WORD *) szDest) = m_FastHeToBi[ hex ];	szDest += 2;
	*(szDest) = '\0';
}

inline void Math::Convert::Hex16ToStr( TCHAR *szDest, WORD hex )
{
	LPBYTE pSrc = (LPBYTE) &hex;

#ifdef BIG_ENDIAN
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
#else
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
#endif
	*(szDest + 4) = '\0';
}

inline void Math::Convert::Hex32ToStr( TCHAR *szDest, unsigned long hex )
{
	LPBYTE pSrc = (LPBYTE) &hex;

#ifdef BIG_ENDIAN
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
#else
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
#endif
	*(szDest + 8) = '\0';
}

inline void Math::Convert::Hex64ToStr( TCHAR *szDest, DWORD64 hex )
{
	LPBYTE pSrc = (LPBYTE) &hex;

#ifdef BIG_ENDIAN
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  8)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 10)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 12)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 14)) = m_FastHeToBi[ *(pSrc++) ];
#else
	*((WORD *) (szDest + 14)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 12)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 10)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  8)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
#endif
	*(szDest + 16) = '\0';
}


//
// this function returns the equivalent binary value for an individual character specified in the ascii format
inline TCHAR Math::Convert::BiToHe(TCHAR cBin)
{
	if((cBin >= '0') && (cBin <= '9')) 
    {
		return (cBin - '0');
	}
    else if((cBin >= 'A') && (cBin <= 'F'))
    {
		return (cBin - 'A' + 0xA);
	}
    if((cBin >= 'a') && (cBin <= 'f'))
    {
		return (cBin -'a' + 0xA);
	}

	return cBin;
}

inline void Math::Convert::AcToHe(char *szDst, char *szSrc, int iCount)
{
	while(iCount--) 
    {
		*szDst = BiToHe(*szSrc) << 4;
		*szSrc++;
		*szDst += BiToHe(*szSrc);
		*szSrc++;
		*szDst++;
	}	
}

inline BYTE Math::Convert::StrToHex08(const TCHAR *szSrc)
{
	const TCHAR*     pStart	= szSrc + 2;
	BYTE            cHex = 0;

    TCHAR c1, c2;

	for (int i=0; i<1; ++i) 
    {
		c1 = BiToHe(*pStart++);
		c1 <<= (8*(7-i)+4);

		c2 = BiToHe(*pStart++);
		c2 <<= (8*(7-i));
		
		cHex += (c1 + c2);
	}

	return cHex;
}

inline WORD Math::Convert::StrToHex16(const TCHAR *szSrc)
{
	const TCHAR*     pStart	= szSrc + 2;
	WORD        	sHex	= 0;

	for (int i=0; i<2; i++) 
    {
		WORD	s1	= BiToHe(*pStart++);
		s1 <<= (8*(1-i)+4);
		
		WORD	s2	= BiToHe(*pStart++);
		s2 <<= (8*(1-i));

		WORD	sRet =  s1  + s2;
		sHex += sRet;
	}

	return sHex;

}

// convert string to hexadecimal value
inline unsigned long Math::Convert::StrToHex32(const TCHAR *szSrc)
{
	const TCHAR*     pStart		= szSrc + 2;
	unsigned long       	nHex		= 0;


	for (int i=0; i<4; i++) 
    {
		unsigned long    n1		= BiToHe(*pStart++);
		n1 <<= (8*(3-i)+4);

        unsigned long    n2		= BiToHe(*pStart++);
		n2 <<= (8*(3-i));

		unsigned long    nRet	=  n1  + n2;
		nHex += nRet;

	}

	return nHex;
}

// convert string to hexadecimal value
inline DWORD64 Math::Convert::StrToHex64(const TCHAR *szSrc)
{
	const TCHAR* pStart	= szSrc + 2;
	DWORD64     dlHex	= 0;

	for (int i=0; i<8; i++) 
    {
		DWORD64	dl1		= BiToHe(*pStart++);
		dl1 <<= (8*(7-i)+4);

		DWORD64	dl2		= BiToHe(*pStart++);
		dl2 <<= (8*(7-i));

		DWORD64	dlRet	=  dl1  + dl2;
		dlHex += dlRet;

	}
	return dlHex;
}


inline BYTE Math::Convert::Atoc(const TCHAR *szSrc)
{
    return (0 == _tcsncmp(szSrc, _T("0x"), 2)) ? StrToHex08(szSrc) : (BYTE) _ttol(szSrc);
}

//
// TCHAR *�� ��Ĩ�ô�! (By Standil)
//
inline WORD Math::Convert::Atos(const TCHAR *szSrc)
{
    return (0 == _tcsncmp(szSrc, _T("0x"), 2)) ? StrToHex16(szSrc) : _ttoi(szSrc);
}

//
// TCHAR *�� ��Ĩ�ô�! (By Standil)
//
inline unsigned long Math::Convert::Atoi(const TCHAR *szSrc)
{
    return (0 == _tcsncmp(szSrc, _T("0x"), 2)) ? StrToHex32(szSrc) : _ttol(szSrc);	
}

//
// TCHAR *�� ��Ĩ�ô�! (By Standil)
//
inline DWORD64 Math::Convert::Atol64(const TCHAR *szSrc)
{
    return (0 == _tcsncmp(szSrc, _T("0x"), 2)) ? StrToHex64(szSrc) : _ttoi64(szSrc);	
}

#endif