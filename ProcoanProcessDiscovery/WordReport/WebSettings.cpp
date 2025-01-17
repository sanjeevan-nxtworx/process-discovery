#include "stdafx.h"
#include "WebSettings.h"


WebSettings::WebSettings():DocXDocPart("word","webSettings.xml")
{
	AddTag("w:webSettings");
	mainTag->AddAttribute("xmlns:mc", "http://schemas.openxmlformats.org/markup-compatibility/2006");
	mainTag->AddAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
	mainTag->AddAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
	mainTag->AddAttribute("xmlns:w14", "http://schemas.microsoft.com/office/word/2010/wordml");
	mainTag->AddAttribute("xmlns:w15", "http://schemas.microsoft.com/office/word/2012/wordml");
	mainTag->AddAttribute("xmlns:w16cid", "http://schemas.microsoft.com/office/word/2016/wordml/cid");
	mainTag->AddAttribute("xmlns:w16se", "http://schemas.microsoft.com/office/word/2015/wordml/symex");
	mainTag->AddAttribute("mc:Ignorable", "w14 w15 w16se w16cid");
}


WebSettings::~WebSettings()
{
}

void WebSettings::SetOptimizeForBrowser()
{
	mainTag->AddTag("w:optimizeForBrowser");
}

void WebSettings::SetAllowPNG()
{
	mainTag->AddTag("w:allowPNG");
}
