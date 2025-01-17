#pragma once
#include <wx/wx.h>
#include <wx/image.h>
#include <list>
using namespace std;

class ProcoanButton : public wxWindow
{
	bool pressedDown;
	wxString text;

	int buttonWidth = 200;
	int buttonHeight = 50;
	wxBitmap dispBitmap;
	wxImage *pNormalImage;
	wxImage *pActiveImage;
	wxImage *pPressedImage;
	wxImage *pDisabledImage;

	wxImage *pSmallNormalImage;
	wxImage *pSmallActiveImage;
	wxImage *pSmallPressedImage;
	wxImage *pSmallDisabledImage;

	bool	bShowSmall;
	int buttonStatus;
	bool bDisabled; 
	wxWindow *pParent;
	wxWindowID buttonID;

public:
	ProcoanButton(wxWindow *parent,
		wxWindowID winid,
		const wxPoint& pos,
		const wxSize& size);
	virtual ~ProcoanButton();
	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	void ShowEnable(bool bFlg);
	void SetButtonPosition(wxPoint pos);
	void SetNormalSmallBitmap(const char *const *xpm_data);
	void SetActiveSmallBitmap(const char *const *xpm_data);
	void SetDisabledSmallBitmap(const char *const *xpm_data);
	void SetPressedSmallBitmap(const char *const *xpm_data);

	void SetNormalBitmap(const char *const *xpm_data);
	void SetActiveBitmap(const char *const *xpm_data);
	void SetDisabledBitmap(const char *const *xpm_data);
	void SetPressedBitmap(const char *const *xpm_data);

	void SetNormalBitmap(wxBitmap *pBitmap);
	void SetActiveBitmap(wxBitmap *pBitmap);
	void SetDisabledBitmap(wxBitmap *pBitmap);
	void SetPressedBitmap(wxBitmap *pBitmap);
	void SetDisabled(bool bDisabledFlg);
	void render(wxDC& dc);

	// some useful events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	void SetShowSmall(bool flg);
	DECLARE_EVENT_TABLE()
};

