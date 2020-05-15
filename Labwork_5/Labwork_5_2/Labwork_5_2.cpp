#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <locale.h>
#include <ctime>
#include <vector>

using namespace std;

HANDLE hSemaphore;

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	DWORD bytesWritten;
	HANDLE file;
	clock_t t_start, t_end;
	LPSTR buffer = new char[100];
	bool running = true;

	
	t_start = clock();
		DWORD dwWaitResult = WaitForSingleObject(hSemaphore, INFINITE);

	
	while (running)
	{
		if (dwWaitResult == WAIT_OBJECT_0) {
			
			Sleep(1000 * (rand() % 3 + 1));
			printf("Thread (%d) doing things with file.\n", GetCurrentThreadId());
			file = CreateFileA("work_time.txt",
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			t_end = clock();

			SetFilePointer(file, 0, NULL, FILE_END);
			sprintf(buffer, "~~%f~~", (double)(t_end - t_start));
			WriteFile(file, buffer, sizeof(buffer), &bytesWritten, NULL);
			if (file == INVALID_HANDLE_VALUE) {
				printf("file handle error: %d\n", GetLastError());
			}
			running = false;
			CloseHandle(file);
			Sleep(1000 * (rand() % 3 + 1));
			printf("\nThread (%d) turned it's attention elsewhere.\n", GetCurrentThreadId());
			if (!ReleaseSemaphore(hSemaphore, 1, NULL)) {
				printf("can't rl semaphore: %d\n", GetLastError());
			}
		}
	}

	return 0;
}

void ErrorExit(const char * lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);
	ExitProcess(0);
}
int main()
{
	int thread_count;
	cout << "Enter number oh threads: ";
	cin >> thread_count;

	vector<HANDLE> hThread;//Threads handles array
	vector<DWORD> IDThread;//Threads IDs array

	for (size_t i = 0; i < thread_count; i++)
	{
		IDThread.push_back(NULL);
	}

	hSemaphore = CreateSemaphore(
		NULL,				    // A pointer to a SECURITY_ATTRIBUTES structure.
		2,	        // The initial count for the semaphore object. 
		2,	        // The maximum count for the semaphore object.
		TEXT("semaphoreName")   // The name of the semaphore object.
	);

	if (hSemaphore == NULL) {
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	for (size_t i = 0; i < thread_count; i++)
	{
		hThread.push_back(NULL);
		hThread[i] = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)MyThreadFunction,
			NULL,
			0,
			&IDThread[i]);

		// Check the return value for success. 
		if (hThread[i] == NULL)
			ErrorExit("CreateThread error\n");

	}
	WaitForMultipleObjects(thread_count, hThread.data(), TRUE, INFINITE);

	for (size_t i = 0; i < thread_count; i++)
		CloseHandle(hThread[i]);

	CloseHandle(hSemaphore);

	return 0;
}
