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

#define ERR_DB_OPEND		-1	//!< DB���I�[�v���ς�
#define ERR_CANNOT_OPEN_DB	-2	//!< DB���J���Ȃ�
#define ERR_NOT_KARAOKE_DB	-3	//!< �J���I�P�pDB����Ȃ�
#define ERR_MEMORY_FAIL		-4	//!< �������m�ێ��s

//-----------------------------------------------
//
// @brief	�f�[�^�Ǘ��N���X
//
//-----------------------------------------------
class CDataManage
{
	sqlite3*	m_pDB;		//!< DB

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