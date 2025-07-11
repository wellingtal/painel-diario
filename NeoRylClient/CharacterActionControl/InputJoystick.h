#pragma once

/*! 
	\class CInputJoystick
	���̽�ƽ ���� Ŭ����.
*/

class CInputJoystick
{
public:

	static CInputJoystick* GetInstance();

	//! ���̽�ƽ�� �Է��� �ִ��� üũ�ϴ� �Լ�.
	bool GetCheckInputJoystick(void);

	//! ���̽�ƽ ����Ű �Է� ��� �Լ�
	unsigned long GetPOV(void);

	//! ���̽�ƽ ��ư �Է� ��� �Լ�
	bool GetJoyButton(unsigned char ucKey);

	//! ���̽�ƽ X ��.
	long GetX(void);

	//! ���̽�ƽ Y ��.
	long GetY(void);

	//! ���̽�ƽ Z ��.
	long GetZ(void);

	//! ���̽�ƽ ȸ�� Z ��.
	long GetRotationZ();

	//! ���̽�ƽ ���� ���� �Լ�
	void SetSensitivity(unsigned char cFactor)	{ m_Sensitivity = (cFactor > 1) ? cFactor : 1; }
	unsigned char GetSensitivity()				{ return m_Sensitivity; }

	/*!
		\brief				���̽�ƽ Force Feedback �����Լ�.
		\param lForceX		������.
		\param lForceY		������(���� 2���϶� ���).
		\param dwTick		���� �ð�(���� : Tick).
	*/
	void StartForceFeedback(long lForceX, long lForceY, unsigned long dwTick);

	//! ���̽�ƽ Force Feedback �����Լ�.
	void StopForceFeedback(void);

	enum Const
	{
		POV_LEFT	= 0x01,
		POV_RIGHT	= 0x02,
		POV_UP		= 0x04,
		POV_DOWN	= 0x08
	};

	unsigned char	m_Sensitivity;	// ����

	CInputJoystick();
	~CInputJoystick();
};