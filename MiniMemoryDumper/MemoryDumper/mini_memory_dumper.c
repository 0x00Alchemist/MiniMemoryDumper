#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <DbgHelp.h>
#include <minidumpapiset.h>

#include "mini_memory_dumper.h"

PROCESSENTRY32 PE32;

/* Dumper_DumpProcess
*  Purpose: creating folder with dumps and creating dump files
*/
static DWORD Dumper_DumpProcess(HANDLE hDumpProcess, DWORD dwPID) {
	// Getting current directory of MiniMemoryDumper
	// for creating dump file in dir of MMD
	WCHAR szFutureFolder[MAX_PATH];
	GetModuleFileName(NULL, (LPWSTR)szFutureFolder, sizeof(szFutureFolder));

	WCHAR *wPos = wcsrchr(szFutureFolder, L'\\');
	wPos[0] = L'\0';
	++wPos;

	// Creating directory for future dumps
	WCHAR *wDumpFolderName = L"Dumps";
	WCHAR szDumpFolder[MAX_PATH];
	wsprintf(szDumpFolder, L"%s\\%s", szFutureFolder, wDumpFolderName);

	if(!CreateDirectoryW((LPCWSTR)szDumpFolder, NULL)) {
		DWORD dwErr = GetLastError();

		if(dwErr != ERROR_ALREADY_EXISTS) {
			wprintf(L"\n[!] Cannot create folder for dump files!");
			return dwErr;
		}
	}

	// Creating dump file
	WCHAR szDumpFile[MAX_PATH];
	wsprintf(szDumpFile, L"%s\\PID_[%ld].dmp", szDumpFolder, dwPID);

	HANDLE hDumpFile = CreateFileW(szDumpFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
	if(hDumpFile == INVALID_HANDLE_VALUE) {
		wprintf(L"\n[!] Cannot create dump file!");
		return DUMPER_FILE_CREATION_ERR;
	}
	
	BOOL isDumped = MiniDumpWriteDump(hDumpProcess, dwPID, hDumpFile, MiniDumpWithFullMemory, NULL, NULL, NULL);
	if(!isDumped) {
		wprintf(L"\n[!] Some error in process dumping...");

		DWORD dwErr = GetLastError();
		return dwErr;
	}

	CloseHandle(hDumpFile);
	return DUMPER_SUCCESFULL_DUMP;
}

/* Dumper_AttachProcess
*  Purpose: Attach to process provided by PID, then dump it
*/
BOOL Dumper_AttachProcess(DWORD dwPID) {
	PE32.th32ProcessID = dwPID;

	// Open process provided by PID
	HANDLE hDumpProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PE32.th32ProcessID);
	if(hDumpProcess == INVALID_HANDLE_VALUE) {
		wprintf(L"\n[!] Cannot attach to a process!");
		return FALSE;
	}

	wprintf(L"\n[!] Attached to a %ld process succesfully!", dwPID);

	// Enter in dump function
	if(Dumper_DumpProcess(hDumpProcess, dwPID) != DUMPER_SUCCESFULL_DUMP)
		return FALSE;
	
	wprintf(L"\n[!] Dumped process with PID %ld successfully!\n", dwPID);

	CloseHandle(hDumpProcess);
	return TRUE;
}


/* Dumper_EnumProcesses
*  Purpose: Enumerating system processes
*/
BOOL Dumper_EnumProcesses() {
	HANDLE ProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	if(ProcessSnapshot == INVALID_HANDLE_VALUE) {
		wprintf(L"Cannot get snapshot of system!");
		return FALSE;
	}

	PE32.dwSize = sizeof(PROCESSENTRY32);

	// Enum processes and print it in console
	INT i = 0;
	if(Process32First(ProcessSnapshot, &PE32)) {
		while(Process32Next(ProcessSnapshot, &PE32))
			wprintf(L"[+] #%d: process name: %s (PID: %u)\n", (++i), PE32.szExeFile, PE32.th32ProcessID);
	}

	CloseHandle(ProcessSnapshot);
	return TRUE;
}
