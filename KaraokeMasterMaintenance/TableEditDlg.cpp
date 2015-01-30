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
	m_bChanged = FALSE;
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
// @brief ダイアログ初期化
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
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


//-----------------------------------------------
//
// @brief DBを読み込んで、リストコントロールに表示する
//
// @param なし
//
// @retval TRUE：成功
// @retval FALSE：失敗
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
		MessageBox(_T("テーブル取得に失敗しました。。。"), m_poDataManage->getProductName());
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
// @brief パラメータを設定する
//
// @param pcParam	(i)パラメータ
//
// @return なし
//
//-----------------------------------------------
inline void CTableEditDlg::setParam(const TCHAR* pcParam)
{
	ASSERT(pcParam);
	m_strParam = pcParam;
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
// @brief 「画面を閉じる」ボタンを押下した
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedCancel()
{
	if (m_bChanged)
	{
		int iResult = MessageBox(_T("DBが変更されてるよ！\n編集した内容を破棄して、画面を閉じていい？"), m_poDataManage->getProductName(), MB_YESNO | MB_ICONQUESTION);
		if (IDNO == iResult)
		{
			return;
		}
	}
	DestroyWindow();
}


//-----------------------------------------------
//
// @brief 「更新を反映」ボタンを押下した
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedOk()
{
	//DestroyWindow();
	MessageBox(_T("DBを更新したよ！（まだ未実装だけど）"), m_poDataManage->getProductName());
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
// @brief ウィンドウサイズ制限
//
//-----------------------------------------------
void CTableEditDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// 幅400高さ300より小さくできないようにする
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 300;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
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

//-----------------------------------------------
//
// @brief リストコントロールのエディットがフォーカスを失った
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
// @brief リストコントロールをダブルクリックしたときの処理
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
// @brief 編集終了時
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
// @brief 「編集を破棄」ボタン押下
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedButton1()
{
	int iResult = MessageBox(_T("DBが変更されてるよ！\n編集した内容を破棄して、DBを読み直していい？"), m_poDataManage->getProductName(), MB_YESNO | MB_ICONQUESTION);
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
// @brief 「カラーテスト」ボタン押下（テストなので消す）
//
//-----------------------------------------------
void CTableEditDlg::OnBnClickedBtnTest()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
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
