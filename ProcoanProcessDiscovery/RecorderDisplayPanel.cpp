#include "stdafx.h"
#include "RecorderDisplayPanel.h"
#include "UIUtility.h"
#include "wx/graphics.h"
#include "LEDDisplayPanel.h"
#include "AudioGauge.h"
#include "RecStatusPanel.h"

BEGIN_EVENT_TABLE(RecorderDisplayPanel, wxPanel)
// catch paint events
EVT_PAINT(RecorderDisplayPanel::paintEvent)

END_EVENT_TABLE()



RecorderDisplayPanel::RecorderDisplayPanel(wxWindow *parent, wxSize size) :
	wxPanel(parent, -1, wxPoint(0, 67), wxSize(size), wxNO_BORDER)
{
	pParent = parent;
	ledPanel = DBG_NEW LEDDisplayPanel(this, wxPoint(110, 45), wxSize(225, 50));
	ledPanel->SetThickness(2);
	ledPanel->SetGap(2);
	ledPanel->SetHeight(10);
	ledPanel->SetText(wxString("00:00:00"));
	ledPanel->SetForeColor(wxColor(0xff, 0xff, 0xff));
	ledPanel->SetBackColor(wxColor(0x29, 0x37, 0x49));
	//ledPanel->paintNow();
	ledPanel->Refresh();

	//audioGauge = new AudioGauge(this, wxPoint(2, 30), wxSize(60, 70));
	//audioGauge->SetBackColor(wxColor(0x29, 0x37, 0x49));
	//audioGauge->SetLEDBackColor(wxColor(RGB(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10)));
	//audioGauge->SetAudioValue(0);
	//audioGauge->paintNow();
	pMonitorList = NULL;
	recMode = 0;
	nBlocksWritten = 0;
	nTotalBlocks = 0;
	pRecState = DBG_NEW RecStatusPanel(this, wxPoint(345, 55), wxSize(90, 30));
	pRecState->SetBackColor(RGB(0x29, 0x37, 0x49));
	pRecState->SetForeColor(RGB(0x0, 0x0ff, 0x0));
	pRecState->SetValue(wxString(""));

	pShutdownStatus = DBG_NEW wxStaticText(this, -1, "   ", wxPoint(120, 90), wxSize(320, 15), wxNO_BORDER);
	pShutdownStatus->SetBackgroundColour(RGB(0x29, 0x37, 0x49));
	pShutdownStatus->SetFont(wxFont(7, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD));
	pShutdownStatus->SetForegroundColour(RGB(0x0ff, 0x0ff, 0x0ff));
}

void RecorderDisplayPanel::SetRecorderMode(int mode)
{
	static int oldMode;

	if (oldMode == mode)
		return;
	
	oldMode = recMode = mode;
	//paintNow();
	Refresh();
	wxString wxText;
	switch (recMode)
	{
	case 0:
		wxText = _("Hold");
		break;
	case 1:
		wxText = _("Recording");
		break;
	case 2:
		wxText = _("Paused");
		break;
	case 3:
		wxText = _("Stopped");
		break;
	}
	pRecState->SetValue(wxText);

}

void RecorderDisplayPanel::SetRecorderWriteStatus(int numBlocksWritten, int totalBlocks, bool bFlg)
{
	wxString wxText;

	nBlocksWritten = numBlocksWritten;
	nTotalBlocks = totalBlocks;
	if (recMode == 3)
	{
		pShutdownStatus->Show();
		if (bFlg)
			wxText = wxString::Format("Please Wait. Writing block %d of %d", nBlocksWritten, nTotalBlocks);
		else
			wxText = _T("Please Wait. Writing Event Data");
	}
	else
	{
		wxText = "";
	}
	pShutdownStatus->SetLabel(wxText);
}

void RecorderDisplayPanel::SetMonitorList(std::list <struct monitorDetails *> *pList)
{
	pMonitorList = pList;
}

void RecorderDisplayPanel::SetVideoFlg(bool flg)
{
	bVideoFlg = flg;
}

void RecorderDisplayPanel::SetAudioFlg(bool flg)
{
	bAudioFlg = flg;
}

void RecorderDisplayPanel::SetEventFlg(bool flg)
{
	bEventFlg = flg;
}
void RecorderDisplayPanel::SetActiveMonitorFlg(bool flg)
{
	bActiveMonitorFlg = flg;
}

void RecorderDisplayPanel::SetFPS(int fps)
{
	nFPS = fps;
}

void RecorderDisplayPanel::paintEvent(wxPaintEvent & WXUNUSED(evt))
{
	wxPaintDC dc(this);
	render(dc);
}

void RecorderDisplayPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void RecorderDisplayPanel::SetAudioValue(unsigned short audioValue)
{
	//audioGauge->SetAudioValue(audioValue);
	//audioGauge->paintNow();
}

void RecorderDisplayPanel::SetRecTimeValue(ULONG recTime)
{
	wxString strTimeVal;
	long milli = recTime;
	long hr = milli / 3600000;
	milli = milli - 3600000 * hr;
	//60000 milliseconds in a minute
	long min = milli / 60000;
	milli = milli - 60000 * min;

	//1000 milliseconds in a second
	long sec = milli / 1000;
	milli = milli - 1000 * sec;

	strTimeVal = strTimeVal.Format("%02d:%02d:%02d", (int)hr, (int)min, (int)sec);
	ledPanel->SetText(strTimeVal);
	//ledPanel->paintNow();
	ledPanel->Refresh();

}

void RecorderDisplayPanel::render(wxDC&  dc)
{
	wxSize winSize = this->GetSize();

	DrawBevel(dc, wxColor(0x29, 0x37, 0x49), wxColor(127, 127, 127), wxPoint(0, 0), winSize, 10, true);
	//DrawBevel(dc, wxColor(0x0, 0x0, 0x0), wxPoint(0, 0), winSize, 4, true);
	HDC hDC = dc.GetHDC();
	HFONT hFont = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);
	int oMode = SetBkMode(hDC, OPAQUE);
	COLORREF oColor = SetTextColor(hDC, RGB(0, 0, 0));
	COLORREF oBkColor = SetBkColor(hDC, RGB(0x89, 0xb4, 0xf9));
	wxString text = "Selected Monitors";
	RECT rect;
	rect.left = 65;
	rect.top = 15;
	rect.right = 160;
	rect.bottom = 28;
	DrawText(hDC, text.c_str(), -1, &rect, DT_LEFT);

	text = "Rec. Mode";
	rect.left = 185;
	rect.right = 250;
	DrawText(hDC, text.c_str(), -1, &rect, DT_LEFT);

	text = "Video Mode";
	rect.left = 270;
	rect.right = 330;
	DrawText(hDC, text.c_str(), -1, &rect, DT_LEFT);

	text = "FPS";
	rect.left = 350;
	rect.right = 395;
	DrawText(hDC, text.c_str(), -1, &rect, DT_LEFT);

	if (pMonitorList->size() != 0)
	{
		std::list <struct monitorDetails *>::iterator itMon = pMonitorList->begin();
		struct monitorDetails *pMonitor = NULL;
		if (itMon == pMonitorList->end())
			DrawLED(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 80, 32, 10, 10); // no monitor
		else
		{
			pMonitor = *itMon;
			itMon++;
			if (pMonitor->bSelect)
				DrawLED(dc, wxColor(0x00, 0xff, 0x00), 80, 32, 10, 10); // selected Monitor
			else
				DrawLED(dc, wxColor(0x00, 0x00, 0xff), 80, 32, 10, 10); // selected Monitor
		}

		if (itMon == pMonitorList->end())
			DrawLED(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 100, 32, 10, 10); // no monitor
		else
		{
			pMonitor = *itMon;
			itMon++;
			if (pMonitor->bSelect)
				DrawLED(dc, wxColor(0x00, 0xff, 0x00), 100, 32, 10, 10); // selected Monitor
			else
				DrawLED(dc, wxColor(0x00, 0x00, 0xff), 100, 32, 10, 10); // selected Monitor
		}

		if (itMon == pMonitorList->end())
			DrawLED(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 120, 32, 10, 10); // no monitor
		else
		{
			pMonitor = *itMon;
			itMon++;
			if (pMonitor->bSelect)
				DrawLED(dc, wxColor(0x00, 0xff, 0x00), 120, 32, 10, 10); // selected Monitor
			else
				DrawLED(dc, wxColor(0x00, 0x00, 0xff), 120, 32, 10, 10); // selected Monitor
		}

		if (itMon == pMonitorList->end())
			DrawLED(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 140, 32, 10, 10); // no monitor
		else
		{
			pMonitor = *itMon;
			itMon++;
			if (pMonitor->bSelect)
				DrawLED(dc, wxColor(0x00, 0xff, 0x00), 140, 32, 10, 10); // selected Monitor
			else
				DrawLED(dc, wxColor(0x00, 0x00, 0xff), 140, 32, 10, 10); // selected Monitor
		}

	}
	if (bVideoFlg)
	{
		DrawVideo(dc, wxColor(0xff, 0xff, 0xff), 180, 32);
	}
	else
	{
		DrawVideo(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 180, 32);
	}

	if (bAudioFlg)
	{
		DrawAudio(dc, wxColor(0xff, 0xff, 0xff), 210, 32);
	}
	else
	{
		DrawAudio(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 210, 32);
	}

	if (bEventFlg)
	{
		DrawBell(dc, wxColor(0xff, 0xff, 0xff), 235, 32);
	}
	else
	{
		DrawBell(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 235, 32);
	}

	if (bActiveMonitorFlg)
	{
		DrawMultipleMonitors(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 270, 32);
		DrawSingleMonitor(dc, wxColor(0xff, 0xff, 0xff), 300, 32);
	}
	else
	{
		DrawMultipleMonitors(dc, wxColor(0xff,0xff,0xff), 270, 32);
		DrawSingleMonitor(dc, wxColor(0x29 + 0x10, 0x37 + 0x10, 0x49 + 0x10), 300, 32);
	}

	HFONT hFont2 = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));
	SelectObject(hDC, hFont2);

	SetTextColor(hDC, RGB(0xff, 0xff, 0xff));
	SetBkMode(hDC, TRANSPARENT);
	text = wxString::Format("%d",nFPS);
	rect.left = 355;
	rect.top = 32;
	rect.right = 390;
	rect.bottom = 45;
	DrawText(hDC, text.c_str(),-1, &rect, DT_LEFT);

	SelectObject(hDC, hFontOld);
	SetBkMode(hDC, oMode);
	SetTextColor(hDC, oColor);
	SetBkColor(hDC, oBkColor);
	DeleteObject(hFont);
	DeleteObject(hFont2);
}