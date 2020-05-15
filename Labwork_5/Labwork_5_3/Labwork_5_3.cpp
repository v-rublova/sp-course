#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

#define ELEMENTS_COUNT 1000000
#define THREAD_COUNT 3

CRITICAL_SECTION criticalSection;
vector<int> arr;

#pragma region Calculations
void Sum(int limit)
{
	int sum = 0;
	for (int i = 0; i < limit; i++)
		sum += arr[i];
	printf("Sum = %d;\n", sum);
}

void Avg(int limit)
{
	double avg = 0;
	for (int i = 0; i < limit; i++) 
	{
		avg += arr[i];
	}
	printf("Average = %f;\n", (double)(avg / limit));
}

void Max(int limit)
{
	int max = 0;
	for (size_t i = 0; i < limit; i++) 
	{
		if ( arr[i] > max)
		{
			max = arr[i];
		}
	}
	printf("Maximum = %d;\n", max);
}

void GenerateArray(int limit)
{
	for (size_t i = 0; i < limit; i++)
		arr.push_back(rand() % 90 + 10);
}
#pragma endregion
#pragma region ThreadFunctions
DWORD WINAPI ThreadSum(LPVOID mode_trigger)
{
	bool mode = (bool)mode_trigger;
	if (mode)
	{
		Sum(ELEMENTS_COUNT);
	}
	else { 
		if (!InitializeCriticalSectionAndSpinCount(&criticalSection, 0)) {
			return 0;
		}
		EnterCriticalSection(&criticalSection);
		Sum(ELEMENTS_COUNT);
		LeaveCriticalSection(&criticalSection);
	}
	return 0;
}

DWORD WINAPI ThreadAverage(LPVOID mode_trigger)
{
	bool mode = (bool)mode_trigger;
	if (mode) {
		Avg(ELEMENTS_COUNT);
	}
	else { 
		if (!InitializeCriticalSectionAndSpinCount(&criticalSection, 0)) {
			return 0;
		}
		EnterCriticalSection(&criticalSection);
		Avg(ELEMENTS_COUNT);
		LeaveCriticalSection(&criticalSection);
	}
	return 0;
}

DWORD WINAPI ThreadMaximum(LPVOID mode_trigger)
{
	bool mode = (bool)mode_trigger;
	if (mode) 
	{
		Max(ELEMENTS_COUNT);
	}
	else { 
		//Initializes a critical section object and sets the spin count for the critical section. 
		if (!InitializeCriticalSectionAndSpinCount(&criticalSection, 0)) {
			return 0;
		}
		EnterCriticalSection(&criticalSection);
		Max(ELEMENTS_COUNT);
		LeaveCriticalSection(&criticalSection);
	}
	return 0;
}
#pragma endregion
void CreateThreadsAndWait(bool mode) 
{

	vector<HANDLE> hThread;//Threads handles array
	vector<DWORD> IDThread;//Threads IDs array

	clock_t t_start, t_end;

	GenerateArray(ELEMENTS_COUNT);

	if (!mode) cout << "Use critical section:\n";
	else cout << "Don't use critical section:\n";

	t_start = clock();

	for (size_t i = 0; i < THREAD_COUNT; i++)
	{
		hThread.push_back(NULL);
		IDThread.push_back(NULL);
	}
	hThread[0] = CreateThread(NULL, 0, ThreadSum, (LPVOID)mode, 0, &IDThread[0]);
	hThread[1] = CreateThread(NULL, 0, ThreadAverage, (LPVOID)mode, 0, &IDThread[1]);
	hThread[2] = CreateThread(NULL, 0, ThreadMaximum, (LPVOID)mode, 0, &IDThread[2]);
	WaitForMultipleObjects(THREAD_COUNT, hThread.data(), TRUE, INFINITE);
	t_end = clock();

	printf("Duration: %f\n\n", (double)(t_end - t_start));

	DeleteCriticalSection(&criticalSection);

	for (int i = 0; i < THREAD_COUNT; i++) {
		CloseHandle(hThread[i]);
	}
}
int main()
{
	CreateThreadsAndWait(1);//no critical section
	CreateThreadsAndWait(0);//critical section
	return 0;
}