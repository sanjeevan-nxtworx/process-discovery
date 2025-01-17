#include <wx/wx.h>
#include <wx/sizer.h>

class MaskImagePanel : public wxScrolledWindow
{

	BITMAPFILEHEADER bfh;
	BITMAPINFO bmi;
	char *maskImage;

	wxPoint point1, point2;

	RECT image1;
	RECT image2;
	RECT image3;
	RECT image4;
	RECT image5;
	RECT image6;
	RECT maskRect;


public:
	MaskImagePanel(wxWindow * parent, wxPoint pt, wxSize size);
	~MaskImagePanel();

	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	void render(wxDC& dc);
	void SetImage();
	char *GetImage(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader);
	void ClearImage();
	char *GetImage();
	void mouseEvents(wxMouseEvent& event);
	RECT GetCurrentRect();
	RECT GetImageRect();

	char *SaveHeaderAndGetBuffer(BITMAPFILEHEADER *pBfh, BITMAPINFOHEADER *pBmiHeader);
	size_t GetBitImageSize();
	void ConvertImageTo32();
	void SetMaskImageRect(RECT rect);
	void SetMatchImage1Rect(RECT rect);
	void SetMatchImage2Rect(RECT rect);
	void SetMatchImage3Rect(RECT rect);
	void SetMatchImage4Rect(RECT rect);
	void SetMatchImage5Rect(RECT rect);
	void SetMatchImage6Rect(RECT rect);

	RECT GetMaskImageRect();
	RECT GetMatchImage1Rect();
	RECT GetMatchImage2Rect();
	RECT GetMatchImage3Rect();
	RECT GetMatchImage4Rect();
	RECT GetMatchImage5Rect();
	RECT GetMatchImage6Rect();


	// some useful events
	/*
	 void mouseMoved(wxMouseEvent& event);
	 void mouseDown(wxMouseEvent& event);
	 void mouseWheelMoved(wxMouseEvent& event);
	 void mouseReleased(wxMouseEvent& event);
	 void rightClick(wxMouseEvent& event);
	 void mouseLeftWindow(wxMouseEvent& event);
	 void keyPressed(wxKeyEvent& event);
	 void keyReleased(wxKeyEvent& event);
	 */

	DECLARE_EVENT_TABLE()
};

