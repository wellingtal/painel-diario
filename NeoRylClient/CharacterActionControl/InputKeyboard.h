#pragma once

/*! 
	\class CInputKeyboard
	Ű���� ���� Ŭ����.
*/

class CInputKeyboard
{
public:

	static CInputKeyboard* GetInstance();

	//! Ű �Է� �Լ�
	bool GetKeyPress(unsigned char ucKey);

	CInputKeyboard();
	~CInputKeyboard();
};