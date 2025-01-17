#include "stdafx.h"
#include "MaskImagePanel.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(MaskImagePanel, wxScrolledWindow)
// some useful events
	EVT_MOUSE_EVENTS(MaskImagePanel::mouseEvents)
 // catch paint events
	EVT_PAINT(MaskImagePanel::paintEvent)

	END_EVENT_TABLE()


	// some useful events
	/*
	 void MaskImagePanel::mouseMoved(wxMouseEvent& event) {}
	 void MaskImagePanel::mouseDown(wxMouseEvent& event) {}
	 void MaskImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
	 void MaskImagePanel::mouseReleased(wxMouseEvent& event) {}
	 void MaskImagePanel::rightClick(wxMouseEvent& event) {}
	 void MaskImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
	 void MaskImagePanel::keyPressed(wxKeyEvent& event) {}
	 void MaskImagePanel::keyReleased(wxKeyEvent& event) {}
	 */

void MaskImagePanel::mouseEvents(wxMouseEvent& event) 
{
	static wxPoint startPt;

	if (event.LeftDown())
	{
		event.GetPosition(&startPt.x, &startPt.y);
		point2.x = 0;
		point2.y = 0;
		point1 = point2;
		paintNow();
	}

	if (event.Dragging())
	{
		point1 = startPt;
		event.GetPosition(&point2.x, &point2.y);

		paintNow();
	}

}


MaskImagePanel::MaskImagePanel(wxWindow * parent, wxPoint pt, wxSize size) :
	wxScrolledWindow(parent, -1, pt, size, wxNO_BORDER)
{
	//bmpVal = NULL;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));
	maskImage = NULL;
}

MaskImagePanel::~MaskImagePanel()
{
	//bmpVal = NULL;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));
	if (maskImage != NULL)
		HeapFree(GetProcessHeap(), 0, maskImage);
}

void MaskImagePanel::ConvertImageTo32()
{
	if (bmi.bmiHeader.biBitCount != 24)
		return;

	int width = bmi.bmiHeader.biWidth;
	int height = bmi.bmiHeader.biHeight;
	if (height < 0)
		height = -height;
	size_t imageSize = width * height * 32;

	typedef union RGB
	{
		uint32_t Color;
		struct
		{
			unsigned char B, G, R, A;
		} RGBA;
	} *PRGB;

	PRGB newImageBuffer = (PRGB)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, imageSize);


	char *oldImagePtr = maskImage;
	bmi.bmiHeader.biBitCount = 32;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			newImageBuffer[(height - 1 - h) * width + w].RGBA.B = *(oldImagePtr++);
			newImageBuffer[(height - 1 - h) * width + w].RGBA.G = *(oldImagePtr++);
			newImageBuffer[(height - 1 - h) * width + w].RGBA.R = *(oldImagePtr++);
			newImageBuffer[(height - 1 - h) * width + w].RGBA.A = 0xff;
		}
		oldImagePtr += width % 4;
	}
	HeapFree(GetProcessHeap(), 0, maskImage);

	maskImage = (char *)newImageBuffer;
}

size_t MaskImagePanel::GetBitImageSize()
{
	int width = bmi.bmiHeader.biWidth;
	int height = bmi.bmiHeader.biHeight;
	if (height < 0)
		height = -height;
	size_t imageSize = width * height * bmi.bmiHeader.biBitCount / 8;
	return imageSize;
}

char *MaskImagePanel::SaveHeaderAndGetBuffer(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader)
{
	CopyMemory((char *)&bfh, pBfh, sizeof(BITMAPFILEHEADER));
	CopyMemory((char *)&bmi.bmiHeader, pBmiHeader, sizeof(BITMAPINFOHEADER));
	int width = bmi.bmiHeader.biWidth;
	int height = bmi.bmiHeader.biHeight;
	if (height < 0)
		height = -height;
	size_t imageSize = width * height * bmi.bmiHeader.biBitCount / 8;
	if (maskImage != NULL)
		HeapFree(GetProcessHeap(), 0, maskImage);
	maskImage = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, imageSize);
	return maskImage;

}



void  MaskImagePanel::SetMaskImageRect(RECT rect)
{
	CopyMemory(&maskRect, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

void  MaskImagePanel::SetMatchImage1Rect(RECT rect)
{
	CopyMemory(&image1, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

void  MaskImagePanel::SetMatchImage2Rect(RECT rect)
{
	CopyMemory(&image2, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

void  MaskImagePanel::SetMatchImage3Rect(RECT rect)
{
	CopyMemory(&image3, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

void  MaskImagePanel::SetMatchImage4Rect(RECT rect)
{
	CopyMemory(&image4, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

void  MaskImagePanel::SetMatchImage5Rect(RECT rect)
{
	CopyMemory(&image5, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

void  MaskImagePanel::SetMatchImage6Rect(RECT rect)
{
	CopyMemory(&image6, &rect, sizeof(RECT));
	point2.x = 0;
	point2.y = 0;
	point1 = point2;
	paintNow();
}

RECT MaskImagePanel::GetImageRect()
{
	int width = bmi.bmiHeader.biWidth;
	int height = bmi.bmiHeader.biHeight;
	if (height < 0)
		height = -height;
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = width;
	rect.bottom = height;

	return rect;
}

RECT MaskImagePanel::GetCurrentRect()
{
	int topX, topY, botX, botY;

	topX = point1.x;
	topY = point1.y;
	botX = point2.x;
	botY = point2.y;
	if (point1.x > point2.x)
	{
		topX = point2.x;
		botX = point1.x;
	}
	if (point1.y > point2.y)
	{
		topY = point2.y;
		botY = point1.y;
	}

	RECT curRect;
	int viewX, viewY;
	GetViewStart(&viewX, &viewY);
	curRect.top = topY + viewY;
	curRect.bottom = botY + viewY;
	curRect.left = topX + viewX;
	curRect.right = botX + viewX;

	return curRect;

}

void MaskImagePanel::ClearImage()
{
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));
	if (maskImage != NULL)
		HeapFree(GetProcessHeap(), 0, maskImage);
	maskImage = NULL;
}

void MaskImagePanel::SetImage()
{
	int width = bmi.bmiHeader.biWidth;
	int height = bmi.bmiHeader.biHeight;
	if (height < 0)
		height = -height;

	/* init scrolled area size, scrolling speed, etc. */
	SetScrollbars(1, 1, width, height, 0, 0);
	maskRect.left = maskRect.top = 0;
	maskRect.right = 0;
	maskRect.bottom = 0;

	CopyMemory(&image1, &maskRect, sizeof(RECT));
	CopyMemory(&image2, &maskRect, sizeof(RECT));
	CopyMemory(&image3, &maskRect, sizeof(RECT));
	CopyMemory(&image4, &maskRect, sizeof(RECT));
	CopyMemory(&image5, &maskRect, sizeof(RECT));
	CopyMemory(&image6, &maskRect, sizeof(RECT));
	Update();
	Refresh();
}



//void MaskImagePanel::SetImage(wxImage *pImage)
//{
//	imageVal = *pImage;
//	if (bmpVal != NULL)
//		delete bmpVal;
//	bmpVal = new wxBitmap(imageVal);
//
//	/* init scrolled area size, scrolling speed, etc. */
//	SetScrollbars(1, 1, pImage->GetWidth(), pImage->GetHeight(), 0, 0);
//	maskRect.x = maskRect.y = 0;
//	maskRect.SetWidth(0);
//	maskRect.SetHeight(0);
//	image1 = maskRect;
//	image2 = maskRect;
//	image3 = maskRect;
//	image4 = maskRect;
//	image5 = maskRect;
//	image6 = maskRect;
//
//	Update();
//	Refresh();
//}
//
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void MaskImagePanel::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
	DoPrepareDC(dc);
	render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void MaskImagePanel::paintNow()
{
	// depending on your system you may need to look at double-buffered dcs
	wxClientDC dc(this);
	DoPrepareDC(dc);
	render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
//void MaskImagePanel::render(wxDC&  dc)
//{
//	if (imageVal.IsOk())
//	{
//		dc.DrawBitmap(*bmpVal, 0, 0, false);
//
//		wxPen imagePen(wxColour(0xff, 0, 0), 2, wxPENSTYLE_SOLID);
//		wxPen maskPen(wxColour(0, 0xff, 0), 2, wxPENSTYLE_SOLID);
//		wxPen currRectPen(wxColour(0, 0, 0xff), 2, wxPENSTYLE_SOLID);
//		dc.SetBrush(*wxTRANSPARENT_BRUSH);
//		dc.SetPen(imagePen);
//		dc.DrawRectangle(image1);
//		dc.DrawRectangle(image2);
//		dc.DrawRectangle(image3);
//		dc.DrawRectangle(image4);
//		dc.DrawRectangle(image5);
//		dc.DrawRectangle(image6);
//
//		dc.SetPen(maskPen);
//		dc.DrawRectangle(maskRect);
//
//		int topX, topY, botX, botY;
//
//		topX = point1.x;
//		topY = point1.y;
//		botX = point2.x;
//		botY = point2.y;
//		if (point1.x > point2.x)
//		{
//			topX = point2.x;
//			botX = point1.x;
//		}
//		if (point1.y > point2.y)
//		{
//			topY = point2.y;
//			botY = point1.y;
//		}
//
//		wxRect curRect;
//		int viewX, viewY;
//		GetViewStart(&viewX, &viewY);
//		curRect.SetTop(topY + viewY);
//		curRect.SetBottom(botY + viewY);
//		curRect.SetLeft(topX + viewX);
//		curRect.SetRight(botX + viewX);
//
//		if (curRect.GetTop() > bmpVal->GetHeight() || curRect.GetRight() > bmpVal->GetWidth())
//		{
//			curRect.SetTop(0);
//			curRect.SetLeft(0);
//			curRect.SetBottom(0);
//			curRect.SetRight(0);
//		}
//
//		if (curRect.GetBottom() > bmpVal->GetHeight())
//			curRect.SetBottom(bmpVal->GetHeight());
//		if (curRect.GetRight() > bmpVal->GetWidth())
//			curRect.SetRight(bmpVal->GetWidth());
//
//		dc.SetPen(currRectPen);
//		dc.DrawRectangle(curRect);
//	}
//
//}


void MaskImagePanel::render(wxDC&  dc)
{
	if (maskImage != NULL)
	{
		BITMAPINFO bmInfo;
		HDC paintDC = dc.GetHDC();

		int width = bmi.bmiHeader.biWidth;
		int height = bmi.bmiHeader.biHeight;
		if (height < 0)
			height = -height;
		ZeroMemory(&bmInfo, sizeof(bmInfo));
		HDC backhdc = CreateCompatibleDC(NULL);
		HBITMAP backbmp = CreateBitmap(width, height, 1, bmi.bmiHeader.biBitCount, NULL);
		HBITMAP oldbmp = (HBITMAP)SelectObject(backhdc, backbmp);
		BitBlt(backhdc, 0, 0, width, height, 0, 0, 0, BLACKNESS); // Clears the screen
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = width;
		bmInfo.bmiHeader.biHeight = -height;
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biBitCount = bmi.bmiHeader.biBitCount;
		bmInfo.bmiHeader.biCompression = BI_RGB;
		bmInfo.bmiHeader.biSizeImage = 0;
		bmInfo.bmiHeader.biYPelsPerMeter = 0;
		bmInfo.bmiHeader.biXPelsPerMeter = 0;
		bmInfo.bmiHeader.biClrUsed = 0;
		bmInfo.bmiHeader.biClrImportant = 0;
		SetStretchBltMode(backhdc, COLORONCOLOR);
		StretchDIBits(backhdc, 0, 0, width, height, 0, 0, width, height, maskImage, &bmInfo, DIB_RGB_COLORS, SRCCOPY);
		BitBlt(paintDC, 0, 0, width, height, backhdc, 0, 0, SRCCOPY);

		wxPen imagePen(wxColour(0xff, 0, 0), 2, wxPENSTYLE_SOLID);
		wxPen maskPen(wxColour(0, 0xff, 0), 2, wxPENSTYLE_SOLID);
		wxPen currRectPen(wxColour(0, 0, 0xff), 2, wxPENSTYLE_SOLID);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(imagePen);
		dc.DrawRectangle(wxRect(image1.left, image1.top, image1.right - image1.left, image1.bottom - image1.top));
		dc.DrawRectangle(wxRect(image2.left, image2.top, image2.right - image2.left, image2.bottom - image2.top));
		dc.DrawRectangle(wxRect(image3.left, image3.top, image3.right - image3.left, image3.bottom - image3.top));
		dc.DrawRectangle(wxRect(image4.left, image4.top, image4.right - image4.left, image4.bottom - image4.top));
		dc.DrawRectangle(wxRect(image5.left, image5.top, image5.right - image5.left, image5.bottom - image5.top));
		dc.DrawRectangle(wxRect(image6.left, image6.top, image6.right - image6.left, image6.bottom - image6.top));

		dc.SetPen(maskPen);
		dc.DrawRectangle(wxRect(maskRect.left, maskRect.top, maskRect.right - maskRect.left, maskRect.bottom - maskRect.top));

		int topX, topY, botX, botY;

		topX = point1.x;
		topY = point1.y;
		botX = point2.x;
		botY = point2.y;
		if (point1.x > point2.x)
		{
			topX = point2.x;
			botX = point1.x;
		}
		if (point1.y > point2.y)
		{
			topY = point2.y;
			botY = point1.y;
		}

		wxRect curRect;
		int viewX, viewY;
		GetViewStart(&viewX, &viewY);
		curRect.SetTop(topY + viewY);
		curRect.SetBottom(botY + viewY);
		curRect.SetLeft(topX + viewX);
		curRect.SetRight(botX + viewX);

		if (curRect.GetTop() > height || curRect.GetRight() > width)
		{
			curRect.SetTop(0);
			curRect.SetLeft(0);
			curRect.SetBottom(0);
			curRect.SetRight(0);
		}

		if (curRect.GetBottom() > height)
			curRect.SetBottom(height);
		if (curRect.GetRight() > width)
			curRect.SetRight(width);

		dc.SetPen(currRectPen);
		dc.DrawRectangle(curRect);
		(HBITMAP)SelectObject(backhdc, oldbmp);
	}

}


RECT MaskImagePanel::GetMaskImageRect()
{
	return maskRect;
}

RECT MaskImagePanel::GetMatchImage1Rect()
{
	return image1;
}

RECT MaskImagePanel::GetMatchImage2Rect()
{
	return image2;
}

RECT MaskImagePanel::GetMatchImage3Rect()
{
	return image3;
}

RECT MaskImagePanel::GetMatchImage4Rect()
{
	return image4;
}

RECT MaskImagePanel::GetMatchImage5Rect()
{
	return image5;
}

RECT MaskImagePanel::GetMatchImage6Rect()
{
	return image6;
}

char *MaskImagePanel::GetImage(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader)
{
	ZeroMemory(pBfh, sizeof(BITMAPFILEHEADER));
	ZeroMemory(pBmiHeader, sizeof(BITMAPINFOHEADER));

	if (maskImage != NULL)
	{
		CopyMemory(pBfh, &bfh, sizeof(BITMAPFILEHEADER));
		CopyMemory(pBmiHeader, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));

		int width = bmi.bmiHeader.biWidth;
		int height = bmi.bmiHeader.biHeight;
		if (height < 0)
			height = -height;
		size_t imageSize = width * height * bmi.bmiHeader.biBitCount / 8;
		char *maskBuffer = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, imageSize);
		CopyMemory(maskBuffer, maskImage, imageSize);
		return maskBuffer;
	}
	return NULL;
}