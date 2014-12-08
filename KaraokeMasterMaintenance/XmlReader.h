/**
* @file  XmlReader.h
* @brief XML�t�@�C�����[�_�[ �w�b�_�[
*
* @author Togyo Tsukasa
* @date 2014-12-05
*/

#pragma once
#include <atlbase.h>
#include "shlwapi.h"

enum 
{
	XML_SUCCESS = 0,	//!< ����
	XML_ERR_LOADED,		//!< XML�Ǎ��ς�
	XML_ERR_LOAD,		//!< XML�Ǎ����s
	XML_ERR_NO_DATA,	//!< �f�[�^����
};

//-----------------------------------------------
//
// @brief	XML�t�@�C�����[�_�[�N���X
//
//-----------------------------------------------
class CXmlReader
{
protected :
//	MSXML2::IXMLDOMDocumentPtr m_poDoc;		//XML�h�L�������g
	IXMLDOMDocumentPtr m_poDoc;		//XML�h�L�������g
	BOOL m_bLoadFlg;

public :
	CXmlReader(void);
	virtual ~CXmlReader();

	virtual int loadXML(const CString& strXMLFile);
	virtual int getContents(const CString& strXpath, CString& strContents);
	virtual int closeXML(void);
};
