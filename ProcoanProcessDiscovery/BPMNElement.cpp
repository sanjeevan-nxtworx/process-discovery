#include "stdafx.h"
#include "BPMNElement.h"
#include "BPMNTask.h"
#include "BPMNSubProcess.h"
#include "BPMNDecision.h"

using namespace json11;

BPMNElement::BPMNElement(ULONG nEventIndexVal, long long timestamp)
{
	nEventIndex = nEventIndexVal;
	nEventTimestamp = timestamp;
	bSyncElement = NULL;
	pParent = NULL;
	ZeroMemory(&szElement, sizeof(SIZE));
	ZeroMemory(&pos, sizeof(POINT));
	ZeroMemory(&boundingRect, sizeof(RECT));
	bDisplayed = false;
	bSyncElement = false;
	bInPath = false;
	bSegElement = false;
	bEllipse = false;
	strAnnotation = "";
	strAutomationID = "";
	strNotes = "";
	strProcessName = "";
	strWindowName = "";
	strControlType = "";
	strControlName = "";
	nEventType = 0;
	nControlType = 0;
	nEventTypeAdditionalInfo = 0;

}

BPMNElement::BPMNElement()
{
	szElement.cx = 140;
	szElement.cy = 40;
	bSyncElement = NULL;
	nEventTimestamp = 0L;
	ZeroMemory(&boundingRect, sizeof(RECT));

	pParent = NULL;
	ZeroMemory(&szElement, sizeof(SIZE));
	ZeroMemory(&pos, sizeof(POINT));
	bDisplayed = false;
	bSyncElement = false;
	bInPath = false;
	bSegElement = false;

	bEllipse = false;

	strAnnotation = "";
	strAutomationID = "";
	strNotes = "";
	strProcessName = "";
	strWindowName = "";
	strControlType = "";
	strControlName = "";
	nEventType = 0;
	nControlType = 0;
	nEventTypeAdditionalInfo = 0;

	nEventIndex = 0L;
	nEventTimestamp = 0LL;
}

void BPMNElement::SetSyncElement(bool bFlg)
{
	bSyncElement = bFlg;
}

void BPMNElement::SetSegElement(bool bFlg)
{
	bSegElement = bFlg;
}

bool BPMNElement::IsSynchronized()
{
	return bSyncElement;
}

bool BPMNElement::IsInSegment()
{
	return bSegElement;
}

bool BPMNElement::IsInPath()
{
	return bInPath;
}

void BPMNElement::SetInPath(bool bFlg)
{
	bInPath = bFlg;
}


BPMNElement::~BPMNElement()
{
}

int BPMNElement::GetWidth()
{
	return szElement.cx;
}

int BPMNElement::GetHeight()
{
	return szElement.cy;
}

BPMNElement *BPMNElement::CreateNewEvent(ULONG nEventId)
{
	BPMNElement *pElement = DBG_NEW BPMNTask(nEventId, nEventTimestamp);
	pElement->strAnnotation = strAnnotation;
	pElement->strAutomationID = strAutomationID;
	pElement->strNotes = strNotes;
	pElement->strProcessName = strProcessName;
	pElement->strWindowName = strWindowName;
	pElement->strControlType = strControlName;
	pElement->strControlName = strControlName;
	pElement->nEventType = nEventType;
	pElement->nControlType = nControlType;
	pElement->nEventTypeAdditionalInfo = nEventTypeAdditionalInfo;
	pElement->SetBoundingRect(&boundingRect);
	pElement->pParent = NULL;

	return pElement;
}

BPMNElement *BPMNElement::CreateNewSubProcessEvent(ULONG nEventId)
{
	BPMNElement *pElement = DBG_NEW BPMNSubProcess(nEventId);
	pElement->strAnnotation = strAnnotation;
	pElement->strNotes = strNotes;
	pElement->nEventTypeAdditionalInfo = nEventTypeAdditionalInfo;
	pElement->pParent = NULL;
	pElement->nEventType = 30;
	return pElement;
}

BPMNElement *BPMNElement::CreateNewBranchEvent(ULONG nEventId)
{
	BPMNElement *pElement = DBG_NEW BPMNDecision(nEventId);
	pElement->strAnnotation = strAnnotation;
	pElement->strNotes = strNotes;
	pElement->nEventTypeAdditionalInfo = nEventTypeAdditionalInfo;
	pElement->pParent = NULL;
	pElement->nEventType = 50;
	return pElement;
}

bool BPMNElement::IsInBottomBranchList()
{
	if (pParent == NULL)
		return false;

	if (pParent->GetType() != 4)
		return false;
	BPMNDecision *pDecision = (BPMNDecision *)pParent;
	return pDecision->IsInBottomBranchList(this);
}

bool BPMNElement::IsInRightBranchList()
{
	if (pParent == NULL)
		return false;

	if (pParent->GetType() != 4)
		return false;
	BPMNDecision *pDecision = (BPMNDecision *)pParent;
	return pDecision->IsInRightBranchList(this);
}

void BPMNElement::GetDisplayPosition(RECT *rect)
{
	rect->top = pos.y;
	rect->left = pos.x;
	rect->right = rect->left + szElement.cx;
	rect->bottom = rect->top + szElement.cy;
}

void BPMNElement::SetDisplayPosition(POINT position)
{
	pos.x = position.x;
	pos.y = position.y;
}

bool BPMNElement::IsPtInElement(POINT pt)
{
	RECT r;
	r.top = pos.y;
	r.left = pos.x;
	r.right = r.left + szElement.cx;
	r.bottom = r.top + szElement.cy;

	if (PtInRect(&r, pt))
		return true;

	return false;
} 


void BPMNElement::GetControlDimension(HDC hDC)
{
	RECT rect;

	int elementType = GetType();
	if (elementType == 1 || elementType == 5)
	{
		szElement.cx = 500;
		szElement.cy = 500;
		return;
	}
	wstring wideString = wstring(strAnnotation.begin(), strAnnotation.end());

	if (szElement.cx != 0 && szElement.cy != 0)
	{
		rect.left = rect.top = 0;
		rect.right = szElement.cx;
		rect.bottom = -szElement.cy;
		DrawText(hDC, wideString.c_str(), -1, &rect, DT_CALCRECT | DT_CENTER | DT_WORDBREAK);
		//		DrawText(hDC, strAnnotation.c_str(), -1, &rect, DT_CALCRECT | DT_CENTER | DT_WORDBREAK);
		rect.bottom = -rect.bottom;
		if (rect.bottom <= szElement.cy && rect.right <= szElement.cx)
		{
			szElement.cx = rect.right + 400;
			szElement.cy = rect.bottom + 400;
			//if (GetType() == 3)
			//	szElement.cy += 100;
			return;
		}
	}
	bEllipse = false;

	int width, height;
	width = 3000;
	height = 1000;

	while (true)
	{
		rect.left = rect.top = 0;
		rect.right = width;
		rect.bottom = -height;
		DrawText(hDC, wideString.c_str(), -1, &rect, DT_CALCRECT | DT_CENTER | DT_WORDBREAK);
		rect.bottom = -rect.bottom;
		if (rect.right <= width && rect.bottom <= height)
		{
			break;
		}
		height += 500;
		if (height > 8000)
		{
			height = 8000;
			break;
		}
		rect.left = rect.top = 0;
		rect.right = width;
		rect.bottom = -height;
		DrawText(hDC, wideString.c_str(), -1, &rect, DT_CALCRECT | DT_CENTER | DT_WORDBREAK);
		rect.bottom = -rect.bottom;
		if (rect.right <= width && rect.bottom <= height)
		{
			break;
		}
		width += 1000;
		if (width > 6000)
			width = 6000;
	}

	szElement.cx = rect.right + 400;
	szElement.cy = rect.bottom + 400;

	//if (GetType() == 3)
	//	szElement.cy += 100;
}


void BPMNElement::SetAutomationID(string strAutomationIDVal)
{
	strAutomationID = strAutomationIDVal;
}

string BPMNElement::GetAutomationID()
{
	return strAutomationID;
}

bool BPMNElement::GetDisplayFlag()
{
	return bDisplayed;
}

void BPMNElement::SetDisplayFlag(bool bFlg)
{
	bDisplayed = bFlg;
}

void BPMNElement::SetAnnotation(string strAnnotationVal)
{
	strAnnotation = strAnnotationVal;
}

void BPMNElement::SetNotes(string strNotesVal)
{
	strNotes = strNotesVal;
	//std::size_t found = strNotes.find("%D");
	//if (found != std::string::npos)
	//{
	//	strNotes.replace(found, 2, "0");
	//}

	string err;
	jSonElement = Json::parse(strNotes, err);
	strAnnotation = jSonElement["Annotation"].string_value();
	string strVal = jSonElement["PName"].string_value();

	size_t posStr = 0;
	string token;
	while ((posStr = strVal.find("\\")) != std::string::npos) {
		token = strVal.substr(0, posStr);
		strVal.erase(0, posStr + 1);
	}

	SetProcessName(strVal);
	strVal = jSonElement["WName"].string_value();
	SetWindowName(strVal);

}

string BPMNElement::GetKeyValueAsString(string key)
{
	string strVal = "";
	strVal = jSonElement[key].string_value();
	return strVal;
}

int BPMNElement::GetKeyValueAsInt(string key)
{
	return jSonElement[key].int_value();
}

double BPMNElement::GetKeyValueAsDouble(string key)
{
	return jSonElement[key].number_value();
}

void BPMNElement::GetKeyValueAsRECT(string key, RECT *pRect)
{
	Json rectVal = jSonElement[key];
	pRect->top = rectVal["Top"].int_value();
	pRect->left = rectVal["Left"].int_value();
	pRect->bottom = rectVal["Bottom"].int_value();
	pRect->right = rectVal["Right"].int_value();
}

void BPMNElement::SetDuration(long long val)
{
	string annotation = GetKeyValueAsString("Annotation");
	string annotationBottom = GetKeyValueAsString("AnnotationBottom");
	string docName = GetKeyValueAsString("DocName");
	string action = GetKeyValueAsString("Action");
	string group = GetKeyValueAsString("Group");
	string pname = GetKeyValueAsString("PName");
	string subgroup = GetKeyValueAsString("SubGroup");
	string type = GetKeyValueAsString("Type");
	string wname = GetKeyValueAsString("WName");
	LONG duration = (long)val;
	LONG ctype = GetKeyValueAsInt("CType");
	RECT rect;
	GetKeyValueAsRECT("WRect",&rect);
	string strJSON = "{";
	strJSON += "\"Annotation\":\"" + annotation + "\",";
	strJSON += "\"AnnotationBottom\":\"" + annotationBottom + "\",";
	strJSON += "\"DocName\":\"" + docName + "\",";
	strJSON += "\"Action\":\"" + action + "\",";
	strJSON += "\"Group\":\"" + group + "\",";
	strJSON += "\"PName\":\"" + pname + "\",";
	strJSON += "\"SubGroup\":\"" + subgroup + "\",";
	strJSON += "\"Type\":\"" + type + "\",";
	strJSON += "\"WName\":\"" + wname + "\",";

	char str[100];
	sprintf_s(str, "\"Duration\": %ld,", duration);
	strJSON += str;
	sprintf_s(str, "\"CType\": %ld,", ctype);
	strJSON += str;

	sprintf_s(str, "\"WRect\": {\"Left\":%d,\"Top\":%d,\"Right\":%d,\"Bottom\":%d}", rect.left, rect.top, rect.right, rect.bottom);
	strJSON += str;

	strJSON += "}";

	SetNotes(strJSON);
}

void BPMNElement::SetProcessName(string strProcessNameVal)
{
	strProcessName = strProcessNameVal;
}

void BPMNElement::SetWindowName(string strWindowNameVal)
{
	strWindowName = strWindowNameVal;
}

void BPMNElement::SetControlType(string strControlTypeVal)
{
	strControlType = strControlTypeVal;
}
void BPMNElement::SetControlName(string strControlNameVal)
{
	strControlName = strControlNameVal;
}

void BPMNElement::SetEventType(int nEventTypeVal)
{
	nEventType = nEventTypeVal;
}

void BPMNElement::SetControlTypeVal(int nControlTypeVal)
{
	nControlType = nControlTypeVal;
}

void BPMNElement::SetEventAdditionalInfo(int nAdditionalVal)
{
	nEventTypeAdditionalInfo = nAdditionalVal;
}

string BPMNElement::GetAnnotation()
{
	return strAnnotation;
}

string BPMNElement::GetNotes()
{
	return strNotes;
}

string BPMNElement::GetProcessName()
{
	return strProcessName;
}

string BPMNElement::GetWindowName()
{
	return strWindowName;
}

string BPMNElement::GetControlType()
{
	return strControlType;
}

string BPMNElement::GetControlName()
{
	return strControlName;
}

int BPMNElement::GetEventType()
{
	return nEventType;
}

int BPMNElement::GetControlTypeVal()
{
	return nControlType;
}

int BPMNElement::GetEventAdditionalInfo()
{
	return nEventTypeAdditionalInfo;
}

long BPMNElement::GetEventIndex()
{
	return nEventIndex;
}

long long BPMNElement::GetEventTimestamp()
{
	return nEventTimestamp;
}

void BPMNElement::SetEventTimestamp(long long val)
{
	nEventTimestamp = val;
}

void BPMNElement::SetBoundingRect(RECT *rVal)
{
	CopyMemory(&boundingRect, rVal, sizeof(RECT));
}

RECT *BPMNElement::GetBoundingRect()
{
	return &boundingRect;
}

BPMNElement *BPMNElement::GetParentElement()
{
	return pParent;
}

void BPMNElement::SetParentElement(BPMNElement *pElement)
{
	pParent = pElement;
}