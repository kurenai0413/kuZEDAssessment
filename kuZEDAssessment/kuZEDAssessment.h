
// kuZEDAssessment.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CkuZEDAssessmentApp: 
// �аѾ\��@�����O�� kuZEDAssessment.cpp
//

class CkuZEDAssessmentApp : public CWinApp
{
public:
	CkuZEDAssessmentApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CkuZEDAssessmentApp theApp;