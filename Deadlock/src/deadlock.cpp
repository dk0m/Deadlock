#include "deadlock.h"

PPROCESS_HANDLE_SNAPSHOT_INFORMATION deadlock::getProcessHandles(HANDLE hProcess) {

	ULONG infoLen = 0;
	PVOID pHandleInfo = NULL;
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;

	while (status == STATUS_INFO_LENGTH_MISMATCH) {
		status = NtQueryInformationProcess(
			hProcess,
			ProcessHandleInformation,
			pHandleInfo,
			infoLen,
			&infoLen
		);

		pHandleInfo = realloc(pHandleInfo, infoLen);
	}

	return (PPROCESS_HANDLE_SNAPSHOT_INFORMATION)pHandleInfo;

}

POBJECT_TYPE_INFORMATION GetObjTypeInfo(HANDLE hFile) {

	ULONG infoLen = 0;
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
	POBJECT_TYPE_INFORMATION pObjInfo = NULL;

	while (status == STATUS_INFO_LENGTH_MISMATCH) {
		status = NtQueryObject(
			hFile, 
			ObjectTypeInformation,
			pObjInfo,
			infoLen,
			&infoLen
		);

		pObjInfo = (POBJECT_TYPE_INFORMATION)realloc(pObjInfo, infoLen);
	}

	return pObjInfo;
}

BOOL deadlock::isFileObj(HANDLE hFile) {

	auto objTypeInfo = GetObjTypeInfo(hFile);

	BOOL result = !wcscmp(objTypeInfo->TypeName.Buffer, L"File");

	free(objTypeInfo);

	return result;
}

HANDLE deadlock::dupHandle(HANDLE handleValue, HANDLE ownerProcess) {

	HANDLE outHandle;

	if (DuplicateHandle(
		ownerProcess,
		handleValue,
		GetCurrentProcess(),
		&outHandle,
		DUPLICATE_SAME_ACCESS,
		FALSE,
		0
	)) {
		return outHandle;
	}
	else {
		return NULL;
	}

}

LPCSTR deadlock::getFilePath(HANDLE hFile) {
	char path[MAX_PATH];

	if (GetFinalPathNameByHandleA(hFile, path, MAX_PATH, VOLUME_NAME_DOS)) {
		return path;
	}
	else {
		return NULL;
	}

}

static FARPROC NtCloseAddress = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtClose");

HANDLE deadlock::remoteCloseHandle(HANDLE hProcess, HANDLE handleValue) {
	return CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)NtCloseAddress,
		(LPVOID)handleValue,
		0,
		0
	);
}

BOOL deadlock::isDiskFile(HANDLE hFile) {
	return GetFileType(hFile) == FILE_TYPE_DISK;
}