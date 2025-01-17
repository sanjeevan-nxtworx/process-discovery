#pragma once
#include "MMHeader.h"
#include <list>
#include <string>
#include <map>
#include "BPMNElement.h"
#include "AnnotationRuleDefinitions.h"

using namespace std;


class BPMNFacade
{
	list <struct eventDetails *> eventList;
	list <BPMNElement *> bpmnDiagram;
	AnnotationRuleDefinitions *pAnnotationRules;

	//void NumberEventList();
	map <ULONG, struct eventListElement *> eventMap;
	map <ULONG, struct widgetDetails *> widgetMap;

public:
	BPMNFacade();
	~BPMNFacade();
	void GetAnnotationRules(char *fileName);
	//void AddEvent(struct eventBufferRecord *pEventBuffer, long eventIndex, long videoIndex, long audioIndex);
	void CreateRecorderEvent(struct eventDetails *pEvent, struct eventBufferRecord *pEventBuffer, ULONG eventIndexVal);
	struct eventListElement *AddEventInDiagram(struct eventFileRecord *pEventRecord, struct eventDetails *pEvent, long long pos);
	struct eventListElement *GetEventInDiagram(ULONG nEventIndex);
	struct eventListElement *ProcessEventList();
	list <BPMNElement *> *GetBPMNDiagram();
	map <ULONG, struct eventListElement *> *GetElementMap();
	list <BPMNElement *> *GetElementList(BPMNElement *pElement);
	void CreateWidgetTree(char *jSONRec);
	RECT *GetBoundingRect(int eventIndex);
};

	