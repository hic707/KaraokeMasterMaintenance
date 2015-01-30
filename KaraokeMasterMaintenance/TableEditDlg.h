/**
* @file  TableEditDlg.h
* @brief �e�[�u���ҏW��ʃN���X �w�b�_�[
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/
#pragma once
#include "afxcmn.h"
#include "DataManage.h"
#include "afxwin.h"


// CTableEditDlg �_�C�A���O

class CTableEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTableEditDlg)

public:
//	CTableEditDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	CTableEditDlg(int iMode, CDataManage& oDataManage, CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CTableEditDlg();

	void setParam(const TCHAR* pcParam);

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
	afx_msg void OnLvnEndlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnBnClickedButton1();

	virtual BOOL loadDB(void);

	void resize(void);
private:
	int m_iMode;
	HWND m_hWnd;
	char** m_ppcDBData;
	CDataManage* m_poDataManage;
	CListCtrl m_oLst;
	CEdit m_oEdt;
	int m_iSelectItem, m_iSelectSubItem;
	BOOL m_bChanged;

	CButton m_oBtnUpdate;
	CButton m_oRtnRollback;

	CString m_strParam;
public:
	afx_msg void OnBnClickedBtnTest();
private:
	CEdit m_oEdtTest;
};
