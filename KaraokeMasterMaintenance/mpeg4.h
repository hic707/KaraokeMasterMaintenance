/**
* @file  mpeg4.h
* @brief mpeg4管理クラスのヘッダー
*
* @author Togyo Tsukasa
* @date 2015-01-03
*/
#pragma once

//#define VOCAL_CLASSIFICATION_NUL	_T("")
#define VOCAL_CLASSIFICATION_OFF	_T("0")
#define VOCAL_CLASSIFICATION_CUT	_T("1")
#define VOCAL_CLASSIFICATION_ON		_T("2")

#define SINGER_NAME_DEF ""

class CMpag4
{
	CString m_strCategoryName;
	CString m_strFileName;
	CString m_strSongName;
	TCHAR	*m_pcVocal;
	TCHAR	*m_pcSubVocal;
	CString m_strTieUpName;
	char	*m_pcSingerName;
	int		m_iTrackCnt;

	virtual void separateFileName(const TCHAR* pcFileName);
	virtual void split(const CString& str, const CString& delim, CStringArray &result);
	virtual UINT32 getBigEndian(UINT32 ui32In);
#ifdef _DEBUG
	virtual wchar_t* CMpag4::convUTF8toUTF16(const char* pcUTF8);
#endif //_DEBUG

public:
	CMpag4(void);
	~CMpag4();

	virtual BOOL setMpeg4FileName(const CString& strFileName, const CString& strMusicDir);
	virtual CString getCategoryPath(void) const{ return m_strCategoryName; }
	virtual CString getFileName(void) const{ return m_strFileName; }
	virtual CString getSongName(void) const{ return m_strSongName; }
	virtual TCHAR* getVocal(void) const{ return m_pcVocal; }
	virtual TCHAR* getSubVocal(void) const{ return m_pcSubVocal; }
	virtual CString getTieUpName(void) const{ return m_strTieUpName; }
	virtual char* getSingerName(void) const{ return m_pcSingerName; }
	virtual int getTrackCnt(void){ return m_iTrackCnt; }
};
