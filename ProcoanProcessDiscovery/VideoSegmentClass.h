#pragma once

#include "MMHeader.h"
#include <list>
using namespace std;
class FileContext;

class VideoSegmentClass
{
	struct IndexHdr segTopIndex;
	struct IndexHdr  topIndex;
	struct IndexHdr  bottomIndex;
	list <BPMNElement *> eventList;
	FileContext *srcfileContext;
	long long	srcStartPosition;
	long long	srcEndPosition;
	int		channelNum;

public:
	void SetTopIndexHeader(struct IndexHdr  *index);
	void SetSegmentTopIndexHeader(struct IndexHdr  *index);
	void SetBottomIndexHeader(struct IndexHdr *index);
	void Clear();
	void SetEventList(list <BPMNElement *> *events);
	void SetFileContext(FileContext *pContext);
	void SetChannelNum(int num);
	void SetStartPosition(long long position);
	void SetEndPosition(long long position);

	void GetTopIndexHeader(struct IndexHdr  *index);
	void GetSegmentTopIndexHeader(struct IndexHdr  *index);
	void GetBottomIndexHeader(struct IndexHdr *index);
	void GetEventList(list <BPMNElement *> *events);
	FileContext *GetFileContext();
	int GetChannelNum();
	long long GetStartPosition();
	long long GetEndPosition();
	VideoSegmentClass();
	~VideoSegmentClass();
};

