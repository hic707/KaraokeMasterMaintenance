
// KaraokeMasterMaintenance.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

enum
{
	EN_CATEGORY = 1,
	EN_USER,
	EN_GENRE,
	EN_SONG,
};


#define WM_EDIT_END_CATEGORY	( WM_USER + EN_CATEGORY )
#define WM_EDIT_END_USER		( WM_USER + EN_USER )
#define WM_EDIT_END_GENRE		( WM_USER + EN_GENRE )
#define WM_EDIT_END_SONG		( WM_USER + EN_SONG )


// CKaraokeMasterMaintenanceApp:
// ���̃N���X�̎����ɂ��ẮAKaraokeMasterMaintenance.cpp ���Q�Ƃ��Ă��������B
//

class CKaraokeMasterMaintenanceApp : public CWinApp
{
public:
	CKaraokeMasterMaintenanceApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CKaraokeMasterMaintenanceApp theApp;