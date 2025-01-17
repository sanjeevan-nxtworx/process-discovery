#include "stdafx.h"
#include "Font.h"


Font::Font(string name):DocXTag("w:font")
{
	AddAttribute("w:name", name);
	psig = NULL;
}



Font::~Font()
{
}

void Font::SetCharset(string val)
{
	DocXTag *pTag = AddTag("w:charset");
	pTag->AddAttribute("w:val", "00");

}

void Font::SetSig()
{
	if (psig == NULL)
	{
		psig = AddTag("w:sig");
	}
}

void Font::SetFamily(string val)
{
	DocXTag *pTag = AddTag("w:family");
	pTag->AddAttribute("w:val", val);
}

void Font::SetPitch(string val)
{
	DocXTag *pTag = AddTag("w:pitch");
	pTag->AddAttribute("w:val", val);
}

void Font::SetPanose1(string val)
{
	DocXTag *pTag = AddTag("w:panose1");
	pTag->AddAttribute("w:val", val);
}

void Font::SetSigcsb(string val0, string val1)
{
	SetSig();
	psig->AddAttribute("w:csb1", val1);
	psig->AddAttribute("w:csb0", val0);
}

void Font::SetSigusb(string val0, string val1, string val2, string val3)
{
	SetSig();
	psig->AddAttribute("w:usb0", val0);
	psig->AddAttribute("w:usb1", val1);
	psig->AddAttribute("w:usb2", val2);
	psig->AddAttribute("w:usb3", val3);
}

