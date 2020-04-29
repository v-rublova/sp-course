#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <string>

using namespace std;
CString CreateArchivatorProcess(string run_mode, string archive_name, string files) {

	CString strResult = L""; // pipe output

	HANDLE hChildStdoutRd; // Read-side
	HANDLE hChildStdoutWr; // Write-side

	BOOL fSuccess;

	//create pipe
	SECURITY_ATTRIBUTES security_attr = { sizeof(SECURITY_ATTRIBUTES) };
	security_attr.bInheritHandle = TRUE;
	security_attr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &security_attr, 0))
	{
		cout << "Can't create pipe.\n";
	}

	STARTUPINFO startup_info;
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);
	PROCESS_INFORMATION process_info;
	ZeroMemory(&process_info, sizeof(process_info));

	startup_info.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
	startup_info.hStdOutput = hChildStdoutWr; 
	startup_info.hStdError = hChildStdoutWr; 
	startup_info.wShowWindow = SW_HIDE; 

	string comand_l_args = "7z " + run_mode + " " + archive_name;
	if (run_mode == "a") {
		comand_l_args += " " + files;
	}
	else {
		comand_l_args += " -o" + files;
	}

	//"7z" +mode -tzip archive1.zip listfile.txt"
	LPTSTR szCmdline = _wcsdup(wstring(comand_l_args.begin(), comand_l_args.end()).c_str());
	if (CreateProcess(TEXT("7z.exe"), szCmdline,
		NULL, NULL, TRUE, 0, NULL, NULL, &startup_info, &process_info))
	{
		Sleep(1000);				
		TerminateProcess(process_info.hProcess, NO_ERROR);
	}
	else {
		cout << "Can't create process.\n" << endl;
	}

	WaitForSingleObject(process_info.hProcess, INFINITE);
	TerminateProcess(process_info.hProcess, 0);

	
	if (!CloseHandle(hChildStdoutWr))
	{
		cout << "cannot close handle";
	}

	// Read output from the child process.
	for (;;)
	{
		DWORD dwRead;
		CHAR chBuf[4096];

		// Read from pipe that is the standard output for child process.
		bool done = !ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, NULL) || dwRead == 0;
		if (done)
		{
			break;
		}
		strResult += CString(chBuf, dwRead);
	}

	CloseHandle(hChildStdoutRd);
	CloseHandle(process_info.hProcess);
	CloseHandle(process_info.hThread);

	return strResult;
}
int main()
{
	enum program_mode { cs_exit, cs_archive, cs_unpack };
	int mode;
	bool end_program_trigger = false;
	string arch_Path;
	string file;
	CString run_process_output = L"";

	//User interface
	
	do {
		cout << "Hi!\nTo archive a file press '1'.\nTo unpack an archive press '2'.\nPress '0' to exit.\n";
		cin >> mode;
		switch (mode)
		{
		case cs_archive: {
			cout << "Splendid! We're here to archive!\nSpecify full path to archive (name included):\n";
			getline(cin, arch_Path);
			getline(cin, arch_Path);
			cout << "Now enter required file (full path, name included)\n";
			getline(cin, file);
			cout << "Doing archiving mojo...Please, stand by...\n";
			run_process_output = CreateArchivatorProcess("a", arch_Path, file);
			break;
		}
		case cs_unpack: {
			cout << "Marvelous! We're here to unpack files!\nSpecify full path to archive (name included):\n";
			getline(cin, arch_Path);
			getline(cin, arch_Path);
			cout << "Now type in destination folder (full path, name included)\n";
			getline(cin, file); //I reused *file* 'cause folder is a file (technically)
			cout << "Doing archiving mojo...Please, stand by...\n";
			run_process_output = CreateArchivatorProcess("x", arch_Path, file);
			break;
		}
		case cs_exit: {
			end_program_trigger = true;
			break;
		}
		default: break;
		}
		/*In task it was specified that the program needs to distinguish,  
		and I quote 'получить содержимое консоли, сформированное программой 7z.exe 
		и по этому тексту определить была ошибка или нет.' not a content of an error, but merely its occurrence*/
		if (run_process_output.Find(_tcsdup(L"Warning"), 0) != -1) {
			cout << "You acquired yourself a warning." << endl;
		}
		else if (run_process_output.Find(_tcsdup(L"Error"), 0) != -1) {
			cout << "You acquired yourself an error." << endl;
		}
		else if (run_process_output != L""){
			cout << "Everything went smoothly." << endl;
		}
	} while (!end_program_trigger);
	return 0;
}
