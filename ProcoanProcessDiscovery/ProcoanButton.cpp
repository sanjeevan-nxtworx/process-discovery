#include "stdafx.h"
#include "ProcoanButton.h"
#include "CustomEvents.h"

// this is a definition so can't be in a header
wxDEFINE_EVENT(PROCOAN_BUTTON_DOWN, wxCommandEvent);


BEGIN_EVENT_TABLE(ProcoanButton, wxWindow)
EVT_MOTION(ProcoanButton::mouseMoved)
EVT_LEFT_DOWN(ProcoanButton::mouseDown)
EVT_LEFT_UP(ProcoanButton::mouseReleased)
EVT_RIGHT_DOWN(ProcoanButton::rightClick)
EVT_LEAVE_WINDOW(ProcoanButton::mouseLeftWindow)
EVT_KEY_DOWN(ProcoanButton::keyPressed)
EVT_KEY_UP(ProcoanButton::keyReleased)
EVT_MOUSEWHEEL(ProcoanButton::mouseWheelMoved)
// catch paint events
EVT_PAINT(ProcoanButton::paintEvent)
END_EVENT_TABLE()


ProcoanButton::ProcoanButton(wxWindow *parent,
	wxWindowID winid,
	const wxPoint& pos = wxDefaultPosition,
	const wxSize& size = wxDefaultSize
	)
{
	pParent = parent;
	buttonID = winid;
	Create(parent, winid, pos, size, wxBORDER_NONE);
	buttonWidth = size.GetWidth();
	buttonHeight = size.GetHeight();
	this->text = text;
	pressedDown = false;
	pNormalImage = NULL;
	pActiveImage = NULL;
	pPressedImage = NULL;
	pDisabledImage = NULL;
	pSmallNormalImage = NULL;
	pSmallActiveImage = NULL;
	pSmallPressedImage = NULL;
	pSmallDisabledImage = NULL;
	buttonStatus = 0;
	bDisabled = false;
	pNormalImage = pActiveImage = pPressedImage = pDisabledImage = NULL;
	bShowSmall = false;
}

ProcoanButton::~ProcoanButton()
{
	if (pNormalImage)
		delete pNormalImage;
	if (pActiveImage)
		delete pActiveImage;
	if (pPressedImage)
		delete pPressedImage;
	if (pDisabledImage)
		delete pDisabledImage;
	if (pSmallNormalImage)
		delete pSmallNormalImage;
	if (pSmallActiveImage)
		delete pSmallActiveImage;
	if (pSmallPressedImage)
		delete pSmallPressedImage;
	if (pSmallDisabledImage)
		delete pSmallDisabledImage;

}

void ProcoanButton::SetShowSmall(bool flg)
{
	bShowSmall = flg;
}

void ProcoanButton::ShowEnable(bool bFlg)
{
	Enable(bFlg);
	//paintNow();
	Refresh();
}

void ProcoanButton::SetButtonPosition(wxPoint pos)
{
	SetPosition(pos);
	//paintNow();
	Refresh();
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void ProcoanButton::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
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
void ProcoanButton::paintNow()
{
	// depending on your system you may need to look at double-buffered dcs
	wxClientDC dc(this);
	render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void ProcoanButton::render(wxDC&  dc)
{
	wxImage *pImageToDisplay = NULL;
	bDisabled = !IsEnabled();
	//int sizeButtonWidth = GetSize().GetWidth();

	//int sizeSmallWidth = pSmallDisabledImage->GetSize().GetWidth();
	//int sizeBigWidth = pDisabledImage->GetSize().GetWidth();

	//int smallDiff = 0;
	//int bigDiff = 0;

	//if (sizeSmallWidth > sizeButtonWidth)
	//	smallDiff = sizeSmallWidth - sizeButtonWidth;
	//else
	//	smallDiff = sizeButtonWidth - sizeSmallWidth;

	//if (sizeBigWidth > sizeButtonWidth)
	//	bigDiff = sizeBigWidth - sizeButtonWidth;
	//else
	//	bigDiff = sizeButtonWidth - sizeBigWidth;

	//if (bigDiff > smallDiff)
	//	bShowSmall = true;
	//else
	//	bShowSmall = false;
	
	if (bShowSmall)
	{
		if (bDisabled && pSmallDisabledImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pSmallDisabledImage);
		}
		else if (buttonStatus == 1 && pSmallActiveImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pSmallActiveImage);
		}
		else if (buttonStatus == 2 && pSmallPressedImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pSmallPressedImage);
		}
		else if (pSmallNormalImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pSmallNormalImage);
		}

	}
	else
	{
		if (bDisabled && pDisabledImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pDisabledImage);
		}
		else if (buttonStatus == 1 && pActiveImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pActiveImage);
		}
		else if (buttonStatus == 2 && pPressedImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pPressedImage);
		}
		else if (pNormalImage != NULL)
		{
			pImageToDisplay = DBG_NEW wxImage(*pNormalImage);
		}
	}

	buttonWidth = GetSize().GetWidth();
	buttonHeight = GetSize().GetHeight();
	if (pImageToDisplay != NULL)
	{
		pImageToDisplay->Rescale(buttonWidth, buttonHeight);
		pImageToDisplay->ResampleBicubic(buttonWidth, buttonHeight);
		wxBitmap newBitmap(*pImageToDisplay);

		HBITMAP hBitmap = newBitmap.GetHBITMAP();
		HDC hdcMemDC = CreateCompatibleDC(dc.GetHDC());
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMemDC, hBitmap);
		BitBlt(dc.GetHDC(), 0, 0, buttonWidth, buttonHeight, hdcMemDC, 0, 0, SRCCOPY);
		SelectObject(hdcMemDC, oldBitmap);
		DeleteDC(hdcMemDC);
		//dc.DrawBitmap(newBitmap, wxPoint(0, 0));
	}
	if (pImageToDisplay != NULL)
		delete pImageToDisplay;
}

void ProcoanButton::SetDisabled(bool bDisabledFlg)
{
	bDisabled = bDisabledFlg;
	if (bDisabled)
		Disable();
	else
		Enable();
	//paintNow();
	Refresh();
}

void ProcoanButton::mouseDown(wxMouseEvent& WXUNUSED(event))
{
	buttonStatus = 2;
	pressedDown = true;
	//paintNow();
	Refresh();
}

void ProcoanButton::mouseReleased(wxMouseEvent& WXUNUSED(event))
{
	buttonStatus = 1;
	pressedDown = false;
	//paintNow();
	Refresh();
	
	wxCommandEvent customEvent(PROCOAN_BUTTON_DOWN, GetId());
	customEvent.SetEventObject(this);

	// Do send it
	ProcessWindowEvent(customEvent);
}


void ProcoanButton::mouseLeftWindow(wxMouseEvent& WXUNUSED(event))
{
	buttonStatus = 0;
	if (pressedDown)
	{
		pressedDown = false;
	}
	//paintNow();
	Refresh();
}

// currently unused events
void ProcoanButton::mouseMoved(wxMouseEvent& WXUNUSED(event))
{
	buttonStatus = 1;
	//paintNow();
	Refresh();
}

void ProcoanButton::mouseWheelMoved(wxMouseEvent& WXUNUSED(event)) {}
void ProcoanButton::rightClick(wxMouseEvent& WXUNUSED(event)) {}
void ProcoanButton::keyPressed(wxKeyEvent& WXUNUSED(event)) {}
void ProcoanButton::keyReleased(wxKeyEvent& WXUNUSED(event)) {}

void ProcoanButton::SetNormalBitmap(const char *const *xpm_data)
{
	if (pNormalImage != NULL)
		delete pNormalImage;

	pNormalImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetActiveBitmap(const char *const *xpm_data)
{
	if (pActiveImage != NULL)
		delete pActiveImage;

	pActiveImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetDisabledBitmap(const char *const *xpm_data)
{
	if (pDisabledImage != NULL)
		delete pDisabledImage;

	pDisabledImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetPressedBitmap(const char *const *xpm_data)
{
	if (pPressedImage != NULL)
		delete pPressedImage;

	pPressedImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetNormalSmallBitmap(const char *const *xpm_data)
{
	if (pSmallNormalImage != NULL)
		delete pSmallNormalImage;

	pSmallNormalImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetActiveSmallBitmap(const char *const *xpm_data)
{
	if (pSmallActiveImage != NULL)
		delete pSmallActiveImage;

	pSmallActiveImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetDisabledSmallBitmap(const char *const *xpm_data)
{
	if (pSmallDisabledImage != NULL)
		delete pSmallDisabledImage;

	pSmallDisabledImage = DBG_NEW wxImage(xpm_data);
}

void ProcoanButton::SetPressedSmallBitmap(const char *const *xpm_data)
{
	if (pSmallPressedImage != NULL)
		delete pSmallPressedImage;

	pSmallPressedImage = DBG_NEW wxImage(xpm_data);
}




void ProcoanButton::SetNormalBitmap(wxBitmap *pBitmap)
{
	if (pNormalImage != NULL)
		delete pNormalImage;

	pNormalImage = DBG_NEW wxImage(pBitmap->ConvertToImage());
}

void ProcoanButton::SetActiveBitmap(wxBitmap *pBitmap)
{
	if (pActiveImage != NULL)
		delete pActiveImage;

	pActiveImage = DBG_NEW wxImage(pBitmap->ConvertToImage());
}

void ProcoanButton::SetDisabledBitmap(wxBitmap *pBitmap)
{
	if (pDisabledImage != NULL)
		delete pDisabledImage;

	pDisabledImage = DBG_NEW wxImage(pBitmap->ConvertToImage());
}

void ProcoanButton::SetPressedBitmap(wxBitmap *pBitmap)
{
	if (pPressedImage != NULL)
		delete pPressedImage;

	pPressedImage = DBG_NEW wxImage(pBitmap->ConvertToImage());
}

