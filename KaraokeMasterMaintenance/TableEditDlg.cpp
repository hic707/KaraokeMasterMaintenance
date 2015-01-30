// TableEditDlg.cpp : �����t�@�C��
//

/**
* @file  TableEditDlg.cpp
* @brief �e�[�u���ҏW��ʃN���X 
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

#include "stdafx.h"
#include "KaraokeMasterMaintenance.h"
#include "TableEditDlg.h"
#include "afxdialogex.h"

typedef int(CDataManage::*GetTableFunc)(char**&, int&, int&);

// CTableEditDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CTableEditDlg, CDialogEx)

//-----------------------------------------------
//
// @brief �R���X�g���N�^
//
// @param iMode			(i)���샂�[�h
// @param oDataManage	(i)�f�[�^�Ǘ�
//
//-----------------------------------------------
CTableEditDlg::CTableEditDlg(int iMode, CDataManage& oDataManage, CWnd* pParent /*=NULL*/)
	: CDialogEx(CTableEditDlg::IDD, pParent)
{
	m_iMode = iMode;
	m_hWnd = pParent->GetSafeHwnd();
	m_ppcDBData = NULL;
	m_poDataManage = &oDataManage;
	m_bChanged = FALSE;
	Create(CTableEditDlg::IDD, pParent);
}

//-----------------------------------------------
//
// @brief �f�X�g���N�^
//
//-----------------------------------------------
CTableEditDlg::~CTableEditDlg()
{
}

void CTableEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_oLst);
	DDX_Control(pDX, IDC_EDIT1, m_oEdt);
	DDX_Control(pDX, IDOK, m_oBtnUpdate);
	DDX_Control(pDX, IDC_BUTTON1, m_oRtnRollback);
	DDX_Control(pDX, IDC_EDT_TEST, m_oEdtTest);
}


BEGIN_MESSAGE_MAP(CTableEditDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CTableEditDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CTableEditDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CTableEditDlg::OnNMCustomdrawList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTableEditDlg::OnNMDblclkList1)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CTableEditDlg::OnEnKillfocusEdit1)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, &CTableEditDlg::OnLvnEndlabeleditList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CTableEditDlg::OnBnClickedButton1)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BTN_TEST, &CTableEditDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


//-----------------------------------------------
//
// @brief �_�C�A���O������
//
//-----------------------------------------------
BOOL CTableEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_oLst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if (loadDB() == FALSE)
	{
		OnCancel();
	}
	
	m_oEdt.ShowWindow(SW_HIDE);
	resize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}


//-----------------------------------------------
//
// @brief DB��ǂݍ���ŁA���X�g�R���g���[���ɕ\������
//
// @param �Ȃ�
//
// @retval TRUE�F����
// @retval FALSE�F���s
//
//-----------------------------------------------
BOOL CTableEditDlg::loadDB(void)
{
	GetTableFunc Func[] = { &CDataManage::getCategory, &CDataManage::getUser, &CDataManage::getGenre, &CDataManage::getSongFile };
	int iRow, iCol;
	char** ppcData;
	BOOL bRet = TRUE;

	BeginWaitCursor();

	int iRet = (m_poDataManage->*Func[m_iMode - EN_CATEGORY])(ppcData, iRow, iCol);

	if (iRet != SQLITE_OK)
	{
		MessageBox(_T("�e�[�u���擾�Ɏ��s���܂����B�B�B"), m_poDataManage->getProductName());
		bRet = FALSE;
		goto EXIT;
	}

	m_oLst.DeleteAllItems();
	for (int iLop = 0; iLop < iCol; iLop++)
	{
		m_oLst.InsertColumn(iLop, m_poDataManage->convUTF8toUTF16(ppcData[iLop]), LVCFMT_LEFT, 100);
	}
	for (int iLop = 1; iLop <= iRow; iLop++)
	{
		m_oLst.InsertItem(iLop, m_poDataManage->convUTF8toUTF16(ppcData[iLop * iCol]));
		for (int jLop = 1; jLop < iCol; jLop++)
		{
			m_oLst.SetItemText(iLop - 1, jLop, m_poDataManage->convUTF8toUTF16(ppcData[iLop * iCol + jLop]));
		}
	}
	m_bChanged = FALSE;
	m_oBtnUpdate.EnableWindow(m_bChanged);
	m_oRtnRollback.EnableWindow(m_bChanged);

EXIT:
	EndWaitCursor();

	return bRet;
}

//-----------------------------------------------
//
// @brief �p�����[�^��ݒ肷��
//
// @param pcParam	(i)�p�����[�^
//
// @return �Ȃ�
//
//-----------------------------------------------
inline void CTableEditDlg::setParam(const TCHAR* pcParam)
{
	ASSERT(pcParam);
	m_strParam = pcParam;
}

// CTableEditDlg ���b�Z�[�W �n���h���[


//-----------------------------------------------
//
// @brief ���b�Z�[�W����
//
//-----------------------------------------------
BOOL CTableEditDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)	//�L�[���쎞
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)	// Enter�L�[�AESC�L�[������
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


//-----------------------------------------------
//
// @brief �_�C�A���O���Œ��O
//
//-----------------------------------------------
void CTableEditDlg::PostNcDestroy()
{
	::SendMessage(m_hWnd, WM_USER + m_iMode, 0, 0);
	delete this;
}


//-----------------------------------------------
//
// @brief �u��ʂ����v�{�^������������
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedCancel()
{
	if (m_bChanged)
	{
		int iResult = MessageBox(_T("DB���ύX����Ă��I\n�ҏW�������e��j�����āA��ʂ���Ă����H"), m_poDataManage->getProductName(), MB_YESNO | MB_ICONQUESTION);
		if (IDNO == iResult)
		{
			return;
		}
	}
	DestroyWindow();
}


//-----------------------------------------------
//
// @brief �u�X�V�𔽉f�v�{�^������������
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedOk()
{
	//DestroyWindow();
	MessageBox(_T("DB���X�V������I�i�܂������������ǁj"), m_poDataManage->getProductName());
}


//-----------------------------------------------
//
// @brief �_�C�A���O�T�C�Y�ύX
//
//-----------------------------------------------
void CTableEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	resize();
}


//-----------------------------------------------
//
// @brief �E�B���h�E�T�C�Y����
//
//-----------------------------------------------
void CTableEditDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// ��400����300��菬�����ł��Ȃ��悤�ɂ���
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 300;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

//-----------------------------------------------
//
// @brief �_�C�A���O�T�C�Y�ύX
//
//-----------------------------------------------
void CTableEditDlg::resize(void)
{
	if (m_oLst.GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(&rect);
		m_oLst.MoveWindow(10, 40, rect.Width() - 20, rect.Height() - 50);
	}
}


//-----------------------------------------------
//
// @brief ���X�g�R���g���[�����[�u���\������
//
//-----------------------------------------------
void CTableEditDlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW lpnmlv = (LPNMLVCUSTOMDRAW)pNMCD;
	if (lpnmlv->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = (long)CDRF_NOTIFYITEMDRAW;
		return;
	}
	if ((lpnmlv->nmcd.dwDrawStage&CDDS_ITEMPREPAINT) == CDDS_ITEMPREPAINT)
	{
		if (lpnmlv->nmcd.dwItemSpec % 2)
		{
			lpnmlv->clrTextBk = RGB(240, 240, 240);
		}
		*pResult = (long)CDRF_NOTIFYITEMDRAW;
	}
	else{
		*pResult = 0;
	}
}

//-----------------------------------------------
//
// @brief ���X�g�R���g���[���̃G�f�B�b�g���t�H�[�J�X��������
//
//-----------------------------------------------
void CTableEditDlg::OnLvnEndlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CString sEdit = pDispInfo->item.pszText;

	if (!sEdit.IsEmpty())
	{
		m_oLst.SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, sEdit);

		m_bChanged = TRUE;
		m_oBtnUpdate.EnableWindow(m_bChanged);
		m_oRtnRollback.EnableWindow(m_bChanged);
	}
	
	m_oLst.SetItemState(pDispInfo->item.iItem, 0, LVNI_FOCUSED | LVNI_SELECTED);

	*pResult = 0;
}


//-----------------------------------------------
//
// @brief ���X�g�R���g���[�����_�u���N���b�N�����Ƃ��̏���
//
//-----------------------------------------------
void CTableEditDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (pNMItemActivate->iItem < 0 || pNMItemActivate->iSubItem < 1)
		return;

	CRect ColumnRect;

	m_oLst.GetSubItemRect(pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_BOUNDS, ColumnRect);

	m_oEdt.SetWindowPos(&m_oLst, ColumnRect.left + 12, ColumnRect.top + 42, ColumnRect.right - ColumnRect.left, ColumnRect.bottom - ColumnRect.top, NULL);
	m_oEdt.ShowWindow(SW_SHOW);

	m_oEdt.SetWindowText(m_oLst.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem));

	m_iSelectItem = pNMItemActivate->iItem;
	m_iSelectSubItem = pNMItemActivate->iSubItem;

	m_oEdt.SetFocus();
}


//-----------------------------------------------
//
// @brief �ҏW�I����
//
//-----------------------------------------------
void CTableEditDlg::OnEnKillfocusEdit1()
{
	if (m_iSelectItem < 0 || m_iSelectSubItem < 1)
	{
		return;
	}

	CString oStr;
	m_oEdt.GetWindowText(oStr);
	if (oStr.Compare(m_oLst.GetItemText(m_iSelectItem, m_iSelectSubItem)) != 0)
	{
		m_bChanged = TRUE;
		m_oBtnUpdate.EnableWindow(m_bChanged);
		m_oRtnRollback.EnableWindow(m_bChanged);
	}

	m_oLst.SetItemText(m_iSelectItem, m_iSelectSubItem, oStr);
	m_oEdt.ShowWindow(SW_HIDE);
}


//-----------------------------------------------
//
// @brief �u�ҏW��j���v�{�^������
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedButton1()
{
	int iResult = MessageBox(_T("DB���ύX����Ă��I\n�ҏW�������e��j�����āADB��ǂݒ����Ă����H"), m_poDataManage->getProductName(), MB_YESNO | MB_ICONQUESTION);
	if (IDNO == iResult)
	{
		return;
	}
	if (loadDB() == FALSE)
	{
		OnCancel();
	}
}



//-----------------------------------------------
//
// @brief �u�J���[�e�X�g�v�{�^�������i�e�X�g�Ȃ̂ŏ����j
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedBtnTest()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	COLORREF stColor = RGB(255, 0x40, 0x80);
	CColorDialog oDlg(stColor);

	if (oDlg.DoModal() == IDOK)
	{	
		CString strWork;
		stColor = oDlg.GetColor();
		strWork.Format(_T("%06X"), stColor);
		m_oEdtTest.SetWindowText(strWork);
	}
}
