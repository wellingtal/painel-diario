// CameraControl.h: interface for the CCameraControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERACONTROL_H__4E002D9F_8CCD_44AF_8371_3BA39A220DC0__INCLUDED_)
#define AFX_CAMERACONTROL_H__4E002D9F_8CCD_44AF_8371_3BA39A220DC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include <d3d8.h>

class CCameraControl  
{
public:
	void SetCamera(vector3 &vecCameraInterPos, vector3 &vecCameraShake, vector3 &vecChrPos);
	static void SetFadeIn(float fStart,float fMaintenance,float fEnd,color FadeColor);
	static void SetFadeOut(float fStart,float fMaintenance,float fEnd);
	void CalcCameraPosition(vector3 vecChrPos,float fCameraDirection,float fCameraYDirection,float fInterCamera,vector3 &vecCameraInterPos);
	void UpdateControlCamera();
	void ModeConvertAnimation();
	void UpdateClickMode(vector3 vecChrPos, float fInitY, float fInitZ = 0.0f);
	void UpdateBattleCharacter(vector3 vecChrPos,float fDirection, float fInitY, float fInitZ = 0.0f);
	int m_nCameraMode;//0 마우스 클릭모드,1 배틀,2 프로그램제어 카메라 (쉐이크,스크립트...)
	int m_nPrepareCameraMode;



	static float m_fJumpRate;
	static float m_fCameraShakeRateTime;	
	static float m_fCameraShakeNowTime;
	static vector3 m_vecCameraShakeLength;	

	static float m_fFadeOutStart;
	static float m_fFadeOutMaintenance;
	static float m_fFadeOutEnd;
	static float m_fFadeOutNowTime;

	static float m_fFadeInStart;
	static float m_fFadeInMaintenance;
	static float m_fFadeInEnd;
	static float m_fFadeInNowTime;
	static color m_FadeInColor;


	
	//
	vector3 m_vecStartPosition,m_vecEndPosition;
	vector3 m_vecVelocity,m_vecAccelate;
	vector3 m_vecNowPosition;
	float m_fTotalTimeInterval;
	float m_fTime;
	//
	float m_fCameraRotX,m_fCameraRotY;//클릭모드용
	float m_fBattleCameraRotY;
	float m_fDirection;
	float m_fMouseClickInterCharacterCamera,m_fBattleInterCharacterCamera;
	vector3 m_vecTargetCameraPosition;
	vector3 m_vecCameraPos;
	//



	static DWORD m_nCameraAnimationTimer;
	CCameraControl();
	virtual ~CCameraControl();

};

#endif // !defined(AFX_CAMERACONTROL_H__4E002D9F_8CCD_44AF_8371_3BA39A220DC0__INCLUDED_)
