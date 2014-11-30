#pragma once
#include "afxcmn.h"
#include "DataManage.h"


// CTableEditDlg �_�C�A���O

class CTableEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTableEditDlg)

public:
//	CTableEditDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	CTableEditDlg(int iMode, CDataManage& oDataManage, CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CTableEditDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DLG_TABLE_EDIT };

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);

	void resize(void);
private:
	int m_iMode;
	HWND m_hWnd;
	char** m_ppcDBData;
	CDataManage* m_poDataManage;
	CListCtrl m_oLst;
};
