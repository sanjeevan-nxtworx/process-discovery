#pragma once
#include <wx/wx.h>

class SliderPanel;
class TimeDisplay;

class PicControlPanel :
	public wxPanel
{
	wxPanel *pParent;
	ULONG controlID;
	SliderPanel* slider;
	TimeDisplay* txtTime;


public:
	PicControlPanel(wxPanel * parent, DWORD id, wxPoint pt, wxSize size);
	void OnMoveAbsolute(wxCommandEvent& event);
	void OnSliderHover(wxCommandEvent& event);
	void OnSliderHoverClear(wxCommandEvent& event);
	int GetHoverXPos();
	void ResizePanel();
	void SetSliderPosition(long position);
	long GetSliderPosition();
	long SetSliderSegmentStart();
	long SetSliderSegmentEnd();
	void ClearSliderSegment();
	void SetSliderMaxTime(ULONG time);
	void SetTimeValue();
	void AddPlayTime(ULONG time);
	ULONG GetThumbTime();
	bool GetSegInProgress();
	bool GetSegPresent();
	ULONG GetSegStartPos();
	ULONG GetSegEndPos();
	void SetSegmentDetails(bool bInProgress, bool bPresent, ULONG startVal, ULONG endVal);

	DECLARE_EVENT_TABLE()
};



