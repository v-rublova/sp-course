#include "pch.h"
#include <windows.h> 
#include <stdio.h> 
#include <iostream>
#include <vector>

#define BUF_SIZE 255

using namespace std;

DWORD dwTlsIndex;
HANDLE ghMutex;

DWORD WINAPI MyThreadFunction(LPVOID lpParam);

void ErrorExit(const char * lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);
	ExitProcess(0);
}

VOID CommonFunc(VOID)
{
	vector<int>& pointer = *(vector<int>*)TlsGetValue(dwTlsIndex);
	int sum = 0;
	if ((pointer[0] == 0) && (GetLastError() != ERROR_SUCCESS))
		ErrorExit("TlsGetValue error");
	else {

		for (size_t i = 0; i < pointer.size(); i++)
		{
			sum += pointer[i];
		}
	}
	printf("Thread %d is printing sum...It is %d\n",
		GetCurrentThreadId(), sum);
}

DWORD WINAPI ThreadFunc(VOID)
{
	LPVOID lpvData;

	// Initialize the TLS index for this thread. 

	lpvData = (LPVOID)LocalAlloc(LPTR, 256);
	if (!TlsSetValue(dwTlsIndex, lpvData))
		ErrorExit("TlsSetValue error");

	printf("thread %d: lpvData=%lx\n", GetCurrentThreadId(), lpvData);

	//CommonFunc();

	// Release the dynamic memory before the thread returns. 

	lpvData = TlsGetValue(dwTlsIndex);
	if (lpvData != 0)
		LocalFree((HLOCAL)lpvData);

	return 0;
}

vector<int> GenerateArray(int size) {
	vector<int> row;
	for (size_t i = 0; i < size; i++)
	{
		row.push_back(rand() % 90 + 10);
	}
	return row;
}
//find biggest divider which is not the number itself
int biggest_divider(int a) {
	int res = 1;
	for (int i = 1; i < a; i++) {
		if (a % i == 0) res = i;
	}
	return res;
}
int main(void)
{
	//number of threads
	int thread_count;
	cout << "Enter number oh threads: ";
	cin >> thread_count;

	vector<HANDLE> hThread;//Threads handles array
	vector<DWORD> IDThread;//Threads IDs array

	vector<vector<int>> data;
	vector<vector<int>*> data_pointers;

	// Allocate a TLS index. 
	if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
		ErrorExit("TlsAlloc failed");

	for (size_t i = 0; i < thread_count; i++)
	{
		IDThread.push_back(NULL);
	}
	
	ghMutex = CreateMutex(NULL, FALSE, NULL);

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}
	//creating data sets
	for (size_t i = 0; i < thread_count; i++)
	{
		data.push_back(GenerateArray(rand() % 5 + 5));
	}
	//getting pointers to data sets
	for (size_t i = 0; i < thread_count; i++)
	{
		data_pointers.push_back((vector<int>*)&(data[i]));
	}
	//creating threads
	for (size_t i = 0; i < thread_count; i++)
	{
		hThread.push_back(NULL);
		hThread[i] = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)MyThreadFunction,
			data_pointers[i],
			0,
			&IDThread[i]);

		// Check the return value for success. 
		if (hThread[i] == NULL)
			ErrorExit("CreateThread error\n");

	}

	WaitForMultipleObjects(thread_count, hThread.data(), TRUE, INFINITE);

	for (size_t i = 0; i < thread_count; i++)
		CloseHandle(hThread[i]);

	TlsFree(dwTlsIndex);
	CloseHandle(ghMutex);

	return 0;
}
void Calculations(LPVOID lpParam) {
	vector<int>& pointer = *(vector<int>*)lpParam;
	printf("Thread %d is printing array...\n",
		GetCurrentThreadId());
	LPVOID lpvData = TlsGetValue(GetCurrentThreadId());
	vector<int> results;
	for (size_t i = 0; i < pointer.size(); i++)
	{
		printf("%d ", pointer[i]);
		int res = biggest_divider(pointer[i]);
		results.push_back(res);
	}
	printf("\n");
	TlsSetValue(dwTlsIndex, (vector<int>*) &(results));
	CommonFunc();
	cout << endl;
}
DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	HANDLE hStdout;
	LPVOID lpvData;

	// Initialize the TLS index for this thread. 
	lpvData = (LPVOID)LocalAlloc(LPTR, 256);
	if (!TlsSetValue(dwTlsIndex, lpvData))
		ErrorExit("TlsSetValue error");

	DWORD dwCount = 0, dwWaitResult;

	// Request ownership of mutex.
	dwWaitResult = WaitForSingleObject(
		ghMutex,    // handle to mutex
		INFINITE);  

	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
	{
		__try {
			Calculations(lpParam);
		}
		__finally {
			// Release ownership of the mutex object
			if (!ReleaseMutex(ghMutex))
			{
				ErrorExit("ReleaseMutex error");
			}
		}
		break;
	}

	// The thread got ownership of an abandoned mutex
	case WAIT_ABANDONED:
		return FALSE;
	}

	lpvData = TlsGetValue(dwTlsIndex);

	if (lpvData != 0)
		LocalFree((HLOCAL)lpvData);

	return 0;
}