#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#include <iostream>

OVERLAPPED g_ReadOV;
LARGE_INTEGER g_LargeRead;
LARGE_INTEGER g_LargeWrite;

CHAR* g_pmbData = nullptr;
CHAR* g_pmbOffsetData = nullptr;
CHAR* g_pmbDataCopy = nullptr;

LARGE_INTEGER Frequency;
LARGE_INTEGER Frequency1;
LARGE_INTEGER Frequency2;

DWORD ASyncLoad(const TCHAR* strFileName);
DWORD FileRead(HANDLE hHandle, DWORD offset);

int main(void)
{
	QueryPerformanceFrequency(&Frequency);
	DWORD dwFileSize = ASyncLoad(L"TBasis3D.zip");
	if (dwFileSize == 0) return 0;
	std::cout << "로딩 완료" << std::endl;

	OVERLAPPED ov;
	ZeroMemory(&ov, sizeof(OVERLAPPED));
	ov.hEvent = 0;

	HANDLE hWriteFile = CreateFile(L"Copy.zip", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	std::cout << "비동기 출력 시작" << std::endl;
	if (hWriteFile != NULL)
	{
		DWORD dwWritten;
		QueryPerformanceCounter(&Frequency1);
		BOOL ret = WriteFile(hWriteFile, g_pmbData, dwFileSize, &dwWritten, &ov);
		if (ret == FALSE)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				std::cout << "작업중" << std::endl;
			}
		}
		if (ret == TRUE)
		{
			std::cout << "출력 완료" << std::endl;
		}
	}

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(hWriteFile, 0);
		if (dwRet == WAIT_OBJECT_0)
		{
			DWORD Tans = 0;
			BOOL Result = GetOverlappedResult(hWriteFile, &ov, &Tans, FALSE);
			if (Tans >= dwFileSize)
			{
				delete[] g_pmbData;
				CloseHandle(hWriteFile);
				QueryPerformanceCounter(&Frequency2);
				std::cout << (Frequency2.LowPart - Frequency1.LowPart) / (float)Frequency.LowPart << "초" << std::endl;
				std::cout << ov.InternalHigh << " 복사 완료" << std::endl;
				break;
			}
		}
		else if (dwRet == WAIT_TIMEOUT)
		{
			static int iCnt = 0;
			DWORD Tans = 0;
			BOOL Result = GetOverlappedResult(hWriteFile, &ov, &Tans, FALSE);
			if (Result == FALSE)
			{
				if (GetLastError() == ERROR_IO_INCOMPLETE)
				{
					std::cout << "출력중: " << iCnt++ << std::endl;
				}
			}

			if (Result == TRUE)
			{
				QueryPerformanceCounter(&Frequency2);
				std::cout << (Frequency2.LowPart - Frequency1.LowPart) / (float)Frequency.LowPart << "초" << std::endl;
				std::cout << ov.InternalHigh << " 복사 완료" << std::endl;
			}
		}
		else
		{
			delete[] g_pmbData;
			CloseHandle(hWriteFile);
			std::cout << "Error" << std::endl;
			break;
		}
		Sleep(1);
	}
	CloseHandle(ov.hEvent);
	std::cout << "종료" << std::endl;
	return 0;
}

DWORD ASyncLoad(const TCHAR* strFileName)
{
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	_tsplitpath(strFileName, Drive, Dir, FName, Ext);

	DWORD cTotalBytes;
	LARGE_INTEGER FileSize;
	HANDLE hHandle = CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		GetFileSizeEx(hHandle, &FileSize);
		cTotalBytes = FileSize.LowPart;
		std::cout << "[파일크기 : " << cTotalBytes / 1024 << "KB" << std::endl;
		g_pmbData = new char[cTotalBytes];
		if (g_pmbData == nullptr)
		{
			CloseHandle(hHandle);
			return 0;
		}

		ZeroMemory(&g_ReadOV, sizeof(OVERLAPPED));

		FileRead(hHandle, 0);
		QueryPerformanceCounter(&Frequency1);

		static int iCnt = 0;
		while (true)
		{
			DWORD dwRet = WaitForSingleObject(hHandle, 0);
			if (dwRet == WAIT_OBJECT_0)
			{
				DWORD Tans = 0;
				BOOL Result = GetOverlappedResult(hHandle, &g_ReadOV, &Tans, TRUE);
				if (Result == TRUE)
				{
					if (g_LargeRead.LowPart >= cTotalBytes)
					{
						QueryPerformanceCounter(&Frequency2);
						std::cout << g_LargeRead.QuadPart << " 읽기 완료" << std::endl;
						double av = (double)g_LargeRead.QuadPart / (double)cTotalBytes * 100.0f;
						std::cout << "파일크기: " << cTotalBytes / 1024 << " KB" << std::endl;
						std::cout << av << ": " << g_LargeRead.QuadPart / 1024 << std::endl;
						break;
					}
					std::cout << iCnt++ << "읽는 중 " << g_LargeRead.QuadPart << std::endl;
					FileRead(hHandle, Tans);
				}
			}
			else if (dwRet == WAIT_TIMEOUT)
			{
				DWORD Tans = 0;
				BOOL Result = GetOverlappedResult(hHandle, &g_ReadOV, &Tans, FALSE);
				if (Result == FALSE)
				{
					if (GetLastError() == ERROR_IO_INCOMPLETE)
					{
						std::cout << "읽는 중: " << iCnt++ << std::endl;
					}
				}
				if (Result == TRUE)
				{
					QueryPerformanceCounter(&Frequency2);
					system("cls");
					std::cout << g_LargeRead.QuadPart << " 읽기 완료" << std::endl;
					double av = (double)g_LargeRead.QuadPart / (double)cTotalBytes * 100.0f;
					std::cout << "파일크기: " << cTotalBytes / 1024 << " KB" << std::endl;
					std::cout << av << ": " << g_LargeRead.QuadPart / 1024 << std::endl;
				}
			}
			else
			{
				std::cout << dwRet << " Error" << std::endl;
				break;
			}
		//	Sleep(1);
		}
	}
	else
	{
		return 0;
	}

	SetFilePointer(hHandle, 0, 0, FILE_BEGIN);
	CloseHandle(hHandle);
	return cTotalBytes;
}
DWORD FileRead(HANDLE hHandle, DWORD offset)
{
	g_LargeRead.QuadPart += offset;
	g_ReadOV.Offset = g_LargeRead.LowPart;
	g_ReadOV.OffsetHigh = g_LargeRead.HighPart;
	g_pmbOffsetData = &g_pmbData[g_LargeRead.LowPart];

	DWORD dwStepSize = 4096 * 1024;
	DWORD dwBytesRead;
	DWORD ret = ReadFile(hHandle, g_pmbOffsetData, dwStepSize, &dwBytesRead, &g_ReadOV);
	if (ret == FALSE)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{

		}
	}
	if (ret == TRUE)
	{
		std::cout << "입력 완료" << std::endl;
		CloseHandle(hHandle);
		delete[] g_pmbData;
		return 0;
	}
	return 1;
}