#include "./src/deadlock.h"
#include<vector>
#include<TlHelp32.h>

using std::vector;
using std::string;

vector<HANDLE> getProcessInstances(LPCSTR procName) {
    vector<HANDLE> handles;

    HANDLE procSs = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    while (Process32Next(procSs, &pe32)) {
        if (!strcmp(procName, pe32.szExeFile)) {

            handles.push_back(OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_DUP_HANDLE,
                FALSE,
                pe32.th32ProcessID
            ));

        }
    }

    CloseHandle(procSs);

    return handles;
}

BOOL isDesiredFile(string& filePath, const char* fileEnd) {
    auto pos = filePath.rfind("\\");
    auto foundSub = filePath.substr(pos + 1);

    return !_strcmpi(foundSub.c_str(), fileEnd);
}

int main(int argc, char* argv[])
{
    const char* targetProcName = argv[1];
    const char* lockedFileTag = argv[2];
    const char* outputFilePath = argv[3];
    const char* closeRemoteArg = argv[4];

    if (!targetProcName || !lockedFileTag || !outputFilePath || !closeRemoteArg) {
        printf("[*] Usage: deadlock.exe <TARGET_PROCESS> <LOCKED_FILE_TAG> <OUTPUT_FILE> <CLOSE_REMOTE?>\n");
        return -1;
    }

    int closeRemote = atoi(closeRemoteArg); // 0 = dont close remote handle, !0 (nonzero) = close remote handle

    auto procInstances = getProcessInstances(targetProcName);

    if (!(procInstances.size() > 0)) {
        printf("[-] No Processes Found.\n");
        return -1;
    }

    for (HANDLE& procInstance : procInstances) {
        auto hInfo = deadlock::getProcessHandles(procInstance);

        for (size_t i = 0; i < hInfo->NumberOfHandles; i++) {

            auto handleInfo = hInfo->Handles[i];
            auto fileHandle = deadlock::dupHandle(handleInfo.HandleValue, procInstance);

            if (!fileHandle)
                continue;

            if (!deadlock::isFileObj(fileHandle))
                continue;

            if (!deadlock::isDiskFile(fileHandle))
                continue;

            auto cFilePath = deadlock::getFilePath(fileHandle);

            if (!cFilePath)
                continue;

            auto filePath = string(cFilePath);

            if (!isDesiredFile(filePath, lockedFileTag))
                continue;

            printf("[+] Found Target File\n\tHandle Owner PID: %ld\n", GetProcessId(procInstance));
            
            if (closeRemote) {
                if (deadlock::remoteCloseHandle(procInstance, handleInfo.HandleValue) != NULL) {
                    printf("[+] Closed Remote Handle Successfully.\n");
                }
            }

            if (!outputFilePath)
                continue;

            SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN);

            DWORD fileSize = GetFileSize(fileHandle, NULL);
            DWORD readBytes = 0;

            PVOID fileBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);

            if (!ReadFile(
                fileHandle,
                fileBuffer,
                fileSize,
                &readBytes,
                NULL
            )) {
                printf("[+] Failed to Read File, Last Error: %ld\n", GetLastError());
                continue;
            }
            else {
                printf("[+] Read Target File\n\tFile Size: %ld Bytes\n\tRead Bytes: %ld Bytes\n", fileSize, readBytes);

                HANDLE outputFileHandle = CreateFileA(
                    outputFilePath,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
                );

                if (!WriteFile(
                    outputFileHandle,
                    fileBuffer,
                    fileSize,
                    &fileSize,
                    NULL
                )) {
                    printf("[+] Failed to Write Output, Last Error: %ld\n", GetLastError());
                    continue;
                }

                CloseHandle(fileHandle);
                CloseHandle(outputFileHandle);
            }

        }

        free(hInfo);
    }
    
    return 0;
}
