#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <strsafe.h>

#define BUFSIZE MAX_PATH

using namespace std;
string SDate(unsigned short day, unsigned short month, unsigned short year) {
	string rs = std::to_string(day) + "th of "s;
	switch (month)
	{
	case 1:
		rs += "January";
		break;
	case 2:
		rs += "February";
		break;
	case 3:
		rs += "March";
		break;
	case 4:
		rs += "April";
		break;
	case 5:
		rs += "May";
		break;
	case 6:
		rs += "June";
		break;
	case 7:
		rs += "July";
		break;
	case 8:
		rs += "August";
		break;
	case 9:
		rs += "September";
		break;
	case 10:
		rs += "October";
		break;
	case 11:
		rs += "November";
		break;
	case 12:
		rs += "December";
		break;
	default:
		rs += "???";
		break;
	}
	rs += " "s + std::to_string(year);
	return rs;
}
string STime(unsigned short hour, unsigned short minute, unsigned short second) {
	return std::string(std::to_string(hour) + ":"s + std::to_string(minute) + ":"s + std::to_string(second));
}
void CurrentDirectory(TCHAR* Path) {
	TCHAR Buffer[BUFSIZE];
	DWORD dwRet;
	dwRet = GetCurrentDirectory(BUFSIZE, Buffer);
	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		return;
	}
	if (dwRet > BUFSIZE)
	{
		printf("Buffer too small; need %d characters\n", dwRet);
		return;
	}
	for (int i = 0; i < BUFSIZE; ++i) {
		Path[i] = Buffer[i];
	}
}
int fileExists(TCHAR* file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		FindClose(handle);
	}
	return found;
}
int main()
{
#pragma region Variables
	char mode;
	bool tr = true;
	TCHAR Path[BUFSIZE];
	string new_Path;
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	SYSTEMTIME lt;
	FILETIME st;
#pragma endregion Variables
	cout << "Welcome to the Labwork_2_2_File_Manager!\n";
	cout << "________________________________________\n";
	do
	{
		CurrentDirectory(Path);
		_tprintf(TEXT("Current directory: %s\n"), Path);
		cout << "Commands:\n1 - change directory;\n2 - list files in current directory;\n3 - copy file;\n4 - create directory;\n5 - delete file (empty directory);\n6 - extended file info;\n0 - close program;\n";
		cin >> mode;
		switch (mode)
		{
		case '0':
			tr = false;
			break;
		case '1': {
			cout << "Change directory to: ";
			getline(cin, new_Path);
			getline(cin, new_Path);
			wstring stemp = wstring(new_Path.begin(), new_Path.end());
			LPCWSTR sw = stemp.c_str();
			if (!SetCurrentDirectory(sw))
			{
				printf("SetCurrentDirectory failed (%d)\n", GetLastError());
			}
			break;
		}
		case '2': {
			StringCchCat(Path, MAX_PATH, TEXT("\\*"));
			hFind = FindFirstFile(Path, &ffd);
			if (INVALID_HANDLE_VALUE == hFind)
			{
				cout << "An error has occurred.";
			}
			else {
				do
				{
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
					}
					else
					{
						filesize.LowPart = ffd.nFileSizeLow;
						filesize.HighPart = ffd.nFileSizeHigh;
						FileTimeToLocalFileTime(&ffd.ftCreationTime, &st);
						FileTimeToSystemTime(&st, &lt);
						_tprintf(TEXT("  %s   %ld bytes   "), ffd.cFileName, filesize.QuadPart);
						cout << SDate(lt.wDay, lt.wMonth, lt.wYear) << " " << STime(lt.wHour, lt.wMinute, lt.wSecond) << endl;
					}
				} while (FindNextFile(hFind, &ffd) != 0);

				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					cout << "An error has occurred.";
				}
				FindClose(hFind);
			}
			break;
		}
		case '3': {
			cout << "Copy file (path): ";
			getline(cin, new_Path);
			getline(cin, new_Path);
			wstring stemp1 = wstring(new_Path.begin(), new_Path.end());
			LPCWSTR source = stemp1.c_str();
			cout << " to ";
			getline(cin, new_Path);
			wstring stemp = wstring(new_Path.begin(), new_Path.end());
			LPCWSTR destination = stemp.c_str();
			if (CopyFile(source, destination, 1) == 0) {
				cout << "An error has occurred.\n";
			}
			break;
		}
		case '4': {
			cout << "Directory (path): ";
			getline(cin, new_Path);
			getline(cin, new_Path);
			wstring stemp = wstring(new_Path.begin(), new_Path.end());
			LPCWSTR destination = stemp.c_str();
			if (CreateDirectoryW(destination, NULL) == 0) {
				if (GetLastError() == ERROR_ALREADY_EXISTS) {
					cout << "The specified directory already exists." << endl;
				}
				else {
					cout << "One or more intermediate directories do not exist" << endl;
				}
			}
			break;
		}
		case '5': {
			cout << "Path: ";
			getline(cin, new_Path);
			getline(cin, new_Path);
			wstring stemp = wstring(new_Path.begin(), new_Path.end());
			LPCWSTR destination = stemp.c_str();
			DWORD ftyp = GetFileAttributesW(destination);
			if (ftyp == INVALID_FILE_ATTRIBUTES) {
				cout << "An error has occurred." << endl;
			}
			else if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
				if (RemoveDirectoryW(destination) == 0) {
					cout << "An error has occurred." << endl;
				}
			}
			else {
				if (DeleteFileW(destination) == 0) {
					if (GetLastError() == ERROR_FILE_NOT_FOUND) {
						cout << "Requested file wasn't found.\n";
					}
					else {
						cout << "Requested file is inaccessible.\n";
					}
				}
			}
			break;
		}
		case '6': {
			cout << "File name (path): ";
			getline(cin, new_Path);
			getline(cin, new_Path);
			wstring stemp = wstring(new_Path.begin(), new_Path.end());
			LPCWSTR destination = stemp.c_str();
			hFind = FindFirstFileW(destination, &ffd);
			if (INVALID_HANDLE_VALUE == hFind)
			{
				cout << "An error has occurred." << endl;
			}
			else {
				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				_tprintf(TEXT("Name: %s\n Size: %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
				FileTimeToLocalFileTime(&ffd.ftCreationTime, &st);
				FileTimeToSystemTime(&st, &lt);
				cout << "Creation Time: ";
				cout << SDate(lt.wDay, lt.wMonth, lt.wYear) << " " << STime(lt.wHour, lt.wMinute, lt.wSecond) << endl;
				FileTimeToLocalFileTime(&ffd.ftLastAccessTime, &st);
				FileTimeToSystemTime(&st, &lt);
				cout << "Last Access Time: ";
				cout << SDate(lt.wDay, lt.wMonth, lt.wYear) << " " << STime(lt.wHour, lt.wMinute, lt.wSecond) << endl;
				FileTimeToLocalFileTime(&ffd.ftLastWriteTime, &st);
				FileTimeToSystemTime(&st, &lt);
				cout << "Last Write Time: ";
				cout << SDate(lt.wDay, lt.wMonth, lt.wYear) << " " << STime(lt.wHour, lt.wMinute, lt.wSecond) << endl;
				cout << "File Attributes: " << ffd.dwFileAttributes << endl;
			}
			break;
		}
		default:
			tr = false;
			break;
		}
	} while (tr);
}