#include "stdafx.h"
#include "Expression.h"
#include <cmath>
#include <algorithm>
#include <regex>
#include "json11.hpp"
#include "Utility.h"
#include "KeyAnnotation.h"

Expression::Expression() {}
Expression::~Expression() {}

NumberExpression::NumberExpression(int val) : value(val) 
{
}

Value NumberExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	return value;
}

BooleanExpression::BooleanExpression(bool val) : value(val)
{
}

Value BooleanExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument) 
{
	return value;
}


StringExpression::StringExpression(std::string val) : value(val)
{
}

Value StringExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument) 
{
	return value;
}

ArithmeticExpression::ArithmeticExpression(char ch, Expression *l, Expression *r)
	: left(l), right(r), op(ch)
{
}

ArithmeticExpression::~ArithmeticExpression() 
{
	if (left)
		delete left;
	if (right)
		delete right;
}

Value ArithmeticExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value result;
	if (left == nullptr || right == nullptr)
		return result;


	Value a = left->evaluate(pEvent, pWidget, pWidgetMap);
	Value b = right->evaluate(pEvent, pWidget, pWidgetMap);

	if (a.GetType() == 1 && b.GetType() == 1)
	{
		int op1, op2;

		a.GetValue(&op1);
		b.GetValue(&op2);

		switch (op) {
		case '+':
			result.SetValue(op1 + op2);
			break;
		case '-':
			result.SetValue(op1 - op2);
			break;
		case '*':
			result.SetValue(op1 * op2);
			break;
		case '/':
			result.SetValue(op1 / op2);
			break;
		}
	}
	else if (a.GetType() == 2 && b.GetType() == 2)
	{
		std::string op1, op2;

		a.GetValue(&op1);
		b.GetValue(&op2);

		if (op == '+')
		{
			result.SetValue(op1 + op2);
		}
	}
	return result;
}


LogicalExpression::LogicalExpression(char ch, Expression *l, Expression *r)
	: left(l), right(r), op(ch)
{
}

LogicalExpression::~LogicalExpression() {
	if (left)
		delete left;
	if (right)
		delete right;
}

Value LogicalExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value result;
	if (left == nullptr || right == nullptr)
		return result;

	Value a = left->evaluate(pEvent, pWidget, pWidgetMap);
	Value b = right->evaluate(pEvent, pWidget, pWidgetMap);

	if (a.GetType() == 3 && b.GetType() == 3)
	{
		bool op1, op2;

		a.GetValue(&op1);
		b.GetValue(&op2);

		switch (op) {
		case '|':
			result.SetValue(op1 || op2);
			break;
		case '&':
			result.SetValue(op1 && op2);
			break;
		}
	}
	return result;
}



RelationalExpression::RelationalExpression(char ch, Expression *l, Expression *r)
	: left(l), right(r), op(ch)
{
}

RelationalExpression::~RelationalExpression() {
	if (left)
		delete left;
	if (right)
		delete right;
}

Value RelationalExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value result;
	if (left == nullptr || right == nullptr)
		return result;

	Value a = left->evaluate(pEvent, pWidget, pWidgetMap);
	Value b = right->evaluate(pEvent, pWidget, pWidgetMap);

	if (a.GetType() == 1 && b.GetType() == 1)
	{
		int op1, op2;

		a.GetValue(&op1);
		b.GetValue(&op2);

		switch (op) {
		case '>':
			result.SetValue(op1 > op2);
			break;
		case '<':
			result.SetValue(op1 < op2);
			break;
		case 'M':
			result.SetValue(op1 >= op2);
			break;
		case 'N':
			result.SetValue(op1 <= op2);
			break;
		case 'S':
			result.SetValue(op1 == op2);
			break;
		case 'T':
			result.SetValue(op1 != op2);
			break;
		}
	}
	else
		if (a.GetType() == 2 && b.GetType() == 2)
		{
			std::string op1, op2;

			a.GetValue(&op1);
			b.GetValue(&op2);

			switch (op) {
			case '>':
				result.SetValue(op1 > op2);
				break;
			case '<':
				result.SetValue(op1 < op2);
				break;
			case 'M':
				result.SetValue(op1 >= op2);
				break;
			case 'N':
				result.SetValue(op1 <= op2);
				break;
			case 'S':
				result.SetValue(op1 == op2);
				break;
			case 'T':
				result.SetValue(op1 != op2);
				break;
			}
		}
	return result;
}

UnaryExpression::UnaryExpression(char ch, Expression *r)
	: right(r), op(ch)
{
}

UnaryExpression::~UnaryExpression() 
{
	if (right)
		delete right;
}

Value UnaryExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value result;
	if (right == nullptr)
		return result;

	Value b = right->evaluate(pEvent, pWidget, pWidgetMap);

	if (b.GetType() == 3)
	{
		int op2;
		b.GetValue(&op2);

		switch (op) {
		case '!':
			result.SetValue(!op2);
			break;
		default:
			break;
		}
	}
	return result;
}


FunctionCallExpression::FunctionCallExpression(Expression *lhs, Expression *rhs)
	: left(lhs), right(rhs)
{
}

FunctionCallExpression::~FunctionCallExpression()
{
	if (left)
		delete left;
	if (right)
		delete right;
}

Value FunctionCallExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value res;
	Value x;
	Value y;
	Value arg;

	x = left->evaluate(pEvent, pWidget, pWidgetMap, &arg);
	if (x.GetType() == 2) // string
	{
		y = right->evaluate(pEvent, pWidget, pWidgetMap);
	}

	if (x.GetType() != 2)
		return res;

	std::string strLabel;
	x.GetValue(&strLabel);

	std::string strLower = strLabel;

	transform(strLabel.begin(), strLabel.end(), strLower.begin(), ::tolower);
	if (strLower == "evaluate")
	{
		if (y.GetType() == 2 && arg.GetType() == 2)
		{
			std::string strRegExp, strValue;
			y.GetValue(&strRegExp);

			arg.GetValue(&strValue);
			smatch m;
			regex regExp(strRegExp);
			if (regex_search(strValue, m, regExp))
			{
				res.SetValue(m.str(0));
			}
		}
	}
	return res;
}


ObjectReferenceExpression::ObjectReferenceExpression(Expression *lhs, Expression *rhs)
	: left(lhs), right(rhs)
{
}

ObjectReferenceExpression::~ObjectReferenceExpression()
{
	if (left)
		delete left;
	if (right)
		delete right;
}

Value ObjectReferenceExpression::GetEventDetails(struct eventDetails *pLeftEvent, Expression *rhs, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	Value x;
	Value res;

	if (pLeftEvent == NULL || rhs == NULL)
		return res;

	x = rhs->evaluate(NULL, NULL, NULL);
	std::string strLabel;
	if (x.GetType() != 2)
		return res;

	x.GetValue(&strLabel);

	std::string strLower = strLabel;

	transform(strLabel.begin(), strLabel.end(), strLower.begin(), ::tolower);
	if (strLower == "descriptorname")
	{
		if (pLeftEvent->jSonElement.is_object())
		{
			json11::Json descriptor = pLeftEvent->jSonElement.object_items();
			res.SetValue(descriptor["Name"].string_value());
		}
	}
	else if (strLower == "mousex")
	{
		res.SetValue(pLeftEvent->mousePt.x);
	}
	else if (strLower == "mousey")
	{
		res.SetValue(pLeftEvent->mousePt.y);
	} 
	else if (strLower == "lbuttondblclick")
	{
		res.SetValue(pLeftEvent->nEventCode == 1);
	}
	else if (strLower == "lbuttondown")
	{
		res.SetValue(pLeftEvent->nEventCode == 2);
	}
	else if (strLower == "lbuttonup")
	{
		res.SetValue(pLeftEvent->nEventCode == 3);
	}
	else if (strLower == "lbuttonclick")
	{
		res.SetValue(pLeftEvent->nEventCode == 10);
	}
	else if (strLower == "mbuttondblclick")
	{
		res.SetValue(pLeftEvent->nEventCode == 4);
	}
	else if (strLower == "mbuttondown")
	{
		res.SetValue(pLeftEvent->nEventCode == 5);
	}
	else if (strLower == "mbuttonup")
	{
		res.SetValue(pLeftEvent->nEventCode == 6);
	}
	else if (strLower == "mbuttonclick")
	{
		res.SetValue(pLeftEvent->nEventCode == 11);
	}
	else if (strLower == "rbuttondblclick")
	{
		res.SetValue(pLeftEvent->nEventCode == 7);
	}
	else if (strLower == "rbuttondown")
	{
		res.SetValue(pLeftEvent->nEventCode == 8);
	}
	else if (strLower == "rbuttonup")
	{
		res.SetValue(pLeftEvent->nEventCode == 9);
	}
	else if (strLower == "rbuttonclick")
	{
		res.SetValue(pLeftEvent->nEventCode == 12);
	}
	else if (strLower == "type")
	{
		if (pLeftEvent->nEventCode < 20)
		{
			res.SetValue(1); // Mouse events
		}
		else
		{	
			res.SetValue(2); // Key Events
		}
	}
	else if (strLower == "keydescriptor")
	{
		string strName = GetControlName(pLeftEvent, pWidgetMap);
		string strKeyEventDescriptor = GetKeyEventDescriptor(pLeftEvent, strName);
		res.SetValue(strKeyEventDescriptor);
	}
	return res;
}

string ObjectReferenceExpression::GetControlName(struct eventDetails *pEvent, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	string strName = "";

	json11::Json descriptor = pEvent->jSonElement.object_items();
	strName = descriptor["Name"].string_value();

	if (strName != "")
		return strName;

	map <ULONG, struct widgetDetails *>::iterator itElement = pWidgetMap->find(pEvent->elementID);
	if (itElement != pWidgetMap->end())
	{
		struct widgetDetails *pElement = itElement->second;
		bool bSearchParent = false;
		itElement = pWidgetMap->find(pElement->leftID);
		if (itElement != pWidgetMap->end())
		{
			struct widgetDetails *pTag = itElement->second;
			if (pTag->controlType == UIA_TextControlTypeId)
			{
				map<int, string>::iterator itName = pTag->nameMap.find(pElement->leftID);
				if (itName != pTag->nameMap.end())
				{
					strName = itName->second;
				}
			}
			else
			{
				bSearchParent = true;
			}
		}
		else
			bSearchParent = true;
		if (bSearchParent)
		{
			itElement = pWidgetMap->find(pElement->parentID);
			if (itElement != pWidgetMap->end())
			{
				struct widgetDetails *pTag = itElement->second;
				if (pTag->controlType == UIA_TextControlTypeId)
				{
					map<int, string>::iterator itName = pTag->nameMap.find(pElement->leftID);
					if (itName != pTag->nameMap.end())
					{
						strName = itName->second;
					}
				}
			}
		}
	}

	return strName;
}


string ObjectReferenceExpression::GetKeyEventDescriptor(struct eventDetails *pEvent, string strName)
{
	// Here parse JSON object of keystrokes
	// evaluate all keys pressed, If multiple readable key pressed, then Annotation will be 
	// "Enter Data in <<Control Name>>
	// else if Control/ALT/Del/Arrow/Function Keys etc, Then decode key and Annotation will be
	// Press <<Key>> Key
	string err;

	json11::Json keyJSonElement = json11::Json::parse(pEvent->strKeyStrokes, err);
	json11::Json descriptor = keyJSonElement["keystrokes"];
	string keysPressed = "";
	string newKeyString = "";

	string keyAnnotationString = "";
	if (descriptor.is_array())
	{
		vector <json11::Json> keys = descriptor.array_items();
		for (size_t i = 0; i < keys.size(); i++)
		{
			vector <json11::Json> keyFlagPair = keys[i].array_items();
			unsigned keyValue = keyFlagPair[0].int_value();
			unsigned flagValue = keyFlagPair[1].int_value();
			if (keyAnnotationString != "")
				keysPressed += " then ";
			else if (keysPressed != "")
				keysPressed += " ";

			if (keyValue > 94)
			{
				keyAnnotationString = "Enter Data";
			}
			else
			{
				keyValue--;
				bool ctrlFlg = false;
				bool shiftFlg = false;
				bool altFlg = false;
				bool singleKeyFlg = true;

				if (flagValue & 0x01)
					singleKeyFlg = false;
				if (flagValue & 0x02)
					ctrlFlg = true;
				if (flagValue & 0x04)
					altFlg = true;
				if (flagValue & 0x08)
					shiftFlg = true;

				if (!ctrlFlg && !altFlg && !shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].baseAnnotation;
				else if (ctrlFlg && !altFlg && !shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].controlAnnotation;
				else if (!ctrlFlg && altFlg && !shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].altAnnotation;
				else if (!ctrlFlg && !altFlg && shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].shiftAnnotation;
				else if (ctrlFlg && altFlg && !shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].controlAltAnnotation;
				else if (ctrlFlg && !altFlg && shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].controlShiftAnnotation;
				else if (!ctrlFlg && altFlg && shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].altShiftAnnotation;
				else if (ctrlFlg && altFlg && shiftFlg)
					keyAnnotationString = keyAnnotationTable[keyValue].controlAltShiftAnnotation;

			}
			keysPressed += keyAnnotationString;
		}
	}

	return keysPressed;
}


//string ObjectReferenceExpression::GetKeyEventDescriptor(struct eventDetails *pEvent, string strName)
//{
//	// Here parse JSON object of keystrokes
//	// evaluate all keys pressed, If multiple readable key pressed, then Annotation will be 
//	// "Enter Data in <<Control Name>>
//	// else if Control/ALT/Del/Arrow/Function Keys etc, Then decode key and Annotation will be
//	// Press <<Key>> Key
//	string err;
//
//	json11::Json keyJSonElement = json11::Json::parse(pEvent->strKeyStrokes, err);
//	json11::Json descriptor = keyJSonElement["keystrokes"];
//	string keysPressed = "";
//	string dataEntered = "";
//	string noTimes = "";
//	string newKeyString = "";
//
//	if (descriptor.is_array())
//	{
//		KeyState *pKeyState = GetKeyState();
//		vector <json11::Json> keys = descriptor.array_items();
//		list <struct keyStrokes *> lstKeys;
//		list <struct keyStrokes *> lstAddKeys;
//		list <struct keyStrokes *> lstAllKeys;
//
//		lstAllKeys.clear();
//		map <int, bool> mapKeyPressed;
//		int lastKey = 0;
//		struct keyStrokes *pKey = NULL;
//
//		for (int i = 0; i < keys.size(); i++)
//		{
//			vector <json11::Json> keyFlagPair = keys[i].array_items();
//			int keyValue = keyFlagPair[0].int_value();
//			int flagValue = keyFlagPair[1].int_value();
//
//			map <int, bool>::iterator itKeyMap = mapKeyPressed.find(keyValue);
//			if (itKeyMap == mapKeyPressed.end())
//			{
//				// Key not found
//				flagValue &= 0x7f;
//			}
//			else
//			{
//				flagValue |= 0x80;
//			}
//			
//			if (lastKey != keyValue)
//			{
//				pKey = DBG_NEW struct keyStrokes;
//				pKey->keyVal = keyValue;
//				pKey->keyFlg = flagValue;
//				pKey->numPressed = 0;
//				lstKeys.push_back(pKey);
//			}
//			else if ((flagValue & 0x80) == 0x0)
//			{
//				struct keyStrokes *pLastKeyAction = lstKeys.back();
//				lstKeys.pop_back();
//				if ((pLastKeyAction->keyFlg & 0x80) != 0x0)
//				{
//					// 09/23
//					struct keyStrokes *pPrevKeyAction = NULL;;
//					if (lstKeys.size() > 0)
//					{
//						pPrevKeyAction = lstKeys.back();
//						lstKeys.pop_back();
//						pPrevKeyAction->numPressed++;
//						lstKeys.push_back(pPrevKeyAction);
//					}
//				}
//				lstKeys.push_back(pLastKeyAction);
//			}
//			else
//			{
//				struct keyStrokes *pLastKeyAction = lstKeys.back();
//				lstKeys.pop_back();
//				if ((pLastKeyAction->keyFlg & 0x80) == 0x80)
//				{
//					pLastKeyAction->numPressed++;
//				}
//				lstKeys.push_back(pLastKeyAction);
//			}
//
//			if ((flagValue & 0x80) == 0) // Key is Pressed
//			{
//				mapKeyPressed.insert({ keyValue, true });
//			}
//			else
//			{
//				lastKey = keyValue;
//				itKeyMap = mapKeyPressed.find(keyValue);
//				if (itKeyMap == mapKeyPressed.end()) // key is not found
//				{
//					if (pKeyState->IsCommandKeys(keyValue))
//					{
//						pKey = DBG_NEW struct keyStrokes;
//
//						pKey->keyVal = keyValue;
//						pKey->keyFlg = flagValue & 0x7f;
//						lstAddKeys.push_front(pKey);
//					}
//					else
//					{
//						pKey = lstKeys.back();
//						lstKeys.pop_back();
//						delete pKey;
//					}
//				}
//				else
//				{
//					// key found
//					mapKeyPressed.erase(keyValue);
//				}
//			}
//		}
//
//		if (lstAddKeys.size() > 0)
//			lstAllKeys.insert(lstAllKeys.end(), lstAddKeys.begin(), lstAddKeys.end());
//		lstAllKeys.insert(lstAllKeys.end(), lstKeys.begin(), lstKeys.end());
//		lstAddKeys.clear();
//		lstKeys.clear();
//
//		if (mapKeyPressed.size() > 0)
//		{
//			map <int, bool>::iterator itMapKeyPressed = mapKeyPressed.begin();
//			while (itMapKeyPressed != mapKeyPressed.end())
//			{
//				int keyValue = itMapKeyPressed->first;
//				itMapKeyPressed++;
//				if (pKeyState->IsPrintableKey(keyValue))
//				{
//					pKey = DBG_NEW struct keyStrokes;
//
//					pKey->keyVal = keyValue;
//					pKey->keyFlg = 0x80;
//					lstAllKeys.push_back(pKey);
//				}
//			}
//		}
//		list <struct keyStrokes *>::iterator itKeyList = lstAllKeys.begin();
//		while (itKeyList != lstAllKeys.end())
//		{
//			struct keyStrokes *pKeys = *itKeyList;
//			int keyValue = pKeys->keyVal;
//			int flagValue = pKeys->keyFlg;
//			int pressedTimes = pKeys->numPressed + 1;
//			delete pKeys;
//			itKeyList++;
//			if (pressedTimes > 1)
//			{
//				char strTimes[50];
//
//				sprintf_s(strTimes, " %d times", pressedTimes);
//				noTimes = strTimes;
//			}
//
//			newKeyString = pKeyState->ProcessKey(keyValue, flagValue);
//			if (newKeyString == "*")
//			{
//				dataEntered += newKeyString;
//			}
//			else if (newKeyString != "")
//			{
//				if (dataEntered != "")
//				{
//					if (keysPressed == "" && strName != "")
//						keysPressed = "Enter " + strName + " ";
//					else if (keysPressed == "")
//					{
//						keysPressed = "Enter Data";
//					}
//				}
//				dataEntered = "";
//				if (keysPressed == "" && strName != "")
//				{
//					keysPressed = "In " + strName + " press " + newKeyString + noTimes + ". ";
//				}
//				else
//				{
//					keysPressed += "Press " + newKeyString + noTimes + ". ";
//				}
//			}
//			noTimes = "";
//			newKeyString = "";
//		}
//		lstAllKeys.clear();
//		DeleteKeyState();
//	}
//	if (dataEntered != "")
//	{
//		if (keysPressed == "" && strName != "")
//			keysPressed = "Enter " + strName + " ";
//		else if (keysPressed == "")
//		{
//			keysPressed = "Enter Data";
//		}
//	}
//
//	if (newKeyString != "")
//	{
//		if (keysPressed == "" && strName != "")
//		{
//			keysPressed = "In " + strName + " press " + newKeyString + noTimes + ". ";
//		}
//		else
//		{
//			keysPressed += "Press " + newKeyString + noTimes + ". ";
//		}
//	}
//
//	return keysPressed;
//}

Value ObjectReferenceExpression::GetWidgetDetails(struct widgetDetails *pLeftWidget, Expression *rhs, struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap)
{
	Value x;
	Value res;

	if (pLeftWidget == NULL || rhs == NULL)
		return res;

	x = rhs->evaluate(NULL, NULL, NULL);

	std::string strLabel;

	if (x.GetType() != 2)
		return res;

	x.GetValue(&strLabel);
	std::string strLower = strLabel;

	transform(strLabel.begin(), strLabel.end(), strLower.begin(), ::tolower);
	if (strLower == "parent")
	{
		// Search for Parent of the left widget
		ULONG parentID = pLeftWidget->parentID;
		map <ULONG, struct widgetDetails *>::iterator itWidget = pWidgetMap->find(parentID);
		if (itWidget != pWidgetMap->end())
		{
			res.SetValue(itWidget->second);
		}
	}
	else if (strLower == "child")
	{
		// Search for Child of the left widget
		ULONG childID = pLeftWidget->childID;
		map <ULONG, struct widgetDetails *>::iterator itWidget = pWidgetMap->find(childID);
		if (itWidget != pWidgetMap->end())
		{
			res.SetValue(itWidget->second);
		}
	}
	else if (strLower == "left")
	{
		// Search for Left Sibling of the left widget
		ULONG leftID = pLeftWidget->leftID;
		map <ULONG, struct widgetDetails *>::iterator itWidget = pWidgetMap->find(leftID);
		if (itWidget != pWidgetMap->end())
		{
			res.SetValue(itWidget->second);
		}
	}
	else if (strLower == "right")
	{
		// Search for Child of the left widget
		ULONG rightID = pLeftWidget->rightID;
		map <ULONG, struct widgetDetails *>::iterator itWidget = pWidgetMap->find(rightID);
		if (itWidget != pWidgetMap->end())
		{
			res.SetValue(itWidget->second);
		}
	}
	else if (strLower == "type")
	{
		res.SetValue(pLeftWidget->controlType);
	}
	else if (strLower == "processname")
	{
		res.SetValue(pLeftWidget->processName);
	}
	else if (strLower == "classname")
	{
		res.SetValue(pLeftWidget->className);
	}
	else if (strLower == "name")
	{
		if (pLeftWidget == pWidget)
		{
			// if Control.Name then get from event
			json11::Json descriptor = pEvent->jSonElement.object_items();
			res.SetValue(descriptor["Name"].string_value());
		}
		else
		{
			map<int, string>::iterator itName = pLeftWidget->nameMap.find(pWidget->elementID);
			if (itName != pLeftWidget->nameMap.end())
			{
				res.SetValue(itName->second);
			}
		}

	}

	return res;
}

Value ObjectReferenceExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value x;
	Value res;

	x = left->evaluate(pEvent, pWidget, pWidgetMap);
	if (x.GetType() == 4)
	{
		struct eventDetails *pLeftEvent = NULL;
		x.GetValue(&pLeftEvent);
		if (pLeftEvent != NULL)
		{
			res = GetEventDetails(pLeftEvent, right, pEvent, pWidget, pWidgetMap);
		}
	}
	else if (x.GetType() == 5)
	{
		struct widgetDetails *pLeftWidget = NULL;
		x.GetValue(&pLeftWidget);
		if (pLeftWidget != NULL)
		{
			res = GetWidgetDetails(pLeftWidget, right, pEvent, pWidget, pWidgetMap);
		}

	}
	else if (x.GetType() == 2) // LHS is a string
	{
		std::string strArg;

		x.GetValue(&strArg);
		argument->SetValue(strArg);	// initialize LHS string as an argument;
		res = right->evaluate(pEvent, pWidget, pWidgetMap);
	}
	return res;
}


LabelExpression::LabelExpression(std::string val)
	: value(val)
{
}

Value LabelExpression::evaluate(struct eventDetails *pEvent, struct widgetDetails *pWidget, map <ULONG, struct widgetDetails *> *pWidgetMap, Value *argument)
{
	Value x;
	std::string strLower = value;

	transform(value.begin(), value.end(), strLower.begin(), ::tolower);

	if (pEvent != NULL && strLower == "event") // Evaluate to object
	{
		x.SetValue(pEvent);
	}
	else if (pWidget != NULL && strLower == "control")
	{
		x.SetValue(pWidget);
	}
	else
	{
		// Return Label
		x.SetValue(value);
	}
	return x;
}


Value::Value()
{
	ValueType = -1; // Error Value Type
}

Value::Value(int value)
{
	ValueType = 1;
	dataValue.intVal = value;
}

Value::Value(std::string value)
{
	ValueType = 2;
	dataValue.strVal = DBG_NEW std::string(value);
}

Value::Value(bool value)
{
	ValueType = 3;
	dataValue.boolVal = value;
}

Value::Value(struct eventDetails *ptr)
{
	ValueType = 4;
	dataValue.objectPtr = (void *)ptr;
}

Value::Value(struct widgetDetails *ptr)
{
	ValueType = 5;
	dataValue.objectPtr = (void *)ptr;
}

Value& Value::operator=(const Value& rhs)
{
	ValueType = rhs.ValueType;
	if (ValueType == 1)  // integer
	{
		dataValue.intVal = rhs.dataValue.intVal;
	}
	else if (ValueType == 2) // string
	{
		dataValue.strVal = DBG_NEW std::string;
		*(dataValue.strVal) = *rhs.dataValue.strVal;
	}
	else if (ValueType == 3) // bool
	{
		dataValue.boolVal = rhs.dataValue.boolVal;
	}
	else
	{
		dataValue.objectPtr = rhs.dataValue.objectPtr;
	}

	return *this;
}


Value::Value(const Value& value)
{
	ValueType = value.ValueType;
	if (ValueType == 1)  // integer
	{
		dataValue.intVal = value.dataValue.intVal;
	}
	else if (ValueType == 2) // string
	{
		dataValue.strVal = DBG_NEW std::string;
		*dataValue.strVal = *value.dataValue.strVal;
	}
	else if (ValueType == 3) // bool
	{
		dataValue.boolVal = value.dataValue.boolVal;
	}
	else
	{
		dataValue.objectPtr = value.dataValue.objectPtr;
	}
}

Value::~Value()
{
	if (ValueType == 2)
		delete dataValue.strVal;
}

void Value::SetValue(int value)
{
	if (ValueType == 2)
		delete dataValue.strVal;
	ValueType = 1;
	dataValue.intVal = value;
}

void Value::SetValue(std::string value)
{
	if (ValueType != 2)
	{
		dataValue.strVal = DBG_NEW std::string;
	}
	ValueType = 2;
	*(dataValue.strVal) = value;
}

void Value::SetValue(bool value)
{
	if (ValueType == 2)
		delete dataValue.strVal;
	ValueType = 3;
	dataValue.boolVal = value;
}

void Value::SetValue(struct eventDetails *ptr)
{
	if (ValueType == 2)
		delete dataValue.strVal;
	ValueType = 4;
	dataValue.objectPtr = (void *)ptr;
}

void Value::SetValue(struct widgetDetails *ptr)
{
	if (ValueType == 2)
		delete dataValue.strVal;
	ValueType = 5;
	dataValue.objectPtr = (void *)ptr;
}




int Value::GetType()
{
	return ValueType;
}

void Value::GetValue(int *value)
{
	if (ValueType == 1)
		*value = dataValue.intVal;
	else
		*value = -1;
}

void Value::GetValue(bool *value)
{
	if (ValueType == 3)
		*value = dataValue.boolVal;
	else
		*value = false;
}

void Value::GetValue(std::string *value)
{
	if (ValueType == 2)
		*value = *dataValue.strVal;
	else
		value->clear();
}

void Value::GetValue(struct eventDetails **ptr)
{
	if (ValueType == 4)
		*ptr = (struct eventDetails *)dataValue.objectPtr;
	else
		*ptr = NULL;
}

void Value::GetValue(struct widgetDetails **ptr)
{
	if (ValueType == 5)
		*ptr = (struct widgetDetails *)dataValue.objectPtr;
	else
		*ptr = NULL;
}