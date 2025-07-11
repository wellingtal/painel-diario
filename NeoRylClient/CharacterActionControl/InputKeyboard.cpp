#include "InputDevice.h"
#include "InputKeyboard.h"

CInputKeyboard::CInputKeyboard()
{
}

CInputKeyboard::~CInputKeyboard()
{
}

CInputKeyboard* CInputKeyboard::GetInstance()
{
	static CInputKeyboard inputKeyboard;
	return &inputKeyboard;
}

bool CInputKeyboard::GetKeyPress(unsigned char ucKey)
{
	unsigned char* pKey = CInputDevice::GetInstance()->GeteyboardState();

	if (pKey[ucKey] & 0x80)
	{
		return true;
	}

	return false;
}