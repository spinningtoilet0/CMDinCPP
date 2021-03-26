// commmand line in c++.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <thread>

void read(HANDLE, HANDLE, DWORD);
void write(HANDLE, DWORD);

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	LPTSTR szCmdline = _tcsdup(TEXT("cmd.exe"));
	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		szCmdline,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		return 1;
	}

	/*     HANDLE  hStdInput;
	 HANDLE  hStdOutput;
	 HANDLE  hStdError;*/
	HANDLE me_hStdError = GetStdHandle(STD_ERROR_HANDLE);
	HANDLE me_hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE me_hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE proc_hStdInput = si.hStdInput;
	HANDLE proc_hStdOutput = si.hStdOutput;
	HANDLE proc_hStdError = si.hStdError;
	DWORD chars;
	//possibly handle error for ReadConsole - TODO  
	std::thread readThread(read, me_hStdInput, proc_hStdOutput, chars);
	readThread.detach();
	std::thread writeThread(write, me_hStdInput, chars);
	writeThread.detach();
	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

void read(HANDLE me_hStdInput, HANDLE proc_hStdOutput, DWORD chars)
{

	char buff[64];
	for (DWORD written = 0, writtenThisTime; written < chars; written += writtenThisTime)
		if (!WriteConsole(proc_hStdOutput, buff + written, chars - written, &writtenThisTime, NULL))
		{
			if (!written == 0)
			{
				std::cout << "write console failed\n---debug information---\nbuf: " << buff << "\nwritten: " << written << std::endl;
			}
		}
}

void write(HANDLE me_hStdInput, DWORD chars)
{
	char buff[4096];
	while (!ReadConsole(me_hStdInput, buff, sizeof(buff), &chars, NULL))
	{
	}
}
