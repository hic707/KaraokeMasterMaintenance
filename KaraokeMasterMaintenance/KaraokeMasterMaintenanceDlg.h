
// KaraokeMasterMaintenanceDlg.h : �w�b�_�[ �t�@�C��
//

/**
* @file  KaraokeMasterMaintenance.h
* @brief �J���I�P�}�X�^�[�����e���C����ʃN���X �w�b�_�[
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

#pragma once
#include "afxwin.h"
#include "DataManage.h"
#include "TableEditDlg.h"

// CKaraokeMasterMaintenanceDlg �_�C�A���O
class CKaraokeMasterMaintenanceDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CKaraokeMasterMaintenanceDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KARAOKEMASTERMAINTENANCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void setEnableButtons(BOOL bFlg);
	virtual BOOL checkEditDlg(void);
	virtual BOOL createTableEditDlg(CTableEditDlg*& poTableEditDlg, int iMode, TCHAR* pcTitle);
	virtual void getProductName(void);

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedBtnDb();
	afx_msg void OnBnClickedBtnCategory();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnUser();
	afx_msg void OnBnClickedBtnGenre();
	afx_msg void OnBnClickedBtnSong();

private:
	CEdit m_oEdtDB;			//!< DB�t�@�C���G�f�B�b�g
	CButton m_oBtnCategory;	//!< �J�e�S���}�X�^�[�{�^��
	CButton m_oBtnUser;		//!< ���[�U�[�}�X�^�[�{�^��
	CButton m_oBtnGenre;	//!< �W�������}�X�^�[�{�^��
	CButton m_oBtnSong;		//!< �\���O�}�X�^�[�{�^��

	CDataManage m_oDataManage;	//!< �f�[�^�Ǘ�

	CTableEditDlg* m_pCategoryDlg;	//!< �J�e�S���}�X�^�[�p��ʃ|�C���^
	CTableEditDlg* m_pUserDlg;		//!< ���[�U�[�}�X�^�[�p��ʃ|�C���^
	CTableEditDlg* m_pGenreDlg;		//!< �W�������}�X�^�[�p��ʃ|�C���^
	CTableEditDlg* m_pSongDlg;		//!< �\���O�}�X�^�[�p��ʃ|�C���^

	//XmlTextReader m_oXml;

	CString m_oProductName;		//!< ���i���i�ѐF���ς�邪�f�[�^�Ǘ��ɓ��ꂽ�����ǂ����E�E�E���񂵓I�Ɂj
	CString m_oMusicFolder;		//!< ���悪�i�[����Ă���t�H���_�[
	CString m_oBackupFolder;	//!< ����̃o�b�N�A�b�v�t�H���_�[
	CString m_oVlcLibFolder;	//!< VLC���C�u�����i�[�t�H���_�[
};
