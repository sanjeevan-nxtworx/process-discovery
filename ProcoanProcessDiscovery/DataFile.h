#pragma once
#include <iostream>
#include <fstream>

using namespace std;

class DataFile
{
	fstream fileobj;
	bool zipMode;

public:
	void ReadDeflateBlockFromFile(char *buffer, ULONG len, ULONG compressedLength, long long pos);
	void ReadCompressedBlockFromFile(char *buffer, ULONG len, long long pos);
	void ReadFromFile(char *buffer, ULONG len, long long pos);

	long long AppendDeflateBlockToFile(char *buffer, ULONG len, ULONG *compressedLength);
	long long AppendCompressedBlockToFile(char *buffer, ULONG len);
	long long AppendToFile(char *buffer, ULONG len);
	void SetZipMode(bool bFlg);
	void WriteToFile(long long fPos, char *buffer, ULONG len);
	DataFile(char *filename);
	DataFile(char *filename, int createFlg);
	~DataFile();
};

