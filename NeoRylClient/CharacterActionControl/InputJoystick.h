#pragma once

/*! 
	\class CInputJoystick
	조이스틱 관련 클래스.
*/

class CInputJoystick
{
public:

	static CInputJoystick* GetInstance();

	//! 조이스틱에 입력이 있는지 체크하는 함수.
	bool GetCheckInputJoystick(void);

	//! 조이스틱 방향키 입력 얻는 함수
	unsigned long GetPOV(void);

	//! 조이스틱 버튼 입력 얻는 함수
	bool GetJoyButton(unsigned char ucKey);

	//! 조이스틱 X 값.
	long GetX(void);

	//! 조이스틱 Y 값.
	long GetY(void);

	//! 조이스틱 Z 값.
	long GetZ(void);

	//! 조이스틱 회전 Z 값.
	long GetRotationZ();

	//! 조이스틱 감도 설정 함수
	void SetSensitivity(unsigned char cFactor)	{ m_Sensitivity = (cFactor > 1) ? cFactor : 1; }
	unsigned char GetSensitivity()				{ return m_Sensitivity; }

	/*!
		\brief				조이스틱 Force Feedback 시작함수.
		\param lForceX		진동값.
		\param lForceY		진동값(축이 2개일때 사용).
		\param dwTick		진동 시간(단위 : Tick).
	*/
	void StartForceFeedback(long lForceX, long lForceY, unsigned long dwTick);

	//! 조이스틱 Force Feedback 정지함수.
	void StopForceFeedback(void);

	enum Const
	{
		POV_LEFT	= 0x01,
		POV_RIGHT	= 0x02,
		POV_UP		= 0x04,
		POV_DOWN	= 0x08
	};

	unsigned char	m_Sensitivity;	// 감도

	CInputJoystick();
	~CInputJoystick();
};