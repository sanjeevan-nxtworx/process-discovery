#include "stdafx.h"
#include "PictureDisplay.h"

// Event table for MyFrame
BEGIN_EVENT_TABLE(PictureDisplay, wxPanel)
EVT_PAINT(PictureDisplay::paintEvent)
END_EVENT_TABLE()

PictureDisplay::PictureDisplay(wxPanel * parent, wxWindowID id, const wxPoint & position, const wxSize & size) :
	wxPanel(parent, id, position, size, wxNO_BORDER)
{
	pResizeBuffer = NULL;
	pThumbBuffer = NULL;
	thumbXPos = 0;
	bShowAnnotation = false;
}


PictureDisplay::~PictureDisplay()
{
}


void PictureDisplay::SetShowSubTitle(bool bFlg)
{
	bShowAnnotation = bFlg;
}
void PictureDisplay::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void PictureDisplay::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

struct ResizeScreenBuffer **PictureDisplay::LockBitmap()
{
	lockBitmap.lock();
	return &pResizeBuffer;
}

struct ResizeScreenBuffer **PictureDisplay::GetThumbBuffer()
{
	return &pThumbBuffer;
}

void PictureDisplay::UnlockBitmap()
{
	lockBitmap.unlock();
}

void PictureDisplay::SetAnnotation(string value)
{
	strAnnotation = value;
}

void PictureDisplay::SetNotes(string value)
{
	strNotes = value;
}

void PictureDisplay::SetThumbXPos(int pos)
{
	thumbXPos = pos;
}

void PictureDisplay::render(wxDC&  dc)
{
	if (pResizeBuffer == NULL && pThumbBuffer == NULL)
		return;
	lockBitmap.lock();
	BITMAPINFO bmInfo;
	HDC paintDC = dc.GetHDC();
	wxSize sizeWnd = GetSize();
	ZeroMemory(&bmInfo, sizeof(bmInfo));
	HDC backhdc = CreateCompatibleDC(NULL);
	HBITMAP backbmp = CreateBitmap(sizeWnd.GetWidth(), sizeWnd.GetHeight(), 1, 32, NULL);
	HBITMAP oldbmp = (HBITMAP)SelectObject(backhdc, backbmp);
	BitBlt(backhdc, 0, 0, sizeWnd.GetWidth(), sizeWnd.GetHeight(), 0, 0, 0, BLACKNESS); // Clears the screen
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = pResizeBuffer->width;
	bmInfo.bmiHeader.biHeight = -pResizeBuffer->height;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biYPelsPerMeter = 0;
	bmInfo.bmiHeader.biXPelsPerMeter = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biClrImportant = 0;
	SetStretchBltMode(backhdc, COLORONCOLOR);
	if (pResizeBuffer != NULL)
		StretchDIBits(backhdc, 0, 0, pResizeBuffer->width, pResizeBuffer->height, 0, 0, pResizeBuffer->width, pResizeBuffer->height, pResizeBuffer->buffer, &bmInfo, DIB_RGB_COLORS, SRCCOPY);

	HFONT hFont = NULL;
	HFONT hFontOld = NULL;
	if (bShowAnnotation)
	{
		RECT rect;
		rect.left = 20;
		rect.right = sizeWnd.GetWidth() - 20;
		rect.top = sizeWnd.GetHeight() - 50;
		rect.bottom = sizeWnd.GetHeight() - 10;
		hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));
		hFontOld = (HFONT)SelectObject(backhdc, hFont);
		SetTextColor(backhdc, RGB(255, 0, 0));
		wstring wideString = wstring(strAnnotation.begin(), strAnnotation.end());
		DrawText(backhdc, wideString.c_str(), -1, &rect, DT_CENTER | DT_WORDBREAK);
	}
	if (pThumbBuffer != NULL)
	{
		int thumbX = thumbXPos - 100;
		if (thumbX + 200 > sizeWnd.GetWidth())
		{
			thumbX = sizeWnd.GetWidth() - 200;
		}
		else if (thumbX < 0)
		{
			thumbX = 0;
		}

		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = pThumbBuffer->width;
		bmInfo.bmiHeader.biHeight = -pThumbBuffer->height;
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biBitCount = 32;
		bmInfo.bmiHeader.biCompression = BI_RGB;
		bmInfo.bmiHeader.biSizeImage = 0;
		bmInfo.bmiHeader.biYPelsPerMeter = 0;
		bmInfo.bmiHeader.biXPelsPerMeter = 0;
		bmInfo.bmiHeader.biClrUsed = 0;
		bmInfo.bmiHeader.biClrImportant = 0;
		SetStretchBltMode(backhdc, COLORONCOLOR);

		StretchDIBits(backhdc, 
			          thumbX, sizeWnd.GetHeight()-pThumbBuffer->height-1, pThumbBuffer->width, pThumbBuffer->height,
			          0, 0, pThumbBuffer->width, pThumbBuffer->height, 
			          pThumbBuffer->buffer, &bmInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	BitBlt(paintDC, 0, 0, sizeWnd.GetWidth(), sizeWnd.GetHeight(), backhdc, 0, 0, SRCCOPY);
	lockBitmap.unlock();

	SelectObject(backhdc, oldbmp);
	if (bShowAnnotation)
	{
		SelectObject(backhdc, hFontOld);
		DeleteObject(hFont);
	}
	DeleteObject(backbmp);
	DeleteDC(backhdc);
}

