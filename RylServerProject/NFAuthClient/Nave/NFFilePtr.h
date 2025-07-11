/** 
 *  @file		NFFilePtr.h
 *  @brief		FILE auto close class
 *  @remarks	
 *  @author		������(edith2580@gmail.com)
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
		/// NFFilePtr ������
		NFFilePtr() : m_pFile(NULL)
		{
		}

		/**
		 * @brief ���� �ڵ����� ������
		 * @param file ���ϸ�
		 * @param mode ���� ���
		 */
		NFFilePtr(LPCSTR file, LPCSTR mode)
		{
			m_pFile = fopen(file, mode);
		}

		/// NFFilePtr �Ҹ���
		~NFFilePtr()
		{
			if(m_pFile)
				fclose(m_pFile);

			m_pFile = NULL;
		}

		/**
		 * @brief NFFilePtr1 = FilePtrW2 ���ͷ�����
		 * @warning NFFilePtr2�� ���̻� ����Ҽ� �����ϴ�.
		 */
		NFFilePtr& operator=(NFFilePtr& _Right) throw()
		{
			Reset(_Right.Release());
			return (*this);
		}

		/**
		 * @brief NFFilePtr = FILE* ���ͷ�����
		 * @warning NFFilePtr���� �ڵ� fclose�� �˴ϴ�.    \r\n
		 *          FILE*��ü�� fclose�ϸ� ������ �߻��� ���ֽ��ϴ�.
		 */
		NFFilePtr& operator=(FILE* _Right) throw()
		{
			Reset(_Right);
			return (*this);
		}

		/**
		 * @brief FILE* ����ȯ ���۷����� �Դϴ�.
		 * @return ������ ���� ������
		 */
		operator FILE*() const throw()
		{
			return m_pFile;
		}

		/**
		 * @brief ���� �����͸� �ʱ�ȭ �մϴ�. FILE�� close ���� �ʽ��ϴ�.
		 * @return  ������ ���� ������
		 */
		FILE* Release()
		{
			FILE* fp = m_pFile;
			m_pFile = NULL;
			return fp;
		}

		/**
		 * @brief ���ο� ������ �缳���մϴ�. ������ FILE�� close �˴ϴ�.
		 * @param fp ���ο� ���� ������
		 */
		void Reset(LPCSTR file, LPCSTR mode)
		{
			if(m_pFile)
				fclose(m_pFile);

			m_pFile = fopen(file, mode);
		}

		/**
		 * @brief ���ο� ������ �缳���մϴ�.
		 *   ������ FILE�� close �˴ϴ�.
		 * @param fp ���ο� ���� ������
		 */
		void Reset(FILE* fp)
		{
			if(m_pFile)
				fclose(m_pFile);

			m_pFile = fp;
		}

		/**
		 * @brief ������ ���̸� �����մϴ�.
		 * @return  ������ ����
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
		/// ���� ������
		FILE* m_pFile;
	};

}