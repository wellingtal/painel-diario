#ifndef __CCAMERASCRIPT_H__
#define __CCAMERASCRIPT_H__

#include <d3d8.h>
#include <d3dx8.h>

#include <vector>
#include <windows.h>
#include <string.h>
#include "matrix.h"


using namespace std;

enum C_INTERPOLATION {
	C_LINE,						// Linear 
	C_BEZIER1,					// Bezier (������ 3)
	C_BEZIER2,					// Bezier (������ 4)
	C_NOTINTERPOLATION,			// ���� ����
};

#define CAMERAEVENTEMPTY -1
#define CAMERAZEROFRAME 0.0f
#define CAMERASCRIPTVERSION 1.0f
#define CAMERAFRAMESTEP	30.0f
#define CAMERASPLINEUNIT	100
class CCameraScript {
private:
	// ������ unit
	class CCameraEvent {
	public:
			int				m_iIndex;				// ������ �ε���
			int				m_iInterpolation;		// �� Event���� ���� ��� ����
			float			m_fFrame;				// �������� ������ ������
			D3DXVECTOR3		m_vecControlPoint[2];	// ������ � Control Point
			D3DXVECTOR3		m_vecCameraPos;
			D3DXVECTOR3		m_vecCameraLook;
			D3DXVECTOR3		m_vecCameraUp;
			
			int				m_iPosStart;			// Pos Spline ������ ���� index
			int				m_iPosEnd;				// Pos Spline ������ ���� index
			
			int				m_iLookStart;			// Look Spline ������ ���� index
			int				m_iLookEnd;				// Look Spline ������ ���� index

		CCameraEvent() {
			m_iIndex = CAMERAEVENTEMPTY;
			m_iInterpolation = C_LINE;	
			m_fFrame = CAMERAZEROFRAME;
			m_vecCameraPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecCameraUp = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecCameraLook = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_iPosStart = CAMERAEVENTEMPTY;
			m_iPosEnd = CAMERAEVENTEMPTY;
			m_iLookStart = CAMERAEVENTEMPTY;
			m_iLookEnd = CAMERAEVENTEMPTY;

		}
		~CCameraEvent() {}
	};
	vector<CCameraEvent>	m_lstEvent;


	vector<D3DXVECTOR3>		m_lstPosSpline;
	bool					m_bCreateSpline;

	vector<D3DXVECTOR3>		m_lstLookSpline;
	bool					m_bCreateLookSpline;
	int						m_iInsertLook;

	int						m_iEventNum;
	int						m_iBeforeEvent;
	float					m_fRunFrame;		// ������ ������ ������ �ð� frame			
	D3DXMATRIX				m_matBeforeView;
	

	CCameraEvent			m_CurrentEvent;		// ���� �����ӿ� �´�, Event Unit
	D3DXMATRIX				m_matCurrentFrame;	// ���� Frame View Matrix
	LPDIRECT3DDEVICE8			m_pDevice;			// Direct3D Device

	DWORD					m_dwBeforeTick;
	DWORD					m_dwCurrentTick;
	bool					m_bStart;

	
	matrix m_matBeforePos;
	matrix m_matBeforeView2;
	
	int						m_iFixCount;

	bool					m_bBackWord;

	D3DXVECTOR3				m_vecCameraInitPos;
	D3DXVECTOR3				m_vecCameraInitLook;
	bool					m_bInitSet;

public:
	CCameraScript();
	~CCameraScript();

	void			InitCameraScript(bool bInitPos = false,D3DXVECTOR3 vecInitPos = D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXVECTOR3 vecInitLook = D3DXVECTOR3(0.0f,0.0f,0.0f));
	void			InitFrame();

	int				InsertEvent(D3DXVECTOR3 vecPos,D3DXVECTOR3 vecLook,D3DXVECTOR3 vecUp,float fFrame,int iInterpol);
	void			InsertLookPos(D3DXVECTOR3 vecLook);
	
	void			FixEvent(int iEvent,D3DXVECTOR3 vecPos,D3DXVECTOR3 vecLook,D3DXVECTOR3 vecUp);
	void			FixEvent(int iEvent,D3DXVECTOR3 vecPos,D3DXVECTOR3 vecLook,D3DXVECTOR3 vecUp,float fFrame,int iInterpol);
	void			FixLook(int iEvent,D3DXVECTOR3 vecLook);

	void			DeleteEvent(float fFrame,int iInterpol);
	void			DeleteEvent(int iIndex);
	
	bool			SaveScript(char *strFileName);
	bool			LoadScript(char *strFileName);

	void			CreateSpline();

	void			PlayScript(int iRoll,int iLoop,bool bFix);
	void			Rewind();
	void			Fowind();
	void			Jump(int iEvent);	//iEvent�� jump 
//	void			SetPlay(bool bFlag);
	void			SetPlay(bool bFlag,bool bBack = false,bool bStartPos = false);

	void            SetVecTakePos();   //��ǥ ���� �Լ�

	D3DXVECTOR3		GetVecPos() { return m_CurrentEvent.m_vecCameraPos; }
	D3DXVECTOR3		GetVecLook() { return m_CurrentEvent.m_vecCameraLook; }
	D3DXVECTOR3		GetVecUp() { return m_CurrentEvent.m_vecCameraUp; }
	D3DXMATRIX		GetCurrentViewMat() { return m_matCurrentFrame; }
	
	void			RenderPos_Up();
	void			RenderLook();
	void			RenderLine();
	void			RenderPosSpline();
	void			RenderLookSpline();
	bool			ISPlay() { return m_bStart;}
	int				GetExistEventNum() { return m_iEventNum;}
	int				GetFixCount(int &iIndex,float &fFrame);
	void			ReWindFix();
	void			FoWindFix();

	void			BackWordEvent();

	//
	void HALT()
	{
		m_bStart = false;
	}

};

#endif
