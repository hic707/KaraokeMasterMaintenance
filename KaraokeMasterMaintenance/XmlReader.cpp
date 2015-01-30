/**
* @file  XmlReader.cpp
* @brief XML�t�@�C�����[�_�[
*
* @author Togyo Tsukasa
* @date 2014-12-05
*/
#include "StdAfx.h"
#include "XmlReader.h"

#import "msxml6.dll" named_guids //raw_interface_only//��������

//-----------------------------------------------
//
// @brief �R���X�g���N�^
//
// @param �Ȃ�
//
//-----------------------------------------------
CXmlReader::CXmlReader(void)
{
	m_bLoadFlg = FALSE;
	::CoInitialize(NULL);
}

//-----------------------------------------------
//
// @brief �f�X�g���N�^
//
//-----------------------------------------------
CXmlReader::~CXmlReader()
{
	m_poDoc.Release();

	//���C�u������������܂��B
//	CoFreeUnusedLibraries();

	::CoUninitialize();
}

//-----------------------------------------------
//
// @brief �w�肳�ꂽXML�t�@�C����ǂݍ���
//
// @param strXMLFile	(i)XML�t�@�C����
//
// @retval XML_SUCCESS		�Ǎ�����
// @retval XML_ERR_LOADED	�Ǎ��ς�
// @retval XML_ERR_LOAD		�Ǎ����s
//
//-----------------------------------------------
int CXmlReader::loadXML(const CString& strXMLFile)
{
	if (m_bLoadFlg)	return XML_ERR_LOADED;
	//xml�I�u�W�F�N�g����
	m_poDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
//	m_poDoc.CreateInstance(CLSID_DOMDocument);

	// �ǂݍ��ݑ҂��ŕ�����ǂݍ���
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
// @brief �w�肳�ꂽXPath�̓��e���擾����
//
// @param strXpath		(i)�ǂݎ�肽��XPath
// @param strContents	(i)���e
//
// @retval XML_SUCCESS		�擾����
// @retval XML_ERR_LOAD		XML���Ǎ�
// @retval XML_ERR_NO_DATA	�f�[�^����
//
//-----------------------------------------------
int CXmlReader::getContents(const CString& strXpath, CString& strContents)
{
	if (!m_bLoadFlg)	return XML_ERR_LOAD;

	// �m�[�h���擾����
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
// @brief �w�肳�ꂽXPath�̓��e���擾����
//
// @param �Ȃ�
//
// @retval XML_SUCCESS		��������
// @retval XML_ERR_LOAD		XML���Ǎ�
//
//-----------------------------------------------
int CXmlReader::closeXML(void)
{
	if (!m_bLoadFlg)	return XML_ERR_LOAD;

	m_poDoc.Release();

	return XML_SUCCESS;
}
