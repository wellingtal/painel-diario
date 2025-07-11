#pragma once

/*! 
	\class CInputJoystick
	마우스 관련 클래스.
*/

class CInputMouse
{
public:

	static CInputMouse* GetInstance();

	//! 마우스에 이동한 만큼에 값을 넘겨 주는 함수
	int GetMousePosX(void);

	//! 마우스에 이동한 만큼에 값을 넘겨 주는 함수
	int GetMousePosY(void);

	//! 마우스에 휠 이동한 만큼에 값을 넘겨 주는 함수
	int GetMouseWheelPos(void);

	//! 마우스에 L 버튼이 다운이 되었는지 넘겨 주는 함수
	bool GetLButtonDown(void);

	//! 마우스에 R 버튼이 다운이 되었는지 넘겨 주는 함수
	bool GetRButtonDown(void);

	CInputMouse();
	~CInputMouse();
};