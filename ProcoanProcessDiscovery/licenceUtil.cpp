#include "stdafx.h"
#include "licenceUtil.h"
#include "encrypt.h"
#include "Utility.h"
#include <chrono>
#include <ctime>
using namespace std;
using namespace std::chrono;


void ClearRegistryLicense()
{
	RegDeleteKey(HKEY_CURRENT_USER, (PWCHAR)L"NXTWORX");
}


bool ValidateRegistryLicense()
{
	DWORD len = 0;
	BOOL status = FALSE;

	status = readDwordValueRegistry(HKEY_CURRENT_USER, (PWCHAR)L"NXTWORX", (PWCHAR)L"Size", &len); //read dword
	if (status != TRUE)
		return false;

	//system_clock::time_point now = system_clock::now();
	//time_t tt = system_clock::to_time_t(now);
	//tm utc_tm = *gmtime(&tt);
	//tm local_tm = *localtime(&tt);
	//char str[100];
	//sprintf_s(str, "%04d%02d%02d", local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday);
	//string currDate = str;
	//char strDate[100] = { '2'+15,'0'+20,'2'+25,'0'+20,'0'+15,'3'+10,'1'+5,'5'+10,0 };
	//char strNewData[10];
	//strNewData[0] = strDate[0] - 15;
	//strNewData[1] = strDate[1] - 20;
	//strNewData[2] = strDate[2] - 25;
	//strNewData[3] = strDate[3] - 20;
	//strNewData[4] = strDate[4] - 15;
	//strNewData[5] = strDate[5] - 10;
	//strNewData[6] = strDate[6] - 5;
	//strNewData[7] = strDate[7] - 10;
	//strNewData[8] = '\0';

	//if (currDate > strNewData)
	//	return false;

	string licRegKey = readUserInfoFromRegistry(HKEY_CURRENT_USER, (PWCHAR)L"NXTWORX", (PWCHAR)L"Value", len);
	string pKey = "78502D1BB583@00804819G83348DG09E";

	string primaryKey = DecryptPrimaryKeys(pKey);
	string encryptedRegKey = DecryptPrimaryKeys(licRegKey);
	string finalLicKey = decrypt(encryptedRegKey, primaryKey);

	string fileDateVal, licKeyVal, licStartDate = "00000000", licEndDate = "00000000", licMainDate = "00000000", function;

	if (finalLicKey.size() >= 12)
		fileDateVal = finalLicKey.substr(0, 12);
	if (finalLicKey.size() >= 32)
		licKeyVal = finalLicKey.substr(12, 32);
	if (finalLicKey.size() >= 44)
		licStartDate = finalLicKey.substr(44, 8);
	if (finalLicKey.size() >= 52)
		licEndDate = finalLicKey.substr(52, 8);
	if (finalLicKey.size() >= 60)
		licMainDate = finalLicKey.substr(60, 8);
	if (finalLicKey.size() >= 68)
		function = finalLicKey.substr(68, 8);

	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);
	char strDate[10];

	int year = ltm.tm_year + 1900;
	int month = ltm.tm_mon + 1;
	int day = ltm.tm_mday;

	sprintf_s(strDate, "%04d%02d%02d", year, month, day);
	string todayDate = strDate;
	string startDate = licStartDate.substr(4, 4) + licStartDate.substr(2, 2) + licStartDate.substr(0, 2);
	string endDate = licEndDate.substr(4, 4) + licEndDate.substr(2, 2) + licEndDate.substr(0, 2);
	string licDate = licMainDate.substr(4, 4) + licMainDate.substr(2, 2) + licMainDate.substr(0, 2);
	//HLH2EbTTNuJG@Hm52ukptWpyu | I:rP`BW0@M4[mlDg`gLAN6HE:qNOPoCE1pFAd`KNFjIST9woRusJ4F[qn4L`8
	//HLH2EbTTNuJG@Hm52ukptWpyu|I:rP`BW0@M4[mlDg`gLAN6HE:qNOPoCE1pFAd`KNFjIST9woRusJ4F[qn4L`8
	bool bValid = true;
	if (todayDate > "20210831")
		bValid = false;
	if (licDate > todayDate)
		bValid = false;
	if (startDate > endDate)
		bValid = false;
	if (startDate < licDate)
		bValid = false;
	if (endDate < todayDate)
		bValid = false;

	wstring cwd = getexepath();

	WIN32_FILE_ATTRIBUTE_DATA fInfo;
	GetFileAttributesEx(cwd.c_str(), GetFileExInfoStandard, &fInfo);
	SYSTEMTIME sysTime;
	char str[100];

	FileTimeToSystemTime(&fInfo.ftCreationTime, &sysTime);
	sprintf_s(str, "%04d%02d%02d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);
	string strFileInfo = str;
	if (strFileInfo != fileDateVal)
		bValid = false;

	return bValid;
}


string CreateLicenceKey(string sPrimaryKey, string sKey, string startDate, string endDate, string lDate, string functFlgs)
{

	string licKey = sKey + startDate + endDate + lDate + functFlgs;

	string strEncryptedKey = encrypt(licKey, sPrimaryKey);
	string strEncryptedKeyInPrimary = EncryptPrimaryKeys(strEncryptedKey);

	return strEncryptedKeyInPrimary;
}


bool ValidateMainLicenseKey(string pKey, string lKey)
{
	string primaryKey = DecryptPrimaryKeys(pKey);

	string strLicKey = DecryptPrimaryKeys(lKey);
	string finalLicKey = decrypt(strLicKey, primaryKey);

	string licKeyVal = finalLicKey.substr(0, 32);
	string licStartDate = finalLicKey.substr(32, 8);
	string licEndDate = finalLicKey.substr(40, 8);
	string licMainDate = finalLicKey.substr(48, 8);
	string function = finalLicKey.substr(56, 8);

	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);
	char strDate[10];

	int year = ltm.tm_year + 1900;
	int month = ltm.tm_mon + 1;
	int day = ltm.tm_mday;

	sprintf_s(strDate, "%04d%02d%02d", year, month, day);
	string todayDate = strDate;
	string startDate = licStartDate.substr(4, 4) + licStartDate.substr(2, 2) + licStartDate.substr(0, 2);
	string endDate = licEndDate.substr(4, 4) + licEndDate.substr(2, 2) + licEndDate.substr(0, 2);
	string licDate = licMainDate.substr(4, 4) + licMainDate.substr(2, 2) + licMainDate.substr(0, 2);

	const char* keyList[] = {
		"605E9C9C0F8449F1921A2821315D99E1",
		"794C2E94CB0948938740732FE68F9C06",
		"11e59ea101bb4fb1b2e355b571a3f167",
		"2939c77c1f214a0b9b82a4a50e9f1230",
		"67edef4ba1864258985eeee1318bbaeb",
		"5867be4fcd954bb0b478ede7f930d49e",
		"ee62abb7280e4a03b23db59e4790a972",
		"30546142fadb421b900ff1fe155fa364",
		"d2f947e0288d48ad8eb15f435e07804b",
		"0e8ec65923764d82acbd6d69ed84dde8",
		"fd519cc5299c47a9a3992640c6957457",
		"08c9321143c944f8a18ed307cc2ed9b2",
		"5e72b4545e334fe39c1418532e8a53d6",
		"f1920a06ce22449890363891642cc36c",
		"1617625ed8f4423fbb2ff8f9d6fb9676",
		"5ad95d9ed4f447c18143f45b13b60cf7",
		"1215648770484824afe4957a17397151",
		"50216e03106a43f1a3ed346cfc4f54d4",
		"c05bd0e97de641aebcb36b05747ef78c",
		"3537e43fa4ce48d2ad02c82ecc92b7bd",
		"117e3fa40eed4ed89f1d610ea0a11566",
		"7c123be3cf8e4b0e9e59098439dd5726",
		"a6fa53163ef24f07b31af4185f64ec44",
		"0b87ea7365804cb2a35f53a915c75cac",
		"a04a89d7a5f74c8bb78a35f3d25c7e3e",
		"02e75de49a2c4f7183e44ce95c279802",
		"173df7630af941f3be9dff4b9cb48e77",
		"1665f6c763d24ef585fafe8a386eecac",
		"3d22edb0104c42d7a6ca5f12d4323648",
		"405803b71ec54a50920a833b3b3c3ba3",
		"f4e66f57d8de405787c7191baa54edf5",
		"dc94b3f596f0445580a46902fde3c7de",
		NULL
		};


	bool bFound = false;

	for (int i = 0; keyList[i] !=  NULL; i++)
	{
		if (!strcmp(keyList[i], licKeyVal.c_str()))
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
		return false;
	if (licDate > todayDate)
		return false;
	if (startDate > endDate)
		return false;
	if (startDate < licDate)
		return false;
	if (endDate < todayDate)
		return false;

	return true;
}

bool ValidateMainLicense(string lKey)
{
	string primaryKey = "78502D1BB583@00804819G83348DG09E";
	string strLicKey = lKey;

	bool IsValid = ValidateMainLicenseKey(primaryKey, strLicKey);
	if (IsValid)
	{
		string strRegistryKey = CreateRegistryKey(primaryKey, strLicKey);
		DWORD len = strRegistryKey.size();

		BOOL status = FALSE;

		status = CreateRegistryKey(HKEY_CURRENT_USER, (PWCHAR)L"NXTWORX"); //create key
		if (status != TRUE)
			return false;

		wstring wStringVal;


		status = WriteDwordInRegistry(HKEY_CURRENT_USER, (PWCHAR)L"NXTWORX", (PWCHAR)L"Size", (DWORD)len); //write dword
		if (status != TRUE)
			return false;

		wStringVal = wstring(strRegistryKey.begin(), strRegistryKey.end());
		status = writeStringInRegistry(HKEY_CURRENT_USER, (PWCHAR)L"NXTWORX", (PWCHAR)L"Value", (PWCHAR)wStringVal.c_str()); //write string
		if (status != TRUE)
			return false;

	}
	return IsValid;
}


string CreateRegistryKey(string pKey, string lKey)
{
	string primaryKey = DecryptPrimaryKeys(pKey);

	string strLicKey = DecryptPrimaryKeys(lKey);
	string finalLicKey = decrypt(strLicKey, primaryKey);

	wstring cwd = getexepath();

	WIN32_FILE_ATTRIBUTE_DATA fInfo;
	GetFileAttributesEx(cwd.c_str(), GetFileExInfoStandard, &fInfo);
	SYSTEMTIME sysTime;
	char str[100];

	FileTimeToSystemTime(&fInfo.ftCreationTime, &sysTime);
	sprintf_s(str, "%04d%02d%02d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);
	string strFileInfo = str;

	string licKey = strFileInfo + finalLicKey;

	string strEncryptedKey = encrypt(licKey, primaryKey);
	string strEncryptedKeyInPrimary = EncryptPrimaryKeys(strEncryptedKey);

	return strEncryptedKeyInPrimary;
}


string EncryptPrimaryKeys(string val)
{
	string strEncryptXOR = encryptDecryptXor(val);
	string strExcryptRailFence = encryptRailFence(strEncryptXOR, 4);
	return strExcryptRailFence;
}

string DecryptPrimaryKeys(string val)
{
	string strDecryptRailFence = decryptRailFence(val, 4);
	string strDecryptXOR = encryptDecryptXor(strDecryptRailFence);
	return strDecryptXOR;
}

// function to encrypt a message 
string encryptRailFence(string text, int key)
{
	// create the matrix to cipher plain text 
	// key = rows , length(text) = columns 
	char rail[10][200];
	int size = text.size();

	// filling the rail matrix to distinguish filled 
	// spaces from blank ones 
	for (int i = 0; i < key; i++)
		for (int j = 0; j < size; j++)
			rail[i][j] = '\n';

	// to find the direction 
	bool dir_down = false;
	int row = 0, col = 0;

	for (int i = 0; i < size; i++)
	{
		// check the direction of flow 
		// reverse the direction if we've just 
		// filled the top or bottom rail 
		if (row == 0 || row == key - 1)
			dir_down = !dir_down;

		// fill the corresponding alphabet 
		rail[row][col++] = text[i];

		// find the next row using direction flag 
		dir_down ? row++ : row--;
	}

	//now we can construct the cipher using the rail matrix 
	string result;
	for (int i = 0; i < key; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (rail[i][j] != '\n')
				result.push_back(rail[i][j]);
		}
	}

	return result;
}

// This function receives cipher-text and key 
// and returns the original text after decryption 
string decryptRailFence(string cipher, int key)
{
	// create the matrix to cipher plain text 
	// key = rows , length(text) = columns 
	int size = cipher.size();
	char rail[10][200];

	// filling the rail matrix to distinguish filled 
	// spaces from blank ones 
	for (int i = 0; i < key; i++)
		for (int j = 0; j < size; j++)
			rail[i][j] = '\n';
	// to find the direction 
	bool dir_down = false;

	int row = 0, col = 0;

	// mark the places with '*' 
	for (int i = 0; i < size; i++)
	{
		// check the direction of flow 
		if (row == 0)
			dir_down = true;
		if (row == key - 1)
			dir_down = false;

		// place the marker 
		rail[row][col++] = '*';

		// find the next row using direction flag 
		dir_down ? row++ : row--;
	}

	// now we can construct the fill the rail matrix 
	int index = 0;
	for (int i = 0; i < key; i++)
		for (int j = 0; j < size; j++)
			if (rail[i][j] == '*' && index < cipher.length())
				rail[i][j] = cipher[index++];
	// now read the matrix in zig-zag manner to construct 
  // the resultant text 
	string result;

	row = 0, col = 0;
	for (int i = 0; i < size; i++)
	{
		// check the direction of flow 
		if (row == 0)
			dir_down = true;
		if (row == key - 1)
			dir_down = false;

		// place the marker 
		if (rail[row][col] != '*')
			result.push_back(rail[row][col++]);

		// find the next row using direction flag 
		dir_down ? row++ : row--;
	}
	return result;
}


string encryptDecryptXor(string inpString)
{
	// Define XOR key 
	// Any character value will work 
	char xorKey = 1;
	string result = inpString;
	// calculate length of input string 
	int len = result.size();

	// perform XOR operation of key 
	// with every caracter in string 
	for (int i = 0; i < len; i++)
	{
		result[i] = result[i] ^ xorKey;
	}
	return result;
}


//Create key in registry
BOOL CreateRegistryKey(HKEY hKeyParent, PWCHAR subkey)
{
	DWORD dwDisposition; //It verify new key is created or open existing key
	HKEY  hKey;
	DWORD Ret;


	Ret =
		RegCreateKeyEx(
			hKeyParent,
			subkey,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hKey,
			&dwDisposition);

	if (Ret != ERROR_SUCCESS)
	{
		printf("Error opening or creating key.\n");
		return FALSE;
	}

	RegCloseKey(hKey);
	return TRUE;
}

//Write data in registry
BOOL WriteDwordInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, DWORD data)
{
	DWORD Ret;
	HKEY hKey;


	//Open the key
	Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_WRITE,
		&hKey
	);

	if (Ret == ERROR_SUCCESS)
	{

		//Set the value in key
		if (ERROR_SUCCESS !=
			RegSetValueEx(
				hKey,
				valueName,
				0,
				REG_DWORD,
				reinterpret_cast<BYTE *>(&data),
				sizeof(data)))
		{
			RegCloseKey(hKey);
			return FALSE;
		}

		//close the key
		RegCloseKey(hKey);

		return TRUE;
	}

	return FALSE;
}

//Read data from registry
BOOL readDwordValueRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, DWORD *readData)
{

	HKEY hKey;
	DWORD Ret;

	//Check if the registry exists
	Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_READ,
		&hKey
	);

	if (Ret == ERROR_SUCCESS)
	{

		DWORD data;
		DWORD len = sizeof(DWORD);//size of data

		Ret = RegQueryValueEx(
			hKey,
			valueName,
			NULL,
			NULL,
			(LPBYTE)(&data),
			&len
		);

		if (Ret == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			(*readData) = data;
			return TRUE;
		}

		RegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//Write range and type into the registry
BOOL writeStringInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR strData)
{
	DWORD Ret;
	HKEY hKey;

	//Check if the registry exists
	Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_WRITE,
		&hKey
	);

	if (Ret == ERROR_SUCCESS)
	{
		if (ERROR_SUCCESS !=
			RegSetValueEx(
				hKey,
				valueName,
				0,
				REG_SZ,
				(LPBYTE)(strData),
				((((DWORD)lstrlen(strData) + 1)) * 2)))
		{
			RegCloseKey(hKey);
			return FALSE;
		}

		RegCloseKey(hKey);
		return TRUE;
	}

	return FALSE;
}


//read customer infromation from the registry
string readUserInfoFromRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, DWORD len)
{
	HKEY hKey;
	DWORD readDataLen = (len + 1) * sizeof(WCHAR);

	PWCHAR readBuffer = (PWCHAR)malloc(readDataLen);
	if (readBuffer == NULL)
		return FALSE;

	//Check if the registry exists
	DWORD Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_READ,
		&hKey
	);

	if (Ret == ERROR_SUCCESS)
	{

		Ret = RegQueryValueEx(
			hKey,
			valueName,
			NULL,
			NULL,
			(BYTE*)readBuffer,
			&readDataLen
		);

		if (Ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return string("");;
		}

		wstring keyString = readBuffer;
		RegCloseKey(hKey);
		delete readBuffer;
		return string(keyString.begin(), keyString.end());
	}
	else
	{
		delete readBuffer;
		return string("");
	}
}

