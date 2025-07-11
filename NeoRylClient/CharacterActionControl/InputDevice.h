
#pragma once

#include <dinput.h>

#define DI_FAILED(hr)		\
	{						\
		if (FAILED(hr))		\
		{					\
			return false;	\
		}					\
	}

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

/*! 
	\class CInputDevice
	DX Input�� ������� �Է� ó���� �ϴ� Ŭ����
*/

class CInputDevice
{
public:

	static CInputDevice* GetInstance()
	{
		static CInputDevice inputDevice;
		return &inputDevice;
	}

	/*!
		\brief				DX Input �ʱ�ȭ �Լ�
		\param hWnd			������ �ڵ�
		\param bKeyboard	Ű���� ��� ����
		\param bMouse		���콺 ��� ����
		\param bJoystick	���̽�ƽ ��� ����
	*/
	bool Initialize(HWND hWnd, bool bKeyboard, bool bMouse, bool bJoystick);
	
	//! DX Input Release �Լ�
	void Release(void);

	//! �Է� ó�� �ϴ� �Լ�
	void Process(void);	

	//! ������ �ڵ��� �Ѱ� �ִ� �Լ�
	inline HWND GetWindowHandle(void)	{ return m_hWnd;		}

	//! Ű���� ��� ������ �Ѱ� �ִ� �Լ�
	inline bool IsKeyboard(void)		{ return m_bKeyboard;	}
	//! ���콺 ��� ������ �Ѱ� �ִ� �Լ�
	inline bool IsMouse(void)			{ return m_bMouse;		}
	//! ���̽�ƽ ��� ������ �Ѱ� �ִ� �Լ�
	inline bool IsJoystick(void)		{ return m_bJoystick;	}
		
	//! Ű ���¸� �Ѱ� �ִ� �Լ�
	inline unsigned char*	GeteyboardState(void)	{ return m_aKey;			}	
	//! ���콺 ���¸� �Ѱ� �ִ� �Լ�
	inline DIMOUSESTATE2*	GetMouseState(void)		{ return &m_dims2;			}	
	//! ���̽�ƽ ���¸� �Ѱ� �ִ� �Լ�
	inline DIJOYSTATE2*		GetJoyState(void)		{ return &m_dijs2;			}	
	//! ���̽�ƽ ��ư�� ���� �Ѱ� �ִ� �Լ�
	inline unsigned char	GetJoyButtonCount(void) { return m_ucButtionCount;	}	

	//! ���̽�ƽ Force Feedback Axis ���� �Ѱ� �ִ� �Լ�
	unsigned long			GetNumForceFeedbackAxis(void)			{ return m_dwNumForceFeedbackAxis;		}

	//! ���̽�ƽ Force Feedback ���� ���θ� �Ѱ� �ִ� �Լ�
	inline bool				GetForceFeedBack(void)					{ return m_bForcefeedback;				}
	//! ���̽�ƽ Force Feedback ���� ���� �����Լ�.
	inline void				SetForceFeedBack(bool bForceFeedBack)	{ m_bForcefeedback = bForceFeedBack;	}

	friend class CInputJoystick;
	friend class CInputMouse;
	friend class CInputKeyboard;
	
private:

	//! ���̽�ƽ ���� �ݹ� �Լ�
	friend BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);

	//! ���̽�ƽ ������Ʈ �ݹ� �Լ�(��ư, Slider, POV).
	friend BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

	//! ���̽�ƽ Axix �ݹ� �Լ�.
	friend BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );	
	
private:

	CInputDevice();
	~CInputDevice();

	HWND					m_hWnd;						//!< ������ �ڵ�

	IDirectInput8*			m_pDirectInput;				//!< ���̷�Ʈ ��ǲ
	IDirectInputDevice8*	m_pDIDeviceKeyboard;		//!< Ű���� ���̷�Ʈ ��ǲ ����̽�
	IDirectInputDevice8*	m_pDIDeviceMouse;			//!< ���콺 ���̷�Ʈ ��ǲ ����̽�
	IDirectInputDevice8*	m_pDIDeviceJoystick;		//!< ���̽�ƽ ���̷�Ʈ ��ǲ ����̽�

	bool					m_bKeyboard;				//!< Ű���� ��� ����
	bool					m_bMouse;					//!< ���콺 ��� ����
	bool					m_bJoystick;				//!< ���̽�ƽ ��� ����

	bool					m_bForcefeedback;			//!< Force Feedback ��� ����

	unsigned char			m_aKey[256];				//!< Ű �Է��� �����ϴ� ����
	DIMOUSESTATE2			m_dims2;					//!< ���콺 ������ �����ϴ� ����
	DIJOYSTATE2				m_dijs2; 

	unsigned char			m_ucSliderCount;			//!< Slider Controls ��.
	unsigned char			m_ucPOVCount;				//!< POV Controls ��.
	unsigned char			m_ucButtionCount;			//!< Buttons ��.

	unsigned long			m_dwNumForceFeedbackAxis;	//!< Force Feed Back Axis ��.

	LPDIRECTINPUTEFFECT		m_lpdiEffect;				//!< Force Feed Back ����ϴ� Effect.
};