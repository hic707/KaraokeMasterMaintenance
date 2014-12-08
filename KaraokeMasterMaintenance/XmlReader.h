/**
* @file  XmlReader.h
* @brief XMLファイルリーダー ヘッダー
*
* @author Togyo Tsukasa
* @date 2014-12-05
*/

#pragma once
#include <atlbase.h>
#include "shlwapi.h"

enum 
{
	XML_SUCCESS = 0,	//!< 正常
	XML_ERR_LOADED,		//!< XML読込済み
	XML_ERR_LOAD,		//!< XML読込失敗
	XML_ERR_NO_DATA,	//!< データ無し
};

//-----------------------------------------------
//
// @brief	XMLファイルリーダークラス
//
//-----------------------------------------------
class CXmlReader
{
protected :
//	MSXML2::IXMLDOMDocumentPtr m_poDoc;		//XMLドキュメント
	IXMLDOMDocumentPtr m_poDoc;		//XMLドキュメント
	BOOL m_bLoadFlg;

public :
	CXmlReader(void);
	virtual ~CXmlReader();

	virtual int loadXML(const CString& strXMLFile);
	virtual int getContents(const CString& strXpath, CString& strContents);
	virtual int closeXML(void);
};
