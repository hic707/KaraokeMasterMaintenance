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

	char**	m_ppcCategory;		//!< カテゴリマスタ
	char**	m_ppcCategoryName;	//!< カテゴリマスタ（CD、名前のみ用）
	char**	m_ppcUser;			//!< ユーザマスタ
	char**	m_ppcGenre;			//!< ジャンルマスタ
	char**	m_ppcSongFile;		//!< 曲データデーブル

	CString m_oProductName;	//!< 製品名

	CString	m_oCategoryCd;	//!< カテゴリコード（曲データデーブルに使う）

public:
	CDataManage(void);
	virtual ~CDataManage();

	virtual int openDB(TCHAR *pcFileName);
	virtual int closeDB(void);

	virtual const char* getDLLVersion(void);

	virtual int getCategory(char**& ppcCategory, int& iRecCnt, int& iColCnt);
	virtual int getCategoryName(char**& ppcCategory, int& iRecCnt, int& iColCnt);
	virtual int getUser(char**& ppcUser, int& iRecCnt, int& iColCnt);
	virtual int getGenre(char**& ppcGenre, int& iRecCnt, int& iColCnt);
	virtual int getSongFile(char**& ppcSongFile, int& iRecCnt, int& iColCnt);

	virtual wchar_t* convUTF8toUTF16(const char* pcUTF8);
	virtual char* convUTF16toUTF8(const wchar_t* pwcUTF16);

	virtual CString& getProductName(void){ return m_oProductName; }
	virtual void setProductName(const CString& oName){ m_oProductName = oName; }

	virtual void setCategoryCd(const CString& oCategoryCd){ m_oCategoryCd = oCategoryCd; }

protected:
	virtual int getTable(const CString& strSQL, char**& ppcResult, int& iRow, int& iCol);
	virtual void freeTables(void);
};