#include "CCameraScript.h"
#include "SceneManager.h"
#include "GMMemory.h"


extern D3DXVECTOR3 g_vecCube[8];
extern WORD g_vecCubeIndex[36];


CCameraScript::CCameraScript() {

	m_pDevice = CSceneManager::GetDevice();
	m_pDevice->GetTransform(D3DTS_VIEW, &m_matBeforeView);
	InitCameraScript();
	m_iFixCount = 0;

	m_bBackWord = false;
	m_bInitSet = false;



}
CCameraScript::~CCameraScript() {
	
	m_lstEvent.clear();
	m_lstPosSpline.clear();
	m_lstLookSpline.clear();

	m_pDevice->SetTransform(D3DTS_VIEW, &m_matBeforeView);

}
void CCameraScript::ReWindFix() {
	if(m_iFixCount > 0)
		m_iFixCount--;
	else
		m_iFixCount = (m_iEventNum - 1);
}

void CCameraScript::Rewind() {
	if(m_iBeforeEvent > 0)
		m_iBeforeEvent--;	
	else 
		m_iBeforeEvent = (m_iEventNum - 1);
	m_fRunFrame = m_lstEvent[m_iBeforeEvent].m_fFrame;

}

void CCameraScript::FoWindFix() {
	if(m_iFixCount < (m_iEventNum - 1))
		m_iFixCount++;
	else
		m_iFixCount = 0;

}

void CCameraScript::Fowind() {
	if(m_iBeforeEvent < (m_iEventNum-1))
		m_iBeforeEvent++;
	else
		m_iBeforeEvent = 0;
	m_fRunFrame = m_lstEvent[m_iBeforeEvent].m_fFrame;
}
void CCameraScript::Jump(int iEvent) {

	if((iEvent >= 0) && (iEvent < m_iEventNum)) {
		m_iBeforeEvent = iEvent;
		m_fRunFrame = m_lstEvent[m_iBeforeEvent].m_fFrame;
	}
}
void CCameraScript::InitFrame() {
//	m_bStart = false;
	m_dwBeforeTick = 0;
	m_fRunFrame = 0.0f;
	m_iBeforeEvent = 0;

	
}

void CCameraScript::SetVecTakePos()
{
	D3DXVECTOR3 vecInterPos = m_lstEvent[m_iBeforeEvent].m_vecCameraPos;
	D3DXVECTOR3 vecInterLook = m_lstEvent[m_iBeforeEvent].m_vecCameraLook;
	D3DXVECTOR3 vecInterUp = m_lstEvent[m_iBeforeEvent].m_vecCameraUp;

	m_CurrentEvent.m_vecCameraPos = vecInterPos;
	m_CurrentEvent.m_vecCameraLook = vecInterLook;
	m_CurrentEvent.m_vecCameraUp = D3DXVECTOR3(0.0f,1.0f,0.0f);
}

	

void CCameraScript::PlayScript(int iRoll,int iLoop,bool bFix) {
	
	int i;
	if(m_bStart == false)
		return;

	m_dwCurrentTick = timeGetTime();
	float fSkipFrame = 0.0f;
	
	if(m_dwBeforeTick == 0) {	// 첫 시작 프레임
		fSkipFrame = 0;
		m_matBeforePos = *(CSceneManager::m_ViewCamera->GetMatPosition());
		m_matBeforeView2 = CSceneManager::m_ViewCamera->m_matView;
	}
	else {
		fSkipFrame = ((int)m_dwCurrentTick - (int)m_dwBeforeTick) / (float)CAMERAFRAMESTEP;
	}
	
	

	if(iRoll == 0) {	// Play 
	
		if(m_fRunFrame < m_lstEvent[m_iBeforeEvent].m_fFrame )
		{
			m_fRunFrame = m_lstEvent[m_iBeforeEvent].m_fFrame;
		}

		i = m_iBeforeEvent + 1;
			
		for( i; i < m_iEventNum; i++ ) {	
			if(m_fRunFrame >= m_lstEvent[i].m_fFrame) {
				m_iBeforeEvent = i;
			}
		}
		
		if((m_iEventNum - 1) == m_iBeforeEvent) {			// Looping 처리
			if(iLoop == 1) {
				m_iBeforeEvent = 0;
				m_dwBeforeTick = 0;
				m_fRunFrame = 0.0f;

			}
			else {						// Looping X
				m_iBeforeEvent = m_iEventNum - 1;
				m_fRunFrame = m_lstEvent[m_iBeforeEvent].m_fFrame;
				m_bStart = false;
				InitFrame();
				if(bFix)
				{

					CSceneManager::m_ViewCamera->SetMatView(m_matBeforeView2);
					CSceneManager::m_ViewCamera->SetMatPosition(m_matBeforePos);
					CSceneManager::m_ViewCamera->MoveFrustum();
					CSceneManager::m_ViewCamera->SetVecPosition(vector3(m_matBeforePos._41,m_matBeforePos._42,m_matBeforePos._43));
				}


			}
		}
		float fInterpol;
		if(m_iBeforeEvent == (m_iEventNum - 1))
			fInterpol = 1.0f;
		else {
			fInterpol = (float)(m_fRunFrame - m_lstEvent[m_iBeforeEvent].m_fFrame) / (m_lstEvent[m_iBeforeEvent + 1].m_fFrame - m_lstEvent[m_iBeforeEvent].m_fFrame);
		}
		m_CurrentEvent.m_fFrame = m_fRunFrame;

		
		// Interpolation에 따른 보간
		D3DXVECTOR3 vecInterpol;
		
		D3DXVECTOR3 vecInterPos;
		D3DXVECTOR3 vecInterLook;
		D3DXVECTOR3 vecInterUp;

		switch(m_lstEvent[m_iBeforeEvent].m_iInterpolation) {
			case C_LINE:
			
				if(fInterpol < 0.000001f) {
					vecInterPos = m_lstEvent[m_iBeforeEvent].m_vecCameraPos;
					vecInterLook = m_lstEvent[m_iBeforeEvent].m_vecCameraLook;
					vecInterUp = m_lstEvent[m_iBeforeEvent].m_vecCameraUp;

				}
				else {

					int PosStart = m_lstEvent[m_iBeforeEvent].m_iPosStart;
					int iInterpolNum = m_lstEvent[m_iBeforeEvent].m_iPosEnd - m_lstEvent[m_iBeforeEvent].m_iPosStart;
					iInterpolNum *= fInterpol;
					vecInterPos = m_lstPosSpline[ PosStart + iInterpolNum];
					
					PosStart = m_lstEvent[m_iBeforeEvent].m_iLookStart;
					iInterpolNum = m_lstEvent[m_iBeforeEvent].m_iLookEnd - m_lstEvent[m_iBeforeEvent].m_iLookStart;
					iInterpolNum *= fInterpol;
					vecInterLook = m_lstLookSpline[ PosStart + iInterpolNum];

		/*			vecInterpol = m_lstEvent[m_iBeforeEvent + 1].m_vecCameraPos - m_lstEvent[m_iBeforeEvent].m_vecCameraPos;
					vecInterPos = m_lstEvent[m_iBeforeEvent].m_vecCameraPos + (vecInterpol * fInterpol); 
					
					vecInterpol = m_lstEvent[m_iBeforeEvent + 1].m_vecCameraLook - m_lstEvent[m_iBeforeEvent].m_vecCameraLook;
					vecInterLook = m_lstEvent[m_iBeforeEvent].m_vecCameraLook + (vecInterpol * fInterpol);
			*/
					vecInterpol = m_lstEvent[m_iBeforeEvent + 1].m_vecCameraUp - m_lstEvent[m_iBeforeEvent].m_vecCameraUp;
					vecInterUp = m_lstEvent[m_iBeforeEvent].m_vecCameraUp + (vecInterpol * fInterpol);
				}

				break;
			case C_BEZIER1:		// 제어점 3
				if(fInterpol < 0.000001f) {
					vecInterPos = m_lstEvent[m_iBeforeEvent].m_vecCameraPos;
					vecInterLook = m_lstEvent[m_iBeforeEvent].m_vecCameraLook;
					vecInterUp = m_lstEvent[m_iBeforeEvent].m_vecCameraUp;
				}
				else {
				
					int PosStart = m_lstEvent[m_iBeforeEvent].m_iPosStart;
					int iInterpolNum = m_lstEvent[m_iBeforeEvent].m_iPosEnd - m_lstEvent[m_iBeforeEvent].m_iPosStart;
					iInterpolNum *= fInterpol;
					vecInterPos = m_lstPosSpline[ PosStart + iInterpolNum];
					
					PosStart = m_lstEvent[m_iBeforeEvent].m_iLookStart;
					iInterpolNum = m_lstEvent[m_iBeforeEvent].m_iLookEnd - m_lstEvent[m_iBeforeEvent].m_iLookStart;
					iInterpolNum *= fInterpol;
					vecInterLook = m_lstLookSpline[ PosStart + iInterpolNum];


					float fInterpol2 = fInterpol * fInterpol;
			/*		vecInterPos = m_lstEvent[m_iBeforeEvent].m_vecCameraPos * ((1.0f - fInterpol) * (1.0f - fInterpol)) 
									+ (m_lstEvent[m_iBeforeEvent].m_vecControlPoint[0] * 2 * (1.0f - fInterpol) * fInterpol ) + (m_lstEvent[m_iBeforeEvent + 1].m_vecCameraPos * fInterpol2);
					vecInterLook = m_lstEvent[m_iBeforeEvent].m_vecCameraLook * ((1.0f - fInterpol) * (1.0f - fInterpol)) 
									+ (m_lstEvent[m_iBeforeEvent].m_vecControlPoint[0] * 2 * (1.0f - fInterpol) * fInterpol ) + (m_lstEvent[m_iBeforeEvent + 1].m_vecCameraLook * fInterpol2);*/
					vecInterUp = m_lstEvent[m_iBeforeEvent].m_vecCameraUp * ((1.0f - fInterpol) * (1.0f - fInterpol)) 
									+ (m_lstEvent[m_iBeforeEvent].m_vecControlPoint[0] * 2 * (1.0f - fInterpol) * fInterpol ) + (m_lstEvent[m_iBeforeEvent + 1].m_vecCameraUp * fInterpol2);
					
				}
				break;
			/*case C_BEZIER2:		// 제어점 4
				if(fInterpol < 0.000001f) {
					vecInterPos = m_lstEvent[m_iBeforeEvent].m_vecCameraPos;
					vecInterLook = m_lstEvent[m_iBeforeEvent].m_vecCameraLook;
					vecInterUp = m_lstEvent[m_iBeforeEvent].m_vecCameraUp;
				}
				else {


				}
				break;*/
			default:
				break;

		};
		
		m_CurrentEvent.m_vecCameraPos = vecInterPos;
		m_CurrentEvent.m_vecCameraLook = vecInterLook;
		m_CurrentEvent.m_vecCameraUp = D3DXVECTOR3(0.0f,1.0f,0.0f);

		
	}

	D3DXMatrixLookAtLH(&m_matCurrentFrame,
						&(m_CurrentEvent.m_vecCameraPos),&(m_CurrentEvent.m_vecCameraLook),&(m_CurrentEvent.m_vecCameraUp));

	//m_fRunFrame += 	m_dwBeforeTick = m_dwCurrentTick = timeGetTime();

	m_fRunFrame += fSkipFrame;
	m_dwBeforeTick = m_dwCurrentTick = timeGetTime();

}

bool CCameraScript::SaveScript(char *strFileName) {
	
	if((strFileName != NULL) && strlen(strFileName) > 1) {
		FILE *fp = fopen(strFileName,"wb");
		if(fp != NULL) {
			float fVersion = CAMERASCRIPTVERSION; 
			fwrite((float *)&fVersion,sizeof(float),1,fp);
			fwrite((int *)&m_iEventNum,sizeof(int),1,fp);
			fwrite((CCameraEvent *)&(m_lstEvent[0]),sizeof(CCameraEvent),m_iEventNum,fp);
			fclose(fp);
	
		}
		else {
			return false;

		}
	}
	return true;		
}
bool CCameraScript::LoadScript(char *strFileName) {
	
	//InitCameraScript();

	if((strFileName != NULL) && strlen(strFileName) > 1) {
		float fVersion = 0.0f;
		
		char strPath[256] = {0};

		strcpy(strPath,EFFECTSCRIPTPATH);
		strcat(strPath,strFileName);
	

		FILE *fp = fopen(strPath,"rb");
		if(fp != NULL) {
			fread((float *)&fVersion,sizeof(float),1,fp);
			if((fVersion >= 0.9f) && (fVersion <= 1.1f)) {	
				int i;

				fread((int *)&m_iEventNum,sizeof(int),1,fp);
				
				for( i = 0; i < m_iEventNum; i++ ) {
					CCameraEvent EmptyNode;
					m_lstEvent.push_back(EmptyNode);
				}
				
				fread((CCameraEvent *)&(m_lstEvent[0]),sizeof(CCameraEvent),m_iEventNum,fp);
			}
			fclose(fp);
			m_iInsertLook = 1;
			CreateSpline();

		}
		else {
			return false;
		}
	}
	return true;
}
void CCameraScript::InitCameraScript(bool bInitPos,D3DXVECTOR3 vecInitPos,D3DXVECTOR3 vecInitLook) {
	m_lstEvent.clear();
	m_iEventNum = 0;
	m_iBeforeEvent = 0;
	m_fRunFrame = 0.0f;

	m_dwBeforeTick = 0;
	m_bStart = false;

	m_lstPosSpline.clear();
	m_bCreateSpline = false;
	
	m_lstLookSpline.clear();
	m_bCreateLookSpline = false;
	m_iInsertLook = 0;
	m_iFixCount = 0;
	m_bBackWord = false;
	m_vecCameraInitPos = vecInitPos;
	m_vecCameraInitLook = vecInitLook;
	m_bInitSet = bInitPos;





}
int CCameraScript::GetFixCount(int &iIndex,float &fFrame) {
	iIndex = m_lstEvent[m_iFixCount].m_iIndex;
	fFrame = m_lstEvent[m_iFixCount].m_fFrame;
	return m_iFixCount;

}
void CCameraScript::FixLook(int iEvent,D3DXVECTOR3 vecLook) {
	m_lstEvent[iEvent].m_vecCameraLook = vecLook;

}
void CCameraScript::InsertLookPos(D3DXVECTOR3 vecLook) {

	if(m_iInsertLook >= (int)m_lstEvent.size()) {
		MessageBox(NULL,"Look Pos  초과 ","error",MB_OK);
		
		return;
	}
	m_lstEvent[m_iInsertLook].m_vecCameraLook = vecLook;
	m_iInsertLook++;

	char tmp[256]= {0};

	int iNum = m_iInsertLook - m_lstEvent.size();
/*	sprintf(tmp,"%d 개의 Target Point 를 더 찍을수 있습니다.",iNum);
	MessageBox(NULL,tmp,"msg",MB_OK);
*/
}
int CCameraScript::InsertEvent( D3DXVECTOR3 vecPos,D3DXVECTOR3 vecLook,D3DXVECTOR3 vecUp,float fFrame,int iInterpol ) {
	
	CCameraEvent Node;
	
	Node.m_iIndex = m_iEventNum;
	Node.m_iInterpolation = iInterpol;
	Node.m_vecCameraPos = vecPos;
	Node.m_vecCameraLook = vecLook;
	Node.m_vecCameraUp = vecUp;
	Node.m_fFrame = fFrame;

	m_lstEvent.push_back(Node);

	return m_iEventNum++;
}
void CCameraScript::FixEvent( int iEvent,D3DXVECTOR3 vecPos,D3DXVECTOR3 vecLook,D3DXVECTOR3 vecUp,float fFrame,int iInterpol ) {
	if((iEvent< 0) || (iEvent >= m_iEventNum) )
		return;
	m_lstEvent[iEvent].m_vecCameraPos = vecPos;
	m_lstEvent[iEvent].m_vecCameraLook = vecLook;
	m_lstEvent[iEvent].m_vecCameraUp = vecUp;
	m_lstEvent[iEvent].m_fFrame = fFrame;
	m_lstEvent[iEvent].m_iInterpolation = iInterpol;
}
void CCameraScript::DeleteEvent(float fFrame,int iInterpol) {

	if(m_iEventNum <= 0)
		return;
	int i;

	for( i = 0 ;  i < m_iEventNum; i++ )
	{
		if(((m_lstEvent[i].m_fFrame - fFrame) + (float)(m_lstEvent[i].m_iInterpolation - iInterpol)) <= 0.001f) {
			m_lstEvent.erase(m_lstEvent.begin() + i);
			m_iEventNum--;
			return;
		}
	}
}
void CCameraScript::DeleteEvent(int iIndex) {

	if((iIndex <= 0) || (iIndex >= m_iEventNum))
		return;
	int i;

	for( i = 0 ;  i < m_iEventNum; i++ )
	{
		if(i == iIndex) {
			m_lstEvent.erase(m_lstEvent.begin() + i);
			m_iEventNum--;
			
			InitFrame();
			m_iFixCount = 0;
			if(m_iInsertLook >= m_iEventNum)
				m_iInsertLook = (m_iEventNum - 1);
			m_bCreateLookSpline = false;
			m_bCreateSpline = false;
			m_bStart = false;


			return;
		}
	}
}
/*
void CCameraScript::SetPlay(bool bFlag) {
	m_bStart = bFlag;

	InitFrame();
	if(bFlag == false) {
		CSceneManager::m_ViewCamera->SetMatView(m_matBeforeView2);
		CSceneManager::m_ViewCamera->SetMatPosition(m_matBeforePos);
		CSceneManager::m_ViewCamera->MoveFrustum();
		CSceneManager::m_ViewCamera->SetVecPosition(vector3(m_matBeforePos._41,m_matBeforePos._42,m_matBeforePos._43));

	}
}*/
void CCameraScript::RenderPos_Up() {

	int i;
	
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pDevice->SetVertexShader(D3DFVF_XYZ);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMATRIX worldTm;

	m_pDevice->GetTransform(D3DTS_WORLD,&worldTm);
	for( i = 0; i < m_iEventNum; i++ ) {
		D3DXMatrixIdentity(&identity);
		m_pDevice->SetTransform(D3DTS_WORLD,&identity);

		identity._41 = m_lstEvent[i].m_vecCameraPos.x + m_lstEvent[i].m_vecCameraUp.x * 50.0f;
		identity._42 = m_lstEvent[i].m_vecCameraPos.y + m_lstEvent[i].m_vecCameraUp.y * 50.0f;
		identity._43 = m_lstEvent[i].m_vecCameraPos.z + m_lstEvent[i].m_vecCameraUp.z * 50.0f;
		
		identity._11 = 1.0f;
		identity._22 = 1.0f;
		identity._33 = 1.0f;
		
		if(i == m_iFixCount) {
			identity._11 = 9.0f;
			identity._22 = 9.0f;
			identity._33 = 9.0f;
		
		}

		m_pDevice->SetTransform(D3DTS_WORLD,&identity);

		if((i != 0) && ( i != (m_iEventNum  -1)))
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffff4444);
		else
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);

		
		if(i == m_iFixCount) {
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff0000ff);
		}

		m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
			0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));

		identity._41 = m_lstEvent[i].m_vecCameraPos.x;
		identity._42 = m_lstEvent[i].m_vecCameraPos.y;
		identity._43 = m_lstEvent[i].m_vecCameraPos.z;
		
		identity._11 = 1.0f;
		identity._22 = 1.0f;
		identity._33 = 1.0f;

		if(i == m_iFixCount) {
			identity._11 = 9.0f;
			identity._22 = 9.0f;
			identity._33 = 9.0f;
		
		}


		m_pDevice->SetTransform(D3DTS_WORLD,&identity);

		if((i != 0) && ( i != (m_iEventNum  -1)))
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffff0000);
		else
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);
		
		if(i == m_iFixCount) {
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff0000ff);
		}

		m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
			0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
		m_pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

		m_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);

	}

	m_pDevice->SetTransform(D3DTS_WORLD,&worldTm);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

}
void CCameraScript::RenderLook() {

	int i;
	
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pDevice->SetVertexShader(D3DFVF_XYZ);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMATRIX worldTm;

	m_pDevice->GetTransform(D3DTS_WORLD,&worldTm);
	for( i = 0; i < m_iEventNum; i++ ) {

		D3DXMatrixIdentity(&identity);

		identity._41 = m_lstEvent[i].m_vecCameraLook.x;
		identity._42 = m_lstEvent[i].m_vecCameraLook.y;
		identity._43 = m_lstEvent[i].m_vecCameraLook.z;

		identity._11 = 10.0f;
		identity._22 = 10.0f;
		identity._33 = 10.0f;
		if(i == m_iFixCount) {
			identity._11 = 15.0f;
			identity._22 = 15.0f;
			identity._33 = 15.0f;


		}

		m_pDevice->SetTransform(D3DTS_WORLD,&identity);
		
		if((i != 0) && ( i != (m_iEventNum  -1))) 
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffff00);
		else
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);
		
		if(i == m_iFixCount) {
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff0000ff);
		}

		m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
			0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
		m_pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

		m_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);

	}

	m_pDevice->SetTransform(D3DTS_WORLD,&worldTm);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

}
void CCameraScript::RenderLookSpline() {

	if((m_lstLookSpline.size() <= 0) || (m_bCreateLookSpline == false))
		return;
	
	
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pDevice->SetVertexShader(D3DFVF_XYZ);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMATRIX worldTm;

	m_pDevice->GetTransform(D3DTS_WORLD,&worldTm);
	int iSplineNum = m_lstLookSpline.size();


	D3DXMatrixIdentity(&identity);
	m_pDevice->SetTransform(D3DTS_WORLD,&identity);

	
	m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffff00);
	
	m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP,(iSplineNum - 1),&(m_lstLookSpline[0]),sizeof(D3DXVECTOR3));
	
	m_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);

	
	m_pDevice->SetTransform(D3DTS_WORLD,&worldTm);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

}

void CCameraScript::RenderPosSpline() {

	if((m_lstPosSpline.size() <= 0) || (m_bCreateSpline == false))
		return;
	
	
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pDevice->SetVertexShader(D3DFVF_XYZ);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMATRIX worldTm;

	m_pDevice->GetTransform(D3DTS_WORLD,&worldTm);
	int iSplineNum = m_lstPosSpline.size();


	D3DXMatrixIdentity(&identity);
	m_pDevice->SetTransform(D3DTS_WORLD,&identity);

	
	m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff00ffff);
	
	m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP,(iSplineNum - 1),&(m_lstPosSpline[0]),sizeof(D3DXVECTOR3));
	
	m_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);

	
	m_pDevice->SetTransform(D3DTS_WORLD,&worldTm);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

}
void CCameraScript::RenderLine() {

	int i;
	
	m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_pDevice->SetVertexShader(D3DFVF_XYZ);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMATRIX worldTm;

	m_pDevice->GetTransform(D3DTS_WORLD,&worldTm);
	for( i = 0; i < m_iEventNum; i++ ) {

		D3DXVECTOR3 vecUp = D3DXVECTOR3(m_lstEvent[i].m_vecCameraPos.x + m_lstEvent[i].m_vecCameraUp.x * 50.0f,
										m_lstEvent[i].m_vecCameraPos.y + m_lstEvent[i].m_vecCameraUp.y * 50.0f,
										m_lstEvent[i].m_vecCameraPos.z + m_lstEvent[i].m_vecCameraUp.z * 50.0f);
		D3DXVECTOR3 vecPos = D3DXVECTOR3(m_lstEvent[i].m_vecCameraPos.x,
										m_lstEvent[i].m_vecCameraPos.y,
										m_lstEvent[i].m_vecCameraPos.z);
		
		D3DXVECTOR3 vecLook = D3DXVECTOR3(m_lstEvent[i].m_vecCameraLook.x,
										m_lstEvent[i].m_vecCameraLook.y,
										m_lstEvent[i].m_vecCameraLook.z);


		D3DXVECTOR3 vecLinePos[2];
		vecLinePos[0] = vecUp;
		vecLinePos[1] = vecPos;
	/*	vecLinePos[2] = vecPos;
		vecLinePos[3] = vecLook;
*/
		
		D3DXMatrixIdentity(&identity);
		m_pDevice->SetTransform(D3DTS_WORLD,&identity);

		
		m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff000000);
		m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST,1,vecLinePos,sizeof(D3DXVECTOR3));
		
		m_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);

	}

	m_pDevice->SetTransform(D3DTS_WORLD,&worldTm);

	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

}
void CCameraScript::CreateSpline() {

	m_lstPosSpline.clear();
	m_lstLookSpline.clear();


	float fU,fU2,fU3;
	float i,j;

	for( i = 0; i < (m_iEventNum); i++ ) {
		m_lstEvent[i].m_iPosStart = m_lstPosSpline.size();
		 
		for(j = 0;j < CAMERASPLINEUNIT;j++) {
			fU = (float)j / (float)CAMERASPLINEUNIT;
			fU2 = fU * fU;
			fU3 = fU2 * fU;
			D3DXVECTOR3 vecNode;

			vecNode.x = ( (-1 * fU3 + 3 * fU2 - 3*fU + 1) *
										m_lstEvent[(i == 0) ?  0 : (i - 1)].m_vecCameraPos.x + 
										(3* fU3 - 6 * fU2 + 0 * fU + 4) *
										m_lstEvent[i + 0].m_vecCameraPos.x + 
										(-3* fU3 + 3 * fU2 + 3 * fU + 1) *
										m_lstEvent[(i <= (m_iEventNum - 2)) ? i + 1 : i].m_vecCameraPos.x + 
										(1* fU3 + 0 * fU2 + 0 * fU + 0) *
										m_lstEvent[(i <= (m_iEventNum - 3)) ? i + 2 : i].m_vecCameraPos.x) / 6.0f;
			vecNode.y = ( (-1 * fU3 + 3 * fU2 - 3*fU + 1) *
										m_lstEvent[(i == 0) ?  0 : (i - 1)].m_vecCameraPos.y + 
										(3* fU3 - 6 * fU2 + 0 * fU + 4) *
										m_lstEvent[i + 0].m_vecCameraPos.y + 
										(-3* fU3 + 3 * fU2 + 3 * fU + 1) *
										m_lstEvent[(i <= (m_iEventNum - 2)) ? i + 1 : i].m_vecCameraPos.y + 
										(1* fU3 + 0 * fU2 + 0 * fU + 0) *
										m_lstEvent[(i <= (m_iEventNum - 3)) ? i + 2 : i].m_vecCameraPos.y) / 6.0f;
			vecNode.z = ( (-1 * fU3 + 3 * fU2 - 3*fU + 1) *
										m_lstEvent[(i == 0) ?  0 : (i - 1)].m_vecCameraPos.z + 
										(3* fU3 - 6 * fU2 + 0 * fU + 4) *
										m_lstEvent[i + 0].m_vecCameraPos.z + 
										(-3* fU3 + 3 * fU2 + 3 * fU + 1) *
										m_lstEvent[(i <= (m_iEventNum - 2)) ? i + 1 : i].m_vecCameraPos.z + 
										(1* fU3 + 0 * fU2 + 0 * fU + 0) *
										m_lstEvent[(i <= (m_iEventNum - 3)) ? i + 2 : i].m_vecCameraPos.z) / 6.0f;

			m_lstPosSpline.push_back(vecNode);
			
		}
		m_lstEvent[i].m_iPosEnd = m_lstPosSpline.size();
	}
	m_bCreateSpline = true;
	if(m_iInsertLook <= 0)
	{
		m_bCreateLookSpline = false;
		return;
	}
	else {

		for( i = 0; i < (m_iEventNum); i++ ) {
			m_lstEvent[i].m_iLookStart = m_lstLookSpline.size();
			for(j = 0;j < CAMERASPLINEUNIT;j++) {
				fU = (float)j / (float)CAMERASPLINEUNIT;
				fU2 = fU * fU;
				fU3 = fU2 * fU;
				D3DXVECTOR3 vecNode;

				vecNode.x = ( (-1 * fU3 + 3 * fU2 - 3*fU + 1) *
											m_lstEvent[(i == 0) ?  0 : (i - 1)].m_vecCameraLook.x + 
											(3* fU3 - 6 * fU2 + 0 * fU + 4) *
											m_lstEvent[i + 0].m_vecCameraLook.x + 
											(-3* fU3 + 3 * fU2 + 3 * fU + 1) *
											m_lstEvent[(i <= (m_iEventNum - 2)) ? i + 1 : i].m_vecCameraLook.x + 
											(1* fU3 + 0 * fU2 + 0 * fU + 0) *
											m_lstEvent[(i <= (m_iEventNum - 3)) ? i + 2 : i].m_vecCameraLook.x) / 6.0f;
				vecNode.y = ( (-1 * fU3 + 3 * fU2 - 3*fU + 1) *
											m_lstEvent[(i == 0) ?  0 : (i - 1)].m_vecCameraLook.y + 
											(3* fU3 - 6 * fU2 + 0 * fU + 4) *
											m_lstEvent[i + 0].m_vecCameraLook.y + 
											(-3* fU3 + 3 * fU2 + 3 * fU + 1) *
											m_lstEvent[(i <= (m_iEventNum - 2)) ? i + 1 : i].m_vecCameraLook.y + 
											(1* fU3 + 0 * fU2 + 0 * fU + 0) *
											m_lstEvent[(i <= (m_iEventNum - 3)) ? i + 2 : i].m_vecCameraLook.y) / 6.0f;
				vecNode.z = ( (-1 * fU3 + 3 * fU2 - 3*fU + 1) *
											m_lstEvent[(i == 0) ?  0 : (i - 1)].m_vecCameraLook.z + 
											(3* fU3 - 6 * fU2 + 0 * fU + 4) *
											m_lstEvent[i + 0].m_vecCameraLook.z + 
											(-3* fU3 + 3 * fU2 + 3 * fU + 1) *
											m_lstEvent[(i <= (m_iEventNum - 2)) ? i + 1 : i].m_vecCameraLook.z + 
											(1* fU3 + 0 * fU2 + 0 * fU + 0) *
											m_lstEvent[(i <= (m_iEventNum - 3)) ? i + 2 : i].m_vecCameraLook.z) / 6.0f;

				m_lstLookSpline.push_back(vecNode);
				
			}
			m_lstEvent[i].m_iLookEnd = m_lstLookSpline.size();
		}
	}
	m_bCreateLookSpline = true;

}
void CCameraScript::BackWordEvent()	// Event를 되돌린다.
{
	int iCurrent;
	int iBackCurrent;
	
	m_bBackWord = !m_bBackWord;

	D3DXVECTOR3 vecTmpSwap;

	
	for(int i = 0; i < floorf((float)m_iEventNum / 2.0f); i++ )
	{
		iCurrent = i;
		iBackCurrent = (m_iEventNum - 1) - i;
		
		if(iCurrent != iBackCurrent)
		{
			vecTmpSwap = m_lstEvent[iCurrent].m_vecCameraLook;
			m_lstEvent[iCurrent].m_vecCameraLook = m_lstEvent[iBackCurrent].m_vecCameraLook;
			m_lstEvent[iBackCurrent].m_vecCameraLook = vecTmpSwap;


			vecTmpSwap = m_lstEvent[iCurrent].m_vecCameraPos;
			m_lstEvent[iCurrent].m_vecCameraPos = m_lstEvent[iBackCurrent].m_vecCameraPos;
			m_lstEvent[iBackCurrent].m_vecCameraPos = vecTmpSwap;

			vecTmpSwap = m_lstEvent[iCurrent].m_vecCameraUp;
			m_lstEvent[iCurrent].m_vecCameraUp = m_lstEvent[iBackCurrent].m_vecCameraUp;
			m_lstEvent[iBackCurrent].m_vecCameraUp = vecTmpSwap;

		}


	}
	if(m_bInitSet)
	{
		D3DXVECTOR3 vecUp = D3DXVECTOR3(0.0f,1.0f,0.0f);	
		FixEvent(m_iEventNum -1,m_vecCameraInitPos,m_vecCameraInitLook,vecUp);
		InsertEvent(m_vecCameraInitPos,m_vecCameraInitLook,vecUp,m_lstEvent[m_iEventNum -1].m_fFrame + 25.0f,0);
		
		//FixEvent(m_iEventNum -2,m_vecCameraInitPos,m_vecCameraInitLook,vecUp);
	}


}
void CCameraScript::FixEvent(int iEvent,D3DXVECTOR3 vecPos,D3DXVECTOR3 vecLook,D3DXVECTOR3 vecUp)
{
	if((iEvent< 0) || (iEvent >= m_iEventNum) )
		return;
	m_lstEvent[iEvent].m_vecCameraPos = vecPos;
	m_lstEvent[iEvent].m_vecCameraLook = vecLook;
	m_lstEvent[iEvent].m_vecCameraUp = vecUp;

}
void CCameraScript::SetPlay(bool bFlag,bool bBack,bool bStartPos) {
	m_bStart = bFlag;
	if(bBack && bFlag)
	{
		if(!m_bBackWord)
		{
			BackWordEvent();
			m_iInsertLook = 1;
			CreateSpline();
		}
	}
	else if(bFlag)
	{
		if(m_bBackWord)
		{
			BackWordEvent();
			m_iInsertLook = 1;
			CreateSpline();
		}
	}
	if(bStartPos && bFlag && !bBack)	// Start pos 부터 보간 시작(0 프레임 이벤트 수정)
	{

		matrix *matPos= CSceneManager::GetCamera()->GetMatPosition();
		vector3 vecCameraToward = CSceneManager::GetCamera()->GetViewTowardVector();
		//vector3 vecCameraUp = CSceneManager::GetCamera()->GetViewUpVector();

		D3DXVECTOR3 vecCameraSPos = D3DXVECTOR3(matPos->_41,matPos->_42,matPos->_43);
		D3DXVECTOR3 vecCameraSLook = D3DXVECTOR3(vecCameraSPos.x + vecCameraToward.x * 50.0f,vecCameraSPos.y + vecCameraToward.y* 50.0f,vecCameraSPos.z + vecCameraToward.z* 50.0f);
		//D3DXVECTOR3 vecCameraSUp = D3DXVECTOR3(vecCameraUp.x,vecCameraUp.y,vecCameraUp.z);
		
		D3DXVECTOR3 vecCameraSUp = D3DXVECTOR3(0.0f,1.0f,0.0f);
		FixEvent(0,vecCameraSPos,vecCameraSLook,vecCameraSUp);
		m_iInsertLook = 1;
		CreateSpline();

	}

	InitFrame();
	if(bFlag == false) {
		CSceneManager::m_ViewCamera->SetMatView(m_matBeforeView2);
		CSceneManager::m_ViewCamera->SetMatPosition(m_matBeforePos);
		CSceneManager::m_ViewCamera->MoveFrustum();
		CSceneManager::m_ViewCamera->SetVecPosition(vector3(m_matBeforePos._41,m_matBeforePos._42,m_matBeforePos._43));

	}
}