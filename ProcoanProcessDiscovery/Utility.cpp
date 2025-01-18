#include "stdafx.h"
#include "Utility.h"
#include <codecvt>
using namespace std;
#include "MMHeader.h"
#include <map>
#include <unordered_map>
#include <algorithm>
#include "SPDLogFile.h"
#include <tlhelp32.h>
#include "SpyHeader.h"

static ConfigClass *pConfigClass = NULL;
static ExecutionManager *pExecutionManager = NULL;
static RecorderWindow *recorderWnd = NULL;
static SPDLogFile *pSPDLogFile = NULL;
static KeyState *pKeyState = NULL;
static TransparentWindow* transparentWnd = NULL;

std::string ProcessIdToName(DWORD processId)
{
	std::string ret;
	HANDLE handle = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE,
		processId /* This is the PID, you can find one from windows task manager */
	);
	if (handle)
	{
		DWORD buffSize = 1024;
		CHAR buffer[1024];
		if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
		{
			ret = buffer;
		}
		CloseHandle(handle);
	}
	return ret;
}

int CompareBitmapMemory(long *src, long *dest, long numBytes)
{
	while (numBytes > 0)
	{
		long val = *dest - *src;
		if (val != 0)
			return (int)val;
		src++;
		dest++;
		numBytes--;
	}

	return 0;
}

string ConvertBSTRToMBS(BSTR bstr)
{
	int wslen = ::SysStringLen(bstr);
	return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

	std::string dblstr(len, '\0');
	len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
		pstr, wslen /* not necessary NULL-terminated */,
		&dblstr[0], len,
		NULL, NULL /* no default char */);

	return dblstr;
}


int CopyBitmapMemory(long *dest, long *src, long numBytes)
{
	while (numBytes > 0)
	{
		*dest = *src;
		src++;
		dest++;
		numBytes--;
	}
	return (int)numBytes;
}

int CopyBitmapMemoryMultiple(long *dest1, long *dest2, long *src, long numBytes)
{
	while (numBytes > 0)
	{
		*dest2 = *dest1 = *src;
		src++;
		dest1++;
		dest2++;
		numBytes--;
	}
	return (int)numBytes;
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = DBG_NEW wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string ws2s(const std::wstring &ws)
{
	int len;
	int slength = (int)ws.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), slength, 0, 0, 0, 0);
	char* buf = DBG_NEW char[len];
	WideCharToMultiByte(CP_ACP, 0, ws.c_str(), slength, buf, len, 0, 0);
	std::string r(buf);
	delete[] buf;
	return r;

}

bool RectInRect(LPRECT wndRect, LPRECT rect)
{
	POINT pt;
	pt.x = rect->left;
	pt.y = rect->top;
	if (PtInRect(wndRect, pt))
		return true;
	pt.y = rect->bottom;
	if (PtInRect(wndRect, pt))
		return true;
	pt.x = rect->right;
	if (PtInRect(wndRect, pt))
		return true;
	pt.y = rect->top;
	if (PtInRect(wndRect, pt))
		return true;

	if (rect->top < wndRect->top && rect->bottom > wndRect->bottom)
		if ((rect->left > wndRect->left && rect->left < wndRect->right) || (rect->right > wndRect->left && rect->right < wndRect->right))
			return true;

	if (rect->left < wndRect->left && rect->right > wndRect->right &&
		((rect->top > wndRect->top && rect->top < wndRect->bottom) || (rect->bottom > wndRect->top && rect->bottom < wndRect->bottom)))
		return true;
	return false;
}

//long PackData(long *dest, long *src, int height, int width, long *dest2)
//{
//	unsigned short pxSecondValue = 0;
//	long size = 0L;
//	unsigned short pxFirstValue = 0;
//	unsigned short cnt = 1;
//	int colNum = 0;
//	long pxValue = 0;
//	unsigned short *destInt = (unsigned short *)dest;
//	unsigned short *destInt2 = (unsigned short *)dest2;
//	for (int rowNum = 0; rowNum < height; rowNum++)
//	{
//		pxValue = *src;
//		src++;
//		pxFirstValue = (unsigned short)(((pxValue << 2) & 0x380) | ((pxValue >> 3) & 0x1C00) | ((pxValue >> 8) & 0xe000));
//		cnt = 1;
//		// start from 2nd pixel since first pixel is already read for compare.
//		for (colNum = 1; colNum < width; colNum++)
//		{
//			pxValue = *src;
//			src++;
//			pxSecondValue = (unsigned short)(((pxValue << 2) & 0x380) | ((pxValue >> 3) & 0x1C00) | ((pxValue >> 8) & 0xe000));
//			if (pxFirstValue != pxSecondValue || cnt == 0x7f)
//			{
//				if (destInt2 != NULL)
//				{
//					*destInt = *destInt2 = (pxFirstValue | cnt);
//					destInt++;
//					destInt2++;
//				}
//				else
//				{
//					*destInt = (pxFirstValue | cnt);
//					destInt++;
//				}
//				pxFirstValue = pxSecondValue;
//				cnt = 1;
//				size += 2;
//			}
//			else
//				cnt++;
//		}
//		if (destInt2 != NULL)
//		{
//			*destInt = *destInt2 = (pxFirstValue | cnt);
//			destInt++;
//			destInt2++;
//		}
//		else
//		{
//			*destInt = (pxFirstValue | cnt);
//			destInt++;
//		}
//		size += 2;
//	}
//	return size;
//}

template <typename A, typename B>
void flip_unordered_map(unordered_map<A, B> & src, multimap <B, A> *dst)
{
	pair<multimap<B, A>::iterator, A> retVal;
	retVal.first = dst->begin();
	for (typename unordered_map<A, B>::const_iterator it = src.begin(); it != src.end(); ++it)
	{
		dst->insert(retVal.first, pair<B, A>(it->second, it->first));
	}
}

long CompressScreenData(char *dest, long *src, long size)
{
	unordered_map<long, long> rleMap;
	long *srcBack = src;
	unordered_map<long, long>::iterator rleMapIT;
	unordered_map<long, long> colorMap;
	unordered_map<long, long>::iterator colorMapIT;
	pair<unordered_map<long, long>::iterator, long> retVal;
	long val = 0;

	long oldVal = 0x01;
	for (long num = 0; num < size; num++)
	{
		val = *src;
		src++;
		if (val != oldVal)
		{
			retVal = rleMap.insert(pair<long, long>(val, 1L));
			if (retVal.second == false)
			{
				(retVal.first)->second++;
			}
			oldVal = val;
		}
		else
		{
			(retVal.first)->second++;
		}
	}

	long cnt = 0;
	if (rleMap.size() >= 127)
	{
		multimap<long, long> sortMap;
		sortMap.clear();
		flip_unordered_map(rleMap, &sortMap);
		rleMap.clear();
		int index = 0;
		multimap<long, long>::const_reverse_iterator it;
		retVal.first = rleMap.begin();
		for (it = sortMap.rbegin(); it != sortMap.rend(); ++it)
		{
			rleMap.insert(pair<long, long>(it->second, 0));
			index++;
			if (index >= 127)
				break;
		}

		rleMapIT = rleMap.begin();
		index = 0;
		while (rleMapIT != rleMap.end())
		{
			rleMapIT->second = index;
			rleMapIT++;
			index += 2;
		}

		oldVal = 0x01;
		for (; it != sortMap.rend(); ++it)
		{
			val = it->second;
			cnt = (val & 0xfc) >> 2;
			val = val & 0xffffff00L;
			if (val != oldVal)
			{
				retVal = colorMap.insert(pair<long, long>(val, cnt));
				if (retVal.second == false)
				{
					(retVal.first)->second += cnt;
				}
				oldVal = val;
			}
			else
				(retVal.first)->second += cnt;
		}
		sortMap.clear();
		if (colorMap.size() > 255)
		{
			flip_unordered_map(colorMap, &sortMap);
			colorMap.clear();
			index = 0;
			
			for (it = sortMap.rbegin(); it != sortMap.rend(); ++it)
			{
				colorMap.insert(pair<long, long>(it->second, index));
				index++;
				if (index >= 255)
					break;
			}
			colorMapIT = colorMap.begin();
			index = 0;
			while (colorMapIT != colorMap.end())
			{
				colorMapIT->second = index;
				index++;
				colorMapIT++;
			}
		}
	}
	long retSize = 0L;
	// start coping the data to the output buffer
	int num = rleMap.size();
	*dest = (char)num;
	dest++;
	retSize++;

	// copy the map to the buffer
	long *destLong = (long *)dest;
	rleMapIT = rleMap.begin();
	while (rleMapIT != rleMap.end())
	{
		*destLong = rleMapIT->first;
		rleMapIT++;
		destLong++;
		retSize += 4;
	}

	dest = (char *)destLong;
	num = colorMap.size();
	*dest = num;
	dest++;
	retSize++;

	// Copy color Map into the buffer
	colorMapIT = colorMap.begin();
	destLong = (long *)dest;
	while (colorMapIT != colorMap.end())
	{
		*destLong = colorMapIT->first;
		colorMapIT++;
		destLong++;
		retSize += 4;
	}

	dest = (char *)destLong;
	src = srcBack;
	long colorVal = 0;
	long rleCnt, colorCnt, fullCnt;
	rleCnt = colorCnt = fullCnt = 0;
	val = oldVal = 0xffffffff;
	for (long numX = 0; numX < size; numX++)
	{
		oldVal = val;
		val = *src;
		src++;
		if (val != oldVal)
			rleMapIT = rleMap.find(val);
		if (rleMapIT != rleMap.end())
		{
			*dest = (char)rleMapIT->second; // This will move a value from 0 to 127 to the buffer
			dest++;
			retSize++;
		}
		else
		{
			if (val != oldVal)
			{
				colorVal = val & 0xffffff00;
				cnt = val & 0xfc;
				colorMapIT = colorMap.find(colorVal);
			}
			if (colorMapIT != colorMap.end())
			{
				*dest = (char)(cnt | 0x1);
				dest++;
				*dest = (char)colorMapIT->second;
				dest++;
				retSize += 2;
			}
			else
			{
				destLong = (long *)dest;
				// put the full value in the dest
				val |= 0x3;
				*destLong = val;
				dest += 4;
				retSize += 4;
			}
		}
	}

	return retSize;
}

long ExpandScreenData(long *dest, unsigned char *src, long size)
{
	unsigned short numRLE = *src;
	src++;
	long *rleTable = (long *)src;
	src += (4L * numRLE);

	unsigned short numColor = (unsigned short)(*src);
	src++;
	long *colorTable = (long *)src;
	src += (4L * numColor);
	
	size = size - (numRLE * 4L) - (numColor * 4L) - 2;
	long retSize = 0L;
	long pxValue = 0L;
	unsigned indexVal = 0L;
	for (int i = 0; i < size;)
	{
		indexVal = (unsigned)(*src);
		if (!(indexVal & 0x1))
		{
			// 1 byte value
			indexVal >>= 1;
			src++;
			pxValue = rleTable[indexVal];
			i++;
		}
		else
		{
			if (!(indexVal & 0x02))
			{
				pxValue = indexVal & 0xfc; // count
				src++;
				indexVal = (unsigned)(*src);
				src++;
				pxValue = (colorTable[indexVal] & 0xffffff00L) | pxValue;
				i += 2;
			}
			else
			{
				pxValue = (*((long *)src)) & 0xfffffffcL;
				src += 4;
				i += 4;
			}
		}

		unsigned short count = ((unsigned short)(pxValue >> 2)) & 0x3f;
		pxValue = ((pxValue >> 8L) & 0xffffffL) | 0xff000000L;
		for (int pxNum = 0; pxNum < count; pxNum++)
		{
			*dest = pxValue;
			dest++;
			retSize += 4;
		}
	}
	return retSize;
}

long PackData(long *dest, long *src, int height, int width, long *dest2)
{
	long pxSecondValue = 0L;
	long size = 0L;
	long pxFirstValue = 0L;
	unsigned short cnt = 1;
	int colNum = 0;

	for (int rowNum = 0; rowNum < height; rowNum++)
	{
		pxFirstValue = *src;
		src++;
		pxFirstValue <<= 8;
		pxFirstValue &= 0xffffff00; // use alpha byte to store count
		cnt = 1;
		// start from 2nd pixel since first pixel is already read for compare.
		for (colNum = 1; colNum < width; colNum++)
		{
			pxSecondValue = *src;
			src++;
			pxSecondValue <<= 8;
			pxSecondValue &= 0xffffff00;
			if (pxFirstValue != pxSecondValue || cnt == 0xff)
			{
				if (dest2 != NULL)
				{
					*dest = *dest2 = (pxFirstValue | cnt);
					dest++;
					dest2++;
				}
				else
				{
					*dest = (pxFirstValue | cnt);
					dest++;
				}
				pxFirstValue = pxSecondValue;
				cnt = 1;
				size += 4;
			}
			else
				cnt++;
		}
		if (dest2 != NULL)
		{
			*dest = *dest2 = (pxFirstValue | cnt);
			dest++;
			dest2++;
		}
		else
		{
			*dest = (pxFirstValue | cnt);
			dest++;
		}
		size += 4;
	}
	return size;
}

long UnpackData(long *dest, long *src, int height, int width)
{

	for (int rowNum = 0; rowNum < height; rowNum++)
	{
		for (int colNum = 0; colNum < width; )
		{
			long pxValue = *src;
			src++;

			unsigned short count = (unsigned short)(pxValue & 0xffL);
			pxValue = ((pxValue >> 8L) & 0xffffffL) | 0xff000000L;
			for (int pxNum = 0; pxNum < count; pxNum++)
			{
				*dest = pxValue;
				dest++;
				colNum++;
			}
		}
	}
	return 0L;
}

//long UnpackData(long *dest, long *src, long size)
//{
//	long retSize = 0;
//	long pxValue = 0L;
//	for (int i = 0; i < size; i++)
//	{
//		pxValue = (*((long *)src)) & 0xffffffffL;
//		src++;
//		unsigned short count = (unsigned short)(pxValue & 0xff);
//		pxValue = ((pxValue >> 8L) & 0xffffffL) | 0xff000000L;
//		for (int pxNum = 0; pxNum < count; pxNum++)
//		{
//			*dest = pxValue;
//			dest++;
//			retSize += 4;
//		}
//	}
//	return retSize;
//
//	//return ExpandScreenData(dest, (unsigned char *)src, size);
//}


//long UnpackCompareData(long *dest, long *src, int width, long numBlocks)
//{
//	unsigned short *srcInt = (unsigned short *)src;
//	long pxStoreValue = 0L;
//
//	for (int i = 0; i < numBlocks; i++)
//	{
//		struct blockDetails *pBlockDetails = (struct blockDetails *)src;
//		srcInt = (unsigned short *)((char *)srcInt + sizeof(struct blockDetails));
//		char *destPtr = (char *)dest + ((DWORD)pBlockDetails->lineNum * width * 4L) + (pBlockDetails->structPos * 4L);
//		long *blockDest = (long *)destPtr;
//		for (int j = 0; j < pBlockDetails->numPixels; j++)
//		{
//			long pxValue = (long)(*srcInt) & 0xffff;
//			srcInt++;
//			unsigned short count = (unsigned short)(pxValue & 0x7fL);
//			pxStoreValue = (((pxValue >> 2) & 0xe0) | ((pxValue << 3) & 0xe000) | ((pxValue << 8) & 0xe00000)) | 0xff0f0f0fL;
//			for (int pxNum = 0; pxNum < count; pxNum++)
//			{
//				*blockDest = pxValue;
//				blockDest++;
//			}
//		}
//	}
//
//	return 0L;
//}

long UnpackCompareData(long *dest, long *src, int width, long numBlocks)
{
	long pxValue = 0L;
	long retSize = 0L;

	for (int i = 0; i < numBlocks; i++)
	{
		struct blockDetails *pBlockDetails = (struct blockDetails *)src;
		src = (long *)((char *)src + sizeof(struct blockDetails));
		char *destPtr = (char *)dest + ((DWORD)pBlockDetails->lineNum * width * 4L) + (pBlockDetails->structPos * 4L);
		long *blockDest = (long *)destPtr;
		for (int j = 0; j < pBlockDetails->numPixels; j++)
		{
			pxValue = *src;
			src++;
			unsigned short count = (unsigned short)(pxValue & 0xff);
			pxValue = ((pxValue >> 8L) & 0xffffffL) | 0xff000000L;
			for (int pxNum = 0; pxNum < count; pxNum++)
			{
				*blockDest = pxValue;
				blockDest++;
				retSize += 4;
			}
		}
	}
	return retSize;

	//return expandcomparedata(dest, (char *)src, width, numblocks);
}


long CompressCompareBuffer(char *dest, long *src, long numBlocks)
{
	unordered_map<long, long> rleMap;
	long *srcBack = src;
	unordered_map<long, long>::iterator rleMapIT;
	unordered_map<long, long> colorMap;
	unordered_map<long, long>::iterator colorMapIT;
	pair<unordered_map<long, long>::iterator, long> retVal;
	long val = 0;
	long retSize = 0L;
	long oldVal = 0xffffffff;

	for (int i = 0; i < numBlocks; i++)
	{
		struct blockDetails *pBlockDetails = (struct blockDetails *)src;
		src = (long *)((char *)src + sizeof(struct blockDetails));
		for (int j = 0; j < pBlockDetails->numPixels; j++)
		{
			val = *src;
			src++;
			if (val != oldVal)
			{
				retVal = rleMap.insert(pair<long, long>(val, 1L));
				if (retVal.second == false)
				{
					(retVal.first)->second++;
				}
				oldVal = val;
			}
			else
			{
				(retVal.first)->second++;
			}
		}
	}

	long cnt = 0;
	if (rleMap.size() >= 127)
	{
		multimap<long, long> sortMap;
		sortMap.clear();
		flip_unordered_map(rleMap, &sortMap);
		rleMap.clear();
		int index = 0;
		multimap<long, long>::const_reverse_iterator it;
		retVal.first = rleMap.begin();
		for (it = sortMap.rbegin(); it != sortMap.rend(); ++it)
		{
			rleMap.insert(pair<long, long>(it->second, index));
			index++;
			if (index >= 127)
				break;
		}

		rleMapIT = rleMap.begin();
		index = 0;
		while (rleMapIT != rleMap.end())
		{
			rleMapIT->second = index;
			rleMapIT++;
			index += 2;
		}

		oldVal = 0xffffffffL;
		for (; it != sortMap.rend(); ++it)
		{
			val = it->second;
			cnt = (val & 0xfc) >> 2;
			val = val & 0xffffff00L;
			if (val != oldVal)
			{
				retVal = colorMap.insert(pair<long, long>(val, cnt));
				if (retVal.second == false)
				{
					(retVal.first)->second += cnt;
				}
				oldVal = val;
			}
			else
				(retVal.first)->second += cnt;
		}
		sortMap.clear();
		if (colorMap.size() > 255)
		{
			flip_unordered_map(colorMap, &sortMap);
			colorMap.clear();
			index = 0;

			for (it = sortMap.rbegin(); it != sortMap.rend(); ++it)
			{
				colorMap.insert(pair<long, long>(it->second, index));
				index++;
				if (index >= 255)
					break;
			}

			colorMapIT = colorMap.begin();
			index = 0;
			while (colorMapIT != colorMap.end())
			{
				colorMapIT->second = index;
				index++;
				colorMapIT++;
			}

		}
	}
	retSize = 0;
	// start coping the data to the output buffer
	int num = rleMap.size();
	*dest = (char)num;
	dest++;
	retSize++;

	// copy the map to the buffer
	rleMapIT = rleMap.begin();
	long *destLong = (long *)dest;
	while (rleMapIT != rleMap.end())
	{
		*destLong = rleMapIT->first;
		rleMapIT++;
		destLong++;
		retSize += 4;
	}

	dest = (char *)destLong;
	num = colorMap.size();
	*dest = num;
	dest++;
	retSize++;

	// Copy color Map into the buffer
	colorMapIT = colorMap.begin();
	destLong = (long *)dest;
	while (colorMapIT != colorMap.end())
	{
		*destLong = colorMapIT->first;
		colorMapIT++;
		destLong++;
		retSize += 4;
	}

	dest = (char *)destLong;
	src = srcBack;
	long colorVal = 0;
	long rleCnt, colorCnt, fullCnt;
	rleCnt = colorCnt = fullCnt = 0;
	val = oldVal = 0xffffffffL;


	for (int i = 0; i < numBlocks; i++)
	{
		struct blockDetails *pBlockDetails = (struct blockDetails *)src;
		// First copy the block details
		CopyMemory(dest, src, sizeof(struct blockDetails));
		dest += sizeof(struct blockDetails);
		src = (long *)((char *)src + sizeof(struct blockDetails));
		for (int j = 0; j < pBlockDetails->numPixels; j++)
		{
			val = *src;
			src++;
			if (val != oldVal)
				rleMapIT = rleMap.find(val);
			if (rleMapIT != rleMap.end())
			{
				*dest = (char)rleMapIT->second; // This will move a value from 0 to 127 to the buffer
				dest++;
				retSize++;
			}
			else
			{
				if (val != oldVal)
				{
					colorVal = val & 0xffffff00;
					cnt = val & 0xfc;
					colorMapIT = colorMap.find(colorVal);
				}
				if (colorMapIT != colorMap.end())
				{
					*dest = (char)colorMapIT->second;
					dest++;
					*dest = (char)cnt;
					dest++;
					retSize += 2;
				}
				else
				{
					destLong = (long *)dest;
					// put the full value in the dest
					val |= 0x03;
					*destLong = val;
					dest += 4;
					retSize += 4;
				}
			}
		}
	}

	return retSize;
}

long ExpandCompareData(long *dest, char *src,  long numBlocks)
{
	int numRLE = *src;
	src++;
	long *rleTable = (long *)src;
	src += (4L * numRLE);

	int numColor = *src;
	src++;
	long *colorTable = (long *)src;
	src += (4L * numColor);

	long retSize = 0L;
	long pxValue = 0L;
	unsigned indexVal = 0L;
	for (int i = 0; i < numBlocks; i++)
	{
		struct blockDetails *pBlockDetails = (struct blockDetails *)src;
		// First copy the block details
		CopyMemory(dest, src, sizeof(struct blockDetails));
		dest = (long *)((char *)dest + sizeof(struct blockDetails));
		src += sizeof(struct blockDetails);
		for (int j = 0; j < pBlockDetails->numPixels; j++)
		{
			indexVal = (int)(*src);
			if (!(indexVal & 0x1))
			{
				// 1 byte value
				src++;
				pxValue = rleTable[indexVal];
			}
			else
			{
				if (!(indexVal & 0x02))
				{
					pxValue = indexVal & 0xfc; // count
					src++;
					indexVal = (int)(*src);
					src++;
					pxValue = (colorTable[indexVal] & 0xffffff00L) | pxValue;
				}
				else
				{
					pxValue = (*((long *)src)) & 0xfffffffcL;
					src += 4;
				}
			}
		}

		unsigned short count = ((unsigned short)(pxValue >> 2)) & 0x3fL;
		pxValue >>= 8L;
		pxValue &= 0x0ffffff;
		pxValue |= 0xff000000L;
		for (int pxNum = 0; pxNum < count; pxNum++)
		{
			*dest = pxValue;
			dest++;
			retSize += 4;
		}
	}
	return retSize;
}


long CompareData(long *dest, long *src, int height, int width)
{
	long size = 0L;

	for (int rowNum = 0; rowNum < height; rowNum++)
	{
		for (int colNum = 0; colNum < width; colNum++)
		{
			long pxLHSValue = (*src) & 0xfffffffc;
			long pxRHSValue = (*dest) & 0xfffffffc;
			if (pxLHSValue != pxRHSValue)
			{
				size = pxLHSValue - pxRHSValue;
				break;
			}
			src++;
			dest++;
		}
	}
	return size;
}

ConfigClass *GetConfigClass()
{
	return pConfigClass;
}

RecorderWindow *GetRecorderWnd()
{
	return recorderWnd;
}

ExecutionManager *GetExecutionManager()
{
	return pExecutionManager;
}

//LogFile *GetLogFile()
//{
//	return pLogFile;
//}

SPDLogFile *CreateSPDLogFile(wxString logFileName)
{
	if (pSPDLogFile != NULL)
	{
		delete pSPDLogFile;
		pSPDLogFile = NULL;
	}
	pSPDLogFile = DBG_NEW SPDLogFile(logFileName.char_str());
	return pSPDLogFile;
}

void DeleteSPDLogFile()
{
	if (pSPDLogFile != NULL)
		delete pSPDLogFile;
	pSPDLogFile = NULL;
}

//LogFile *CreateLogFile(wxString logFileName)
//{
//	pLogFile = new LogFile(logFileName.char_str());
//	return pLogFile;
//}

RecorderWindow *CreateRecorderWindow()
{
	if (recorderWnd == NULL)
		recorderWnd = DBG_NEW RecorderWindow();
	return recorderWnd;
}

TransparentWindow* CreateTransparentWindow()
{
	if (transparentWnd == NULL)
	{
		transparentWnd = DBG_NEW TransparentWindow();
		transparentWnd->CreateTransparentWindow();
	}
	return transparentWnd;
}

void DeleteRecorderWindow()
{
	if (recorderWnd != NULL)
		delete recorderWnd;
	recorderWnd = NULL;
}

void DeleteExecutionManager()
{
	if (pExecutionManager != NULL)
		delete pExecutionManager;
	pExecutionManager = NULL;
}


ExecutionManager *CreateExecutionManager()
{
	pExecutionManager = DBG_NEW ExecutionManager();
	return pExecutionManager;
}

ConfigClass *CreateConfigClass(wxString configFileName)
{
	pConfigClass = DBG_NEW ConfigClass(configFileName.char_str());
	return pConfigClass;
}

void DeleteConfigClass()
{
	if (pConfigClass != NULL)
		delete pConfigClass;
	pConfigClass = NULL;
}

KeyState *GetKeyState()
{
	if (pKeyState == NULL)
		pKeyState = DBG_NEW KeyState;
	return pKeyState;
}

void DeleteKeyState()
{
	if (pKeyState != NULL)
		delete pKeyState;
	pKeyState = NULL;
}


wstring getexepath()
{
	WCHAR result[MAX_PATH];
	GetModuleFileName(NULL, result, MAX_PATH);
	wstring stringResult = result;
	return stringResult;
}

string getcurrentpath()
{
	wstring str;
	str = getexepath();

	string strPath(str.begin(), str.end());
	size_t pos = strPath.find_last_of('\\');

	string retPath = strPath.substr(0, pos+1);

	return retPath;

}


VOID startup(LPCTSTR lpApplicationName, STARTUPINFO *pSI, PROCESS_INFORMATION *pPI)
{

	// start the program up
	CreateProcess(lpApplicationName,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		pSI,            // Pointer to STARTUPINFO structure
		pPI             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
}

struct EnumWindowsCallbackArgs {
	EnumWindowsCallbackArgs(DWORD p) : pid(p) { }
	const DWORD pid;
	std::vector<HWND> handles;
};

static BOOL CALLBACK EnumWindowsCallback(HWND hnd, LPARAM lParam)
{
	EnumWindowsCallbackArgs *args = (EnumWindowsCallbackArgs *)lParam;

	DWORD windowPID;
	(void)::GetWindowThreadProcessId(hnd, &windowPID);
	if (windowPID == args->pid) {
		args->handles.push_back(hnd);
	}

	return TRUE;
}

std::vector<HWND> getToplevelWindows(DWORD pID)
{
	EnumWindowsCallbackArgs args(pID);
	if (::EnumWindows(&EnumWindowsCallback, (LPARAM)&args) == FALSE) {
		// XXX Log error here
		return std::vector<HWND>();
	}
	return args.handles;
}


static BOOL CALLBACK EnumChildWindowCallback(HWND hnd, LPARAM lParam)
{
	EnumWindowsCallbackArgs *args = (EnumWindowsCallbackArgs *)lParam;

	args->handles.push_back(hnd);

	return TRUE;
}

std::vector<HWND> getChildWindows(HWND hWnd)
{
	EnumWindowsCallbackArgs args(0);
	if (::EnumChildWindows(hWnd, &EnumChildWindowCallback, (LPARAM)&args) == FALSE) {
		// XXX Log error here
		return std::vector<HWND>();
	}
	return args.handles;
}



BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	DWORD dwThreadId, dwProcessId;
	HINSTANCE hInstance;
	char String[255];
	HANDLE hProcess;
	if (!hWnd)
		return TRUE;		// Not a window
	if (!::IsWindowVisible(hWnd))
		return TRUE;		// Not visible
	if (!SendMessage(hWnd, WM_GETTEXT, sizeof(String), (LPARAM)String))
		return TRUE;		// No window title
	hInstance = (HINSTANCE)GetWindowLongPtrA(hWnd, GWLP_HINSTANCE);
	dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	// GetModuleFileNameEx uses psapi, which works for NT only!
	CloseHandle(hProcess);
	return TRUE;
}

void GetAllWindowsFromProcessID(DWORD dwProcessID, std::vector <HWND> &vhWnds)
{
	// find all hWnds (vhWnds) associated with a process id (dwProcessID)
	HWND hCurWnd = NULL;
	do
	{
		hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
		DWORD dwWndProcessID = 0;
		GetWindowThreadProcessId(hCurWnd, &dwWndProcessID);
		if (dwProcessID == dwWndProcessID)
		{
			vhWnds.push_back(hCurWnd);  // add the found hCurWnd to the vector
			//wprintf(L"Found hWnd %d\n", hCurWnd);
		}
	} while (hCurWnd != NULL);
}


HWND FindTopWindow(DWORD pid)
{
	std::pair<HWND, DWORD> params = { 0, pid };

	// Enumerate the windows using a lambda to process each window
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto pParams = (std::pair<HWND, DWORD>*)(lParam);

		DWORD processId;
		if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
		{
			// Stop enumerating
			SetLastError((DWORD)-1L);
			pParams->first = hwnd;
			return FALSE;
		}

		// Continue enumerating
		return TRUE;
	}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

vector<string> split(const string& i_str, const string& i_delim)
{
	vector<string> result;

	size_t found = i_str.find(i_delim);
	size_t startIndex = 0;

	while (found != string::npos)
	{
		result.push_back(string(i_str.begin() + startIndex, i_str.begin() + found));
		startIndex = found + i_delim.size();
		found = i_str.find(i_delim, startIndex);
	}
	if (startIndex != i_str.size())
		result.push_back(string(i_str.begin() + startIndex, i_str.end()));
	return result;
}

/*
string ProcessIdToName(DWORD processId)
{
	string ret;
	HANDLE handle = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE,
		processId 
	);
	if (handle)
	{
		DWORD buffSize = 1024;
		CHAR buffer[1024];
		if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
		{
			ret = buffer;
		}
		CloseHandle(handle);
	}
	return ret;
}

*/

std::vector<struct processdata *> getProcesses()
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	vector<struct processdata *> processList;

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);


	if (Process32First(h, &pe)) {
		do {
			struct processdata *pData = new struct processdata;

			pData->pid = pe.th32ProcessID;
			pData->parent = pe.th32ParentProcessID;
			wstring name(pe.szExeFile);
			pData->processName = ws2s(name);
			processList.push_back(pData);
		} while (Process32Next(h, &pe));
	}
	CloseHandle(h);
	return processList;
}

string EscapeJSonString(string str)
{
	string strRet = str;
	strRet = FindReplaceString(strRet, "\r", "\\r");
	strRet = FindReplaceString(strRet, "\n", "\\n");
	strRet = FindReplaceString(strRet, "\t", "\\t");
	return strRet;
}

string EscapeJSonAnnotationString(string str)
{
	string strRet = str;
	strRet = FindReplaceString(strRet, "\\", "\\\\");
	strRet = FindReplaceString(strRet, "\"", "\\\"");
	strRet = FindReplaceString(strRet, "\r", "\\r");
	strRet = FindReplaceString(strRet, "\n", "\\n");
	strRet = FindReplaceString(strRet, "\t", "\\t");
	return strRet;
}


string EscapeString(string strVal)
{
	size_t found = strVal.find('\\');
	if (found != string::npos)
	{
		string strRes = strVal;

		found = strRes.find("\\");

		// Repeat till end is reached
		while (found != string::npos)
		{
			// Replace this occurrence of Sub String
			strRes.replace(found, 1, "\\\\");
			// Get the next occurrence from the current position
			found = strRes.find("\\", found + 2);
		}

		return strRes;
	}
	return strVal;
}

string FindReplaceString(string str, string strFind, string strReplace)
{
	// Replace double quotes with single quotes
	size_t found = str.find(strFind);
	if (found != string::npos)
	{
		string strRes = str;

		found = strRes.find(strFind);

		// Repeat till end is reached
		while (found != string::npos)
		{
			// Replace this occurrence of Sub String
			strRes.replace(found, 1, strReplace);
			// Get the next occurrence from the current position
			found = strRes.find(strFind, found + strReplace.length() + 1);
		}

		return strRes;
	}
	return str;
}
