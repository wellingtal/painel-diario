
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
	DX Input을 기반으로 입력 처리를 하는 클래스
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
		\brief				DX Input 초기화 함수
		\param hWnd			윈도우 핸들
		\param bKeyboard	키보드 사용 유무
		\param bMouse		마우스 사용 유무
		\param bJoystick	조이스틱 사용 유무
	*/
	bool Initialize(HWND hWnd, bool bKeyboard, bool bMouse, bool bJoystick);
	
	//! DX Input Release 함수
	void Release(void);

	//! 입력 처리 하는 함수
	void Process(void);	

	//! 윈도우 핸들을 넘겨 주는 함수
	inline HWND GetWindowHandle(void)	{ return m_hWnd;		}

	//! 키보드 사용 유무를 넘겨 주는 함수
	inline bool IsKeyboard(void)		{ return m_bKeyboard;	}
	//! 마우스 사용 유무를 넘겨 주는 함수
	inline bool IsMouse(void)			{ return m_bMouse;		}
	//! 조이스틱 사용 유무를 넘겨 주는 함수
	inline bool IsJoystick(void)		{ return m_bJoystick;	}
		
	//! 키 상태를 넘겨 주는 함수
	inline unsigned char*	GeteyboardState(void)	{ return m_aKey;			}	
	//! 마우스 상태를 넘겨 주는 함수
	inline DIMOUSESTATE2*	GetMouseState(void)		{ return &m_dims2;			}	
	//! 조이스틱 상태를 넘겨 주는 함수
	inline DIJOYSTATE2*		GetJoyState(void)		{ return &m_dijs2;			}	
	//! 조이스틱 버튼에 수를 넘겨 주는 함수
	inline unsigned char	GetJoyButtonCount(void) { return m_ucButtionCount;	}	

	//! 조이스틱 Force Feedback Axis 수를 넘겨 주는 함수
	unsigned long			GetNumForceFeedbackAxis(void)			{ return m_dwNumForceFeedbackAxis;		}

	//! 조이스틱 Force Feedback 지원 여부를 넘겨 주는 함수
	inline bool				GetForceFeedBack(void)					{ return m_bForcefeedback;				}
	//! 조이스틱 Force Feedback 지원 여부 설정함수.
	inline void				SetForceFeedBack(bool bForceFeedBack)	{ m_bForcefeedback = bForceFeedBack;	}

	friend class CInputJoystick;
	friend class CInputMouse;
	friend class CInputKeyboard;
	
private:

	//! 조이스틱 생성 콜백 함수
	friend BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);

	//! 조이스틱 오브젝트 콜백 함수(버튼, Slider, POV).
	friend BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

	//! 조이스틱 Axix 콜백 함수.
	friend BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );	
	
private:

	CInputDevice();
	~CInputDevice();

	HWND					m_hWnd;						//!< 윈도우 핸들

	IDirectInput8*			m_pDirectInput;				//!< 다이렉트 인풋
	IDirectInputDevice8*	m_pDIDeviceKeyboard;		//!< 키보드 다이렉트 인풋 디바이스
	IDirectInputDevice8*	m_pDIDeviceMouse;			//!< 마우스 다이렉트 인풋 디바이스
	IDirectInputDevice8*	m_pDIDeviceJoystick;		//!< 조이스틱 다이렉트 인풋 디바이스

	bool					m_bKeyboard;				//!< 키보드 사용 유무
	bool					m_bMouse;					//!< 마우스 사용 유무
	bool					m_bJoystick;				//!< 조이스틱 사용 유무

	bool					m_bForcefeedback;			//!< Force Feedback 사용 유무

	unsigned char			m_aKey[256];				//!< 키 입력을 저장하는 변수
	DIMOUSESTATE2			m_dims2;					//!< 마우스 정보를 저장하는 변수
	DIJOYSTATE2				m_dijs2; 

	unsigned char			m_ucSliderCount;			//!< Slider Controls 수.
	unsigned char			m_ucPOVCount;				//!< POV Controls 수.
	unsigned char			m_ucButtionCount;			//!< Buttons 수.

	unsigned long			m_dwNumForceFeedbackAxis;	//!< Force Feed Back Axis 수.

	LPDIRECTINPUTEFFECT		m_lpdiEffect;				//!< Force Feed Back 사용하는 Effect.
};