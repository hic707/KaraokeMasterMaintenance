/**
* @file  DataManage.cpp
* @brief �J���I�P�f�[�^�Ǘ��N���X
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
// SQL��UTF-8�ɕϊ����ēn���Ă��K�v������
//
//-----------------------------------------------
#define SQL_GET_CATEGORY		_T("select CategoryCd, CategoryName, GenreCd, Path from Category;")	//!< �S�J�e�S���}�X�^�擾
#define SQL_GET_CATEGORY_NAME	_T("select CategoryCd, CategoryName from Category;")	//!< �S�J�e�S���}�X�^�擾
#define SQL_GET_USER			_T("select * from User;")		//!< �S���[�U�}�X�^�擾
#define SQL_GET_GENRE			_T("select * from Genre;")		//!< �S�W�������}�X�^�擾
#define SQL_GET_SONG_FILE		_T("select CategoryCd, SongName, SongKana, Singer, SingerKana, TieUpName, TieUpKana, DuoFlg, DuoColor1, DuoColor2, DuoColor3, DuoColor4, DuoColor5, DuoColorAll from SongFile where CategoryCd like \"%s\";")	//!< �ȃf�[�^�e�[�u��(�J�e�S���ōi��)�擾
#define SQL_GET_MUSIC_LOGS		_T("select * from MusicLog;")	//!< �S���̃e�[�u���擾

#define SQL_SEARCH_CATEGORY_BY_CATEGORY_PATH	_T("select CategoryCd, SubCategoryCd from Category where Path like '%s';")	//!< �p�X����J�e�S������
#define SQL_SEARCH_DUO_FLG_FILE_BY_FILE_NAME	_T("select DuoFlg, DuoColor1, DuoColor2, DuoColor3, DuoColor4, DuoColor5, DuoColor5 from SongFile where FileName like '%s';")	//!< �t�@�C��������DUO�t���O����
#define SQL_SEARCH_SINGER_KANA_BY_SINGER		_T("select SingerKana from SongFile where Singer like '%s' group by SingerKana;")	//!< �̎肩��̎�J�i����
#define SQL_SEARCH_TIE_UP_KANA_BY_TIE_UP_NAME	_T("select TieUpKana from SongFile where TieUpName like '%s' group by TieUpKana;")	//!< �̎肩��̎�J�i����

#define SQL_INSERT_sample	_T("insert into test values(1,'test1');")

#define SQL_UPDATE_sample	_T("update staff set name = 'Yamaoka' where id = 3;")

#define SQL_DELETE_SONG_FILE_BY_SONG_ID	_T("delete from SongFile where SongId = %s;")

#define MAX_STR_LEN 512	//!< �����o�b�t�@�T�C�Y

//-----------------------------------------------
//
// @brief �R���X�g���N�^
//
// @param �Ȃ�
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
// @brief �f�X�g���N�^
//
//-----------------------------------------------
CDataManage::~CDataManage()
{
	closeDB();
}

//-----------------------------------------------
//
// @brief SQLite3DLL�̃o�[�W������Ԃ�
//
// @param �Ȃ�
//
// @return SQLite3DLL�̃o�[�W�����i������j
//
//-----------------------------------------------
const char* CDataManage::getDLLVersion(void)
{
	return sqlite3_libversion();
}

//-----------------------------------------------
//
// @brief �w�肳�ꂽSQLite3DB���J��
//
// @param pcFileName	(i)SQLite3DB�t�@�C����
//
// @retval SQLITE_OK�F����
// @retval ERR_DB_OPEND�FDB���I�[�v���ς�
// @retval ERR_CANNOT_OPEN_DB�FDB���J���Ȃ�
// @retval ERR_NOT_KARAOKE_DB�F�J���I�P�pDB����Ȃ�
//
//-----------------------------------------------
int CDataManage::openDB(TCHAR *pcFileName)
{
	if (m_pDB)
	{
		return ERR_DB_OPEND;
	}

	// �t�@�C���̗L�����`�F�L���ׂ��i�Ȃ���΁AERR_CANNOT_OPEN_DB�j

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
// @brief DB�����
//
// @param �Ȃ�
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
// @brief �J���Ă���e�[�u����S�ĕ���
//
// @param �Ȃ�
//
// @return �Ȃ�
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
// @brief UTF8��UTF16����
//
// @param pcUTF8	(i)UTF8�̕�����
//
// @return UTF16�̕�����
//
// @remark �ϊ����ʂ́A���߂ɕʂ̕ϐ��Ɉڂ��Ă��������B
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
// @brief UTF16��UTF8����
//
// @param pwcUTF16	(i)UTF16�̕�����
//
// @return UTF8�̕�����
//
// @remark �ϊ����ʂ́A���߂ɕʂ̕ϐ��Ɉڂ��Ă��������B
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
// @brief DB���w�肳�ꂽ�f�B���N�g���Ƀo�b�O�A�b�v����
//
// @param pcBuckupDir	(i)�o�b�O�A�b�v�f�B���N�g��
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_CANNOT_UPDATE_DB�F�R�s�[���s
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
// @brief DB���w�肳�ꂽ�f�B���N�g���Ƀo�b�O�A�b�v����
//
// @param pcBuckupDir	(i)�o�b�O�A�b�v�f�B���N�g��
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_CANNOT_UPDATE_DB�F�R�s�[���s
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
// @brief �t�H���_�[�̒��ɂ���t�@�C���ƃT�u�t�H���_�[�ɂ���t�@�C�����擾����
//
// @param strDir			(i)�T���t�H���_�[�p�X
// @param strArrayFilePath	(i)�������t�@�C���p�X
//
// @return �Ȃ�
//
//-----------------------------------------------
void CDataManage::TravelFolder(const CString& strDir, CStringArray& strArrayFilePath)
{
	CFileFind filefind;
	CString strWildpath = strDir + _T("\\*.sqlite3");	//�w�肵���g���q�̃t�@�C��
	if (filefind.FindFile(strWildpath, 0))	//�����J�n
	{
		BOOL bRet = TRUE;
		while (bRet)
		{
			bRet = filefind.FindNextFile();	//��Ԗڂ̃t�@�C��
			if (filefind.IsDots())	continue;	//. ������ ..�̏ꍇ�A����
			if (!filefind.IsDirectory())	//�T�u�t�H���_�łȂ���΁A�t�@�C������ǉ�
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
// @brief �w�肳�ꂽSQL�i�擾�A�����j�����s���A���ʂ��擾����
//
// @param strSQL		(i)SQL
// @param ppcResult	(o)SQL�̎��s����
// @param iRow			(o)���R�[�h��
// @param iCol			(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
// @brief �w�肳�ꂽSQL�i�ǉ��A�X�V�j�����s���A���ʂ��擾����
//
// @param strSQL		(i)SQL
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
// @brief Table Category�̓��e��S�Ď擾����
//
// @param ppcCategory	(o)Category�̓��e
// @param iRecCnt		(o)���R�[�h��
// @param iColCnt		(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
		// �f�[�^���擾����R�[�h
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
// @brief Table Category�̓��e��S�Ď擾����
//
// @param ppcCategory	(o)Category�̓��e
// @param iRecCnt		(o)���R�[�h��
// @param iColCnt		(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
		// �f�[�^���擾����R�[�h
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
// @brief Table User�̓��e��S�Ď擾����
//
// @param ppcUser	(o)User�̓��e
// @param iRecCnt	(o)���R�[�h��
// @param iColCnt	(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
		// �f�[�^���擾����R�[�h
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
// @brief Table Genre�̓��e��S�Ď擾����
//
// @param ppcGenre	(o)Genre�̓��e
// @param iRecCnt	(o)���R�[�h��
// @param iColCnt	(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
		// �f�[�^���擾����R�[�h
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
// @brief Table Genre�̓��e��S�Ď擾����
//
// @param ppcGenre	(o)Genre�̓��e
// @param iRecCnt	(o)���R�[�h��
// @param iColCnt	(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
		// �f�[�^���擾����R�[�h
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
// @brief �V���O���N�H�[�g(')��(\')�ɒu��������
//
// @param strString			(i/o)�ϊ�������������
//
// @return �ϊ�����
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
// @brief �J�e�S���p�X����J�e�S�����擾����
//
// @param strPath			(i)�J�e�S���p�X
// @param strCategory		(o)�J�e�S��
// @param strSubCategory	(o)�T�u�J�e�S��
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_SEARCH_FAIL_DB�FDB�������s
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

	// Category�����邩�m�F�iPath�Ō�������j
	strSQL.Format(SQL_SEARCH_CATEGORY_BY_CATEGORY_PATH, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// ����΁ACategoryCd,SubCategoryCd���o���Ă���
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
// @brief �̎肩��̎�i�J�i�j���擾����
//
// @param pcSinger		(i)�̎�
// @param strSingerKana	(o)�̎�i�J�i�j
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_SEARCH_FAIL_DB�FDB�������s
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

	// SingerKana�����邩�m�F�iSinger�Ō�������j
	strSQL.Format(SQL_SEARCH_SINGER_KANA_BY_SINGER, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// ����΁ASingerKana���o���Ă���
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
// @brief �^�C�A�b�v����^�C�A�b�v�i�J�i�j���擾����
//
// @param strTieUpName	(i)�^�C�A�b�v
// @param strTieUpKana	(o)�^�C�A�b�v�i�J�i�j
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_SEARCH_FAIL_DB�FDB�������s
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

	// SingerKana�����邩�m�F�iSinger�Ō�������j
	strSQL.Format(SQL_SEARCH_TIE_UP_KANA_BY_TIE_UP_NAME, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// ����΁ATieUpKana���o���Ă���
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
// @brief �t�@�C�������炻�̑������擾����
//
// @param strFileName		(i)�t�@�C����
// @param strDuoFlg			(o)DUO�t���O
// @param strDuoColor1		(o)DUO�P�l�ڂ̉̎��F
// @param strDuoColor2		(o)DUO�Q�l�ڂ̉̎��F
// @param strDuoColor3		(o)DUO�R�l�ڂ̉̎��F
// @param strDuoColor4		(o)DUO�S�l�ڂ̉̎��F
// @param strDuoColor5		(o)DUO�T�l�ڂ̉̎��F
// @param strDuoColorAll	(o)DUO�S���̉̎��F
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_SEARCH_FAIL_DB�FDB�������s
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

	// SingerKana�����邩�m�F�iSinger�Ō�������j
	strSQL.Format(SQL_SEARCH_DUO_FLG_FILE_BY_FILE_NAME, strWork);
	int iRet = getTable(strSQL, ppcDb, iRecCnt, iColCnt);
	if (iRet != SQLITE_OK)
	{
		goto EXIT;
	}

	if (iRecCnt >= 1)
	{
		// ����΁ADuoFlg,DuoColor1�`DuoColor5,DuoColorAl���o���Ă���
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
// @brief Table Genre�̓��e��S�Ď擾����
//
// @param strArraySongFiles	(i)�ȃt�@�C���p�X�̔z��
// @param strMusicFolder	(i)Musicm�t�H���_�[�p�X
// @param strLogFileName	(i)���O�t�@���C��
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
// @retval ERR_CANNOT_UPDATE_DB�FDB�X�V���s
// @retval ERR_NO_REGIST_MUSIC�FDB�ɖ��o�^�̋Ȃ���
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

		// Category�����邩�m�F�iPath�Ō�������j
		if (searchCategoryByPath(oMpeg4.getCategoryPath(), strCategory, strSubCategory) != SQLITE_OK)
		{
			// �Ȃ���΁A�W���������m��ł��Ȃ����߁A��΂�
			// ��΂����t�@�C���́A�I�����A���b�Z�[�W��\������B
			oWork.Format(_T("%s : �J�e�S���}�X�^�� %s �����݂��Ȃ��̂Œǉ��ł��܂���B"), strArraySongFiles[iLop], oMpeg4.getCategoryPath());
			if (iLogWriteFlg)
				oFile.WriteString(oWork);
			iNonRegistFlg = 1;
			continue;
		}

		if (strCategoryPath.Compare(oMpeg4.getCategoryPath()) == 0)
		{
			// �O�̃p�X�ƈ�v����΁A�V�[�P���X�ԍ����X�V����B
			iSequenceNum++;
			if (iSequenceNum >= 1000)
			{
				// SequenceNum��999�𒴂����ꍇ�́A��΂��B
				oWork.Format(_T("%s : �t�H���_�[(%s)���̃t�@�C����1000�𒴂����̂Œǉ��ł��܂���B\n"), strArraySongFiles[iLop], oMpeg4.getCategoryPath());
				if (iLogWriteFlg)
					oFile.WriteString(oWork);
				iNonRegistFlg = 1;
				// break����for�������ɉ񂵂������A�Ӑ}�ʂ�ɓ����Ȃ��ȁE�E�E
				goto NEXT;
			}
		}
		else
		{
			// �O�̃p�X�ƈ�v���Ȃ���΁A�V�[�P���X�ԍ������Z�b�g����B
			iSequenceNum = 0;
			strCategoryPath = oMpeg4.getCategoryPath();
		}

		// SongName�����邩�m�F�iFileName�Ō�������j
		if (searchDuoFlgByFileName(oMpeg4.getFileName(), strDuoFlg, strDuoColor1, strDuoColor2, strDuoColor3, strDuoColor4, strDuoColor5, strDuoColorAll) != SQLITE_OK)
		{
			iRet = ERR_CANNOT_UPDATE_DB;
			goto EXIT;
		}

		// SingerKana�́ASinger����łȂ���΁A�������āASingerKana���ݒ肳��Ă���΁A�ݒ肷��B
		if (searchSingerKanaBySinger(convUTF8toUTF16(oMpeg4.getSingerName()), strSingerKana) != SQLITE_OK)
		{
			iRet = ERR_CANNOT_UPDATE_DB;
			goto EXIT;
		}

		// TieUpKana�́ATieUpName����łȂ���΁A�������āATieUpKana���ݒ肳��Ă���΁A�ݒ肷��B
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
