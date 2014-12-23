
// KaraokeMasterMaintenance.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

/**
* @file  KaraokeMasterMaintenance.h
* @brief �J���I�P�}�X�^�[�����e�c�[�� �A�v���P�[�V�����̃��C�� �w�b�_�[
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

//-----------------------------------------------
//
// @brief	�e�}�X�^�[�̌ŗL�ԍ��F�C�x���g��A�֐��|�C���^�z��̓Y�����ɂ��g�p
//
//-----------------------------------------------
enum
{
	EN_CATEGORY = 1,	//!< �J�e�S���}�X�^
	EN_USER,			//!< ���[�U�}�X�^
	EN_GENRE,			//!< �W�������}�X�^
	EN_SONG,			//!< �ȃf�[�^�f�[�u��
	EN_MUSIC_LOG,		//!< ���̃e�[�u��
};


//-----------------------------------------------
//
// �E�B���h�E���b�Z�[�W�ԍ�
//
//-----------------------------------------------
#define WM_EDIT_END_CATEGORY	( WM_USER + EN_CATEGORY )	//!< �J�e�S���}�X�^�X�V�I��
#define WM_EDIT_END_USER		( WM_USER + EN_USER )		//!< ���[�U�}�X�^�X�V�I��
#define WM_EDIT_END_GENRE		( WM_USER + EN_GENRE )		//!< �W�������}�X�^�X�V�I��
#define WM_EDIT_END_SONG		( WM_USER + EN_SONG )		//!< �ȃf�[�^�f�[�u���X�V�I��
#define WM_EDIT_END_MUSIC_LOG	( WM_USER + EN_MUSIC_LOG )	//!< ���̃e�[�u���X�V�I��


// CKaraokeMasterMaintenanceApp:
// ���̃N���X�̎����ɂ��ẮAKaraokeMasterMaintenance.cpp ���Q�Ƃ��Ă��������B
//

//-----------------------------------------------
//
// @brief	�J���I�P�}�X�^�[�����e�c�[���̃��C���N���X
//
//-----------------------------------------------
class CKaraokeMasterMaintenanceApp : public CWinApp
{
public:
	CKaraokeMasterMaintenanceApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()

private:
	HANDLE m_hMutexApp;  // ��d�N���h�~�p�~���[�e�b�N�X
};

extern CKaraokeMasterMaintenanceApp theApp;