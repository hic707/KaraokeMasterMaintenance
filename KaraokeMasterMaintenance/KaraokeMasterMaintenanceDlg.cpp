
// KaraokeMasterMaintenanceDlg.cpp : �����t�@�C��
//

/**
* @file  KaraokeMasterMaintenanceDlg.cpp
* @brief �J���I�P�}�X�^�[�����e���C����ʃN���X
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


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
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


// CKaraokeMasterMaintenanceDlg �_�C�A���O



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


// CKaraokeMasterMaintenanceDlg ���b�Z�[�W �n���h���[

//-----------------------------------------------
//
// @brief �_�C�A���O������
//
//-----------------------------------------------
BOOL CKaraokeMasterMaintenanceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
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

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

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

	// DLL�̃o�[�W����
	const char* pcVersion = m_oDataManage.getDLLVersion();
	if (strcmp(pcVersion, SQLITE_VERSION) != 0)
	{
		CString oStr;
		CString oVersion;
		oVersion = pcVersion;
		oStr.Format(_T("sqlite3.dll�̃o�[�W������ %s �łȂ��̂ŁA�����Ȃ������B\n����ł����s���܂������H\n���݂̃o�[�W�����F%s"), _T(SQLITE_VERSION), oVersion);
		int iResult = MessageBox(oStr, m_oProductName, MB_YESNO | MB_ICONQUESTION);
		if (IDNO == iResult)
		{
			CDialogEx::OnCancel();
		}
	}
	// �_�C�A���O�Ńh���b�O���h���b�v���󂯎���悤�ݒ�
	DragAcceptFiles(TRUE);

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CKaraokeMasterMaintenanceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CKaraokeMasterMaintenanceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//-----------------------------------------------
//
// @brief ���b�Z�[�W����
//
//-----------------------------------------------
BOOL CKaraokeMasterMaintenanceDlg::PreTranslateMessage(MSG* pMsg)
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
// @brief ���b�Z�[�W����
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
// @brief �uDB���J���v�{�^������������
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnDb()
{
	if (checkEditDlg())
	{
		MessageBox(_T("�e�[�u�����ҏW���Ȃ̂ŁA�܂��A���Ă���������"), m_oProductName);
		return;
	}

	CFileDialog oDlg(TRUE, NULL, _T("*.sqlite3"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("SQLite3̧�� | *.sqlite3 | �S�Ă�̧��(*.*) | *.*||"), this);

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
// @brief �h���b�O�A���h�h���b�v���ꂽ
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnDropFiles(HDROP hDropInfo)
{
	UINT uiCnt = 0;
	TCHAR szFile[MAX_PATH + 1];
	BOOL bEnable = FALSE;

	if (checkEditDlg())
	{
		MessageBox(_T("�e�[�u�����ҏW���Ȃ̂ŁA�܂��A���Ă���������"), m_oProductName);
		return;
	}

	// �h���b�v���ꂽ�t�@�C�������擾
	uiCnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (uiCnt != 1){
		MessageBox(_T("�����̃t�@�C���͎󂯎��܂����"), m_oProductName);
		goto EXIT;
	}
	memset(szFile, 0, sizeof(szFile));
	// �t�@�C���̐�΃p�X���擾
	DragQueryFile(hDropInfo, 0, szFile, sizeof(szFile));
	if (::PathIsDirectory(szFile)){
		MessageBox(_T("�t�H���_�[�͎󂯎��܂����"), m_oProductName);
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
		MessageBox(_T("DB���J���܂���ł����B�B�B"), m_oProductName);
	}

EXIT:
	// �A�v���P�[�V�����փt�@�C������]�����邽�߂ɃV�X�e�������蓖�Ă������������
	DragFinish(hDropInfo);
	setEnableButtons(bEnable);

	CDialogEx::OnDropFiles(hDropInfo);
}

//-----------------------------------------------
//
// @brief �{�^���̗L���E������ݒ肷��
//
// @param bFlg	(i)TRUE�F�{�^���L���AFALSE�F�{�^������
//
// @return �Ȃ�
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
// @brief �e�[�u�����ҏW�����ǂ������`�F�b�N����
//
// @param �Ȃ�
//
// @retval TRUE�F�ҏW���i�����̃_�C�A���O���J����Ă���j
// @retval FALSE�F���ҏW
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
// @brief ���\�[�X���琻�i���𔲂�
//
// @param �Ȃ�
//
// @retval �Ȃ�
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::getProductName(void)
{
	// ���݂̎��s���W���[���̃t���E�p�X���擾
	TCHAR szModuleName[_MAX_PATH + 1];
	int nRet = ::GetModuleFileName(NULL, szModuleName, _MAX_PATH);

	DWORD dwSize = 0;
	DWORD dwReserved;
	dwSize = ::GetFileVersionInfoSize(szModuleName, &dwReserved);

	// �t�@�C���̃o�[�W���������擾
	TCHAR *pBuf = new TCHAR[dwSize + 1];
	BOOL bRet = ::GetFileVersionInfo(szModuleName, 0, dwSize, pBuf);

	//charset�ϊ��e�[�u�����擾
	LPWORD  lpWord;
	UINT  dwLen;
	bRet = ::VerQueryValue(pBuf, _T("\\VarFileInfo\\Translation"), (void **)&lpWord, &dwLen);
	if (bRet)
	{
		// lpWord[0] �͌���Z�b�g lpWord[1] �̓L�����N�^�Z�b�g
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
// @brief �e�[�u���ҏW�_�C�A���O���J��
//
// @param poTableEditDlg	(i)�_�C�A���O�|�C���^
// @param iMode				(i)�J�����[�h
// @param pcTitle			(i)�_�C�A���O�ɕ\������^�C�g��
//
// @retval TRUE�F�����i�J�����j
// @retval FALSE�F���s�i�J���Ȃ������j
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
			MessageBox(_T("�������s���ŊJ���܂���ł����B�B�B"), m_oProductName);
		}
	}
	else
	{
		MessageBox(_T("�����J���Ă܂����w"), m_oProductName);
	}
	return bRet;
}


//-----------------------------------------------
//
// @brief �u�I���v�{�^������������
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedCancel()
{
	if (checkEditDlg())
	{
		MessageBox(_T("�e�[�u�����ҏW���Ȃ̂ŁA�܂��A���Ă���������"), m_oProductName);
		return;
	}
	CDialogEx::OnCancel();
}


//-----------------------------------------------
//
// @brief �u�J�e�S���}�X�^�[�v�{�^������������
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnCategory()
{
	BOOL bRet = createTableEditDlg(m_pCategoryDlg, EN_CATEGORY, _T("�J�e�S���}�X�^�["));
	if (bRet)
	{
			m_oBtnCategory.EnableWindow(FALSE);
	}
}


//-----------------------------------------------
//
// @brief �u���[�U�[�}�X�^�[�v�{�^������������
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnUser()
{
	BOOL bRet = createTableEditDlg(m_pUserDlg, EN_USER, _T("���[�U�[�}�X�^�["));
	if (bRet)
	{
			m_oBtnUser.EnableWindow(FALSE);
	}
}


//-----------------------------------------------
//
// @brief �u�W�������}�X�^�[�v�{�^������������
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnGenre()
{
	BOOL bRet = createTableEditDlg(m_pGenreDlg, EN_GENRE, _T("�W�������}�X�^�["));
	if (bRet)
	{
			m_oBtnGenre.EnableWindow(FALSE);
	}
}


//-----------------------------------------------
//
// @brief �u�\���O�}�X�^�[�v�{�^������������
//
//-----------------------------------------------
void CKaraokeMasterMaintenanceDlg::OnBnClickedBtnSong()
{
	BOOL bRet = createTableEditDlg(m_pSongDlg, EN_SONG, _T("�\���O�}�X�^�["));
	if (bRet)
	{
			m_oBtnSong.EnableWindow(FALSE);
	}
}
