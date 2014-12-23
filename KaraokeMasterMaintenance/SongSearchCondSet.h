#pragma once
#include "DataManage.h"
#include "afxwin.h"


// CSongSearchCondSet �_�C�A���O

class CSongSearchCondSet : public CDialogEx
{
	DECLARE_DYNAMIC(CSongSearchCondSet)

public:
	CSongSearchCondSet(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSongSearchCondSet();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DLG_SELECT_GENRE };

	virtual void setDataManage(CDataManage* poDataManage);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
	CDataManage* m_poDataManage;
	CComboBox m_oCmbBox;
	CStringArray	m_oCategoryCdArray;
//	CString	m_oCategoryCd;
};
