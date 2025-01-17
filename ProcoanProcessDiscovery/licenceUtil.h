#pragma once
#include <string>

using namespace std;

string encryptRailFence(string text, int key);
string decryptRailFence(string cipher, int key);
string encryptDecryptXor(string inpString);

string DecryptPrimaryKeys(string val);
string EncryptPrimaryKeys(string val);

BOOL CreateRegistryKey(HKEY hKeyParent, PWCHAR subkey);
BOOL WriteDwordInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, DWORD data);
BOOL readDwordValueRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, DWORD *readData);
BOOL writeStringInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR strData);
string readUserInfoFromRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, DWORD len);
string CreateLicenceKey(string sPrimaryKey, string sKey, string startDate, string endDate, string lDate, string functFlgs);
bool ValidateMainLicenseKey(string pKey, string lKey);
string CreateRegistryKey(string pKey, string lKey);
bool ValidateMainLicense(string lKey);
bool ValidateRegistryLicense();
void ClearRegistryLicense();
