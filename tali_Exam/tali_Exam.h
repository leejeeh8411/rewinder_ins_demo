
// tali_Exam.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// Ctali_ExamApp:
// �� Ŭ������ ������ ���ؼ��� tali_Exam.cpp�� �����Ͻʽÿ�.
//

class Ctali_ExamApp : public CWinApp
{
public:
	Ctali_ExamApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern Ctali_ExamApp theApp;