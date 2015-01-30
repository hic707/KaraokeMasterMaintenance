/**
* @file  mpeg4.cpp
* @brief mpag4�Ǘ��N���X
*
* @author Togyo Tsukasa
* @date 2014-11-29
*/
#include "stdafx.h"
#include "mpeg4.h"

typedef struct
{
	UINT32	ui32Size;
	BYTE	byName[4];
}ST_ATOM;

#define MAX_STR_LEN 256	//!< �����o�b�t�@�T�C�Y

//-----------------------------------------------
//
// @brief �R���X�g���N�^
//
// @param �Ȃ�
//
//-----------------------------------------------
CMpag4::CMpag4(void)
{
	m_pcVocal = VOCAL_CLASSIFICATION_OFF;
	m_pcSubVocal = VOCAL_CLASSIFICATION_ON;
	m_pcSingerName = NULL;
}

//-----------------------------------------------
//
// @brief �f�X�g���N�^
//
//-----------------------------------------------
CMpag4::~CMpag4()
{
	if (m_pcSingerName)	delete[]m_pcSingerName;
}

#ifdef _DEBUG
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
wchar_t* CMpag4::convUTF8toUTF16(const char* pcUTF8)
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
#endif //_DEBUG

//-----------------------------------------------
//
// @brief mpeg4�̃t�@�C���p�X��ݒ肷��
//
// @param strFileName	(i)mpeg4�̃t�@�C���p�X
// @param strMusicDir	(i)Music�t�H���_�[�p�X
//
// @retval TRUE�F�ݒ萬��
// @retval FALSE�F�ݒ莸�s
//
//-----------------------------------------------
BOOL CMpag4::setMpeg4FileName(const CString& strFileName, const CString& strMusicDir)
{
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_PATH];
	TCHAR szFname[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath_s(strFileName, szDrive, szDir, szFname, szExt);

	CString strExt = szExt;

	BOOL bRet = TRUE;
	if (m_pcSingerName)	delete[]m_pcSingerName;
	m_pcSingerName = NULL;

	if (strExt.CompareNoCase(_T(".mp4")) != 0 && strExt.CompareNoCase(_T(".flv")) != 0)	return FALSE;

	m_strFileName = szFname;
//	m_strFileName += szExt;

	m_strCategoryName = szDrive;
	m_strCategoryName += szDir;
//	m_strCategoryName.Replace(strMusicDir+_T("\\"), _T(""));
	m_strCategoryName.Replace(strMusicDir, _T(""));
	// ������'\'������
	m_strCategoryName = m_strCategoryName.Mid(0, m_strCategoryName.GetLength() - 1);

	separateFileName(szFname);

	if (strExt.CompareNoCase(_T(".mp4")) != 0)	return TRUE;

	// mp4�́A�A�[�e�B�X�g��񂪂���Δ���
	ST_ATOM stAtom;
	FILE* fp;

	_tfopen_s(&fp, strFileName, _T("rb"));
	if (fp == NULL)	return FALSE;

	// moov�܂œǂݔ�΂��i�K�����݂���j
	m_iTrackCnt = 0;
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "moov", 4) != 0)
	{
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	INT32 i32MaxSize = getBigEndian(stAtom.ui32Size);
	// udta�܂œǂݔ�΂�
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "udta", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		if (i32MaxSize <= 0)	goto EXIT;
		if (strncmp((char*)stAtom.byName, "trak", 4) == 0)	m_iTrackCnt++;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	i32MaxSize = getBigEndian(stAtom.ui32Size);
	// meta�܂œǂݔ�΂�
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "meta", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		if (i32MaxSize <= 0)	goto EXIT;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}

	i32MaxSize = getBigEndian(stAtom.ui32Size);
	// ilst�܂œǂݔ�΂�
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "ilst", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		// meta�̒����4byte�󔒂�����炵���E�E�E
		if (getBigEndian(stAtom.ui32Size) == 0)
		{
			fseek(fp, 4, SEEK_CUR);
			i32MaxSize -= 4;
		}
		if (i32MaxSize <= 0)	goto EXIT;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	i32MaxSize = getBigEndian(stAtom.ui32Size);
	// ?ART�܂œǂݔ�΂�
	char* pcART = "\xa9\x41\x52\x54";
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, pcART, 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		if (i32MaxSize <= 0)	goto EXIT;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	i32MaxSize = getBigEndian(stAtom.ui32Size);
	// data�܂œǂݔ�΂�
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "data", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		if (i32MaxSize <= 0)	goto EXIT;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	// Class/Flag ��ǂݎ̂Ă�
	fseek(fp, 8, SEEK_CUR);

	int iBuffSize = getBigEndian(stAtom.ui32Size) - sizeof(stAtom) - 8;

	m_pcSingerName = new char[iBuffSize + 1];
	if (m_pcSingerName == NULL)
	{
		bRet = FALSE;
		goto EXIT;
	}

	memset(m_pcSingerName, 0, iBuffSize + 1);

	fread(m_pcSingerName, iBuffSize, 1, fp);
#ifdef _DEBUG
	convUTF8toUTF16(m_pcSingerName);
#endif //_DEBUG

EXIT:
	fclose(fp);

	m_iTrackCnt--;

	return bRet;
}

//-----------------------------------------------
//
// @brief mpeg4�̃t�@�C���𕪊����A�Ȗ��A���H�[�J���A�^�C�A�b�v���擾����
//
// @param pcFileName	(i)mpeg4�̃t�@�C����
//
// @return �Ȃ�
//
//-----------------------------------------------
void CMpag4::separateFileName(const TCHAR* pcFileName)
{
	CString strFileName(pcFileName);

	m_strSongName = pcFileName;
	m_pcVocal = VOCAL_CLASSIFICATION_OFF;
	m_strTieUpName.Empty();

	CString strFileNameLower(pcFileName);
	strFileNameLower.MakeLower();
	int iPos = -1, iLength = 0;
	if ((iPos = strFileNameLower.Find(_T("(off)"))) != -1)
	{
		iLength = _tcslen(_T("(off)"));
	}
	else if((iPos = strFileNameLower.Find(_T("(cut)"))) != -1)
	{
		m_pcVocal = VOCAL_CLASSIFICATION_CUT;
		iLength = _tcslen(_T("(cut)"));
	}
	else if((iPos = strFileNameLower.Find(_T("(on)"))) != -1)
	{
		m_pcVocal = VOCAL_CLASSIFICATION_ON;
		iLength = _tcslen(_T("(on)"));
	}

	if (iPos != -1)
	{
		m_strSongName = strFileName.Mid(0, iPos);
		m_strSongName.Trim();
		m_strTieUpName = strFileName.Mid(iPos + iLength);
		m_strTieUpName.Trim();
	}
}

//-----------------------------------------------
//
// @brief ��������w�肳�ꂽ�f���~�^�ŕ�������
//
// @param strIn		(i)�����Ώە�����
// @param strDelim	(i)�f���~�^
// @param strResult	(o)�����㕶����
//
// @return �Ȃ�
//
//-----------------------------------------------
void CMpag4::split(const CString& strIn, const CString& strDelim, CStringArray &strResult)
{
	strResult.RemoveAll();

	CString strWork = strIn;
	int i;

	while ((i = strWork.Find(strDelim)) != -1)
	{
		if (i > 0)
		{
			strResult.Add(strWork.Mid(0, i));
		}
		strWork = strWork.Mid(i + 1);
	}
	if (strWork.GetLength() > 0)
	{
		strResult.Add(strWork);
	}
}

//-----------------------------------------------
//
// @brief ���g���G���f�B�A���̐��l���r�b�O�G���f�B�A���̐��l�ɕϊ�����
//
// @param ui32In	(i)���g���G���f�B�A�����l
//
// @return �r�b�O�G���f�B�A�����l
//
//-----------------------------------------------
UINT32 CMpag4::getBigEndian(UINT32 ui32In)
{
	UINT32 ui32Work = 0;

	ui32Work  = (ui32In & 0x000000ff) << 24;
	ui32Work |= (ui32In & 0x0000ff00) << 8;
	ui32Work |= (ui32In & 0x00ff0000) >> 8;
	ui32Work |= (ui32In & 0xff000000) >> 24;

	return ui32Work;
}
