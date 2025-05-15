#pragma once
#include <iostream>
#include "ntapi.h"

#define FILE_READ_WRITE 0x12019f

typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO
{
    HANDLE HandleValue;
    ULONG_PTR HandleCount;
    ULONG_PTR PointerCount;
    ACCESS_MASK GrantedAccess;
    ULONG ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} PROCESS_HANDLE_TABLE_ENTRY_INFO, * PPROCESS_HANDLE_TABLE_ENTRY_INFO;

typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[ANYSIZE_ARRAY];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION, * PPROCESS_HANDLE_SNAPSHOT_INFORMATION;

namespace deadlock {
    PPROCESS_HANDLE_SNAPSHOT_INFORMATION getProcessHandles(HANDLE hProcess);

    HANDLE dupHandle(HANDLE handleValue, HANDLE ownerProcess);

    BOOL isFileObj(HANDLE hFile);

    LPCSTR getFilePath(HANDLE hFile);
    BOOL isDiskFile(HANDLE hFile);

    HANDLE remoteCloseHandle(HANDLE hProcess, HANDLE handleValue);
}
