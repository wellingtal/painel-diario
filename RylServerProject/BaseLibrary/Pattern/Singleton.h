#ifndef _CSINGLETON_H_
#define _CSINGLETON_H_

#include <cassert>

template<typename Derived>
class CSingleton
{
private:

	static Derived* ms_pSingleton;

protected:

	CSingleton();
	~CSingleton();

public:

	static Derived& GetInstance();
	static Derived* GetInstancePtr();
};


template<typename Derived>
CSingleton<Derived>::CSingleton()
{
	assert(!ms_pSingleton && "Singleton Ŭ������ �̹� �����Ǿ� �ֽ��ϴ�.");
	size_t nOffset = (size_t)(Derived*) 1 - (size_t)(CSingleton<Derived>*)(Derived*) 1;
	ms_pSingleton = (Derived*)((size_t)this + nOffset);
}


template<typename Derived>
CSingleton<Derived>::~CSingleton() 
{
	assert(ms_pSingleton && "Singleton Ŭ������ �������� �ʾҽ��ϴ�");
    ms_pSingleton = 0;
}

template<typename Derived>
inline Derived& CSingleton<Derived>::GetInstance() 
{
	assert(ms_pSingleton && "Singleton Ŭ������ �������� �ʾҽ��ϴ�");
    return (*ms_pSingleton);
}

template<typename Derived>
inline Derived* CSingleton<Derived>::GetInstancePtr()
{
	assert(ms_pSingleton && "Singleton Ŭ������ �������� �ʾҽ��ϴ�");
    return (ms_pSingleton);
}

template<typename Derived>
Derived* CSingleton<Derived>::ms_pSingleton = 0;



// Scotte Meyer's Implementation
template<typename Derived>
class CStaticSingleton
{
public:

	inline static Derived& GetInstance() 
    {
        static Derived Instance;
        return Instance;
    }
};

template<typename Derived>
class CLeakStaticSingleton
{
public:

    inline static Derived& GetInstance()
    {
        static Derived* lpInstance = new Derived;
        return *lpInstance;
    }
};



// C++ FAQ's Implementation
template<typename T>
class CNiftyCounterSingleton
{
public:

    CNiftyCounterSingleton() { ++nifty_count_; }
    ~CNiftyCounterSingleton() { if ( 0 == --nifty_count_ && pInstance_ ) { delete pInstance_; } }

    static T& GetInstance()
    {
        if (NULL == pInstance_) { pInstance_ = new T; }
        return	*pInstance_;
    }

private:

    static int  nifty_count_;
    static T*   pInstance_;
};

template<typename T>
int CNiftyCounterSingleton<T>::nifty_count_	= 0;

template<typename T>
T*  CNiftyCounterSingleton<T>::pInstance_	= 0;

/*
    Ŭ���� ����� ������ ������ ��.
    Ŭ���� ����, �Ҹ��ڴ� private�̰�, CNiftyCounterSingleton�� ����� ���� ��.

    namespace
    {
        CNiftyCounterSingleton<T>   TNiftyCounter;
    }
*/

#endif