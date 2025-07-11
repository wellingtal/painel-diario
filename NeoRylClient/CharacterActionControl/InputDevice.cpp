
#include "stdafx.h"
#include "InputDevice.h"

CInputDevice::CInputDevice() 
:	m_pDirectInput(NULL), m_pDIDeviceKeyboard(NULL), m_pDIDeviceMouse(NULL), m_pDIDeviceJoystick(NULL), 
	m_bKeyboard(false), m_bMouse(false), m_bJoystick(false), m_bForcefeedback(false), m_ucSliderCount(0),
	m_lpdiEffect(NULL), m_ucPOVCount(0), m_ucButtionCount(0)
{	
	memset(m_aKey, 0, sizeof(unsigned char)*256); 
	memset(&m_dims2, 0, sizeof(DIMOUSESTATE2)); 
	memset(&m_dijs2, 0, sizeof(DIJOYSTATE2)); 

	for (unsigned char ucIndex = 0; ucIndex < 4; ucIndex++)
	{
		m_dijs2.rgdwPOV[ucIndex] = -1;
	}
}

CInputDevice::~CInputDevice()
{
}

bool CInputDevice::Initialize(HWND hWnd, bool bKeyboard, bool bMouse, bool bJoystick)
{	
	HRESULT hr = DirectInput8Create(
		GetModuleHandle(NULL), 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(VOID**)&m_pDirectInput, 
		NULL); 	

	DI_FAILED(hr);

	m_hWnd = hWnd;

	// 虐焊靛 积己
	if (true == bKeyboard)
	{
		hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pDIDeviceKeyboard, NULL);
		DI_FAILED(hr);

		hr = m_pDIDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard); 
		DI_FAILED(hr);

		hr = m_pDIDeviceKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE); 
		DI_FAILED(hr);

		m_pDIDeviceKeyboard->Acquire();

		m_bKeyboard = bKeyboard;
	}
	
	// 付快胶 积己
	if (true == bMouse)
	{
		hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pDIDeviceMouse, NULL);
		DI_FAILED(hr);

		hr = m_pDIDeviceMouse->SetDataFormat(&c_dfDIMouse2);
		DI_FAILED(hr);

		hr = m_pDIDeviceMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);			
		DI_FAILED(hr);
		
		m_pDIDeviceMouse->Acquire();

		m_bMouse = bMouse;
	}

	// 炼捞胶平 积己
	if (true == bJoystick)
	{
		hr = m_pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);			

		DI_FAILED(hr);
	}

	return true;
}

void CInputDevice::Release(void)
{
	if (NULL != m_pDIDeviceKeyboard)
	{
		m_pDIDeviceKeyboard->Unacquire();
		SAFE_RELEASE(m_pDIDeviceKeyboard);				
	}

	if (NULL != m_pDIDeviceMouse)
	{				
		m_pDIDeviceMouse->Unacquire();
		SAFE_RELEASE(m_pDIDeviceMouse);
	}

	if (NULL != m_lpdiEffect)
	{
		m_lpdiEffect->Stop();
		SAFE_RELEASE(m_lpdiEffect);
	}

	if (NULL != m_pDIDeviceJoystick)
	{
		m_pDIDeviceJoystick->Unacquire();	
		SAFE_RELEASE(m_pDIDeviceJoystick);		
	}

	SAFE_RELEASE(m_pDirectInput);
}

void CInputDevice::Process(void)
{
	HRESULT hr;

	// 虐焊靛 贸府
	if (NULL != m_pDIDeviceKeyboard && true == m_bKeyboard)
	{
		ZeroMemory(m_aKey, sizeof(m_aKey));

		hr = m_pDIDeviceKeyboard->GetDeviceState(sizeof(m_aKey), m_aKey);

		if (FAILED(hr)) 
		{			
			hr = m_pDIDeviceKeyboard->Acquire();			

			if (hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED) 
			{				
			}
		}
	}

	// 付快胶 贸府
	if (NULL != m_pDIDeviceMouse && true == m_bMouse)
	{			
		ZeroMemory(&m_dims2, sizeof(DIMOUSESTATE2));

		hr = m_pDIDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_dims2);

		if (FAILED(hr)) 
		{
			hr = m_pDIDeviceMouse->Acquire();
			
			if (hr==DIERR_OTHERAPPHASPRIO || hr==DIERR_NOTACQUIRED) 				
			{
			}			
		}
	}

	// 炼捞胶平 贸府
	if (NULL != m_pDIDeviceJoystick && true == m_bJoystick)
	{					
		hr = m_pDIDeviceJoystick->Poll(); 

		if (FAILED(hr)) 
		{
 			hr = m_pDIDeviceJoystick->Acquire();

			if (hr==DIERR_OTHERAPPHASPRIO || hr==DIERR_NOTACQUIRED) 				
			{
			}			
		}		
		else
		{
			ZeroMemory(&m_dijs2, sizeof(DIJOYSTATE2));

			hr = m_pDIDeviceJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &m_dijs2);

			if (FAILED(hr)) 
			{
				hr = m_pDIDeviceJoystick->Acquire();

				if (hr==DIERR_OTHERAPPHASPRIO || hr==DIERR_NOTACQUIRED) 				
				{
				}			
			}
		}		
	}
}

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)								
{	
	CInputDevice* pInputDevice = CInputDevice::GetInstance();
	
	HRESULT hr = pInputDevice->m_pDirectInput->CreateDevice(pdidInstance->guidInstance, &pInputDevice->m_pDIDeviceJoystick, NULL);
		
	DI_FAILED(hr);

	hr = pInputDevice->m_pDIDeviceJoystick->SetDataFormat(&c_dfDIJoystick2);	
	
	DI_FAILED(hr);

	hr = pInputDevice->m_pDIDeviceJoystick->SetCooperativeLevel(pInputDevice->GetWindowHandle(), DISCL_EXCLUSIVE | DISCL_FOREGROUND);
											
	DI_FAILED(hr);

	DIDEVCAPS didevcaps; 
	memset(&didevcaps, 0, sizeof(DIDEVCAPS));
	didevcaps.dwSize = sizeof(DIDEVCAPS);

	hr = pInputDevice->m_pDIDeviceJoystick->GetCapabilities(&didevcaps);

	DI_FAILED(hr);

	if (didevcaps.dwFlags & DIDC_FORCEFEEDBACK)
	{
		pInputDevice->m_bForcefeedback = true;
	}

	if (pInputDevice->m_bForcefeedback)
	{
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = FALSE;

		hr = pInputDevice->m_pDIDeviceJoystick->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph);

		DI_FAILED(hr);

		hr = pInputDevice->m_pDIDeviceJoystick->EnumObjects(EnumAxesCallback, 
								(VOID*)&pInputDevice->m_dwNumForceFeedbackAxis, DIDFT_AXIS);

		DI_FAILED(hr);

		if (pInputDevice->m_dwNumForceFeedbackAxis > 2)
		{
			pInputDevice->m_dwNumForceFeedbackAxis = 2;
		}

		DWORD           rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };
		LONG            rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf              = { 0 };

		DIEFFECT eff;
		ZeroMemory(&eff, sizeof(eff));
		eff.dwSize                  = sizeof(DIEFFECT);
		eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration              = INFINITE;
		eff.dwSamplePeriod          = 0;
		eff.dwGain                  = DI_FFNOMINALMAX;
		eff.dwTriggerButton         = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes                   = pInputDevice->m_dwNumForceFeedbackAxis;
		eff.rgdwAxes                = rgdwAxes;
		eff.rglDirection            = rglDirection;
		eff.lpEnvelope              = 0;
		eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams   = &cf;
		eff.dwStartDelay            = 0;

		hr = pInputDevice->m_pDIDeviceJoystick->CreateEffect( GUID_ConstantForce,
								&eff, &pInputDevice->m_lpdiEffect, NULL);

		DI_FAILED(hr);

		if (!pInputDevice->m_lpdiEffect)
		{
			return FALSE;
		}
	}	
	
	pInputDevice->m_ucButtionCount = static_cast<unsigned char>(didevcaps.dwButtons);				

	hr = pInputDevice->m_pDIDeviceJoystick->EnumObjects(EnumObjectsCallback,
							(VOID*)pInputDevice->GetWindowHandle(), DIDFT_ALL);

	DI_FAILED(hr);	

	pInputDevice->m_pDIDeviceJoystick->Acquire();	

	pInputDevice->m_bJoystick = true;

	return TRUE;
}

BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext)
{	
	CInputDevice* pInputDevice = CInputDevice::GetInstance();
	
	if (pdidoi->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE diprg; 
		diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprg.diph.dwHow        = DIPH_BYID; 
		diprg.diph.dwObj        = pdidoi->dwType;
		diprg.lMin              = -1000; 
		diprg.lMax              = +1000; 
		
		HRESULT hr = pInputDevice->m_pDIDeviceJoystick->SetProperty(DIPROP_RANGE, &diprg.diph );
		
		DI_FAILED(hr);
	}

	if (GUID_Slider == pdidoi->guidType)
	{
		pInputDevice->m_ucSliderCount++;
	}

	if (GUID_POV == pdidoi->guidType)
	{
		pInputDevice->m_ucPOVCount++;
	}

	return TRUE;
}

BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	DWORD* pdwNumForceFeedbackAxis = (DWORD*) pContext;

	if( (pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
	{
		(*pdwNumForceFeedbackAxis)++;
	}	

	return DIENUM_CONTINUE;
}
