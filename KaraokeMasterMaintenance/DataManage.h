#pragma once

//#include "TableData.h"
#include "../../include/sqlite3.h"

#define ERR_DB_OPEND		-1
#define ERR_CANNOT_OPEN_DB	-2
#define ERR_MEMORY_FAIL		-3

//! データ管理クラス
class CDataManage
{
	sqlite3*	m_pDB;

	char**	m_ppcCategory;
	char**	m_ppcUser;
	char**	m_ppcGenre;
	char**	m_ppcSongFile;

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