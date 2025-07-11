// CameraControl.cpp: implementation of the CCameraControl class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraControl.h"
#include "SceneManager.h"
#include "FrameTimer.h"
#include "FullSceneEffect.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

float CCameraControl::m_fJumpRate=0.0f;
float CCameraControl::m_fCameraShakeRateTime=-1.0f;	
float CCameraControl::m_fCameraShakeNowTime=0.0f;
vector3 CCameraControl::m_vecCameraShakeLength=vector3(0.0f,0.0f,0.0f);

float CCameraControl::m_fFadeOutStart=0.0f;
float CCameraControl::m_fFadeOutMaintenance=0.0f;
float CCameraControl::m_fFadeOutEnd=0.0f;
float CCameraControl::m_fFadeOutNowTime=0.0f;

float CCameraControl::m_fFadeInStart=0.0f;
float CCameraControl::m_fFadeInMaintenance=0.0f;
float CCameraControl::m_fFadeInEnd=0.0f;
float CCameraControl::m_fFadeInNowTime=0.0f;

color CCameraControl::m_FadeInColor;

DWORD CCameraControl::m_nCameraAnimationTimer=0xffffffff;


CCameraControl::CCameraControl()
{
	m_fCameraRotX = 3.14159f / 2.0f;
	m_fCameraRotY = 0.4f;

	m_fBattleCameraRotY = 0.3f;
	m_fBattleInterCharacterCamera = 600.0f; // Minotaurs  배틀모드 초기 카메라 위치
	m_fMouseClickInterCharacterCamera=1000.0f;

	m_nCameraMode=1;	
	m_fDirection = 0.0f;

	m_FadeInColor.c=0xffffffff;
}

CCameraControl::~CCameraControl()
{

}

void CCameraControl::UpdateBattleCharacter(vector3 vecChrPos, float fDirection, float fInitY, float fInitZ)
{	
	if(m_nCameraMode>=2)
	{
		UpdateControlCamera();
		
		return;
	}

	m_fDirection = fDirection;

	vecChrPos += vector3(0.0f, fInitY, fInitZ);	
	
	if(m_fBattleCameraRotY > 3.14159f/2.0f-0.3f)
		m_fBattleCameraRotY=3.14159f/2.0f-0.3f;
	if(m_fBattleCameraRotY< -3.14159f/2.0f+0.7f)
		m_fBattleCameraRotY=-3.14159f/2.0f+0.7f;	

	vector3 vecCameraInterPos;
	
	if(m_fJumpRate>0.0f)
	{
		vector3 vecJumpRate=sinf(m_fJumpRate)*vector3(0.0f,10.0f,0.0f);
		vecChrPos-=vecJumpRate;		
		
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);
		m_fJumpRate+=((int)fCameraUpdate)*0.08f;

		if(m_fJumpRate>3.14159f)
		{
			m_fJumpRate=0.0f;
		}
	}

	m_fCameraShakeRateTime;	
	static vector3 m_vecCameraShakeLength;

	vector3 vecCameraShake=vector3(0.0f,0.0f,0.0f);
	
	static bool bStart = false;

	if(m_fCameraShakeRateTime>0.0f)
	{		
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);
		m_fCameraShakeNowTime+=(int)fCameraUpdate;
		if(m_fCameraShakeNowTime>m_fCameraShakeRateTime)
		{
			m_fCameraShakeRateTime=-1.0f;
		}

		vecCameraShake=vector3( (rand()%1000)-500 , (rand()%1000)-500 , (rand()%1000)-500 );
		vecCameraShake.Normalize();
		vecCameraShake.x=vecCameraShake.x*CCameraControl::m_vecCameraShakeLength.x;
		vecCameraShake.y=vecCameraShake.y*CCameraControl::m_vecCameraShakeLength.y;
		vecCameraShake.z=vecCameraShake.z*CCameraControl::m_vecCameraShakeLength.z;
		CSceneManager::m_pFullSceneEffect->SetFlag(FULLSCENE_MOTIONBLUR,true);
		bStart = true;
	}
	else if(bStart){
		bStart = false;
		CSceneManager::m_pFullSceneEffect->SetFlag(FULLSCENE_MOTIONBLUR,false);
		
	}

	//vecCameraInterPos+=vecCameraShake;

	CalcCameraPosition(vecChrPos,fDirection,m_fBattleCameraRotY,m_fBattleInterCharacterCamera,vecCameraInterPos);
	
	if(m_fBattleInterCharacterCamera < 200.0f)
	{
		float fDownFocus = (40.0f - ((m_fBattleInterCharacterCamera - 100.0f) / 2.50f));
		
		vecChrPos.y = vecChrPos.y - fDownFocus;
		vecCameraInterPos.y = vecCameraInterPos.y - fDownFocus;
	}

	SetCamera(vecCameraInterPos, vecCameraShake, vecChrPos);
/*	matrix matView;
	matView.CameraLookAt(vecCameraInterPos+vecCameraShake, vecChrPos+vecCameraShake, vector3(0.0f, 1.0f, 0.0f));
	matrix matInv;
	matInv.Inverse(matView);

	m_vecCameraPos=vecCameraInterPos;
	
	CSceneManager::GetCamera()->SetMatPosition(matInv);
	CSceneManager::GetCamera()->SetMatView(matView);
	CSceneManager::GetCamera()->SetVecPosition(vecChrPos);
	CSceneManager::GetCamera()->MoveFrustum();

	if(m_fFadeOutStart> 0.0f)
	{
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);

		m_fFadeOutNowTime+=(int)fCameraUpdate;

		float fSetTime;

		if(m_fFadeOutNowTime <= m_fFadeOutStart)
		{
			fSetTime=m_fFadeOutNowTime/m_fFadeOutStart;
		}			
		if( m_fFadeOutNowTime > m_fFadeOutStart &&
			m_fFadeOutNowTime < m_fFadeOutStart+m_fFadeOutMaintenance)
		{
			fSetTime=1.0f;
		}
		if( m_fFadeOutNowTime >= m_fFadeOutStart+m_fFadeOutMaintenance)
		{
			fSetTime=m_fFadeOutNowTime-(m_fFadeOutStart+m_fFadeOutMaintenance);
			fSetTime=1.0f-(fSetTime/m_fFadeOutEnd);
		}		
		CSceneManager::m_WeatherManager.m_CustomWaterColor=true;
		CSceneManager::m_WeatherManager.m_CustomWaterRate=fSetTime;		

		if( m_fFadeOutNowTime >= m_fFadeOutStart+m_fFadeOutMaintenance+m_fFadeOutEnd)
		{
			CSceneManager::m_WeatherManager.m_CustomWaterColor=false;

			m_fFadeOutStart=0.0f;
			m_fFadeOutMaintenance=0.0f;
			m_fFadeOutEnd=0.0f;
			m_fFadeOutNowTime=0.0f;

		}		
	}

	if(m_fFadeInStart> 0.0f)
	{
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);

		m_fFadeInNowTime+=(int)fCameraUpdate;

		float fSetTime;

		if(m_fFadeInNowTime <= m_fFadeInStart)
		{
			fSetTime=m_fFadeInNowTime/m_fFadeInStart;
		}			
		if( m_fFadeInNowTime > m_fFadeInStart &&
			m_fFadeInNowTime < m_fFadeInStart+m_fFadeInMaintenance)
		{
			fSetTime=1.0f;
		}
		if( m_fFadeInNowTime >= m_fFadeInStart+m_fFadeInMaintenance)
		{
			fSetTime=m_fFadeInNowTime-(m_fFadeInStart+m_fFadeInMaintenance);
			fSetTime=1.0f-(fSetTime/m_fFadeInEnd);
		}
		if(fSetTime<1.0f && fSetTime>0.0f)
		{
			int a=0;
		}
		color InterColor;
		color ZeroColor;
		ZeroColor.c=0x00000000;
		InterColor=color::Interpolation(ZeroColor,m_FadeInColor,fSetTime);
		CSceneManager::m_FullSceneFade=InterColor;
		//CSceneManager::m_FullSceneFade.c=0x00ff0000;

		if( m_fFadeInNowTime >= m_fFadeInStart+m_fFadeInMaintenance+m_fFadeInEnd)
		{			
			m_fFadeInStart=0.0f;
			m_fFadeInMaintenance=0.0f;
			m_fFadeInEnd=0.0f;
			m_fFadeInNowTime=0.0f;			

			CSceneManager::m_FullSceneFade.c=0x0;
		}		
	}*/
}

void CCameraControl::UpdateClickMode(vector3 vecChrPos, float fInitY, float fInitZ)
{
	if(m_nCameraMode>=2)
	{
		UpdateControlCamera();
		return;
	}
	vecChrPos += vector3(0.0f, fInitY, fInitZ);	// 포커스를 정하는 백터
	
	if(m_fCameraRotY > 3.14159f/2.0f-0.3f)
		m_fCameraRotY=3.14159f/2.0f-0.3f;
	if(m_fCameraRotY< -3.14159f/2.0f+0.7f)
		m_fCameraRotY=-3.14159f/2.0f+0.7f;	


	//CalcCameraPosition(vecChrPos,-m_fCameraRotX,vecCameraInterPos);

	m_fCameraShakeRateTime;	
	static vector3 m_vecCameraShakeLength;

	vector3 vecCameraShake=vector3(0.0f,0.0f,0.0f);

	if(m_fCameraShakeRateTime>0.0f)
	{		
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);
		m_fCameraShakeNowTime+=(int)fCameraUpdate;
		if(m_fCameraShakeNowTime>m_fCameraShakeRateTime)
		{
			m_fCameraShakeRateTime=-1.0f;
		}

		vecCameraShake=vector3( (rand()%1000)-500 , (rand()%1000)-500 , (rand()%1000)-500 );
		vecCameraShake.Normalize();
		vecCameraShake.x=vecCameraShake.x*CCameraControl::m_vecCameraShakeLength.x;
		vecCameraShake.y=vecCameraShake.y*CCameraControl::m_vecCameraShakeLength.y;
		vecCameraShake.z=vecCameraShake.z*CCameraControl::m_vecCameraShakeLength.z;
	}


	vector3 vecCameraInterPos;

	CalcCameraPosition(vecChrPos,-m_fCameraRotX,m_fCameraRotY,m_fMouseClickInterCharacterCamera,vecCameraInterPos);

	if(m_fMouseClickInterCharacterCamera < 200.0f)
	{
		float fDownFocus = (40.0f - ((m_fMouseClickInterCharacterCamera - 100.0f) / 2.50f));
		
		vecChrPos.y = vecChrPos.y - fDownFocus;
		vecCameraInterPos.y = vecCameraInterPos.y - fDownFocus;
	}

	SetCamera(vecCameraInterPos, vecCameraShake, vecChrPos);
	/*
	matrix matRotation,matResult;

	matResult.Translation(vector3(1.0f,0.0f,0.0f));
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll(&qR,-m_fCameraRotX,0.0f,m_fCameraRotY);
	D3DXMatrixRotationQuaternion(matRotation,&qR);

	matResult=matResult*matRotation;	
	//vector3 vecCameraPosition=matResult.GetLoc();		
	vector3 vecCameraInterPos=matResult.GetLoc();	
	vector3 vecResultCameraPos = (vecCameraInterPos * m_fMouseClickInterCharacterCamera) + vecChrPos;


	/////////////////////Terrain//////////////////
	
	//List<vector3> PolyList;
	CSceneManager::m_HeightField.GetLineIntersectPoly(vecChrPos,vecResultCameraPos,PolyList);

	float fCollisionLens=m_fMouseClickInterCharacterCamera;

	float fLens;
	vector3 vecPoly[3];
	if(PolyList.num>0)
	{
		for(int cIndices=0;cIndices<PolyList.num/3;cIndices++)
		{
			vecPoly[0]=PolyList[cIndices*3+0];
			vecPoly[1]=PolyList[cIndices*3+1];
			vecPoly[2]=PolyList[cIndices*3+2];
			
			if(CIntersection::PolygonRay(vecChrPos,vecResultCameraPos,vecPoly,fLens) && fCollisionLens > fLens && fLens > 0.0f)
			{
				fCollisionLens=fLens;
			}
		}
	}

	float fChrInter=fCollisionLens;
	vecCameraInterPos=vecCameraInterPos*fChrInter+vecChrPos;

	float fCameraHeight=CSceneManager::m_HeightField.GetHeight(vecCameraInterPos);
	if(vecCameraInterPos.y < fCameraHeight +30.0f)
	{
		vecCameraInterPos.y=fCameraHeight +30.0f;
	}	
	*/
}

void CCameraControl::ModeConvertAnimation()
{
	float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);
	//fCameraUpdate=
}

void CCameraControl::UpdateControlCamera()
{
	if(m_nCameraMode==100)
		return;

	float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);
	m_fTime+=fCameraUpdate;		

	vector3 vecAdder=m_vecVelocity*fCameraUpdate;
	m_vecNowPosition=m_vecNowPosition+vecAdder;	

	vector3 vecForLens=m_vecEndPosition-m_vecStartPosition;
	vector3 vecNowLens=m_vecNowPosition-m_vecStartPosition;
	
	static float fVelScalar=0.0f;	

	m_vecVelocity+=m_vecAccelate*fCameraUpdate;

	if(vecForLens.GetLens() < vecNowLens.GetLens())
	{
		m_nCameraMode=m_nPrepareCameraMode;
	}

	matrix matView;
	matView.CameraLookAt(m_vecNowPosition, m_vecTargetCameraPosition, vector3(0.0f, 1.0f, 0.0f));
	matrix matInv;
	matInv.Inverse(matView);

	m_vecCameraPos=m_vecNowPosition;
	
	CSceneManager::GetCamera()->SetMatPosition(matInv);
	CSceneManager::GetCamera()->SetMatView(matView);
	CSceneManager::GetCamera()->SetVecPosition(m_vecNowPosition);
	CSceneManager::GetCamera()->MoveFrustum();	
}

void CCameraControl::CalcCameraPosition(vector3 vecChrPos,float fCameraDirection,float fCameraYDirection,float fInterCamera,vector3 &vecCameraInterPos)
{
	matrix matRotation,matResult;

	matResult.Translation(vector3(1.0f,0.0f,0.0f));
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll(&qR,fCameraDirection,0.0f,fCameraYDirection);
	D3DXMatrixRotationQuaternion(matRotation,&qR);

	matResult=matResult*matRotation;	
	//vector3 vecCameraPosition=matResult.GetLoc();		
	vecCameraInterPos=matResult.GetLoc();	
	vector3 vecResultCameraPos = (vecCameraInterPos * fInterCamera) + vecChrPos;


	/////////////////////Terrain//////////////////
	
	std::vector<vector3> PolyList;
	CSceneManager::m_HeightField.GetLineIntersectPoly(vecChrPos,vecResultCameraPos,PolyList);

	float fCollisionLens=fInterCamera;

	float fLens;
	vector3 vecPoly[3];
	if(PolyList.size()>0)
	{
		for(int cIndices=0;cIndices<(int)PolyList.size()/3;cIndices++)
		{
			vecPoly[0]=PolyList[cIndices*3+0];
			vecPoly[1]=PolyList[cIndices*3+1];
			vecPoly[2]=PolyList[cIndices*3+2];
			
			if(CIntersection::PolygonRay(vecChrPos,vecResultCameraPos,vecPoly,fLens) && fCollisionLens > fLens && fLens > 0.0f)
			{
				fCollisionLens=fLens;
			}
		}
	}

	float fChrInter=fCollisionLens;
	vecCameraInterPos=vecCameraInterPos*fChrInter+vecChrPos;

	float fCameraHeight=CSceneManager::m_HeightField.GetHeight(vecCameraInterPos);
	if(vecCameraInterPos.y < fCameraHeight +30.0f)
	{
		vecCameraInterPos.y = fCameraHeight +30.0f;
	}
}

void CCameraControl::SetFadeOut(float fStart, float fMaintenance, float fEnd)
{
	m_fFadeOutStart=fStart;
	m_fFadeOutMaintenance=fMaintenance;
	m_fFadeOutEnd=fEnd;
	m_fFadeOutNowTime=0.0f;
}

void CCameraControl::SetFadeIn(float fStart, float fMaintenance, float fEnd,color FadeColor)
{
	m_fFadeInStart=fStart;
	m_fFadeInMaintenance=fMaintenance;
	m_fFadeInEnd=fEnd;
	m_fFadeInNowTime=0.0f;
	m_FadeInColor=FadeColor;
}

void CCameraControl::SetCamera(vector3 &vecCameraInterPos, vector3 &vecCameraShake, vector3 &vecChrPos)
{
	matrix matView;
	matView.CameraLookAt(vecCameraInterPos+vecCameraShake, vecChrPos, vector3(0.0f, 1.0f, 0.0f));
	matrix matInv;
	matInv.Inverse(matView);

	m_vecCameraPos=vecCameraInterPos;
	
	CSceneManager::GetCamera()->SetMatPosition(matInv);
	CSceneManager::GetCamera()->SetMatView(matView);
	CSceneManager::GetCamera()->SetVecPosition(vecChrPos);
	CSceneManager::GetCamera()->MoveFrustum();

	if(m_fFadeOutStart> 0.0f)
	{
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);

		m_fFadeOutNowTime+=(int)fCameraUpdate;

		float fSetTime;

		if(m_fFadeOutNowTime <= m_fFadeOutStart)
		{
			fSetTime=m_fFadeOutNowTime/m_fFadeOutStart;
		}			
		if( m_fFadeOutNowTime > m_fFadeOutStart &&
			m_fFadeOutNowTime < m_fFadeOutStart+m_fFadeOutMaintenance)
		{
			fSetTime=1.0f;
		}
		if( m_fFadeOutNowTime >= m_fFadeOutStart+m_fFadeOutMaintenance)
		{
			fSetTime=m_fFadeOutNowTime-(m_fFadeOutStart+m_fFadeOutMaintenance);
			fSetTime=1.0f-(fSetTime/m_fFadeOutEnd);
			//if(fSetTime<0.0f)
			//	fSetTime=0.0f;
		}	
		if(fSetTime>=0.0f)
		{
			CSceneManager::m_WeatherManager.m_CustomWaterColor=true;
			CSceneManager::m_WeatherManager.m_CustomWaterRate=fSetTime;		
		}		

		if( m_fFadeOutNowTime >= m_fFadeOutStart+m_fFadeOutMaintenance+m_fFadeOutEnd)
		{
			CSceneManager::m_WeatherManager.m_CustomWaterColor=false;

			m_fFadeOutStart=0.0f;
			m_fFadeOutMaintenance=0.0f;
			m_fFadeOutEnd=0.0f;
			m_fFadeOutNowTime=0.0f;

		}		
	}

	if(m_fFadeInStart> 0.0f)
	{
		float fCameraUpdate=CFrameTimer::GetUpdateTimer(m_nCameraAnimationTimer);

		m_fFadeInNowTime+=(int)fCameraUpdate;

		float fSetTime;

		if(m_fFadeInNowTime <= m_fFadeInStart)
		{
			fSetTime=m_fFadeInNowTime/m_fFadeInStart;
		}			
		if( m_fFadeInNowTime > m_fFadeInStart &&
			m_fFadeInNowTime < m_fFadeInStart+m_fFadeInMaintenance)
		{
			fSetTime=1.0f;
		}
		if( m_fFadeInNowTime >= m_fFadeInStart+m_fFadeInMaintenance)
		{
			fSetTime=m_fFadeInNowTime-(m_fFadeInStart+m_fFadeInMaintenance);
			fSetTime=1.0f-(fSetTime/m_fFadeInEnd);
		}
		if(fSetTime<1.0f && fSetTime>0.0f)
		{
			int a=0;
		}
		color InterColor;
		color ZeroColor;
		ZeroColor.c=0x00000000;
		InterColor=color::Interpolation(ZeroColor,m_FadeInColor,fSetTime);
		CSceneManager::m_FullSceneFade=InterColor;
		//CSceneManager::m_FullSceneFade.c=0x00ff0000;

		if( m_fFadeInNowTime >= m_fFadeInStart+m_fFadeInMaintenance+m_fFadeInEnd)
		{			
			m_fFadeInStart=0.0f;
			m_fFadeInMaintenance=0.0f;
			m_fFadeInEnd=0.0f;
			m_fFadeInNowTime=0.0f;			

			CSceneManager::m_FullSceneFade.c=0x0;
		}		
	}
}
