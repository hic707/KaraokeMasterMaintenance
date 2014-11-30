#include "stdafx.h"
#include "DataManage.h"

#define SQL_GET_CATEGORY	"select * from Category;"
#define SQL_GET_USER		"select * from User;"
#define SQL_GET_GENRE		"select * from Genre;"
#define SQL_GET_SONG_FILE	"select * from SongFile;"

#define MAX_STR_LEN 256

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
//
//-----------------------------------------------
int CDataManage::openDB(TCHAR *pcFileName)
{
	int iRet;
	if (m_pDB)
	{
		return ERR_DB_OPEND;
	}
#ifdef _UNICODE
	CStringA strWork;
	strWork = pcFileName;
	char* pcWork = strWork.GetBuffer(strWork.GetLength() + 1);
#else
	char* pcWork = pcFileName;
#endif//_UNICODE
	iRet = sqlite3_open(pcWork, &m_pDB);
#ifdef _UNICODE
	strWork.ReleaseBuffer();
#endif//_UNICODE
	char** ppcCategory = NULL;
	int iCnt, iCol;
	iRet = getCategory(ppcCategory, iCnt, iCol);
//	closeDB();
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
// @brief 指定されたSQLを実行し、結果を取得する
//
// @param pcSQL			(i)SQL
// @param ppcResult	(o)SQLの実行結果
// @param iRow			(o)レコード数
// @param iCol			(o)カラム数
//
// @retval SQLITE_OK：成功
// @retval ERR_CANNOT_OPEN_DB：DBが開かれていない
//
//-----------------------------------------------
int CDataManage::getTable(const char* pcSQL, char**& ppcResult, int& iRow, int& iCol)
{
	char *pcErr;
	if (m_pDB == NULL)
	{
		return ERR_CANNOT_OPEN_DB;
	}
	return sqlite3_get_table(m_pDB, pcSQL, &ppcResult, &iRow, &iCol, &pcErr);
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
	int iRet = getTable(SQL_GET_SONG_FILE, m_ppcSongFile, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// データを取得するコード
#ifdef _DEBUG
		for (int iLop = 0; iLop <= iRecCnt; iLop++)
		{
			CString oWrok;
			for (int jLop = 0; jLop < iColCnt; jLop++)
			{
				oWrok += convUTF8toUTF16(m_ppcSongFile[iLop * iColCnt + jLop]);
				oWrok += _T("\t");
			}
			oWrok += _T("\n");
			TRACE(oWrok);
		}
#endif//_DEBUG

		ppcSongFile = m_ppcSongFile;
	}
	return iRet;
}
