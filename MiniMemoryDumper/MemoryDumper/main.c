#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <wchar.h>
#include <stdio.h>

#include "mini_memory_dumper.h"

/* HelpMenu
*  Purpose: 
*/
VOID HelpMenu() {
	wprintf(L"\n[?] Help menu:\n");
	wprintf(L"[?] \"help\" - print this menu.\n");
	wprintf(L"[?] \"dump\" - attach to the specific process (after entering this you should provide a PID of process)\n");
	wprintf(L"[?] \"exit\" - exit from Mini Memory Dumper\n");
}

/* CommandHandler
*  Purpose: awful parsing user input
*/
VOID CommandHandler(WCHAR cmd[]) {
	WCHAR *cmdArr = cmd;
	INT i = 0;

	// Shot in the knee
	while(cmdArr[i] != '\0') {
		if(lstrcmpW(cmdArr, L"help") == 0) {
			HelpMenu();
			break;
		} else if(lstrcmpW(cmdArr, L"dump") == 0) {
			DWORD dwPID = 0;

			wprintf(L"\n[!] Provide PID of process: ");
			wscanf(L"%d", &dwPID);

			Dumper_AttachProcess(dwPID);
			
			break;
		} else if(lstrcmpW(cmdArr, L"exit") == 0) {
			ExitProcess(0);
		} else {
			wprintf(L"[!] Unknown command!\n");
			break;
		}

		i++;
	}
	
}

INT main(VOID) {
	wprintf(L"--===[Memory dumper]===--\n\n");

	// Print list of processes
	wprintf(L"[+] List of processes:\n");
	Dumper_EnumProcesses();
	
	wchar_t wOption[30];
	while(TRUE) {
		wprintf(L"\n[?] Provide a command (or type \"help\" to print help list): ");

		wscanf(L"%ls", wOption);
		CommandHandler(wOption);
	}

	return 0;
}
