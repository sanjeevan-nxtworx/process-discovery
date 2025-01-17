#include "wxPecanButton.h"

BEGIN_EVENT_TABLE(wxPecanButton, wxButton)

//EVT_MOTION(wxPecanButton::mouseMoved)
//EVT_LEFT_DOWN(wxPecanButton::mouseDown)
//EVT_LEFT_UP(wxPecanButton::mouseReleased)
//EVT_RIGHT_DOWN(wxPecanButton::rightClick)
//EVT_LEAVE_WINDOW(wxPecanButton::mouseLeftWindow)
//EVT_KEY_DOWN(wxPecanButton::keyPressed)
//EVT_KEY_UP(wxPecanButton::keyReleased)
//EVT_MOUSEWHEEL(wxPecanButton::mouseWheelMoved)
//
//// catch paint events
//EVT_PAINT(wxPecanButton::paintEvent)

END_EVENT_TABLE()

wxPecanButton::wxPecanButton(wxWindow *parent, wxWindowID id, const wxString &label, const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator, const wxString &name)
	: wxButton(parent, id, label,pos, size,style, validator,name)
{
}

void wxPecanButton::SetBitmapCurrent(wxBitmap bmpValue)
{
	normalBitmap = bmpValue;
}
void wxPecanButton::SetBitmapDisabled(wxBitmap bmpValue)
{
	disabledBitmap = bmpValue;
}

void wxPecanButton::SetBitmapPressed(wxBitmap bmpValue)
{
	pressedBitmap = bmpValue;
}

void wxPecanButton::SetBitmapFocus(wxBitmap bmpValue)
{
	focusBitmap = bmpValue;
}

void wxPecanButton::render(wxDC&  dc)
{
	wxSize size = GetSize();

	if (IsEnabled())
	{

	}
	else if (pressedDown)
	{

	}
	else if (mouseInControl)
	{

	}
	else
	{
	}


	//else if (mouseInControl &&)
	//if (pressedDown)
	//	dc.SetBrush(*wxRED_BRUSH);
	//else
	//	dc.SetBrush(*wxGREY_BRUSH);

	//dc.DrawRectangle(0, 0, buttonWidth, buttonHeight);
	//dc.DrawText(text, 20, 15);
}

void wxPecanButton::mouseDown(wxMouseEvent& event)
{
	pressedDown = true;
	//paintNow();
}

void wxPecanButton::mouseReleased(wxMouseEvent& event)
{
	pressedDown = false;
	//paintNow();

}

void wxPecanButton::mouseLeftWindow(wxMouseEvent& event)
{
	if (pressedDown)
	{
		pressedDown = false;
		//paintNow();
	}
}

// currently unused events
void wxPecanButton::mouseMoved(wxMouseEvent& event) {}
void wxPecanButton::mouseWheelMoved(wxMouseEvent& event) {}
void wxPecanButton::rightClick(wxMouseEvent& event) {}
void wxPecanButton::keyPressed(wxKeyEvent& event) {}
void wxPecanButton::keyReleased(wxKeyEvent& event) {}
