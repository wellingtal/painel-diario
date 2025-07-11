#include "InputDevice.h"
#include "InputMouse.h"

CInputMouse::CInputMouse()
{
}

CInputMouse::~CInputMouse()
{
}

CInputMouse* CInputMouse::GetInstance()
{
	static CInputMouse inputMouse;
	return &inputMouse;
}

bool CInputMouse::GetLButtonDown()
{
	DIMOUSESTATE2* pdms	= CInputDevice::GetInstance()->GetMouseState();

	if (pdms->rgbButtons[0])
	{
		return true;
	}

	return false;
}

bool CInputMouse::GetRButtonDown()
{
	DIMOUSESTATE2* pdms	= CInputDevice::GetInstance()->GetMouseState();

	if (pdms->rgbButtons[1])
	{
		return true;
	}

	return false;
}

int CInputMouse::GetMousePosX()
{ 
	DIMOUSESTATE2* pdms	= CInputDevice::GetInstance()->GetMouseState();

	return pdms->lX;
}

int CInputMouse::GetMousePosY()
{
	DIMOUSESTATE2* pdms	= CInputDevice::GetInstance()->GetMouseState();

	return pdms->lY;
}

int CInputMouse::GetMouseWheelPos()
{ 
	DIMOUSESTATE2* pdms	= CInputDevice::GetInstance()->GetMouseState();

	return pdms->lZ;
}