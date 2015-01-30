/**
* @file  mpeg4.cpp
* @brief mpag4管理クラス
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

#define MAX_STR_LEN 256	//!< 文字バッファサイズ

//-----------------------------------------------
//
// @brief コンストラクタ
//
// @param なし
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
// @brief デストラクタ
//
//-----------------------------------------------
CMpag4::~CMpag4()
{
	if (m_pcSingerName)	delete[]m_pcSingerName;
}

#ifdef _DEBUG
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
// @brief mpeg4のファイルパスを設定する
//
// @param strFileName	(i)mpeg4のファイルパス
// @param strMusicDir	(i)Musicフォルダーパス
//
// @retval TRUE：設定成功
// @retval FALSE：設定失敗
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
	// 末尾の'\'を消す
	m_strCategoryName = m_strCategoryName.Mid(0, m_strCategoryName.GetLength() - 1);

	separateFileName(szFname);

	if (strExt.CompareNoCase(_T(".mp4")) != 0)	return TRUE;

	// mp4は、アーティスト情報があれば抜く
	ST_ATOM stAtom;
	FILE* fp;

	_tfopen_s(&fp, strFileName, _T("rb"));
	if (fp == NULL)	return FALSE;

	// moovまで読み飛ばす（必ず存在する）
	m_iTrackCnt = 0;
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "moov", 4) != 0)
	{
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	INT32 i32MaxSize = getBigEndian(stAtom.ui32Size);
	// udtaまで読み飛ばす
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
	// metaまで読み飛ばす
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "meta", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		if (i32MaxSize <= 0)	goto EXIT;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}

	i32MaxSize = getBigEndian(stAtom.ui32Size);
	// ilstまで読み飛ばす
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "ilst", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		// metaの直後に4byte空白があるらしい・・・
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
	// ?ARTまで読み飛ばす
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
	// dataまで読み飛ばす
	fread(&stAtom, sizeof(stAtom), 1, fp);
	while (strncmp((char*)stAtom.byName, "data", 4) != 0)
	{
		i32MaxSize -= getBigEndian(stAtom.ui32Size);
		if (i32MaxSize <= 0)	goto EXIT;
		fseek(fp, getBigEndian(stAtom.ui32Size) - sizeof(stAtom), SEEK_CUR);
		fread(&stAtom, sizeof(stAtom), 1, fp);
	}
	// Class/Flag を読み捨てる
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
// @brief mpeg4のファイルを分割し、曲名、ヴォーカル、タイアップを取得する
//
// @param pcFileName	(i)mpeg4のファイル名
//
// @return なし
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
// @brief 文字列を指定されたデリミタで分割する
//
// @param strIn		(i)分割対象文字列
// @param strDelim	(i)デリミタ
// @param strResult	(o)分割後文字列
//
// @return なし
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
// @brief リトルエンディアンの数値をビッグエンディアンの数値に変換する
//
// @param ui32In	(i)リトルエンディアン数値
//
// @return ビッグエンディアン数値
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
