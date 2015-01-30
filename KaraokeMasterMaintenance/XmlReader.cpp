/**
* @file  XmlReader.cpp
* @brief XMLファイルリーダー
*
* @author Togyo Tsukasa
* @date 2014-12-05
*/
#include "StdAfx.h"
#include "XmlReader.h"

#import "msxml6.dll" named_guids //raw_interface_only//無視られる

//-----------------------------------------------
//
// @brief コンストラクタ
//
// @param なし
//
//-----------------------------------------------
CXmlReader::CXmlReader(void)
{
	m_bLoadFlg = FALSE;
	::CoInitialize(NULL);
}

//-----------------------------------------------
//
// @brief デストラクタ
//
//-----------------------------------------------
CXmlReader::~CXmlReader()
{
	m_poDoc.Release();

	//ライブラリを解放します。
//	CoFreeUnusedLibraries();

	::CoUninitialize();
}

//-----------------------------------------------
//
// @brief 指定されたXMLファイルを読み込む
//
// @param strXMLFile	(i)XMLファイル名
//
// @retval XML_SUCCESS		読込成功
// @retval XML_ERR_LOADED	読込済み
// @retval XML_ERR_LOAD		読込失敗
//
//-----------------------------------------------
int CXmlReader::loadXML(const CString& strXMLFile)
{
	if (m_bLoadFlg)	return XML_ERR_LOADED;
	//xmlオブジェクト生成
	m_poDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
//	m_poDoc.CreateInstance(CLSID_DOMDocument);

	// 読み込み待ちで文書を読み込む
	m_poDoc->put_async(VARIANT_FALSE);
//	m_poDoc->validateOnParse = VARIANT_TRUE;
	VARIANT_BOOL vbHoge;
	HRESULT hr = m_poDoc->load(CComVariant(strXMLFile), &vbHoge);
	if (hr != S_OK)
	{
		return XML_ERR_LOAD;
	}
	m_bLoadFlg = TRUE;

	return XML_SUCCESS;
}

//-----------------------------------------------
//
// @brief 指定されたXPathの内容を取得する
//
// @param strXpath		(i)読み取りたいXPath
// @param strContents	(i)内容
//
// @retval XML_SUCCESS		取得成功
// @retval XML_ERR_LOAD		XML未読込
// @retval XML_ERR_NO_DATA	データ無し
//
//-----------------------------------------------
int CXmlReader::getContents(const CString& strXpath, CString& strContents)
{
	if (!m_bLoadFlg)	return XML_ERR_LOAD;

	// ノードを取得する
//	MSXML2::IXMLDOMNodePtr pNode;
	IXMLDOMNodePtr pNode;
	CString oWork = strXpath;
	BSTR bsrtXpath = oWork.GetBuffer();
	oWork.ReleaseBuffer();
//	bsrtXpath = oWork;
//	pNode = m_poDoc->selectSingleNode((LPCTSTR)strContents);
	HRESULT hr = m_poDoc->selectSingleNode(bsrtXpath, &pNode);
//	::SysFreeString(bsrtXpath);
	if (!SUCCEEDED(hr))
	{
		return XML_ERR_NO_DATA;
	}

	BSTR value = NULL;
	hr = pNode->get_text(&value);
	if (!SUCCEEDED(hr))
	{
		return XML_ERR_NO_DATA;
	}
	strContents = value;
	::SysFreeString(value);

	return XML_SUCCESS;
}

//-----------------------------------------------
//
// @brief 指定されたXPathの内容を取得する
//
// @param なし
//
// @retval XML_SUCCESS		処理成功
// @retval XML_ERR_LOAD		XML未読込
//
//-----------------------------------------------
int CXmlReader::closeXML(void)
{
	if (!m_bLoadFlg)	return XML_ERR_LOAD;

	m_poDoc.Release();

	return XML_SUCCESS;
}
