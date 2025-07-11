#pragma once

/*! 
	\class CInputKeyboard
	키보드 관련 클래스.
*/

class CInputKeyboard
{
public:

	static CInputKeyboard* GetInstance();

	//! 키 입력 함수
	bool GetKeyPress(unsigned char ucKey);

	CInputKeyboard();
	~CInputKeyboard();
};