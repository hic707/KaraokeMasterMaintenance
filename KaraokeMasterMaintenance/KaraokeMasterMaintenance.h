
// KaraokeMasterMaintenance.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

/**
* @file  KaraokeMasterMaintenance.h
* @brief カラオケマスターメンテツール アプリケーションのメイン ヘッダー
*
* @author Togyo Tsukasa
* @date 2014-11-30
*/

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

//-----------------------------------------------
//
// @brief	各マスターの固有番号：イベントや、関数ポインタ配列の添え字にも使用
//
//-----------------------------------------------
enum
{
	EN_CATEGORY = 1,	//!< カテゴリマスタ
	EN_USER,			//!< ユーザマスタ
	EN_GENRE,			//!< ジャンルマスタ
	EN_SONG,			//!< 曲データデーブル
	EN_MUSIC_LOG,		//!< 既歌テーブル
};


//-----------------------------------------------
//
// ウィンドウメッセージ番号
//
//-----------------------------------------------
#define WM_EDIT_END_CATEGORY	( WM_USER + EN_CATEGORY )	//!< カテゴリマスタ更新終了
#define WM_EDIT_END_USER		( WM_USER + EN_USER )		//!< ユーザマスタ更新終了
#define WM_EDIT_END_GENRE		( WM_USER + EN_GENRE )		//!< ジャンルマスタ更新終了
#define WM_EDIT_END_SONG		( WM_USER + EN_SONG )		//!< 曲データデーブル更新終了
#define WM_EDIT_END_MUSIC_LOG	( WM_USER + EN_MUSIC_LOG )	//!< 既歌テーブル更新終了


// CKaraokeMasterMaintenanceApp:
// このクラスの実装については、KaraokeMasterMaintenance.cpp を参照してください。
//

//-----------------------------------------------
//
// @brief	カラオケマスターメンテツールのメインクラス
//
//-----------------------------------------------
class CKaraokeMasterMaintenanceApp : public CWinApp
{
public:
	CKaraokeMasterMaintenanceApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()

private:
	HANDLE m_hMutexApp;  // 二重起動防止用ミューテックス
};

extern CKaraokeMasterMaintenanceApp theApp;