
// KaraokeMasterMaintenanceDlg.cpp : 実装ファイル
//

/**
* @file  KaraokeMasterMaintenanceDlg.cpp
* @brief カラオケマスターメンテメイン画面クラス
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

#include "stdafx.h"
#include "KaraokeMasterMaintenance.h"
#include "KaraokeMasterMaintenanceDlg.h"
#include "afxdialogex.h"
#include "XmlReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKaraokeMasterMaintenanceDlg ダイアログ



CKaraokeMasterMaintenanceDlg::CKaraokeMasterMaintenanceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKaraokeMasterMaintenanceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCategoryDlg = NULL;
	m_pUserDlg = NULL;
	m_pGenreDlg = NULL;
	m_pSongDlg = NULL;
}

void CKaraokeMasterMaintenanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_DB, m_oEdtDB);
	DDX_Control(pDX, IDC_BTN_CATEGORY, m_oBtnCategory);
	DDX_Control(pDX, IDC_BTN_USER, m_oBtnUser);
	DDX_Control(pDX, IDC_BTN_GENRE, m_oBtnGenre);
	DDX_Control(pDX, IDC_BTN_SONG, m_oBtnSong);
}

BEGIN_MESSAGE_MAP(CKaraokeMasterMaintenanceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BTN_DB, &CKaraokeMasterMaintenanceDlg::OnBnClickedBtnDb)
	ON_BN_CLICKED(IDC_BTN_CATEGORY, &CKaraokeMasterMaintenanceDlg::OnBnClickedBtnCategory)
	ON_BN_CLICKED(IDCANCEL, &CKaraokeMasterMaintenanceDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_USER, &CKaraokeMasterMaintenanceDlg::OnBnClickedBtnUser)
	ON_BN_CLICKED(IDC_BTN_GENRE, &CKaraokeMasterMaintenanceDlg::OnBnClickedBtnGenre)
	ON_BN_CLICKED(IDC_BTN_SONG, &CKaraokeMasterMaintenanceDlg::OnBnClickedBtnSong)
END_MESSAGE_MAP()


// CKaraokeMasterMaintenanceDlg メッセージ ハンドラー

//-----------------------------------------------
//
// @brief ダイアログ初期化
//
//-----------------------------------------------
BOOL CKaraokeMasterMaintenanceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	getProductName();

	CXmlReader oXMLReader;

	int iRet = oXMLReader.loadXML(_T("Config.xml"));
	if (iRet == XML_SUCCESS)
	{
		CString oStr;
		if (oXMLReader.getContents(_T("root/DataBasePath"), oStr) == XML_SUCCESS)
		{
			int iRet = m_oDataManage.openDB(oStr.GetBuffer(0));
			oStr.ReleaseBuffer();
			if (iRet == SQLITE_OK)
			{
				m_oEdtDB.SetWindowText(oStr);
				setEnableButtons(TRUE);
			}
		}
		oXMLReader.getContents(_T("root/MusicFolder"), m_oMusicFolder);
		oXMLReader.getContents(_T("root/BackupFolder"), m_oBackupFolder);
		oXMLReader.getContents(_T("root/LibFolder"), m_oVlcLibFolder);
	}

	// DLLのバージョン
	const char* pcVersion = m_oDataManage.getDLLVersion();
	if (strcmp(pcVersion, SQLITE_VERSION) != 0)
	{
		CString oStr;
		CString oVersion;
		oVersion = pcVersion;
		oStr.Format(_T("sqlite3.dllのバージョンが %s でないので、動かないかも。\nそれでも実行しますかい？\n現在のバージョン：%s"), _T(SQLITE_VERSION), oVersion);
		int iResult = MessageBox(oStr, m_oProductName, MB_YESNO | MB_ICONQUESTION);
		if (IDNO == iResult)
		{
			CDialogEx::OnCancel();
		}
	}
	// ダイアログでドラッグ＆ドロップを受け取れるよう設定
	DragAcceptFiles(TRUE);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CKaraokeMasterMaintenanceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CKaraokeMasterMaintenanceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CKaraokeMasterMaintenanceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//-----------------------------------------------
//
// @brief メッセージ制御
//
//-----------------------------------------------
BOOL CKaraokeMasterMaintenanceDlg::PreTranslateMessage(MSG* pMsg)
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
// @brief メッセージ制御
//
//-----------------------------------------------
LRESULT CKaraokeMasterMaintenanceDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_EDIT_END_CATEGORY:
		m_oBtnCategory.EnableWindow(TRUE);
		m_pCategoryDlg = NULL;
		break;
	case WM_EDIT_END_USER:
		m_oBtnUser.EnableWindow(TRUE);
		m_pUserDlg = NULL;
		break;
	case WM_EDIT_END_GENRE:
		m_oBtnGenre.EnableWindow(TRUE);
		m_pGenreDlg = NULL;
		break;
	case WM_EDIT_END_SONG:
		m_oBtnSong.EnableWindow(TRUE);
		m_pSongDlg = NULL;
		break;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


//-----------------------------------------------
//
// @brief 「DBを開く」ボタンを押下した
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnDb()
{
	if (checkEditDlg())
	{
		MessageBox(_T("テーブルが編集中なので、まず、閉じてくださいな"), m_oProductName);
		return;
	}

	CFileDialog oDlg(TRUE, NULL, _T("*.sqlite3"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("SQLite3ﾌｧｲﾙ | *.sqlite3 | 全てのﾌｧｲﾙ(*.*) | *.*||"), this);

	if (oDlg.DoModal() == IDOK){
		m_oEdtDB.SetWindowText(oDlg.GetPathName());
		setEnableButtons(TRUE);
	}
	else{
		setEnableButtons(FALSE);
	}
}


//-----------------------------------------------
//
// @brief ドラッグアンドドロップされた
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnDropFiles(HDROP hDropInfo)
{
	UINT uiCnt = 0;
	TCHAR szFile[MAX_PATH + 1];
	BOOL bEnable = FALSE;

	if (checkEditDlg())
	{
		MessageBox(_T("テーブルが編集中なので、まず、閉じてくださいな"), m_oProductName);
		return;
	}

	// ドロップされたファイル数を取得
	uiCnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (uiCnt != 1){
		MessageBox(_T("複数のファイルは受け取れませんて"), m_oProductName);
		goto EXIT;
	}
	memset(szFile, 0, sizeof(szFile));
	// ファイルの絶対パスを取得
	DragQueryFile(hDropInfo, 0, szFile, sizeof(szFile));
	if (::PathIsDirectory(szFile)){
		MessageBox(_T("フォルダーは受け取れませんて"), m_oProductName);
		goto EXIT;
	}
	m_oDataManage.closeDB();
	int iRet = m_oDataManage.openDB(szFile);
	if (iRet == SQLITE_OK)
	{
		m_oEdtDB.SetWindowText(szFile);
		bEnable = TRUE;
	}
	else
	{
		m_oEdtDB.SetWindowText(_T(""));
		MessageBox(_T("DBが開けませんでした。。。"), m_oProductName);
	}

EXIT:
	// アプリケーションへファイル名を転送するためにシステムが割り当てたメモリを解放
	DragFinish(hDropInfo);
	setEnableButtons(bEnable);

	CDialogEx::OnDropFiles(hDropInfo);
}

//-----------------------------------------------
//
// @brief ボタンの有効・無効を設定する
//
// @param bFlg	(i)TRUE：ボタン有効、FALSE：ボタン無効
//
// @return なし
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::setEnableButtons(BOOL bFlg)
{
	m_oBtnCategory.EnableWindow(bFlg);
	m_oBtnUser.EnableWindow(bFlg);
	m_oBtnGenre.EnableWindow(bFlg);
	m_oBtnSong.EnableWindow(bFlg);
}


//-----------------------------------------------
//
// @brief テーブルが編集中かどうかをチェックする
//
// @param なし
//
// @retval TRUE：編集中（何かのダイアログが開かれている）
// @retval FALSE：未編集
//
//-----------------------------------------------
BOOL CKaraokeMasterMaintenanceDlg::checkEditDlg(void)
{
	if (m_pCategoryDlg)	return TRUE;
	if (m_pUserDlg)	return TRUE;
	if (m_pGenreDlg)	return TRUE;
	if (m_pSongDlg)	return TRUE;
	return FALSE;
}


//-----------------------------------------------
//
// @brief リソースから製品名を抜く
//
// @param なし
//
// @retval なし
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::getProductName(void)
{
	// 現在の実行モジュールのフル・パスを取得
	TCHAR szModuleName[_MAX_PATH + 1];
	int nRet = ::GetModuleFileName(NULL, szModuleName, _MAX_PATH);

	DWORD dwSize = 0;
	DWORD dwReserved;
	dwSize = ::GetFileVersionInfoSize(szModuleName, &dwReserved);

	// ファイルのバージョン情報を取得
	TCHAR *pBuf = new TCHAR[dwSize + 1];
	BOOL bRet = ::GetFileVersionInfo(szModuleName, 0, dwSize, pBuf);

	//charset変換テーブルを取得
	LPWORD  lpWord;
	UINT  dwLen;
	bRet = ::VerQueryValue(pBuf, _T("\\VarFileInfo\\Translation"), (void **)&lpWord, &dwLen);
	if (bRet)
	{
		// lpWord[0] は言語セット lpWord[1] はキャラクタセット
		DWORD dwLangAndCharSet = MAKELONG(lpWord[1], lpWord[0]);
		//version
		TCHAR szKey[256];
		LPTSTR  lpStr;
		_sntprintf_s(szKey, 256, _T("\\StringFileInfo\\%08x\\%s"), dwLangAndCharSet, _T("ProductName"));
		bRet = ::VerQueryValue(pBuf, szKey, (void **)&lpStr, &dwLen);
		if (bRet)
		{
			//SetDlgItemText(IDC_VER,lpStr);
			m_oProductName = lpStr;
		}
	}
	delete pBuf;
}


//-----------------------------------------------
//
// @brief テーブル編集ダイアログを開く
//
// @param poTableEditDlg	(i)ダイアログポインタ
// @param iMode				(i)開くモード
// @param pcTitle			(i)ダイアログに表示するタイトル
//
// @retval TRUE：成功（開けた）
// @retval FALSE：失敗（開けなかった）
//
//-----------------------------------------------
BOOL CKaraokeMasterMaintenanceDlg::createTableEditDlg(CTableEditDlg*& poTableEditDlg, int iMode, TCHAR* pcTitle)
{
	BOOL bRet = FALSE;
	if (poTableEditDlg == NULL)
	{
		poTableEditDlg = new CTableEditDlg(iMode, m_oDataManage, this);
		if (poTableEditDlg)
		{
			poTableEditDlg->SetWindowText(pcTitle);
			poTableEditDlg->ShowWindow(TRUE);
			bRet = TRUE;
		}
		else
		{
			MessageBox(_T("メモリ不足で開けませんでした。。。"), m_oProductName);
		}
	}
	else
	{
		MessageBox(_T("もう開いてますやんw"), m_oProductName);
	}
	return bRet;
}


//-----------------------------------------------
//
// @brief 「終了」ボタンを押下した
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedCancel()
{
	if (checkEditDlg())
	{
		MessageBox(_T("テーブルが編集中なので、まず、閉じてくださいな"), m_oProductName);
		return;
	}
	CDialogEx::OnCancel();
}


//-----------------------------------------------
//
// @brief 「カテゴリマスター」ボタンを押下した
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnCategory()
{
	BOOL bRet = createTableEditDlg(m_pCategoryDlg, EN_CATEGORY, _T("カテゴリマスター"));
	if (bRet)
	{
			m_oBtnCategory.EnableWindow(FALSE);
	}
}


//-----------------------------------------------
//
// @brief 「ユーザーマスター」ボタンを押下した
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnUser()
{
	BOOL bRet = createTableEditDlg(m_pUserDlg, EN_USER, _T("ユーザーマスター"));
	if (bRet)
	{
			m_oBtnUser.EnableWindow(FALSE);
	}
}


//-----------------------------------------------
//
// @brief 「ジャンルマスター」ボタンを押下した
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnGenre()
{
	BOOL bRet = createTableEditDlg(m_pGenreDlg, EN_GENRE, _T("ジャンルマスター"));
	if (bRet)
	{
			m_oBtnGenre.EnableWindow(FALSE);
	}
}


//-----------------------------------------------
//
// @brief 「ソングマスター」ボタンを押下した
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnSong()
{
	BOOL bRet = createTableEditDlg(m_pSongDlg, EN_SONG, _T("ソングマスター"));
	if (bRet)
	{
			m_oBtnSong.EnableWindow(FALSE);
	}
}
