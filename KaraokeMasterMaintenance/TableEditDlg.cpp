// TableEditDlg.cpp : 実装ファイル
//

/**
* @file  TableEditDlg.cpp
* @brief テーブル編集画面クラス 
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

#include "stdafx.h"
#include "KaraokeMasterMaintenance.h"
#include "TableEditDlg.h"
#include "afxdialogex.h"

typedef int(CDataManage::*GetTableFunc)(char**&, int&, int&);

// CTableEditDlg ダイアログ

IMPLEMENT_DYNAMIC(CTableEditDlg, CDialogEx)

//-----------------------------------------------
//
// @brief コンストラクタ
//
// @param iMode			(i)動作モード
// @param oDataManage	(i)データ管理
//
//-----------------------------------------------
CTableEditDlg::CTableEditDlg(int iMode, CDataManage& oDataManage, CWnd* pParent /*=NULL*/)
	: CDialogEx(CTableEditDlg::IDD, pParent)
{
	m_iMode = iMode;
	m_hWnd = pParent->GetSafeHwnd();
	m_ppcDBData = NULL;
	m_poDataManage = &oDataManage;
	Create(CTableEditDlg::IDD, pParent);
}

//-----------------------------------------------
//
// @brief デストラクタ
//
//-----------------------------------------------
CTableEditDlg::~CTableEditDlg()
{
}

void CTableEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_oLst);
}


BEGIN_MESSAGE_MAP(CTableEditDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CTableEditDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CTableEditDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CTableEditDlg::OnNMCustomdrawList1)
END_MESSAGE_MAP()


//-----------------------------------------------
//
// @brief ダイアログ初期化
//
//-----------------------------------------------
BOOL CTableEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetTableFunc Func[] = { &CDataManage::getCategory, &CDataManage::getUser, &CDataManage::getGenre, &CDataManage::getSongFile };
	int iRow, iCol;
	char** ppcData;

	int iRet = (m_poDataManage->*Func[m_iMode - EN_CATEGORY])(ppcData, iRow, iCol);

	if (iRet != SQLITE_OK)
	{
		MessageBox(_T("テーブル取得に失敗しました。。。"));
		OnCancel();
	}

	m_oLst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
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
//			m_oLst.SetItem(iLop - 1, jLop, LVIF_TEXT, m_poDataManage->convUTF8toUTF16(ppcData[iLop * iCol + jLop]), 0, 0, 0, 0);
		}
	}
	

	resize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


// CTableEditDlg メッセージ ハンドラー


//-----------------------------------------------
//
// @brief メッセージ制御
//
//-----------------------------------------------
BOOL CTableEditDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)	//キー操作時
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)	// Enterキー、ESCキー押下時
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


//-----------------------------------------------
//
// @brief ダイアログ消滅直前
//
//-----------------------------------------------
void CTableEditDlg::PostNcDestroy()
{
	::SendMessage(m_hWnd, WM_USER + m_iMode, 0, 0);
	delete this;
}


//-----------------------------------------------
//
// @brief 「破棄」ボタンを押下した
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedCancel()
{
	DestroyWindow();
}


//-----------------------------------------------
//
// @brief 「更新」ボタンを押下した
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedOk()
{
	DestroyWindow();
}


//-----------------------------------------------
//
// @brief ダイアログサイズ変更
//
//-----------------------------------------------
void CTableEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	resize();
}

//-----------------------------------------------
//
// @brief ダイアログサイズ変更
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
// @brief リストコントロールをゼブラ表示する
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
