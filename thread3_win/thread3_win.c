#include <stdio.h>
#include <Windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI ThreadInc(void * arg);
unsigned WINAPI ThreadDes(void * arg);
long long num = 0;


int main()
{
	HANDLE tHandles[NUM_THREAD];
	int i;

	printf("sizeof long long: %d \n", sizeof(long long));

	for (i = 0; i < NUM_THREAD; ++i)
	{
		if (i % 2)
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadInc, NULL, 0, NULL);
		else
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadDes, NULL, 0, NULL);
	}

	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
	printf("result : %lld\n", num);

	return 0;
}

unsigned WINAPI ThreadInc(void * arg)
{
	int i;
	for (i = 0; i < 50000000; ++i)
	{
		num += 1;
	}
	return 0;
}
unsigned WINAPI ThreadDes(void * arg)
{
	int i;
	for (i = 0; i < 50000000; ++i)
	{
		num -= 1;
	}
	return 0;
}