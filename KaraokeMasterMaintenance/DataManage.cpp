#include "stdafx.h"
#include "DataManage.h"

#define SQL_GET_CATEGORY	"select * from Category;"
#define SQL_GET_USER		"select * from User;"
#define SQL_GET_GENRE		"select * from Genre;"
#define SQL_GET_SONG_FILE	"select * from SongFile;"

#define MAX_STR_LEN 256

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
// @brief �w�肳�ꂽSQL�����s���A���ʂ��擾����
//
// @param pcSQL			(i)SQL
// @param ppcResult	(o)SQL�̎��s����
// @param iRow			(o)���R�[�h��
// @param iCol			(o)�J������
//
// @retval SQLITE_OK�F����
// @retval ERR_CANNOT_OPEN_DB�FDB���J����Ă��Ȃ�
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
	int iRet = getTable(SQL_GET_SONG_FILE, m_ppcSongFile, iRecCnt, iColCnt);
	if (iRet == SQLITE_OK)
	{
		// �f�[�^���擾����R�[�h
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
