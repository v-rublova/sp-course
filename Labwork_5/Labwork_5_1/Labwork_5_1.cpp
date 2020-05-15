#include <stdio.h>
#include <Windows.h>
#include <iostream>

HANDLE ghMutex;

int main()
{
	ghMutex = CreateMutex(NULL, FALSE, TEXT("mutexName"));

	// Request ownership of mutex.
	DWORD dwWaitResult = WaitForSingleObject(
		ghMutex,    // handle to mutex
		0);

	if (dwWaitResult != WAIT_OBJECT_0) 
	{
		std::cout<<"Error! Can't start another copy of the program.\n";
		system("pause");
		return 0;
	}
	else 
	{
		std::cout<<"Is Running...\n";
		do {} while (true);
	}
	return 0;
}
