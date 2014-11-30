
// KaraokeMasterMaintenance.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

enum
{
	EN_CATEGORY = 1,
	EN_USER,
	EN_GENRE,
	EN_SONG,
};


#define WM_EDIT_END_CATEGORY	( WM_USER + EN_CATEGORY )
#define WM_EDIT_END_USER		( WM_USER + EN_USER )
#define WM_EDIT_END_GENRE		( WM_USER + EN_GENRE )
#define WM_EDIT_END_SONG		( WM_USER + EN_SONG )


// CKaraokeMasterMaintenanceApp:
// このクラスの実装については、KaraokeMasterMaintenance.cpp を参照してください。
//

class CKaraokeMasterMaintenanceApp : public CWinApp
{
public:
	CKaraokeMasterMaintenanceApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CKaraokeMasterMaintenanceApp theApp;