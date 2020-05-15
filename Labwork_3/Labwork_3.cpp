#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <atlstr.h>
#include <string>
#include "winreg.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

typedef struct KEY_DATA {
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 
} KEY_DATA, *PKEY_DATA;

using namespace std;
void menu_1();
void menu_2();
bool getKey(HKEY** hKey, LPSTR keyPath, REGSAM samDesired);
void ListSubkeys(HKEY hKey);
void ListParameters(HKEY hKey);
bool findString(HKEY hKey, string search_s, LPSTR keyPath);
void SaveToFile(HKEY hKey);
BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
int main()
{
	enum program_mode { cs_exit, cs_subkeys, cs_keyparams, cs_subsearch, cs_tofile };
	int mode;
	bool end_program_trigger = false;

	HKEY hKey;
	PHKEY phKey = &hKey;
	CHAR keyPath[MAX_PATH];

	do
	{
		menu_1();
		cin >> mode;
		switch (mode)
		{
		case cs_exit: {
			end_program_trigger = true;
			break;
		}
		case cs_subkeys: {
			if (getKey(&phKey, keyPath, KEY_READ)) {
				ListSubkeys(hKey);
			}
			else
			{
				cout << "No such key!" << endl;
			}
			break;
		}
		case cs_keyparams: {
			if (getKey(&phKey, keyPath, KEY_READ)) {
				ListParameters(hKey);
			}
			else
			{
				cout << "No such key!" << endl;
			}
			break;
		}
		case cs_subsearch: {
			if (getKey(&phKey, keyPath, KEY_READ))
			{
				string req_str = "";
				cout << "Input string: ";
				getline(cin, req_str);
				findString(hKey, req_str, keyPath);
			}
			else
			{
				cout << "No such key!" << endl;
			}
			break;
		}
		case cs_tofile: {
			
			HANDLE processToken;
			//get access token 
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &processToken))
			{
				cout << "Cant get access rights "<< endl;
			}
			else {
				// set privilege
				if (SetPrivilege(processToken, SE_BACKUP_NAME, TRUE))
				{
					if (getKey(&phKey, keyPath, KEY_READ)) {
						SaveToFile(hKey);
					}
					else
					{
						cout << "No such key!" << endl;
					}
				}
			}
			break;
		}
		default:
		{
			cout << "Oops! No such operation. Try again.\n";
			break;
		}
		}

	} while (!end_program_trigger);
}
//Purely MSDN function
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}
bool getKey(HKEY** hKey, LPSTR keyPath, REGSAM samDesired)
{
	HKEY rootKey;
	enum root_keys { cs_exit, classes_root, current_user, local_machine, users, current_config, performance_data };

	int mode = 0;

	memset(keyPath, '\0', sizeof(keyPath));

	menu_2();
	cin >> mode;
	switch (mode)
	{
	case classes_root:
	{
		rootKey = HKEY_CLASSES_ROOT;
		strcpy(keyPath, "HKEY_CLASSES_ROOT\\");
		break;
	}
	case current_user:
	{
		rootKey = HKEY_CURRENT_USER;
		strcpy(keyPath, "HKEY_CURRENT_USER\\");
		break;
	}
	case local_machine:
	{
		rootKey = HKEY_LOCAL_MACHINE;
		strcpy(keyPath, "HKEY_LOCAL_MACHINE\\");
		break;
	}
	case users:
	{
		rootKey = HKEY_USERS;
		strcpy(keyPath, "HKEY_USERS\\");
		break;
	}
	case current_config:
	{
		rootKey = HKEY_CURRENT_CONFIG;
		strcpy(keyPath, "HKEY_CURRENT_CONFIG\\");
		break;
	}
	case 6:
	{
		rootKey = HKEY_PERFORMANCE_DATA;
		strcpy(keyPath, "HKEY_PERFORMANCE_DATA\\");
		break;
	}
	default:
		return false;
	}
	string destKey = "";

	cout << "Please, input required subkey path (to list root key subkeys press ENTER):\n";
	getline(cin, destKey);
	getline(cin, destKey);
	LPSTR destination_key = const_cast<char *>(destKey.c_str());
	if (RegOpenKeyExA(rootKey, destination_key, 0, samDesired, *hKey) == ERROR_SUCCESS)
	{
		strcat(keyPath, destination_key);
		return true;
	}
	return false;
}
void menu_1() {
	cout << "Hi!\nTo print a list of keys' subkeys press '1'.\n";
	cout << "To print a list of keys' parameters with values and types press '2'.\n";
	cout << "To search for a string in keys' names, params names and its' values press '3'.\n";
	cout << "To save a key to a file press '4'.\n\n";
	cout << "Press '0' to exit.\n";
}
void menu_2() {
	cout << "Choose yourself a root key!:\n";
	cout << "Press '1' - HKEY_CLASSES_ROOT;\n";
	cout << "Press '2' - HKEY_CURRENT_USER;\n";
	cout << "Press '3' - HKEY_LOCAL_MACHINE;\n";
	cout << "Press '4' - HKEY_USERS;\n";
	cout << "Press '5' - HKEY_CURRENT_CONFIG;\n";
	cout << "Press '6' - HKEY_PERFORMANCE_DATA;\n";
	cout << "Press '0' to abort operation;\n";
}
bool GetKeyData(HKEY key, KEY_DATA * keyData)
{
	DWORD retCode = RegQueryInfoKey(key,
		(*keyData).achClass,
		&(*keyData).cchClassName,
		NULL,
		&(*keyData).cSubKeys,
		&(*keyData).cbMaxSubKey,
		&(*keyData).cchMaxClass,
		&(*keyData).cValues,
		&(*keyData).cchMaxValue,
		&(*keyData).cbMaxValueData,
		&(*keyData).cbSecurityDescriptor,
		&(*keyData).ftLastWriteTime);
	if (retCode == ERROR_SUCCESS)
		return true;
	else
		return false;
}
#pragma region RegistryAccess
void SaveToFile(HKEY hKey) {

	string file_path = "";
	DWORD retCode = ERROR_SUCCESS;

	cout << "Input file path:\n";
	getline(cin, file_path);

	retCode = RegSaveKeyA(hKey, file_path.c_str(), NULL);

	switch (retCode)
	{
	case ERROR_SUCCESS:
	{
		cout << "Saved into file:\n" << file_path << endl;
		break;
	}
	case ERROR_ALREADY_EXISTS:
	{
		cout << "Oops! File already exists!\n" << endl;
		break;
	}
	default:
		cout << "Oops! Cant save key into file\n" << endl;
		break;
	}
	RegCloseKey(hKey);
}
void ListSubkeys(HKEY hKey)
{
	KEY_DATA keyData = {};
	GetKeyData(hKey, &keyData);

	if (keyData.cSubKeys)
	{
		printf("\nNumber of subkeys: %d\n", keyData.cSubKeys);
		for (size_t i = 0; i < keyData.cSubKeys; i++)
		{
			keyData.cbName = MAX_KEY_LENGTH;

			DWORD retCode = RegEnumKeyEx(hKey,
				i,
				keyData.achKey,
				&keyData.cbName,
				NULL,
				NULL,
				NULL,
				NULL);
			if (retCode == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), i + 1, keyData.achKey);
			}
		}
	}
	else {
		cout << "No subkeys to list.\n";
	}
	RegCloseKey(hKey);
}
bool findString(HKEY hKey, string search_s, LPSTR keyPath)
{
	LPSTR request_string = const_cast<char*>(search_s.c_str());
	KEY_DATA keyData = {};
	DWORD retCode = ERROR_SUCCESS;
	LPSTR subkeyPath;
	if (!GetKeyData(hKey, &keyData))
	{
		return false;
	}

	if (keyData.cSubKeys)
	{
		for (size_t i = 0; i < keyData.cSubKeys; i++)
		{
			keyData.cbName = MAX_KEY_LENGTH;

			retCode = RegEnumKeyEx(hKey,
				i,
				keyData.achKey,
				&keyData.cbName,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{

				wstring wide_string = wstring(search_s.begin(), search_s.end());
				if (_tcscmp(keyData.achKey, wide_string.c_str()) == 0)
				{
					cout << "Found in subkey's name: " << request_string << endl;
				}

				subkeyPath = (LPSTR)malloc(MAX_VALUE_NAME * sizeof(TCHAR));
				wstring s = keyData.achKey;
				string b = string(s.begin(), s.end());
				strcpy(subkeyPath, const_cast<char*>(b.c_str()));
				strcat(subkeyPath, "\\");
				strcat(subkeyPath, request_string);

				HKEY subKey = {};

				if (RegOpenKeyEx(hKey, keyData.achKey, 0, KEY_READ, &subKey) == ERROR_SUCCESS)
				{
					findString(subKey, request_string, subkeyPath);
				}
			}
		}
	}

	if (keyData.cValues)
	{
		CHAR achValue[MAX_VALUE_NAME];
		DWORD cchValue = MAX_VALUE_NAME;
		DWORD dwType = 0;
		LPBYTE lpData = NULL;
		DWORD dwData = 0;


		for (int i = 0; i < keyData.cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			//get data size for allocation
			retCode = RegEnumValueA(hKey, i, achValue, &cchValue, NULL, NULL, NULL, &dwData);
			lpData = (LPBYTE)malloc((dwData + 1) * sizeof(BYTE));
			cchValue = keyData.cchMaxValue + 1;
			retCode = RegEnumValueA(hKey, i,
				achValue,
				&cchValue,
				NULL,
				&dwType,
				lpData,
				&dwData);

			if (retCode == ERROR_SUCCESS)
			{
				if (_strcmpi(achValue, request_string) == 0)
				{
					cout << "Found in value's name: " << keyPath << "; " << achValue << endl;
				}
				if (((dwType & REG_EXPAND_SZ) == REG_EXPAND_SZ) || ((dwType & REG_SZ) == REG_SZ))
				{
					if (_strcmpi((LPSTR)lpData, request_string) == 0)
					{
						cout << "Found in value's " << keyPath << " data; " << achValue << ";\nData:" << lpData << endl;
					}
				}
			}
		}
	}
	RegCloseKey(hKey);
}
void ListParameters(HKEY hKey)
{
	DWORD retCode = ERROR_SUCCESS;
	DWORD i = 0;
	CHAR achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;
	DWORD dwType = 0;
	LPBYTE lpData = NULL;
	DWORD dwData = 0;

	KEY_DATA keyData = {};
	GetKeyData(hKey, &keyData);

	if (keyData.cValues)
	{
		printf("\nNumber of values: %d\n", keyData.cValues);

		for (i = 0, retCode = ERROR_SUCCESS; i < keyData.cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			//get data size for allocation
			retCode = RegEnumValueA(hKey, i, achValue, &cchValue, NULL, NULL, NULL, &dwData);
			lpData = (LPBYTE)malloc((dwData + 1) * sizeof(BYTE));
			cchValue = keyData.cchMaxValue + 1;
			retCode = RegEnumValueA(hKey, i,
				achValue,
				&cchValue,
				NULL,
				&dwType,
				lpData,
				&dwData);

			if (retCode == ERROR_SUCCESS)
			{
				printf("(%d) %s\n", i + 1, achValue);
				switch (dwType)
				{
				case REG_BINARY:
				{
					printf("Value type: REG_BINARY\nValue data: binary\n");
					break;
				}
				case REG_DWORD:
				{
					printf("Value type: REG_DWORD\nValue data: %#x|%u\n", *(DWORD*)(lpData), *(DWORD*)(lpData));
					break;
				}
				case REG_EXPAND_SZ:
				{
					printf("Value type: REG_EXPAND_SZ\nValue data: %s\n", lpData);
					break;
				}
				case REG_LINK:
				{
					LPCWCHAR data = reinterpret_cast<wchar_t*>(lpData);
					wprintf(L"Value type: REG_LINK\nValue data: %ws\n", data);
					break;
				}
				case REG_SZ:
				{
					printf("Value type: REG_SZ\nValue data: %s\n", lpData);
					break;
				}
				case REG_NONE:
				{
					printf("Value type: REG_NONE\nValue data: %x\n", *(DWORD*)(lpData));
					break;
				}
				default:
					printf("Value type: ???\nValue data: %x\n", *(DWORD*)(lpData));
					break;
				}
			}
			free(lpData);
		}
	}
	else {
		cout << "No parameters to list.\n";
	}
	RegCloseKey(hKey);

}
#pragma endregion 