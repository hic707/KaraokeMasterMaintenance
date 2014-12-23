#pragma once
#include "DataManage.h"
#include "afxwin.h"


// CSongSearchCondSet ダイアログ

class CSongSearchCondSet : public CDialogEx
{
	DECLARE_DYNAMIC(CSongSearchCondSet)

public:
	CSongSearchCondSet(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSongSearchCondSet();

// ダイアログ データ
	enum { IDD = IDD_DLG_SELECT_GENRE };

	virtual void setDataManage(CDataManage* poDataManage);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
	CDataManage* m_poDataManage;
	CComboBox m_oCmbBox;
	CStringArray	m_oCategoryCdArray;
//	CString	m_oCategoryCd;
};
