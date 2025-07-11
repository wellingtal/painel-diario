/** 
 *  @file		NFFilePtr.h
 *  @brief		FILE auto close class
 *  @remarks	
 *  @author		강동명(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

#include <stdio.h>												// standard I/O

namespace Nave {

	/** 
	 *  @class		  NFFilePtr  
	 *  @brief        FILE auto close class
	 *  @remarks      NFFilePtr file(L"c:\\1.txt", L"rt");		\r\n
	 *                fgets(str, 128, file);					\r\n
	 *                return;
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFFilePtr
	{
	public:
		/// NFFilePtr 생성자
		NFFilePtr() : m_pFile(NULL)
		{
		}

		/**
		 * @brief 파일 자동열기 생성자
		 * @param file 파일명
		 * @param mode 파일 모드
		 */
		NFFilePtr(LPCSTR file, LPCSTR mode)
		{
			m_pFile = fopen(file, mode);
		}

		/// NFFilePtr 소멸자
		~NFFilePtr()
		{
			if(m_pFile)
				fclose(m_pFile);

			m_pFile = NULL;
		}

		/**
		 * @brief NFFilePtr1 = FilePtrW2 이터레이터
		 * @warning NFFilePtr2는 더이상 사용할수 없습니다.
		 */
		NFFilePtr& operator=(NFFilePtr& _Right) throw()
		{
			Reset(_Right.Release());
			return (*this);
		}

		/**
		 * @brief NFFilePtr = FILE* 이터레이터
		 * @warning NFFilePtr에서 자동 fclose가 됩니다.    \r\n
		 *          FILE*객체를 fclose하면 문제가 발생할 수있습니다.
		 */
		NFFilePtr& operator=(FILE* _Right) throw()
		{
			Reset(_Right);
			return (*this);
		}

		/**
		 * @brief FILE* 형변환 오퍼레이터 입니다.
		 * @return 현재의 파일 포인터
		 */
		operator FILE*() const throw()
		{
			return m_pFile;
		}

		/**
		 * @brief 파일 포인터를 초기화 합니다. FILE이 close 되지 않습니다.
		 * @return  현재의 파일 포인터
		 */
		FILE* Release()
		{
			FILE* fp = m_pFile;
			m_pFile = NULL;
			return fp;
		}

		/**
		 * @brief 새로운 값으로 재설정합니다. 기존의 FILE은 close 됩니다.
		 * @param fp 새로운 파일 포인터
		 */
		void Reset(LPCSTR file, LPCSTR mode)
		{
			if(m_pFile)
				fclose(m_pFile);

			m_pFile = fopen(file, mode);
		}

		/**
		 * @brief 새로운 값으로 재설정합니다.
		 *   기존의 FILE은 close 됩니다.
		 * @param fp 새로운 파일 포인터
		 */
		void Reset(FILE* fp)
		{
			if(m_pFile)
				fclose(m_pFile);

			m_pFile = fp;
		}

		/**
		 * @brief 파일의 길이를 리턴합니다.
		 * @return  파일의 길이
		 */
		long Length()
		{
			fpos_t cuspos;
			if(fgetpos( m_pFile, &cuspos ) != 0)
			return -1; // error

			fseek( m_pFile, 0L, SEEK_END );
			long length = ftell( m_pFile );

			fsetpos(m_pFile, &cuspos);
			return length;
		}


	private:
		/// 파일 포인터
		FILE* m_pFile;
	};

}