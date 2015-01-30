/**
* @file  DataManage.h
* @brief �J���I�P�f�[�^�Ǘ��N���X�̃w�b�_�[
*
* @author Togyo Tsukasa
* @date 2014-11-29
*/
#pragma once

//#include "TableData.h"
#include "../../include/sqlite3.h"

#define ERR_DB_OPEND			-1	//!< DB���I�[�v���ς�
#define ERR_CANNOT_OPEN_DB		-2	//!< DB���J���Ȃ�
#define ERR_NOT_KARAOKE_DB		-3	//!< �J���I�P�pDB����Ȃ�
#define ERR_MEMORY_FAIL			-4	//!< �������m�ێ��s
#define ERR_CANNOT_UPDATE_DB	-5	//!< DB�X�V���s
#define ERR_SEARCH_FAIL_DB		-6	//!< DB�������s
#define ERR_NO_REGIST_MUSIC		-7	//!< DB�ɖ��o�^�̋Ȃ���

//-----------------------------------------------
//
// @brief	�f�[�^�Ǘ��N���X
//
//-----------------------------------------------
class CDataManage
{
	sqlite3*	m_pDB;		//!< DB

	CString m_strDBFileName;	//!< DB�t�@�C����

	char**	m_ppcCategory;		//!< �J�e�S���}�X�^
	char**	m_ppcCategoryName;	//!< �J�e�S���}�X�^�iCD�A���O�̂ݗp�j
	char**	m_ppcUser;			//!< ���[�U�}�X�^
	char**	m_ppcGenre;			//!< �W�������}�X�^
	char**	m_ppcSongFile;		//!< �ȃf�[�^�f�[�u��

	CString m_oProductName;	//!< ���i��

	CString	m_oCategoryCd;	//!< �J�e�S���R�[�h�i�ȃf�[�^�f�[�u���Ɏg���j

public:
	CDataManage(void);
	virtual ~CDataManage();

	virtual int openDB(TCHAR *pcFileName);
	virtual int closeDB(void);

	virtual const char* getDLLVersion(void);

	virtual int backupDB(const TCHAR* pcBuckupDir);
	virtual int restoreDB(const TCHAR* pcBuckupDir);

	virtual int getCategory(char**& ppcCategory, int& iRecCnt, int& iColCnt);
	virtual int getCategoryName(char**& ppcCategory, int& iRecCnt, int& iColCnt);
	virtual int getUser(char**& ppcUser, int& iRecCnt, int& iColCnt);
	virtual int getGenre(char**& ppcGenre, int& iRecCnt, int& iColCnt);
	virtual int getSongFile(char**& ppcSongFile, int& iRecCnt, int& iColCnt);

//	virtual int importSongFile(const CStringArray& strArraySongFiles, const CString& strMusicFolder, CString& strNotUpdateFile, CString& strDeleteFile);
	virtual int importSongFile(const CStringArray& strArraySongFiles, const CString& strMusicFolder, const CString& strLogFileName);

	virtual wchar_t* convUTF8toUTF16(const char* pcUTF8);
	virtual char* convUTF16toUTF8(const wchar_t* pwcUTF16);

	virtual CString& getProductName(void){ return m_oProductName; }
	virtual void setProductName(const CString& oName){ m_oProductName = oName; }

	virtual void setCategoryCd(const CString& oCategoryCd){ m_oCategoryCd = oCategoryCd; }

protected:
	virtual void TravelFolder(const CString& strDir, CStringArray& strArrayFilePath);

//	virtual CString convSingleQuote(CString& strString);

	virtual int getTable(const CString& strSQL, char**& ppcResult, int& iRow, int& iCol);
	virtual int updateTable(const CString& strSQL);
	virtual int searchCategoryByPath(const CString& strPath, CString& strCategory, CString& strSubCategory);
	virtual int searchSingerKanaBySinger(const TCHAR* pcSinger, CString& strSingerKana);
	virtual int searchTieUpKanaByTieUp(const CString& strTieUpName, CString& strTieUpKana);
	virtual int searchDuoFlgByFileName(const CString& strFileName, CString& strDuoFlg, CString& strDuoColor1, CString& strDuoColor2, CString& strDuoColor3, CString& strDuoColor4, CString& strDuoColor5, CString& strDuoColorAll);
	virtual void freeTables(void);
};