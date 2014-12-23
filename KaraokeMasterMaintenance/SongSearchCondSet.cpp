// SongSearchCondSet.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KaraokeMasterMaintenance.h"
#include "SongSearchCondSet.h"
#include "afxdialogex.h"


// CSongSearchCondSet ダイアログ

IMPLEMENT_DYNAMIC(CSongSearchCondSet, CDialogEx)

CSongSearchCondSet::CSongSearchCondSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSongSearchCondSet::IDD, pParent)
{

}

CSongSearchCondSet::~CSongSearchCondSet()
{
}

void CSongSearchCondSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_oCmbBox);
}


BEGIN_MESSAGE_MAP(CSongSearchCondSet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSongSearchCondSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CSongSearchCondSet メッセージ ハンドラー


//-----------------------------------------------
//
// @brief ダイアログ初期化
//
//-----------------------------------------------
BOOL CSongSearchCondSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int iRow, iCol;
	char** ppcData;

	int iRet = m_poDataManage->getCategoryName(ppcData, iRow, iCol);

	if (iRet != SQLITE_OK)
	{
		MessageBox(_T("テーブル取得に失敗しました。。。"), m_poDataManage->getProductName());
		return FALSE;
	}

	for (int iLop = 1; iLop <= iRow; iLop++)
	{
		m_oCategoryCdArray.Add(m_poDataManage->convUTF8toUTF16(ppcData[iLop * iCol + 0]));
		m_oCmbBox.InsertString(-1, m_poDataManage->convUTF8toUTF16(ppcData[iLop * iCol + 1]));
	}
	m_oCmbBox.SetCurSel(0);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

//-----------------------------------------------
//
// @brief データ管理クラスを設定する
//
// @param poDataManage	(i)データ管理クラス
//
// @return なし
//
//-----------------------------------------------
inline void CSongSearchCondSet::setDataManage(CDataManage* poDataManage)
{
	ASSERT(poDataManage);
	m_poDataManage = poDataManage;
}


//-----------------------------------------------
//
// @brief 抽出ボタン押下
//
//-----------------------------------------------
void CSongSearchCondSet::OnBnClickedOk()
{
	m_poDataManage->setCategoryCd( m_oCategoryCdArray[m_oCmbBox.GetCurSel()] );
	CDialogEx::OnOK();
}
