#pragma once
#include <Windows.h>

#define DUMPER_FILE_CREATION_ERR	0x3BADF11EL
#define DUMPER_SUCCESFULL_DUMP		0xD249ED00L

BOOL Dumper_EnumProcesses();
BOOL Dumper_AttachProcess(DWORD dwPID);
