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

	char**	m_ppcCategory;	//!< �J�e�S���}�X�^
	char**	m_ppcUser;		//!< ���[�U�}�X�^
	char**	m_ppcGenre;		//!< �W�������}�X�^
	char**	m_ppcSongFile;	//!< �ȃf�[�^�f�[�u��

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