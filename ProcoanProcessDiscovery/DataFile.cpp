#include "stdafx.h"
#include "DataFile.h"
#include "MMHeader.h"
#include <tchar.h>
#include "Zlib\zlib.h"
#include <mutex>
#include "spdlog/spdlog.h"
#include "lz4.h"

#define ZIPMODE false

mutex lckDataFile;


DataFile::DataFile(char *filename)
{
	int errNum = 0;
	zipMode = ZIPMODE;
	fileobj.open(filename,ios::in | ios::out | ios::binary | ios::trunc);
	if (fileobj.rdstate() & ios::eofbit)
		errNum = 1;
	else if (fileobj.rdstate() & ios::badbit)
		errNum = 2;
	else if (fileobj.rdstate() & ios::failbit)
		errNum = 3;
}

DataFile::DataFile(char *filename, int createFlg)
{
	zipMode = ZIPMODE;
	if (createFlg)
	{
		fileobj.open(filename, ios::in | ios::out | ios::binary | ios::trunc);
	}
	else
	{
		fileobj.open(filename, ios::in | ios::out | ios::binary);
	}
	if (fileobj.fail())
		fileobj.clear();
}

DataFile::~DataFile()
{
	fileobj.close();
}

void DataFile::SetZipMode(bool bFlg)
{
	zipMode = bFlg;
}

long long DataFile::AppendToFile(char *buffer, ULONG len)
{
	lckDataFile.lock();
	//spdlog::info("Appending to Datafile");

	// Acquire a mutex to ensure that only one thread is writing
	// goto end of file
	// get file position
	// write the block
	// return file position before writing

	long long endPos;
	fileobj.seekg(0, ios::end);
	fileobj.clear();
	endPos = fileobj.tellg();

	fileobj.write(buffer, len);
	if (fileobj.fail())
		fileobj.clear();
	lckDataFile.unlock();
	return endPos;

}

void DataFile::WriteToFile(long long fPos, char *buffer, ULONG len)
{
	lckDataFile.lock();
	//spdlog::info("Writing to Datafile");

	// Acquire a mutex to ensure that only one thread is writing
	// goto end of file
	// get fileposition
	// write the block
	// return file position before writing
	int errNum = 0;
	fileobj.seekg(fPos, ios::beg);
	if (fileobj.rdstate() & ios::eofbit)
		errNum = 1;
	else if (fileobj.rdstate() & ios::badbit)
		errNum = 2;
	else if (fileobj.rdstate() & ios::failbit)
		errNum = 3;

	fileobj.clear();
	fileobj.write(buffer, len);
	if (fileobj.rdstate() & ios::eofbit)
		errNum = 1;
	else if (fileobj.rdstate() & ios::badbit)
		errNum = 2;
	else if (fileobj.rdstate() & ios::failbit)
		errNum = 3;

	if (fileobj.fail())
		fileobj.clear();
	lckDataFile.unlock();
}

long long DataFile::AppendDeflateBlockToFile(char *buffer, ULONG len, ULONG *compressedLength)
{
	lckDataFile.lock();
//	spdlog::info("Appending to Datafile after Encryption");

	// Acquire a mutex to ensure that only one thread is writing
	// goto end of file
	// get file position
	// compress the block 
	// write the block
	// update compressed length and return file position before writing
	long long endPos;
	time_t t1, t2;
	t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	fileobj.seekg(0, ios::end);
	fileobj.clear();
	endPos = fileobj.tellg();
	t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	long seekTime = (long)(t2 - t1);
	// compress the block
	ULONG sizeDataCompressed = (ULONG)(len * 1.1) + 12;

	t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	if (!zipMode)
	{
		sizeDataCompressed = LZ4_compressBound(len);
	}

	BYTE * dataCompressed = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeDataCompressed);

	if (zipMode)
	{
		//int z_result = 
		compress(

			dataCompressed,         // destination buffer,
									// must be at least
									// (1.01X + 12) bytes as large
									// as source.. we made it 1.1X + 12bytes

			&sizeDataCompressed,    // pointer to var containing
									// the current size of the
									// destination buffer.
									// WHEN this function completes,
									// this var will be updated to
									// contain the NEW size of the
									// compressed data in bytes.

			(Bytef *)buffer,           // source data for compression

			len);    // size of source data in bytes
	}
	else
	{
		sizeDataCompressed = LZ4_compress_default(buffer, (char *)dataCompressed, len, sizeDataCompressed);
	}
	t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	long compressTime = (long)(t2 - t1);

	t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	*compressedLength = sizeDataCompressed;
	fileobj.write((char *)dataCompressed, *compressedLength);
	HeapFree(GetProcessHeap(), 0, dataCompressed);
	t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	long writeTime = (long)(t2 - t1);

	char str[1000];
	sprintf_s(str, "SeekTime = %ld, CompressTime = %ld, WriteTime = %ld", seekTime, compressTime, writeTime);
	//spdlog::info(str);
	if (fileobj.fail())
		fileobj.clear();
	lckDataFile.unlock();
	return endPos;
}


void DataFile::ReadCompressedBlockFromFile(char *buffer, ULONG len, long long pos)
{
	lckDataFile.lock();
	//spdlog::info("Reading Encrypted Block from Datafile");
	fileobj.clear();
	fileobj.seekg(pos, ios::beg);
	fileobj.clear();
	fileobj.read((char *)buffer, len);
	if (fileobj.fail())
		fileobj.clear();
	lckDataFile.unlock();
}

long long DataFile::AppendCompressedBlockToFile(char *buffer, ULONG len)
{
	lckDataFile.lock();
	long long endPos;
	//spdlog::info("Append Encrypted Block from Datafile");

	fileobj.seekg(0, ios::end);
	fileobj.clear();
	endPos = fileobj.tellg();

	fileobj.write((char *)buffer, len);
	if (fileobj.fail())
		fileobj.clear();
	lckDataFile.unlock();
	return endPos;
}


void DataFile::ReadDeflateBlockFromFile(char *buffer, ULONG len, ULONG compressedLength, long long pos)
{
	lckDataFile.lock();
	//spdlog::info("Reading Block and Decrypt from Datafile");
	// Acquire a mutex to ensure that only one thread is writing
	// goto end of file
	// get file position
	// compress the block 
	// write the block
	// update compressed length and return file position before writing
	fileobj.clear();
	fileobj.seekg(pos, ios::beg);
	fileobj.clear();

	// compress the block
	ULONG sizeDataCompressed = compressedLength;
	BYTE * dataUnCompressed = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeDataCompressed);
	
	fileobj.read((char *)dataUnCompressed, compressedLength);

	if (zipMode)
	{
		//int z_result = 
		uncompress(

			(Bytef *)buffer,         // destination buffer,
									// must be at least
									// (1.01X + 12) bytes as large
									// as source.. we made it 1.1X + 12bytes

			&len,					// pointer to var containing
									// the current size of the
									// destination buffer.
									// WHEN this function completes,
									// this var will be updated to
									// contain the NEW size of the
									// compressed data in bytes.

			dataUnCompressed,           // source data for compression

			sizeDataCompressed);    // size of source data in bytes
	}
	else
	{
		//ULONG unCompressedSize = LZ4_decompress_safe((char *)dataUnCompressed, buffer, sizeDataCompressed, len);
		LZ4_decompress_safe((char*)dataUnCompressed, buffer, sizeDataCompressed, len);
	}
	HeapFree(GetProcessHeap(), 0, dataUnCompressed);
	if (fileobj.fail())
		fileobj.clear();
	lckDataFile.unlock();
}

void DataFile::ReadFromFile(char *buffer, ULONG len, long long pos)
{
	lckDataFile.lock();
	//spdlog::info("Reading Block from Datafile");

	// Acquire a mutex to ensure that only one thread is writing
	// goto end of file
	// get file position
	// write the block
	// return file position before writing
	//int errNum = 0;
	if (pos >= 0L)
	{
		fileobj.clear();
		fileobj.seekg(pos, ios::beg);
		fileobj.clear();
	}
	long long endPos;

	endPos = fileobj.tellg();
	fileobj.clear();
	fileobj.read(buffer, len);
	fileobj.clear();
	lckDataFile.unlock();
}

