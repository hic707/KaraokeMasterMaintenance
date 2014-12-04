/**
* @file  DataManage.h
* @brief カラオケデータ管理クラスのヘッダー
*
* @author Togyo Tsukasa
* @date 2014-11-29
*/
#pragma once

//#include "TableData.h"
#include "../../include/sqlite3.h"

#define ERR_DB_OPEND		-1	//!< DBをオープン済み
#define ERR_CANNOT_OPEN_DB	-2	//!< DBが開けない
#define ERR_NOT_KARAOKE_DB	-3	//!< カラオケ用DBじゃない
#define ERR_MEMORY_FAIL		-4	//!< メモリ確保失敗

//-----------------------------------------------
//
// @brief	データ管理クラス
//
//-----------------------------------------------
class CDataManage
{
	sqlite3*	m_pDB;		//!< DB

	char**	m_ppcCategory;	//!< カテゴリマスタ
	char**	m_ppcUser;		//!< ユーザマスタ
	char**	m_ppcGenre;		//!< ジャンルマスタ
	char**	m_ppcSongFile;	//!< 曲データデーブル

public:
	CDataManage(void);
	~CDataManage();

	int openDB(TCHAR *pcFileName);
	int closeDB(void);

	const char* getDLLVersion(void);

	int getCategory(char**& ppcCategory, int& iRecCnt, int& iColCnt);
	int getUser(char**& ppcUser, int& iRecCnt, int& iColCnt);
	int getGenre(char**& ppcGenre, int& iRecCnt, int& iColCnt);
	int getSongFile(char**& ppcSongFile, int& iRecCnt, int& iColCnt);

	wchar_t* convUTF8toUTF16(const char* pcUTF8);
	char* convUTF16toUTF8(const wchar_t* pwcUTF16);

private:
	int getTable(const char* pcSQL, char**& ppcResult, int& iRow, int& iCol);
	void freeTables(void);
};