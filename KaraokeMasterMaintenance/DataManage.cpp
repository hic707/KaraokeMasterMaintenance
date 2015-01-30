/**
* @file  DataManage.cpp
* @brief カラオケデータ管理クラス
*
* @author Togyo Tsukasa
* @date 2014-11-29
*/
#include "stdafx.h"
#include "locale.h"
#include "DataManage.h"
#include "mpeg4.h"

//-----------------------------------------------
//
// SQL⇒UTF-8に変換して渡してやる必要がある
//
//-----------------------------------------------
#define SQL_GET_CATEGORY		_T("select CategoryCd, CategoryName, GenreCd, Path from Category;")	//!< 全カテゴリマスタ取得
#define SQL_GET_CATEGORY_NAME	_T("select CategoryCd, CategoryName from Category;")	//!< 全カテゴリマスタ取得
#define SQL_GET_USER			_T("select * from User;")		//!< 全ユーザマスタ取得
#define SQL_GET_GENRE			_T("select * from Genre;")		//!< 全ジャンルマスタ取得
#define SQL_GET_SONG_FILE		_T("select CategoryCd, SongName, SongKana, Singer, SingerKana, TieUpName, TieUpKana, DuoFlg, DuoColor1, DuoColor2, DuoColor3, DuoColor4, DuoColor5, DuoColorAll from SongFile where CategoryCd like \"%s\";")	//!< 曲データテーブル(カテゴリで絞る)取得
#define SQL_GET_MUSIC_LOGS		_T("select * from MusicLog;")	//!< 全既歌テーブル取得

#define SQL_SEARCH_CATEGORY_BY_CATEGORY_PATH	_T("select CategoryCd, SubCategoryCd from Category where Path like '%s';")	//!< パスからカテゴリ検索
#define SQL_SEARCH_DUO_FLG_FILE_BY_FILE_NAME	_T("select DuoFlg, DuoColor1, DuoColor2, DuoColor3, DuoColor4, DuoColor5, DuoColor5 from SongFile where FileName like '%s';")	//!< ファイル名からDUOフラグ検索
#define SQL_SEARCH_SINGER_KANA_BY_SINGER		_T("select SingerKana from SongFile where Singer like '%s' group by SingerKana;")	//!< 歌手から歌手カナ検索
#define SQL_SEARCH_TIE_UP_KANA_BY_TIE_UP_NAME	_T("select TieUpKana from SongFile where TieUpName like '%s' group by TieUpKana;")	//!< 歌手から歌手カナ検索

#define SQL_INSERT_sample	_T("insert into test values(1,'test1');")

#define SQL_UPDATE_sample	_T("update staff set name = 'Yamaoka' where id = 3;")

#define SQL_DELETE_SONG_FILE_BY_SONG_ID	_T("delete from SongFile where SongId = %s;")

#define MAX_STR_LEN 512	//!< 文字バッファサイズ

//-----------------------------------------------
//
// @brief コンストラクタ
//
// @param なし
//
//-----------------------------------------------
CDataManage::CDataManage(void)
{
	m_pDB = NULL;

	m_ppcCategory = NULL;
	m_ppcUser = NULL;
	m_ppcGenre = NULL;
	m_ppcSongFile = NULL;
}

//-----------------------------------------------
//
// @brief デストラクタ
//
//-----------------------------------------------
CDataManage::~CDataManage()
{
	closeDB();
}

//-----------------------------------------------
//
// @brief SQLite3DLLのバージョンを返す
//
// @param なし
//
// @return SQLite3DLLのバージョン（文字列）
//
//-----------------------------------------------
const char* CDataManage::getDLLVersion(void)
{
	return sqlite3_libversion();
}

//-----------------------------------------------
//
// @brief 指定されたSQLite3DBを開く
//
// @param pcFileName	(i)SQLite3DBファイル名
//
// @retval SQLITE_OK：成功
// @retval ERR_DB_OPEND：DBをオープン済み
// @retval ERR_CANNOT_OPEN_DB：DBが開けない
// @retval ERR_NOT_KARAOKE_DB：カラオケ用DBじゃない
//
//-----------------------------------------------
int CDataManage::openDB(TCHAR *pcFileName)
{
	if (m_pDB)
	{
		return ERR_DB_OPEND;
	}

	// ファイルの有無をチェキすべし（なければ、ERR_CANNOT_OPEN_DB）

	int iRet = sqlite3_open(convUTF16toUTF8(pcFileName), &m_pDB);

	if (iRet == SQLITE_OK)
	{
		char** ppcCategory = NULL;
		int iCnt, iCol;
		iRet = getCategory(ppcCategory, iCnt, iCol);
		if (iRet != SQLITE_OK)
		{
			iRet = ERR_NOT_KARAOKE_DB;
		}
		else
		{
			m_strDBFileName = pcFileName;
		}
	}
	else
	{
		iRet = ERR_CANNOT_OPEN_DB;
	}

	return iRet;
}

//-----------------------------------------------
//
// @brief DBを閉じる
//
// @param なし
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::closeDB(void)
{
	int iRet;
	if (m_pDB == NULL)
	{
		return ERR_CANNOT_OPEN_DB;
	}
	freeTables();
	iRet = sqlite3_close(m_pDB);
	m_pDB = NULL;
	return iRet;
}

//-----------------------------------------------
//
// @brief 開いているテーブルを全て閉じる
//
// @param なし
//
// @return なし
//
//-----------------------------------------------
void CDataManage::freeTables(void)
{
	if (m_ppcCategory)
	{
		sqlite3_free_table(m_ppcCategory);
		m_ppcCategory = NULL;
	}
	if (m_ppcUser)
	{
		sqlite3_free_table(m_ppcUser);
		m_ppcUser = NULL;
	}
	if (m_ppcGenre)
	{
		sqlite3_free_table(m_ppcGenre);
		m_ppcGenre = NULL;
	}
	if (m_ppcSongFile)
	{
		sqlite3_free_table(m_ppcSongFile);
		m_ppcSongFile = NULL;
	}
}

//-----------------------------------------------
//
// @brief UTF8をUTF16する
//
// @param pcUTF8	(i)UTF8の文字列
//
// @return UTF16の文字列
//
// @remark 変換結果は、早めに別の変数に移してください。
//
//-----------------------------------------------
wchar_t* CDataManage::convUTF8toUTF16(const char* pcUTF8)
{
	static wchar_t wcWork[MAX_STR_LEN];
	wcWork[0] = 0;
	if (pcUTF8 == NULL)
	{
		return wcWork;
	}
	int iBuffSize = MultiByteToWideChar(CP_UTF8, 0, pcUTF8, strlen(pcUTF8) + 1, NULL, 0);
	if (MAX_STR_LEN >= iBuffSize)
	{
		MultiByteToWideChar(CP_UTF8, 0, pcUTF8, strlen(pcUTF8) + 1, wcWork, MAX_STR_LEN);
	}
	return wcWork;
}

//-----------------------------------------------
//
// @brief UTF16をUTF8する
//
// @param pwcUTF16	(i)UTF16の文字列
//
// @return UTF8の文字列
//
// @remark 変換結果は、早めに別の変数に移してください。
//
//-----------------------------------------------
char* CDataManage::convUTF16toUTF8(const wchar_t* pwcUTF16)
{
	static char wcWork[MAX_STR_LEN];
	wcWork[0] = 0;
	if (pwcUTF16 == NULL)
	{
		return wcWork;
	}
	int iBuffSize = WideCharToMultiByte(CP_UTF8, 0, pwcUTF16, wcslen(pwcUTF16) + 1, NULL, 0, NULL, NULL);
	if (MAX_STR_LEN >= iBuffSize)
	{
		WideCharToMultiByte(CP_UTF8, 0, pwcUTF16, wcslen(pwcUTF16) + 1, wcWork, MAX_STR_LEN, NULL, NULL);
	}
	return wcWork;
}

//-----------------------------------------------
//
// @brief DBを指定されたディレクトリにバッグアップする
//
// @param pcBuckupDir	(i)バッグアップディレクトリ
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_CANNOT_UPDATE_DB：コピー失敗
//
//-----------------------------------------------
int CDataManage::backupDB(const TCHAR* pcBuckupDir)
{
	int iRet = SQLITE_OK;
	if (m_strDBFileName.IsEmpty())
	{
		return ERR_CANNOT_OPEN_DB;
	}
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_PATH];
	TCHAR szFname[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath_s(m_strDBFileName, szDrive, szDir, szFname, szExt);
	CTime cTime;
	cTime = CTime::GetCurrentTime();
	CString strCopyName = pcBuckupDir;
	strCopyName += _T("\\");
	strCopyName += szFname;
	strCopyName += cTime.Format("%y%m%d%H%M");
	strCopyName += szExt;
	if(!CopyFile(m_strDBFileName, strCopyName, FALSE))
	{
		iRet = ERR_CANNOT_UPDATE_DB;
	}

	return iRet;
}

//-----------------------------------------------
//
// @brief DBを指定されたディレクトリにバッグアップする
//
// @param pcBuckupDir	(i)バッグアップディレクトリ
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_CANNOT_UPDATE_DB：コピー失敗
//
//-----------------------------------------------
int CDataManage::restoreDB(const TCHAR* pcBuckupDir)
{
	int iRet = SQLITE_OK;
	if (m_strDBFileName.IsEmpty())
	{
		return ERR_CANNOT_OPEN_DB;
	}
	CStringArray strArray;
	TravelFolder(pcBuckupDir, strArray);
	if (!CopyFile(strArray[strArray.GetCount()-1], m_strDBFileName, FALSE))
	{
		iRet = ERR_CANNOT_UPDATE_DB;
	}

	return iRet;
}

//-----------------------------------------------
//
// @brief フォルダーの中にあるファイルとサブフォルダーにあるファイルを取得する
//
// @param strDir			(i)探すフォルダーパス
// @param strArrayFilePath	(i)見つけたファイルパス
//
// @return なし
//
//-----------------------------------------------
void CDataManage::TravelFolder(const CString& strDir, CStringArray& strArrayFilePath)
{
	CFileFind filefind;
	CString strWildpath = strDir + _T("\\*.sqlite3");	//指定した拡張子のファイル
	if (filefind.FindFile(strWildpath, 0))	//検索開始
	{
		BOOL bRet = TRUE;
		while (bRet)
		{
			bRet = filefind.FindNextFile();	//一番目のファイル
			if (filefind.IsDots())	continue;	//. 或いは ..の場合、次へ
			if (!filefind.IsDirectory())	//サブフォルダでなければ、ファイル名を追加
			{
				CString strTextOut = strDir + CString(_T("\\")) + filefind.GetFileName();
				strArrayFilePath.Add(strTextOut);
			}
		}
		filefind.Close();
	}
}

//-----------------------------------------------
//
// @brief 指定されたSQL（取得、検索）を実行し、結果を取得する
//
// @param strSQL		(i)SQL
// @param ppcResult	(o)SQLの実行結果
// @param iRow			(o)レコード数
// @param iCol			(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getTable(const CString& strSQL, char**& ppcResult, int& iRow, int& iCol)
{
	char *pcErr;
	if (m_pDB == NULL)
	{
		return ERR_CANNOT_OPEN_DB;
	}
	int iRet = sqlite3_get_table(m_pDB, convUTF16toUTF8(strSQL), &ppcResult, &iRow, &iCol, &pcErr);
	if (iRet != SQLITE_OK)
	{
		sqlite3_free(pcErr);
	}
	return iRet;
}

//-----------------------------------------------
//
// @brief 指定されたSQL（追加、更新）を実行し、結果を取得する
//
// @param strSQL		(i)SQL
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::updateTable(const CString& strSQL)
{
//	char *pcErr;
	if (m_pDB == NULL)
	{
		return ERR_CANNOT_OPEN_DB;
	}
//	int iRet = sqlite3_exec(m_pDB, convUTF16toUTF8(strSQL), NULL, NULL, &pcErr);
//	if (iRet != SQLITE_OK)
//	{
//		sqlite3_free(pcErr);
//	}
//	return iRet;
	sqlite3_stmt * stmt;
	int iRet = sqlite3_prepare(m_pDB, convUTF16toUTF8(strSQL), -1, &stmt, NULL);
	sqlite3_reset(stmt);
	if (stmt == NULL)
		return sqlite3_exec(m_pDB, convUTF16toUTF8(strSQL), NULL, NULL, NULL);
	while (SQLITE_DONE != sqlite3_step(stmt));
	sqlite3_finalize(stmt);
	return SQLITE_OK;
}

//-----------------------------------------------
//
// @brief Table Categoryの内容を全て取得する
//
// @param ppcCategory	(o)Categoryの内容
// @param iRecCnt		(o)レコード数
// @param iColCnt		(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getCategory(char**& ppcCategory, int& iRecCnt, int& iColCnt)
{
	ppcCategory = NULL;
	if (m_ppcCategory)
	{
		sqlite3_free_table(m_ppcCategory);
		m_ppcCategory = NULL;
	}
	int iRet = getTable(SQL_GET_CATEGORY, m_ppcCategory, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// データを取得するコード
#ifdef _DEBUG
		for (int iLop = 0; iLop <= iRecCnt; iLop++)
		{
			CString oWrok;
			for (int jLop = 0; jLop < iColCnt; jLop++)
			{
				oWrok += convUTF8toUTF16(m_ppcCategory[iLop * iColCnt + jLop]);
				oWrok += _T("\t");
			}
			oWrok += _T("\n");
			TRACE(oWrok);
		}
#endif//_DEBUG

		ppcCategory = m_ppcCategory;
	}
	return iRet;
}

//-----------------------------------------------
//
// @brief Table Categoryの内容を全て取得する
//
// @param ppcCategory	(o)Categoryの内容
// @param iRecCnt		(o)レコード数
// @param iColCnt		(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getCategoryName(char**& ppcCategory, int& iRecCnt, int& iColCnt)
{
	m_ppcCategoryName = NULL;
	if (m_ppcCategoryName)
	{
		sqlite3_free_table(m_ppcCategoryName);
		m_ppcCategoryName = NULL;
	}
	int iRet = getTable(SQL_GET_CATEGORY_NAME, m_ppcCategoryName, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// データを取得するコード
#ifdef _DEBUG
		for (int iLop = 0; iLop <= iRecCnt; iLop++)
		{
			CString oWrok;
			for (int jLop = 0; jLop < iColCnt; jLop++)
			{
				oWrok += convUTF8toUTF16(m_ppcCategoryName[iLop * iColCnt + jLop]);
				oWrok += _T("\t");
			}
			oWrok += _T("\n");
			TRACE(oWrok);
		}
#endif//_DEBUG

		ppcCategory = m_ppcCategoryName;
	}
	return iRet;
}

//-----------------------------------------------
//
// @brief Table Userの内容を全て取得する
//
// @param ppcUser	(o)Userの内容
// @param iRecCnt	(o)レコード数
// @param iColCnt	(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getUser(char**& ppcUser, int& iRecCnt, int& iColCnt)
{
	ppcUser = NULL;
	if (m_ppcUser)
	{
		sqlite3_free_table(m_ppcUser);
		m_ppcUser = NULL;
	}
	int iRet = getTable(SQL_GET_USER, m_ppcUser, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// データを取得するコード
#ifdef _DEBUG
		for (int iLop = 0; iLop <= iRecCnt; iLop++)
		{
			CString oWrok;
			for (int jLop = 0; jLop < iColCnt; jLop++)
			{
				oWrok += convUTF8toUTF16(m_ppcUser[iLop * iColCnt + jLop]);
				oWrok += _T("\t");
			}
			oWrok += _T("\n");
			TRACE(oWrok);
		}
#endif//_DEBUG

		ppcUser = m_ppcUser;
	}
	return iRet;
}

//-----------------------------------------------
//
// @brief Table Genreの内容を全て取得する
//
// @param ppcGenre	(o)Genreの内容
// @param iRecCnt	(o)レコード数
// @param iColCnt	(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getGenre(char**& ppcGenre, int& iRecCnt, int& iColCnt)
{
	ppcGenre = NULL;
	if (m_ppcGenre)
	{
		sqlite3_free_table(m_ppcGenre);
		m_ppcGenre = NULL;
	}
	int iRet = getTable(SQL_GET_GENRE, m_ppcGenre, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// データを取得するコード
#ifdef _DEBUG
		for (int iLop = 0; iLop <= iRecCnt; iLop++)
		{
			CString oWrok;
			for (int jLop = 0; jLop < iColCnt; jLop++)
			{
				oWrok += convUTF8toUTF16(m_ppcGenre[iLop * iColCnt + jLop]);
				oWrok += _T("\t");
			}
			oWrok += _T("\n");
			TRACE(oWrok);
		}
#endif//_DEBUG

		ppcGenre = m_ppcGenre;
	}
	return iRet;
}

//-----------------------------------------------
//
// @brief Table Genreの内容を全て取得する
//
// @param ppcGenre	(o)Genreの内容
// @param iRecCnt	(o)レコード数
// @param iColCnt	(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getSongFile(char**& ppcSongFile, int& iRecCnt, int& iColCnt)
{
	ppcSongFile = NULL;
	if (m_ppcSongFile)
	{
		sqlite3_free_table(m_ppcSongFile);
		m_ppcSongFile = NULL;
	}
	CString oWork;
	oWork.Format(SQL_GET_SONG_FILE, m_oCategoryCd);
	int iRet = getTable(oWork, m_ppcSongFile, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// データを取得するコード
#ifdef _DEBUG
//		for (int iLop = 0; iLop <= iRecCnt; iLop++)
//		{
//			CString oWrok;
//			for (int jLop = 0; jLop < iColCnt; jLop++)
//			{
//				oWrok += convUTF8toUTF16(m_ppcSongFile[iLop * iColCnt + jLop]);
//				oWrok += _T("\t");
//			}
//			oWrok += _T("\n");
//			TRACE(oWrok);
//		}
#endif//_DEBUG

		ppcSongFile = m_ppcSongFile;
	}
	return iRet;
}

//-----------------------------------------------
//
// @brief シングルクォート(')を(\')に置き換える
//
// @param strString			(i/o)変換したい文字列
//
// @return 変換結果
//
//-----------------------------------------------
//CString CDataManage::convSingleQuote(CString& strString)
//{
//	int iRet = strString.Find(_T("'"));
//	while (iRet != -1)
//	{
//		strString.Replace
//	}
//}

//-----------------------------------------------
//
// @brief カテゴリパスからカテゴリを取得する
//
// @param strPath			(i)カテゴリパス
// @param strCategory		(o)カテゴリ
// @param strSubCategory	(o)サブカテゴリ
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_SEARCH_FAIL_DB：DB検索失敗
//
//-----------------------------------------------
int CDataManage::searchCategoryByPath(const CString& strPath, CString& strCategory, CString& strSubCategory)
{
	char**	ppcDb;
	int iRecCnt, iColCnt;
	CString strSQL;
	CString strWork = strPath;

	strWork.Replace(_T("'"), _T("''"));

	strCategory.Empty();
	strSubCategory.Empty();

	// Categoryがあるか確認（Pathで検索する）
	strSQL.Format(SQL_SEARCH_CATEGORY_BY_CATEGORY_PATH, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// あれば、CategoryCd,SubCategoryCdを覚えておく
		strCategory = convUTF8toUTF16(ppcDb[2]);
		strCategory.Replace(_T("'"), _T("''"));
		strSubCategory = convUTF8toUTF16(ppcDb[3]);
		strSubCategory.Replace(_T("'"), _T("''"));
	}
	else
	{
		iRet = ERR_SEARCH_FAIL_DB;
	}

EXIT:
	sqlite3_free_table(ppcDb);

	return iRet;
}

//-----------------------------------------------
//
// @brief 歌手から歌手（カナ）を取得する
//
// @param pcSinger		(i)歌手
// @param strSingerKana	(o)歌手（カナ）
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_SEARCH_FAIL_DB：DB検索失敗
//
//-----------------------------------------------
int CDataManage::searchSingerKanaBySinger(const TCHAR* pcSinger, CString& strSingerKana)
{
	char**	ppcDb;
	int iRecCnt, iColCnt;
	CString strSQL;

	if (pcSinger == NULL)	return ERR_SEARCH_FAIL_DB;
	CString strWork = pcSinger;

	strWork.Replace(_T("'"), _T("''"));

	strSingerKana.Empty();

	// SingerKanaがあるか確認（Singerで検索する）
	strSQL.Format(SQL_SEARCH_SINGER_KANA_BY_SINGER, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// あれば、SingerKanaを覚えておく
		for (int iLop = 1; iLop <= iRecCnt;iLop++)
		{
			strSingerKana = convUTF8toUTF16(ppcDb[iLop]);
			strSingerKana.Replace(_T("'"), _T("''"));
			if (!strSingerKana.IsEmpty())	break;
		}
	}

EXIT:
	sqlite3_free_table(ppcDb);

	return iRet;
}

//-----------------------------------------------
//
// @brief タイアップからタイアップ（カナ）を取得する
//
// @param strTieUpName	(i)タイアップ
// @param strTieUpKana	(o)タイアップ（カナ）
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_SEARCH_FAIL_DB：DB検索失敗
//
//-----------------------------------------------
int CDataManage::searchTieUpKanaByTieUp(const CString& strTieUpName, CString& strTieUpKana)
{
	char**	ppcDb;
	int iRecCnt, iColCnt;
	CString strSQL;
	CString strWork = strTieUpName;

	strWork.Replace(_T("'"), _T("''"));

	strTieUpKana.Empty();

	// SingerKanaがあるか確認（Singerで検索する）
	strSQL.Format(SQL_SEARCH_TIE_UP_KANA_BY_TIE_UP_NAME, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// あれば、TieUpKanaを覚えておく
		for (int iLop = 1; iLop <= iRecCnt; iLop++)
		{
			strTieUpKana = convUTF8toUTF16(ppcDb[iLop]);
			strTieUpKana.Replace(_T("'"), _T("''"));
			if (!strTieUpKana.IsEmpty())	break;
		}
	}

EXIT:
	sqlite3_free_table(ppcDb);

	return iRet;
}

//-----------------------------------------------
//
// @brief ファイル名からその他情報を取得する
//
// @param strFileName		(i)ファイル名
// @param strDuoFlg			(o)DUOフラグ
// @param strDuoColor1		(o)DUO１人目の歌詞色
// @param strDuoColor2		(o)DUO２人目の歌詞色
// @param strDuoColor3		(o)DUO３人目の歌詞色
// @param strDuoColor4		(o)DUO４人目の歌詞色
// @param strDuoColor5		(o)DUO５人目の歌詞色
// @param strDuoColorAll	(o)DUO全員の歌詞色
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_SEARCH_FAIL_DB：DB検索失敗
//
//-----------------------------------------------
int CDataManage::searchDuoFlgByFileName(const CString& strFileName, CString& strDuoFlg, CString& strDuoColor1, CString& strDuoColor2, CString& strDuoColor3, CString& strDuoColor4, CString& strDuoColor5, CString& strDuoColorAll)
{
	char**	ppcDb;
	int iRecCnt, iColCnt;
	CString strSQL;
	CString strWork = strFileName;

	strWork.Replace(_T("'"), _T("''"));

	strDuoFlg.Empty();
	strDuoColor1.Empty();
	strDuoColor2.Empty();
	strDuoColor3.Empty();
	strDuoColor4.Empty();
	strDuoColor5.Empty();
	strDuoColorAll.Empty();

	// SingerKanaがあるか確認（Singerで検索する）
	strSQL.Format(SQL_SEARCH_DUO_FLG_FILE_BY_FILE_NAME, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// あれば、DuoFlg,DuoColor1～DuoColor5,DuoColorAlを覚えておく
		strDuoFlg = convUTF8toUTF16(ppcDb[7]);
		strDuoFlg.Replace(_T("'"), _T("''"));
		strDuoColor1 = convUTF8toUTF16(ppcDb[8]);
		strDuoColor1.Replace(_T("'"), _T("''"));
		strDuoColor2 = convUTF8toUTF16(ppcDb[9]);
		strDuoColor2.Replace(_T("'"), _T("''"));
		strDuoColor3 = convUTF8toUTF16(ppcDb[10]);
		strDuoColor3.Replace(_T("'"), _T("''"));
		strDuoColor4 = convUTF8toUTF16(ppcDb[11]);
		strDuoColor4.Replace(_T("'"), _T("''"));
		strDuoColor5 = convUTF8toUTF16(ppcDb[12]);
		strDuoColor5.Replace(_T("'"), _T("''"));
		strDuoColorAll = convUTF8toUTF16(ppcDb[13]);
		strDuoColorAll.Replace(_T("'"), _T("''"));
	}

EXIT:
	sqlite3_free_table(ppcDb);

	return iRet;
}

//-----------------------------------------------
//
// @brief Table Genreの内容を全て取得する
//
// @param strArraySongFiles	(i)曲ファイルパスの配列
// @param strMusicFolder	(i)Musicmフォルダーパス
// @param strLogFileName	(i)ログファルイ名
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
// @retval ERR_CANNOT_UPDATE_DB：DB更新失敗
// @retval ERR_NO_REGIST_MUSIC：DBに未登録の曲あり
//
//-----------------------------------------------
//int CDataManage::importSongFile(const CStringArray& strArraySongFiles, const CString& strMusicFolder, CString& strNotUpdateFile, CString& strDeleteFile)
int CDataManage::importSongFile(const CStringArray& strArraySongFiles, const CString& strMusicFolder, const CString& strLogFileName)
{
	if (m_pDB == NULL)
	{
		return ERR_CANNOT_OPEN_DB;
	}
	CMpag4 oMpeg4;
	CString oWork;
	CString strCategory, strSubCategory;
	CString strCategoryPath;
	CString strFileName, strSongName, strSingerName, strTieUpName, strVocal, strSubVocal;
	CString strSongKana, strSingerKana, strTieUpKana, strDuoFlg;
	CString strDuoColor1, strDuoColor2, strDuoColor3, strDuoColor4, strDuoColor5, strDuoColorAll;
	CString strSQL, strValues;
	CStringArray strArraySQL;
	int iRet;
	int iSequenceNum = 0;
	int iSongId = 1;
	int iNonRegistFlg = 0;
	int iLogWriteFlg = 1;
	_tsetlocale(LC_ALL, _T("japanese"));
	CStdioFile oFile;
	if (!oFile.Open(strLogFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		iLogWriteFlg = 0;
	}
	strArraySQL.SetSize(strArraySongFiles.GetCount());
	for (int iLop = 0; iLop < strArraySongFiles.GetCount(); iLop++)
	{
		oMpeg4.setMpeg4FileName(strArraySongFiles[iLop], strMusicFolder);

		// Categoryがあるか確認（Pathで検索する）
		if (searchCategoryByPath(oMpeg4.getCategoryPath(), strCategory, strSubCategory) != SQLITE_OK)
		{
			// なければ、ジャンルが確定できないため、飛ばす
			// 飛ばしたファイルは、終了時、メッセージを表示する。
			oWork.Format(_T("%s : カテゴリマスタに %s が存在しないので追加できません。"), strArraySongFiles[iLop], oMpeg4.getCategoryPath());
			if (iLogWriteFlg)
				oFile.WriteString(oWork);
			iNonRegistFlg = 1;
			continue;
		}

		if (strCategoryPath.Compare(oMpeg4.getCategoryPath()) == 0)
		{
			// 前のパスと一致すれば、シーケンス番号を更新する。
			iSequenceNum++;
			if (iSequenceNum >= 1000)
			{
				// SequenceNumが999を超えた場合は、飛ばす。
				oWork.Format(_T("%s : フォルダー(%s)内のファイルが1000を超えたので追加できません。\n"), strArraySongFiles[iLop], oMpeg4.getCategoryPath());
				if (iLogWriteFlg)
					oFile.WriteString(oWork);
				iNonRegistFlg = 1;
				// breakしてfor文を次に回したいが、意図通りに動かないな・・・
				goto NEXT;
			}
		}
		else
		{
			// 前のパスと一致しなければ、シーケンス番号をリセットする。
			iSequenceNum = 0;
			strCategoryPath = oMpeg4.getCategoryPath();
		}

		// SongNameがあるか確認（FileNameで検索する）
		if (searchDuoFlgByFileName(oMpeg4.getFileName(), strDuoFlg, strDuoColor1, strDuoColor2, strDuoColor3, strDuoColor4, strDuoColor5, strDuoColorAll) != SQLITE_OK)
		{
			iRet = ERR_CANNOT_UPDATE_DB;
			goto EXIT;
		}

		// SingerKanaは、Singerが空でなければ、検索して、SingerKanaが設定されていれば、設定する。
		if (searchSingerKanaBySinger(convUTF8toUTF16(oMpeg4.getSingerName()), strSingerKana) != SQLITE_OK)
		{
			iRet = ERR_CANNOT_UPDATE_DB;
			goto EXIT;
		}

		// TieUpKanaは、TieUpNameが空でなければ、検索して、TieUpKanaが設定されていれば、設定する。
		if (searchTieUpKanaByTieUp(oMpeg4.getTieUpName(), strTieUpKana) != SQLITE_OK)
		{
			iRet = ERR_CANNOT_UPDATE_DB;
			goto EXIT;
		}

		strFileName = oMpeg4.getFileName();
		strFileName.Replace(_T("'"), _T("''"));
		strSongName = oMpeg4.getSongName();
		strSongName.Replace(_T("'"), _T("''"));
		strSingerName = oMpeg4.getSingerName() ? convUTF8toUTF16(oMpeg4.getSingerName()) : _T("");
		strSingerName.Replace(_T("'"), _T("''"));
		strTieUpName = oMpeg4.getTieUpName();
		strTieUpName.Replace(_T("'"), _T("''"));
		strVocal = oMpeg4.getVocal();
		strVocal.Replace(_T("'"), _T("''"));
		strSubVocal = oMpeg4.getSubVocal();
		strSubVocal.Replace(_T("'"), _T("''"));
		strSQL.Format(_T("insert into SongFile values(%d,'%s','%s','%03d','%s','%s','%s','%s','%s','%s','%s',%s,%s,%s,'%s','%s','%s','%s','%s','%s');"),
			iSongId, strCategory, strSubCategory, iSequenceNum, strFileName, strSongName, strSongKana, strSingerName, strSingerKana,
			strTieUpName,strTieUpKana, strVocal, strSubVocal, strDuoFlg.IsEmpty() ? _T("0") : strDuoFlg, strDuoColor1, strDuoColor2,
			strDuoColor3, strDuoColor4, strDuoColor5, strDuoColorAll);
/*		strSQL = _T("INSERT INTO SongFile(SongId, CategoryCd, SubCategoryCd, SequenceNum, FileName, SongName, SongKana, Singer, SingerKana, TieUpName, TieUpKana, VocalClassification, SubVocalClassification");
		if (!strDuoFlg.IsEmpty() && strDuoFlg.Compare(_T("0")) != 0)
		{
			strSQL += _T(", DuoFlg, DuoColor1, DuoColor2, DuoColor3, DuoColor4, DuoColor5, DuoColorAll");
		}
		strValues.Format(_T(") values(%d, '%s', '%s', '%03d', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, %s"),
			iSongId, strCategory, strSubCategory, iSequenceNum, strFileName, strSongName, strSongKana,
			strSingerName, strSingerKana, strTieUpName, strTieUpKana, strVocal, strSubVocal);
		strSQL += strValues;
		if (!strDuoFlg.IsEmpty() && strDuoFlg.Compare(_T("0")) != 0)
		{
			strValues.Format(_T(", %s, '%s', '%s', '%s', '%s', '%s', '%s'"),
				strDuoFlg, strDuoColor1, strDuoColor2, strDuoColor3, strDuoColor4, strDuoColor5, strDuoColorAll);
			strSQL += strValues;
		}
		strSQL += _T(");");*/

		strArraySQL.SetAt(iSongId - 1, strSQL);

		iSongId++;
NEXT:;
	}

	strSQL = _T("delete from SongFile;");
	iRet = updateTable(strSQL);
	if (iRet != SQLITE_OK)	goto EXIT;

//	for (int iLop = 0; iLop < strArraySQL.GetCount(); iLop++)
	for (int iLop = 0; iLop < iSongId - 1; iLop++)
	{
		iRet = updateTable(strArraySQL[iLop]);
		if (iRet != SQLITE_OK)	goto EXIT;
	}
EXIT :
	oFile.Close();
	if (iNonRegistFlg && iRet == SQLITE_OK)
		iRet = ERR_NO_REGIST_MUSIC;
	if (iRet == SQLITE_OK)
		DeleteFile(strLogFileName);
	return iRet;
}
