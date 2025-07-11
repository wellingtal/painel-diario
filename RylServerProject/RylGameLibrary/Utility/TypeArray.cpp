
#include "stdafx.h"
#include "TypeArray.h"


bool CTypeName::Test(const CTypeName* lpTypeArray, int nArrayNum, const char* lpTypeArrayName)
{
	for(int nCount = 0; nCount < nArrayNum; ++nCount) 
    {
		if(lpTypeArray[nCount].m_nType != nCount)
        {
			ERRLOG2(g_Log, "�˻��� Ÿ�Ե��� �迭 �ε����� Ÿ�� ���� ���� �ʽ��ϴ�."
				"Ÿ�� ���̺� �̸��� %s�̰�, ���� �ʴ� Ŀ�ǵ� ��ȣ�� %d�Դϴ�.", lpTypeArrayName, nCount);
			return false;
		}
	}
	return true;
}
