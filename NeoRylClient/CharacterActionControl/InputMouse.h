#pragma once

/*! 
	\class CInputJoystick
	���콺 ���� Ŭ����.
*/

class CInputMouse
{
public:

	static CInputMouse* GetInstance();

	//! ���콺�� �̵��� ��ŭ�� ���� �Ѱ� �ִ� �Լ�
	int GetMousePosX(void);

	//! ���콺�� �̵��� ��ŭ�� ���� �Ѱ� �ִ� �Լ�
	int GetMousePosY(void);

	//! ���콺�� �� �̵��� ��ŭ�� ���� �Ѱ� �ִ� �Լ�
	int GetMouseWheelPos(void);

	//! ���콺�� L ��ư�� �ٿ��� �Ǿ����� �Ѱ� �ִ� �Լ�
	bool GetLButtonDown(void);

	//! ���콺�� R ��ư�� �ٿ��� �Ǿ����� �Ѱ� �ִ� �Լ�
	bool GetRButtonDown(void);

	CInputMouse();
	~CInputMouse();
};