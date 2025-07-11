#include "InputDevice.h"
#include "InputJoystick.h"
#include <math.h>

CInputJoystick::CInputJoystick()
{
	m_Sensitivity = 200;
}

CInputJoystick::~CInputJoystick()
{
}

CInputJoystick* CInputJoystick::GetInstance()
{
	static CInputJoystick inputJoystick;
	return &inputJoystick;
}

bool CInputJoystick::GetCheckInputJoystick()
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();
	
	for (unsigned char ucKey = 0; ucKey < CInputDevice::GetInstance()->GetJoyButtonCount(); ucKey++)
	{
		if (GetJoyButton(ucKey))
		{
			return true;
		}
	}

	if (GetX() || GetY() || GetZ())
	{
		return true;
	}

	if (GetRotationZ())
	{
		return true;
	}

	if (GetPOV())
	{
		return true;
	}

	return false;
}

bool CInputJoystick::GetJoyButton(unsigned char ucKey)
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();

	if (ucKey < CInputDevice::GetInstance()->GetJoyButtonCount())
	{
		if ( pdijs2 ->rgbButtons[ucKey] & 0x80 )
		{				
			return true;
		}
	}
	return false;
}

unsigned long CInputJoystick::GetPOV()
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();

	int iPOV = (int)pdijs2->rgdwPOV[0];

	unsigned long dwKey = 0;

	if (iPOV!=-1)
	{
		iPOV = iPOV / 100;

		if (iPOV >= 0 && iPOV <= 360)
		{
			switch (iPOV)
			{
			case 0:
				dwKey |= POV_UP;
				break;
			case 90:
				dwKey |= POV_RIGHT;
				break;
			case 180:
				dwKey |= POV_DOWN;
				break;
			case 270:
				dwKey |= POV_LEFT;
				break;
			case 45:
				dwKey |= POV_UP;
				dwKey |= POV_RIGHT;
				break;
			case 135:
				dwKey |= POV_DOWN;
				dwKey |= POV_RIGHT;
				break;
			case 225:
				dwKey |= POV_DOWN;
				dwKey |= POV_LEFT;
				break;
			case 315:
				dwKey |= POV_UP;
				dwKey |= POV_LEFT;
				break;
			}			

			return dwKey;
		}
	}

	return 0;
}

void CInputJoystick::StartForceFeedback(long lForceX, long lForceY, unsigned long dwTick)
{
	CInputDevice* pInputDevice = CInputDevice::GetInstance();

	if (pInputDevice->GetForceFeedBack())
	{
		LONG rglDirection[2] = { 0, 0 };

		DICONSTANTFORCE cf;		

		if( pInputDevice->GetNumForceFeedbackAxis() == 1 )
		{
			cf.lMagnitude	= lForceX;
			rglDirection[0] = 0;
		}
		else
		{		
			rglDirection[0] = lForceX;
			rglDirection[1] = lForceY;
			cf.lMagnitude	= (unsigned long)sqrt((double)(lForceX*lForceX + lForceY*lForceY));
		}

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize					= sizeof(DIEFFECT);
		eff.dwFlags					= DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.cAxes					= CInputDevice::GetInstance()->GetNumForceFeedbackAxis();
		eff.rglDirection			= rglDirection;
		eff.lpEnvelope				= 0;
		eff.cbTypeSpecificParams	= sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams	= &cf;
		eff.dwStartDelay            = 0;		
		eff.dwDuration				= dwTick*1000;

		pInputDevice->m_lpdiEffect->SetParameters( &eff, DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS | DIEP_DURATION | DIEP_START );

		pInputDevice->m_pDIDeviceJoystick->Acquire();

		pInputDevice->m_lpdiEffect->Start(1, 0);	
	}
}

void CInputJoystick::StopForceFeedback()
{
	CInputDevice* pInputDevice = CInputDevice::GetInstance();

	if (pInputDevice->GetForceFeedBack())
	{
		pInputDevice->m_lpdiEffect->Stop();
	}	
}

long CInputJoystick::GetX()
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();

	return pdijs2->lX / GetSensitivity();	
}

long CInputJoystick::GetY()
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();

	return pdijs2->lY / GetSensitivity();	
}

long CInputJoystick::GetZ()
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();

	return pdijs2->lZ / GetSensitivity();
}

long CInputJoystick::GetRotationZ()
{
	DIJOYSTATE2* pdijs2 = CInputDevice::GetInstance()->GetJoyState();

	return pdijs2->lRz / GetSensitivity();
}