#include "stdafx.h"
#include "SpyControlProxy.h"


SpyControlProxy::SpyControlProxy(DWORD id)
{
	controlID = id;
	parentData = NULL;
	childData.clear();
	controlType = 0;
}

void SpyControlProxy::SetParent(SpyControlProxy *value)
{
	parentData = value;
}

void SpyControlProxy::SetType(DWORD value)
{
	controlType = value;
}

DWORD SpyControlProxy::GetType()
{
	return controlType;
}

SpyControlProxy *SpyControlProxy::GetParent()
{
	return parentData;
}


void SpyControlProxy::SetAncestorKey(vector <string> &value)
{
	ancestorsKey = value;
}

void SpyControlProxy::SetLeftSiblingKey(vector <string> &value)
{
	leftSiblingKey = value;
}

void SpyControlProxy::SetRightSiblingKey(vector <string> &value)
{
	rightSiblingKey = value;
}

void SpyControlProxy::SetChildrenKey(vector <string> &value)
{
	childrenKey = value;
}

void SpyControlProxy::CreateNeighborKey()
{
	neighbourKey.clear();
	if (ancestorsKey.size() > 0)
	{
		string key = ancestorsKey.front();
		neighbourKey.push_back(key);
	}
	else
		neighbourKey.push_back("");

	if (leftSiblingKey.size() > 0)
	{
		string key = leftSiblingKey.back();
		neighbourKey.push_back(key);
	}
	else
		neighbourKey.push_back("");

	if (rightSiblingKey.size() > 0)
	{
		string key = rightSiblingKey.front();
		neighbourKey.push_back(key);
	}
	else
		neighbourKey.push_back("");

}

int SpyControlProxy::MatchKeys(vector <string> &leftKey, vector <string> &proxyKey)
{
	// Left key is from the window, right key is from the layout window.
	if (leftKey.size() == 0)
		return 100;

	vector <string>::iterator itValue = leftKey.begin();
	vector <string>::iterator itKey = proxyKey.begin();
	while (itValue != leftKey.end() && itKey != proxyKey.end())
	{
		if (*itValue == *itKey)
		{
			itValue++;
		}
		itKey++;
	}

	if (itValue == leftKey.end())
		return 100;

	itValue = leftKey.begin();
	itKey = proxyKey.begin();
	int noMatched = 1;
	while (itValue != leftKey.end() && itKey != proxyKey.end())
	{
		if (*itValue == *itKey)
		{
			noMatched++;
			itKey++;
		}
		itValue++;
	}

	return noMatched;
}

int SpyControlProxy::MatchAncestorKey(vector <string> &value)
{
	return MatchKeys(value, ancestorsKey);
}


bool SpyControlProxy::AncestorKeyValid(vector <string>& value)
{
	// Check if any key in the window controls ancestor key is present in the right, left or children list
	if (IsAnyKeyInKey(value, childrenKey))
		return false;
	if (IsAnyKeyInKey(value, leftSiblingKey))
		return false;
	if (IsAnyKeyInKey(value, rightSiblingKey))
		return false;
	return true;
}


bool SpyControlProxy::LeftSiblingKeyValid(vector <string>& value)
{
	if (IsAnyKeyInKey(value, rightSiblingKey))
		return false;
	if (IsAnyKeyInKey(value, ancestorsKey))
		return false;
	if (IsAnyKeyInKey(value, childrenKey))
		return false;
	return true;
}

bool SpyControlProxy::RightSiblingKeyValid(vector <string>& value)
{
	if (IsAnyKeyInKey(value, leftSiblingKey))
		return false;
	if (IsAnyKeyInKey(value, childrenKey))
		return false;
	if (IsAnyKeyInKey(value, ancestorsKey))
		return false;
	return true;
}

bool SpyControlProxy::ChildrenKeyValid(vector <string>& value)
{
	if (IsAnyKeyInKey(value, ancestorsKey))
		return false;
	if (IsAnyKeyInKey(value, leftSiblingKey))
		return false;
	if (IsAnyKeyInKey(value, rightSiblingKey))
		return false;
	return true;
}

bool SpyControlProxy::IsAnyKeyInKey(vector <string>& key1, vector <string>& key2)
{
	bool found = false;

	vector <string>::iterator itValue = key1.begin();
	while (itValue != key1.end())
	{
		vector <string>::iterator itKey = key2.begin();
		while (itKey != key2.end())
		{
			if (*itValue == *itKey)
				found = true;

			if (found)
				break;
			itKey++;
		}
		if (found)
			break;
		itValue++;
	}
	return found;
}

int SpyControlProxy::MatchLeftSiblingKey(vector <string> &value)
{
	return MatchKeys(value, leftSiblingKey);
}

int SpyControlProxy::MatchRightSiblingKey(vector <string> &value)
{
	return MatchKeys(value, rightSiblingKey);
}

int SpyControlProxy::MatchNeighbourKey(vector <string> &value)
{
	return MatchKeys(value, neighbourKey);
}


int SpyControlProxy::MatchChildrenKey(vector <string> &value)
{
	return MatchKeys(value, childrenKey);
}

int SpyControlProxy::MatchType(DWORD value)
{
	if (controlType == value)
		return 100;

	return 0;
}

SpyControlProxy::~SpyControlProxy()
{
	vector <SpyControlProxy *>::iterator itChildControl = childData.begin();

	while (itChildControl != childData.end())
	{
		SpyControlProxy *pChildControl = *itChildControl;

		delete pChildControl;
		itChildControl++;
	}

	childData.clear();
}

SpyControlProxy *SpyControlProxy::AddChildControl(DWORD id)
{
	SpyControlProxy *pChildControl = new SpyControlProxy(id);
	childData.push_back(pChildControl);
	return pChildControl;
}

DWORD SpyControlProxy::GetControlID()
{
	return controlID;
}

vector <SpyControlProxy *> *SpyControlProxy::GetChildControls()
{
	return &childData;
}

SpyControlProxy *SpyControlProxy::GetControl(DWORD id)
{
	if (controlID == id)
		return this;

	vector <SpyControlProxy *>::iterator itChildControl = childData.begin();

	while (itChildControl != childData.end())
	{
		SpyControlProxy *pChildControl = *itChildControl;
		SpyControlProxy *pFoundControl = pChildControl->GetControl(id);
		if (pFoundControl != NULL)
			return pFoundControl;
		itChildControl++;
	}

	return NULL;
}


