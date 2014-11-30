
// KaraokeMasterMaintenanceDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "DataManage.h"
#include "TableEditDlg.h"

// CKaraokeMasterMaintenanceDlg ダイアログ
class CKaraokeMasterMaintenanceDlg : public CDialogEx
{
// コンストラクション
public:
	CKaraokeMasterMaintenanceDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_KARAOKEMASTERMAINTENANCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void setEnableButtons(BOOL bFlg);
	BOOL checkEditDlg(void);
	BOOL createTableEditDlg(CTableEditDlg*& poTableEditDlg, int iMode, TCHAR* pcTitle);
	void getProductName(void);

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedBtnDb();
	afx_msg void OnBnClickedBtnCategory();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnUser();
	afx_msg void OnBnClickedBtnGenre();
	afx_msg void OnBnClickedBtnSong();

private:
	CEdit m_oEdtDB;
	CButton m_oBtnCategory;
	CButton m_oBtnUser;
	CButton m_oBtnGenre;
	CButton m_oBtnSong;

	CDataManage m_oDataManage;

	CTableEditDlg* m_pCategoryDlg;
	CTableEditDlg* m_pUserDlg;
	CTableEditDlg* m_pGenreDlg;
	CTableEditDlg* m_pSongDlg;

	CString m_oProductName;
};
