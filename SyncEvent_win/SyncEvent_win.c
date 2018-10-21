#include <stdio.h>
#include <windows.h>
#include <process.h>

#define STR_LEN 100

unsigned WINAPI NumberofA(void * arg);
unsigned WINAPI NumberofOthers(void * arg);

static char str[STR_LEN];
static HANDLE hEvent;

int main()
{
	HANDLE hThread1, hThread2;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberofA, NULL, 0, NULL);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberofOthers, NULL, 0, NULL);

	fputs("Input string: ", stdout);
	fgets(str, STR_LEN, stdin);
	SetEvent(hEvent);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	ResetEvent(hEvent);
	CloseHandle(hEvent);
	return 0;
}


unsigned WINAPI NumberofA(void * arg)
{
	int i, cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (i = 0; str[i] != 0; ++i)
	{
		if (str[i] == 'A' || str[i] == 'a')
			cnt++;
	}
	printf("Num of A: %d\n", cnt);
	return 0;
}
unsigned WINAPI NumberofOthers(void * arg)
{
	int i, cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (i = 0; str[i] != 0; ++i)
	{
		if (str[i] != 'A')
			cnt++;
	}
	printf("Num of others: %d\n", cnt);
	return 0;
}