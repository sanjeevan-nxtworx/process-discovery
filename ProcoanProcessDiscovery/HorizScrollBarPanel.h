#pragma once
#include <wx/wx.h>

class HorizScrollBarPanel :
	public wxPanel
{
	wxPanel *pParent;
	wxColor backColor;
	wxColor knobColor;
	wxColor segColor;

	ULONG	maxVal;
	ULONG   currVal;

	ULONG	thumbSize;

	ULONG	controlID;


public:
	HorizScrollBarPanel(wxPanel * parent, ULONG id, wxPoint pt, wxSize size);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);

	void SetBackColor(wxColor color);
	wxColor GetBackColor(void);

	void SetKnobColor(wxColor color);
	wxColor GetKnobColor(void);

	void SetSegmentColor(wxColor color);
	wxColor GetSegmentColor(void);

	void SetMaxValue(ULONG value);
	ULONG GetMaxValue(void);

	void SetCurrentValue(ULONG value);
	ULONG GetCurrentValue(void);

	void SetThumbSize(ULONG value);
	ULONG GetThumbSize(void);

	void OnEnterWnd(wxMouseEvent & event);
	void OnLeaveWnd(wxMouseEvent & event);
	void OnMouseMove(wxMouseEvent & event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};



