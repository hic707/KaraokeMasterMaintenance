
// KaraokeMasterMaintenanceDlg.h : ヘッダー ファイル
//

/**
* @file  KaraokeMasterMaintenance.h
* @brief カラオケマスターメンテメイン画面クラス ヘッダー
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

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

	virtual void setEnableButtons(BOOL bFlg);
	virtual BOOL checkEditDlg(void);
	virtual BOOL createTableEditDlg(CTableEditDlg*& poTableEditDlg, int iMode, TCHAR* pcTitle);
	virtual void getProductName(void);

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedBtnDb();
	afx_msg void OnBnClickedBtnCategory();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnUser();
	afx_msg void OnBnClickedBtnGenre();
	afx_msg void OnBnClickedBtnSong();

private:
	CEdit m_oEdtDB;			//!< DBファイルエディット
	CButton m_oBtnCategory;	//!< カテゴリマスターボタン
	CButton m_oBtnUser;		//!< ユーザーマスターボタン
	CButton m_oBtnGenre;	//!< ジャンルマスターボタン
	CButton m_oBtnSong;		//!< ソングマスターボタン

	CDataManage m_oDataManage;	//!< データ管理

	CTableEditDlg* m_pCategoryDlg;	//!< カテゴリマスター用画面ポインタ
	CTableEditDlg* m_pUserDlg;		//!< ユーザーマスター用画面ポインタ
	CTableEditDlg* m_pGenreDlg;		//!< ジャンルマスター用画面ポインタ
	CTableEditDlg* m_pSongDlg;		//!< ソングマスター用画面ポインタ

	//XmlTextReader m_oXml;

	CString m_oProductName;		//!< 製品名（毛色が変わるがデータ管理に入れた方が良いか・・・取り回し的に）
	CString m_oMusicFolder;		//!< 動画が格納されているフォルダー
	CString m_oBackupFolder;	//!< 動画のバックアップフォルダー
	CString m_oVlcLibFolder;	//!< VLCライブラリ格納フォルダー
};
