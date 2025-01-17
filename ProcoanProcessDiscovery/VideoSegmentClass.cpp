#include "stdafx.h"
#include "VideoSegmentClass.h"
#include "FileContext.h"

VideoSegmentClass::VideoSegmentClass()
{
	ZeroMemory(&topIndex, sizeof(struct IndexHdr));
	ZeroMemory(&bottomIndex, sizeof(struct IndexHdr));
}


VideoSegmentClass::~VideoSegmentClass()
{
}

void VideoSegmentClass::SetTopIndexHeader(struct IndexHdr  *index)
{
	CopyMemory(&topIndex, index, sizeof(struct IndexHdr));
}


void VideoSegmentClass::SetSegmentTopIndexHeader(struct IndexHdr  *index)
{
	CopyMemory(&segTopIndex, index, sizeof(struct IndexHdr));
}


void VideoSegmentClass::SetBottomIndexHeader(struct IndexHdr  *index)
{
	CopyMemory(&bottomIndex, index, sizeof(struct IndexHdr));
}

void VideoSegmentClass::SetFileContext(FileContext *pContext)
{
	srcfileContext = pContext;
}

void VideoSegmentClass::SetStartPosition(long long position)
{
	srcStartPosition = position;
}

void VideoSegmentClass::SetEndPosition(long long position)
{
	srcEndPosition = position;
}

void VideoSegmentClass::GetSegmentTopIndexHeader(struct IndexHdr  *index)
{
	CopyMemory(index, &segTopIndex, sizeof(struct IndexHdr));
}

void VideoSegmentClass::GetTopIndexHeader(struct IndexHdr  *index)
{
	CopyMemory(index, &topIndex, sizeof(struct IndexHdr));
}

void VideoSegmentClass::GetBottomIndexHeader(struct IndexHdr *index)
{
	CopyMemory(index, &bottomIndex, sizeof(struct IndexHdr));
}

void VideoSegmentClass::GetEventList(list <BPMNElement *> *events)
{
	events->clear();
	events->insert(events->begin(), eventList.begin(), eventList.end());
}
FileContext *VideoSegmentClass::GetFileContext()
{
	return srcfileContext;
}

long long VideoSegmentClass::GetStartPosition()
{
	return srcStartPosition;
}

long long VideoSegmentClass::GetEndPosition()
{
	return srcEndPosition;
}

void VideoSegmentClass::SetChannelNum(int num)
{
	channelNum = num;
}

int VideoSegmentClass::GetChannelNum()
{
	return channelNum;
}

void VideoSegmentClass::SetEventList(list <BPMNElement *> *events)
{
	eventList.insert(eventList.begin(), events->begin(), events->end());
}


void VideoSegmentClass::Clear()
{
	ZeroMemory(&topIndex, sizeof(struct IndexHdr));
	ZeroMemory(&bottomIndex, sizeof(struct IndexHdr));
}
