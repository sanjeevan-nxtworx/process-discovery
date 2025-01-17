#include "stdafx.h"
#include "Theme1.h"


Theme1::Theme1() : DocXDocPart("word/theme","theme1.xml")
{
	mainTag = AddTag("a:theme");
	mainTag->AddAttribute("xmlns:a", "http://schemas.openxmlformats.org/drawingml/2006/main");
	mainTag->AddAttribute("name", "Office Theme");
}


Theme1::~Theme1()
{
}

void Theme1::BuildNormalTheme()
{
	DocXTag *pSingleTag = NULL, *pNested1Tag = NULL, *pNested2Tag = NULL, *pNested3Tag = NULL, *pNested4Tag = NULL;
	DocXTag *pNested5Tag = NULL, *pNested6Tag = NULL, *pNested7Tag = NULL;

	pSingleTag = mainTag->AddTag("a:themeElements");

	pNested1Tag = pSingleTag->AddTag("a:clrScheme");
	pNested1Tag->AddAttribute("name", "Office");
	pNested2Tag = pNested1Tag->AddTag("a:dk1");
	pNested3Tag = pNested2Tag->AddTag("a:sysClr");
	pNested3Tag->AddAttribute("val", "windowText");
	pNested3Tag->AddAttribute("lastClr", "000000");
	pNested2Tag = pNested1Tag->AddTag("a:lt1");
	pNested3Tag = pNested2Tag->AddTag("a:sysClr");
	pNested3Tag->AddAttribute("val", "window");
	pNested3Tag->AddAttribute("lastClr", "FFFFFF");
	pNested2Tag = pNested1Tag->AddTag("a:dk2");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "44546A");
	pNested2Tag = pNested1Tag->AddTag("a:lt2");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "E7E6E6");
	pNested2Tag = pNested1Tag->AddTag("a:accent1");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "4472C4");
	pNested2Tag = pNested1Tag->AddTag("a:accent2");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "ED7D31");
	pNested2Tag = pNested1Tag->AddTag("a:accent3");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "A5A5A5");
	pNested2Tag = pNested1Tag->AddTag("a:accent4");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "FFC000");
	pNested2Tag = pNested1Tag->AddTag("a:accent5");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "5B9BD5");
	pNested2Tag = pNested1Tag->AddTag("a:accent6");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "70AD47");
	pNested2Tag = pNested1Tag->AddTag("a:hlink");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "0563C1");
	pNested2Tag = pNested1Tag->AddTag("a:folHlink");
	pNested3Tag = pNested2Tag->AddTag("a:srgbClr");
	pNested3Tag->AddAttribute("val", "954F72");


	pNested1Tag = pSingleTag->AddTag("a:fontScheme");
	pNested1Tag->AddAttribute("name", "Office");

	pNested2Tag = pNested1Tag->AddTag("a:majorFont");
	pNested3Tag = pNested2Tag->AddTag("a:latin");
	pNested3Tag->AddAttribute("typeface", "Calibri Light");
	pNested3Tag->AddAttribute("panose", "020F0302020204030204");
	pNested3Tag = pNested2Tag->AddTag("a:ea");
	pNested3Tag->AddAttribute("typeface", "");
	pNested3Tag = pNested2Tag->AddTag("a:cs");
	pNested3Tag->AddAttribute("typeface", "");

	pNested3Tag = pNested2Tag->AddTag("a:font");
	pNested3Tag->AddAttribute("script", "Arab");
	pNested3Tag->AddAttribute("typeface", "Times New Roman");

	pNested3Tag = pNested2Tag->AddTag("a:font");
	pNested3Tag->AddAttribute("script", "Hebr");
	pNested3Tag->AddAttribute("typeface", "Times New Roman");

	pNested2Tag = pNested1Tag->AddTag("a:minorFont");
	pNested3Tag = pNested2Tag->AddTag("a:latin");
	pNested3Tag->AddAttribute("typeface", "Calibri");
	pNested3Tag->AddAttribute("panose", "020F0502020204030204");
	pNested3Tag = pNested2Tag->AddTag("a:ea");
	pNested3Tag->AddAttribute("typeface", "");
	pNested3Tag = pNested2Tag->AddTag("a:cs");
	pNested3Tag->AddAttribute("typeface", "");

	pNested3Tag = pNested2Tag->AddTag("a:font");
	pNested3Tag->AddAttribute("script", "Arab");
	pNested3Tag->AddAttribute("typeface", "Arial");

	pNested3Tag = pNested2Tag->AddTag("a:font");
	pNested3Tag->AddAttribute("script", "Hebr");
	pNested3Tag->AddAttribute("typeface", "Arial");


	pNested1Tag = pSingleTag->AddTag("a:fmtScheme");
	pNested1Tag->AddAttribute("name", "Office");
	pNested2Tag = pNested1Tag->AddTag("a:fillStyleLst");
	pNested3Tag = pNested2Tag->AddTag("a:solidFill");
	pNested4Tag = pNested3Tag->AddTag("a:schemeClr");
	pNested4Tag->AddAttribute("val", "phClr");

	pNested3Tag = pNested2Tag->AddTag("a:gradFill");
	pNested3Tag->AddAttribute("rotWithShape", "1");
	pNested4Tag = pNested3Tag->AddTag("a:gsLst");
	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "0");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "110000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "105000");
	pNested7Tag = pNested6Tag->AddTag("a:tint");
	pNested7Tag->AddAttribute("val", "67000");

	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "50000");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "105000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "103000");
	pNested7Tag = pNested6Tag->AddTag("a:tint");
	pNested7Tag->AddAttribute("val", "73000");

	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "100000");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "105000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "109000");
	pNested7Tag = pNested6Tag->AddTag("a:tint");
	pNested7Tag->AddAttribute("val", "81000");

	pNested4Tag = pNested3Tag->AddTag("a:lin");
	pNested4Tag->AddAttribute("ang", "5400000");
	pNested4Tag->AddAttribute("scaled", "0");

	pNested3Tag = pNested2Tag->AddTag("a:gradFill");
	pNested3Tag->AddAttribute("rotWithShape", "1");
	pNested4Tag = pNested3Tag->AddTag("a:gsLst");
	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "0");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "103000");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "102000");
	pNested7Tag = pNested6Tag->AddTag("a:tint");
	pNested7Tag->AddAttribute("val", "94000");

	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "50000");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "100000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "110000");
	pNested7Tag = pNested6Tag->AddTag("a:shade");
	pNested7Tag->AddAttribute("val", "10000");

	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "100000");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "99000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "120000");
	pNested7Tag = pNested6Tag->AddTag("a:shade");
	pNested7Tag->AddAttribute("val", "78000");

	pNested4Tag = pNested3Tag->AddTag("a:lin");
	pNested4Tag->AddAttribute("ang", "5400000");
	pNested4Tag->AddAttribute("scaled", "0");



	pNested2Tag = pNested1Tag->AddTag("a:lnStyleLst");

	pNested3Tag = pNested2Tag->AddTag("a:ln");
	pNested3Tag->AddAttribute("w", "6350");
	pNested3Tag->AddAttribute("cap", "flat");
	pNested3Tag->AddAttribute("cmpd", "sng");
	pNested3Tag->AddAttribute("algn", "ctr");
	pNested4Tag = pNested3Tag->AddTag("a:solidFill");
	pNested5Tag = pNested4Tag->AddTag("a:schemeClr");
	pNested5Tag->AddAttribute("val", "phClr");
	pNested4Tag = pNested3Tag->AddTag("a:prstDash");
	pNested4Tag->AddAttribute("val", "solid");
	pNested4Tag = pNested3Tag->AddTag("a:miter");
	pNested4Tag->AddAttribute("lim", "800000");

	pNested3Tag = pNested2Tag->AddTag("a:ln");
	pNested3Tag->AddAttribute("w", "12700");
	pNested3Tag->AddAttribute("cap", "flat");
	pNested3Tag->AddAttribute("cmpd", "sng");
	pNested3Tag->AddAttribute("algn", "ctr");
	pNested4Tag = pNested3Tag->AddTag("a:solidFill");
	pNested5Tag = pNested4Tag->AddTag("a:schemeClr");
	pNested5Tag->AddAttribute("val", "phClr");
	pNested4Tag = pNested3Tag->AddTag("a:prstDash");
	pNested4Tag->AddAttribute("val", "solid");
	pNested4Tag = pNested3Tag->AddTag("a:miter");
	pNested4Tag->AddAttribute("lim", "800000");

	pNested3Tag = pNested2Tag->AddTag("a:ln");
	pNested3Tag->AddAttribute("w", "19050");
	pNested3Tag->AddAttribute("cap", "flat");
	pNested3Tag->AddAttribute("cmpd", "sng");
	pNested3Tag->AddAttribute("algn", "ctr");
	pNested4Tag = pNested3Tag->AddTag("a:solidFill");
	pNested5Tag = pNested4Tag->AddTag("a:schemeClr");
	pNested5Tag->AddAttribute("val", "phClr");
	pNested4Tag = pNested3Tag->AddTag("a:prstDash");
	pNested4Tag->AddAttribute("val", "solid");
	pNested4Tag = pNested3Tag->AddTag("a:miter");
	pNested4Tag->AddAttribute("lim", "800000");

	pNested2Tag = pNested1Tag->AddTag("a:effectStyleLst");
	pNested3Tag = pNested2Tag->AddTag("a:effectStyle");
	pNested4Tag = pNested3Tag->AddTag("a:effectLst");
	pNested3Tag = pNested2Tag->AddTag("a:effectStyle");
	pNested4Tag = pNested3Tag->AddTag("a:effectLst");

	pNested3Tag = pNested2Tag->AddTag("a:effectStyle");
	pNested4Tag = pNested3Tag->AddTag("a:effectLst");

	pNested5Tag = pNested4Tag->AddTag("a:outerShdw");
	pNested5Tag->AddAttribute("blurRad", "57150");
	pNested5Tag->AddAttribute("dist", "19050");
	pNested5Tag->AddAttribute("dir", "5400000");
	pNested5Tag->AddAttribute("algn", "ctr");
	pNested5Tag->AddAttribute("rotWithShape", "0");

	pNested6Tag = pNested5Tag->AddTag("a:srgbClr");
	pNested6Tag->AddAttribute("val", "000000");

	pNested7Tag = pNested6Tag->AddTag("a:alpha");
	pNested7Tag->AddAttribute("val", "63000");

	pNested2Tag = pNested1Tag->AddTag("a:bgFillStyleLst");
	pNested3Tag = pNested2Tag->AddTag("a:solidFill");
	pNested4Tag = pNested3Tag->AddTag("a:schemeClr");
	pNested4Tag->AddAttribute("val", "phClr");

	pNested3Tag = pNested2Tag->AddTag("a:solidFill");
	pNested4Tag = pNested3Tag->AddTag("a:schemeClr");
	pNested4Tag->AddAttribute("val", "phClr");
	pNested5Tag = pNested4Tag->AddTag("a:tint");
	pNested5Tag->AddAttribute("val", "95000");
	pNested5Tag = pNested4Tag->AddTag("a:satMod");
	pNested5Tag->AddAttribute("val", "170000");

	pNested3Tag = pNested2Tag->AddTag("a:gradFill");
	pNested3Tag->AddAttribute("rotWithShape", "1");

	pNested4Tag = pNested3Tag->AddTag("a:gsLst");
	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "0");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:tint");
	pNested7Tag->AddAttribute("val", "93000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "150000");
	pNested7Tag = pNested6Tag->AddTag("a:shade");
	pNested7Tag->AddAttribute("val", "98000");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "102000");

	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "50000");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:tint");
	pNested7Tag->AddAttribute("val", "98000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "130000");
	pNested7Tag = pNested6Tag->AddTag("a:shade");
	pNested7Tag->AddAttribute("val", "90000");
	pNested7Tag = pNested6Tag->AddTag("a:lumMod");
	pNested7Tag->AddAttribute("val", "103000");

	pNested5Tag = pNested4Tag->AddTag("a:gs");
	pNested5Tag->AddAttribute("pos", "100000");
	pNested6Tag = pNested5Tag->AddTag("a:schemeClr");
	pNested6Tag->AddAttribute("val", "phClr");
	pNested7Tag = pNested6Tag->AddTag("a:shade");
	pNested7Tag->AddAttribute("val", "63000");
	pNested7Tag = pNested6Tag->AddTag("a:satMod");
	pNested7Tag->AddAttribute("val", "120000");

	pNested4Tag = pNested3Tag->AddTag("a:lin");
	pNested4Tag->AddAttribute("ang", "5400000");
	pNested4Tag->AddAttribute("scaled", "0");

	pSingleTag = mainTag->AddTag("a:objectDefaults");
	pSingleTag = mainTag->AddTag("a:extraClrSchemeLst");

	pSingleTag = mainTag->AddTag("a:extLst");
	pNested1Tag = pSingleTag->AddTag("a:ext");
	pNested1Tag->AddAttribute("uri", "{05A4C25C-085E-4340-85A3-A5531E510DB2}");

	pNested2Tag = pNested1Tag->AddTag("thm15:themeFamily");
	pNested2Tag->AddAttribute("xmlns:thm15", "http://schemas.microsoft.com/office/thememl/2012/main");
	pNested2Tag->AddAttribute("name", "Office Theme");
	pNested2Tag->AddAttribute("id", "{62F939B6-93AF-4DB8-9C6B-D6C7DFDC589F}");
	pNested2Tag->AddAttribute("vid", "{4A3C46E8-61CC-4603-A589-7422A47A8E4A}");
}
