#pragma once
#include <wx/wx.h>

class SliderPanel :
	public wxPanel
{
	wxPanel *pParent;
	wxColor backColor;
	wxColor sliderColor;
	wxColor knobColor;
	wxColor segColor;
	bool	bIsHoverOn;

	ULONG	maxVal;
	ULONG   currVal;

	ULONG	startSegVal;
	ULONG	endSegVal;
	int		hoverXPos;

	ULONG	controlID;
	bool	bSegInProgress;
	bool	bSegPresent;
	time_t	startTime;
	bool	bHoverOn;
	wxTimer timer;

	void OnTimer(wxTimerEvent& event);

public:
	SliderPanel(wxPanel * parent, ULONG id, wxPoint pt,  wxSize size);
	void paintEvent(wxPaintEvent &evt);
	void paintNow();
	void render(wxDC & dc);

	void SetBackColor(wxColor color);
	wxColor GetBackColor(void);

	void SetSliderColor(wxColor color);
	wxColor GetSliderColor(void);

	void SetKnobColor(wxColor color);
	wxColor GetKnobColor(void);

	void SetSegmentColor(wxColor color);
	wxColor GetSegmentColor(void);

	void SetMaxValue(ULONG value);
	ULONG GetMaxValue(void);

	void SetCurrentValue(ULONG value);
	ULONG GetCurrentValue(void);

	void SetStartSegValue(ULONG value);
	ULONG GetStartSegValue(void);

	bool IsSegInProgress();
	bool IsSegPresent();
	void SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal);

	ULONG StartSegment();
	ULONG EndSegment();
	void SetEndSegValue(ULONG value);
	ULONG GetEndSegValue(void);
	ULONG GetThumbTime();
	int GetHoverXPos();
	void ClearSegValue();
	void AddPlayTime(ULONG time);

	void OnEnterWnd(wxMouseEvent & event);
	void OnLeaveWnd(wxMouseEvent & event);
	void OnMouseMove(wxMouseEvent & event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};



