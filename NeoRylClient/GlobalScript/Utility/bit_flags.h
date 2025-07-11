#ifndef BITFLAGS_H
#define BITFLAGS_H
#pragma once
/***************************************************************\

    ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^
    ^^    ^^  ^^    ^^     ^^     ^^    ^^
    ^^        ^^^^^^^^     ^^     ^^^^^^^^
    ^^   ^^^  ^^    ^^     ^^     ^^    ^^
    ^^    ^^  ^^    ^^     ^^     ^^    ^^
    ^^^^^^^^  ^^    ^^  ^^^^^^^^  ^^    ^^

    sample code from the book...
    Real Time 3D Terrain Engines Using C++ and DirectX

    by Greg Snook
    greg@mightystudios.com

\***************************************************************/
#include "data_types.h"

// disable the MSVC warning regarding 
// forcing values to true or false
#pragma warning( disable : 4800 )

//	Name Space declaration
namespace gaia
{

/*	Bit Flag Macros
-----------------------------------------------------------------
    
    Macros for simple bit and flag collection testing.
    
-----------------------------------------------------------------
*/
#define FLAG(x) (1<<(x))
#define TEST_BIT(flag, bit) (((flag.value) & FLAG((bit))) !=0)
#define TEST_ALL(flag, set) (((flag.value) & (set)) == (set))
#define TEST_ANY(flag, set) (((flag.value) & (set)) !=0)
#define SET_BIT(flag, bit) ((flag.value) |= FLAG((bit)))
#define CLEAR_BIT(flag, bit) ((flag.value) &= ~FLAG((bit)))
#define SET_ALL(flag, set) ((flag.value) |= (set))
#define CLEAR_ALL(flag, set) ((flag.value) &= ~(set))

/*	cBitFlags
-----------------------------------------------------------------
    
    For more robust use, the cBitFlags template provides a
	generic flag testing interface for flags held in an
	arbitrary data type (T)
    
-----------------------------------------------------------------
*/
template <class T>
class cBitFlags
{
public:

	T value;

	// Creators...
	cBitFlags();
	cBitFlags(T data);
	cBitFlags(const cBitFlags& Src);
	~cBitFlags();

	// Operators...
	cBitFlags& operator=( const cBitFlags& Src);
	cBitFlags& operator=( T Src);
	operator T()const {return(value);};
	bool operator==(const cBitFlags& Src)const;
	bool operator!=(const cBitFlags& Src)const;

	// Mutators...
	void set(T settings);
	void clear();
	void setFlags(T settings);
	void clearFlags(T settings);
	void setBit(int bit);
	void clearBit(int bit);
	void setBit(int bit, bool setting);

	// Accessors...
	bool isEmpty()const;
	bool testBit(int bit)const;
	bool testFlags(T test)const;
	bool testAny(T test)const;

	int	totalBits()const;
	int	totalSet()const;
};

// common flag typedefs
typedef cBitFlags<uint8> u8Flags;	// 8 bits of flags
typedef cBitFlags<uint16> u16Flags; // 16 bits of flags
typedef cBitFlags<uint32> u32Flags; // 32 bits of flags

//- Inline Functions --------------------------------------------

/*	cBitFlags
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
template <class T>
inline cBitFlags<T>::cBitFlags()
:value(0)
{
}

template <class T>
inline cBitFlags<T>::cBitFlags(T settings)
:value(settings)
{
}

template <class T>
inline cBitFlags<T>::cBitFlags( const cBitFlags& Src)
:value(Src.value)
{
}

/*	~cBitFlags
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
template <class T>
inline cBitFlags<T>::~cBitFlags()
{
}

//. Operators ............................................................................
template <class T>
inline cBitFlags<T>& cBitFlags<T>::operator=( const cBitFlags<T>& Src)
{
	value = Src.value;
	return(*this);
}

template <class T>
inline cBitFlags<T>& cBitFlags<T>::operator=( T Src)
{
	value = Src;
	return(*this);
}

template <class T>
bool cBitFlags<T>::operator==( const cBitFlags& Src)const
{
	return(value == Src.value);
}

template <class T>
bool cBitFlags<T>::operator!=( const cBitFlags& Src)const
{
	return(value != Src.value);
}

//. Mutators .............................................................................
template <class T>
inline void cBitFlags<T>::set(T settings)
{
	value = settings;
}

template <class T>
inline void cBitFlags<T>::clear()
{
	value = 0;
}

template <class T>
inline void cBitFlags<T>::setFlags(T settings)
{
	value |= settings;
}

template <class T>
inline void cBitFlags<T>::clearFlags(T settings)
{
	value &= ~settings;
}


template <class T>
inline void cBitFlags<T>::setBit(int bit)
{
	value |= (1<<bit);
}

template <class T>
inline void cBitFlags<T>::setBit(int bit, bool setting)
{
	if (setting)
	{
		value |= (1<<bit);
	}
	else
	{
		clearBit(bit);
	}
}

template <class T>
inline void cBitFlags<T>::clearBit(int bit)
{
	value &= (~(1<<bit));
}

//. Accessors ............................................................................
template <class T>
inline bool cBitFlags<T>::isEmpty()const
{
	return(value == static_cast<T>(0));
}

template <class T>
inline bool cBitFlags<T>::testBit(int bit)const
{
	return(value & (1<<bit));
}


template <class T>
inline bool cBitFlags<T>::testFlags(T test)const
{
	return((value & test) == test);
}

template <class T>
inline bool cBitFlags<T>::testAny(T test)const
{
	return(value & test);
}

template <class T>
inline int cBitFlags<T>::totalBits()const
{
	return(sizeof(T)<<3);
}

template <class T>
inline int cBitFlags<T>::totalSet()const
{
	int count=0;
	int total = totalBits();
	T TestValue = value;
	
	for (int i=total;i;--i)
	{
		count += (TestValue & 1);
		TestValue >>= 1;
	}
	return(count);
}

//***************************************************************

} //End NameSpace: gaia

#pragma warning( default : 4800 )

#endif  // end of file      ( cBitFlags.h )

//----------------------------------------------------------
//$Log: $