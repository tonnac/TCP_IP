#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#include <iostream>

CHAR* g_pmbData = nullptr;
CHAR* g_pmbDataCopy = nullptr;

LARGE_INTEGER Frequency;
LARGE_INTEGER Frequency1;
LARGE_INTEGER Frequency2;

DWORD ASyncLoad(const TCHAR* strFileName);

int main(void)
{
	QueryPerformanceFrequency(&Frequency);
	DWORD dwFileSize = ASyncLoad(L"TBasis3D.zip");
	if (dwFileSize == 0) return 0;
	std::cout << "�ε� �Ϸ�" << std::endl;

	OVERLAPPED ov;
	ZeroMemory(&ov, sizeof(OVERLAPPED));
	ov.hEvent = 0;

	HANDLE hWriteFile = CreateFile(L"Copy.zip", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	std::cout << "�񵿱� ��� ����" << std::endl;
	if (hWriteFile != NULL)
	{
		DWORD dwWritten;
		QueryPerformanceCounter(&Frequency1);
		BOOL ret = WriteFile(hWriteFile, g_pmbData, dwFileSize, &dwWritten, &ov);
		if (ret == FALSE)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				std::cout << "�۾���" << std::endl;
			}
		}
		if (ret == TRUE)
		{
			std::cout << "��� �Ϸ�" << std::endl;
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
				std::cout << (Frequency2.LowPart - Frequency1.LowPart) / (float)Frequency.LowPart << "��" << std::endl;
				std::cout << ov.InternalHigh << " ���� �Ϸ�" << std::endl;
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
					std::cout << "�����: " << iCnt++ << std::endl;
				}
			}

			if (Result == TRUE)
			{
				QueryPerformanceCounter(&Frequency2);
				std::cout << (Frequency2.LowPart - Frequency1.LowPart) / (float)Frequency.LowPart << "��" << std::endl;
				std::cout << ov.InternalHigh << " ���� �Ϸ�" << std::endl;
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
	std::cout << "����" << std::endl;
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
		std::cout << "[����ũ�� : " << cTotalBytes / 1024 << "KB" << std::endl;
		g_pmbData = new char[cTotalBytes];
		if (g_pmbData == nullptr)
		{
			CloseHandle(hHandle);
			return 0;
		}

		OVERLAPPED readOV;
		ZeroMemory(&readOV, sizeof(OVERLAPPED));

		DWORD dwBytesRead;
		DWORD ret = ReadFile(hHandle, g_pmbData, cTotalBytes, &dwBytesRead, &readOV);
		QueryPerformanceCounter(&Frequency1);
		if (ret == FALSE)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				std::cout << "�б� �۾���" << std::endl;
			}
		}

		if (ret == TRUE)
		{
			std::cout << "�Է� �Ϸ�." << std::endl;
			CloseHandle(hHandle);
			delete[] g_pmbData;
			return 0;
		}

		while (true)
		{
			DWORD dwRet = WaitForSingleObject(hHandle, 0);
			if (dwRet == WAIT_OBJECT_0)
			{
				DWORD Tans = 0;
				BOOL Result = GetOverlappedResult(hHandle, &readOV, &Tans, FALSE);
				if (Tans >= cTotalBytes)
				{
					QueryPerformanceCounter(&Frequency2);
					std::cout << (Frequency2.LowPart - Frequency1.LowPart) / (float)Frequency.LowPart << "��" << std::endl;
					std::cout << readOV.InternalHigh << " �б� �Ϸ�" << std::endl;
					break;
				}
			}
			else if (dwRet == WAIT_TIMEOUT)
			{
				static int iCnt = 0;
				DWORD Tans = 0;
				BOOL Result = GetOverlappedResult(hHandle, &readOV, &Tans, FALSE);
				if (Result == FALSE)
				{
					if (GetLastError() == ERROR_IO_INCOMPLETE)
					{
						std::cout << "�д� ��: " << iCnt++ << std::endl;
					}
				}
				if (Result == TRUE)
				{
					QueryPerformanceCounter(&Frequency2);
					std::cout << (Frequency2.LowPart - Frequency1.LowPart) / (float)Frequency.LowPart << "��" << std::endl;
					std::cout << readOV.InternalHigh << " �б� �Ϸ�." << std::endl;
				}
			}
			else
			{
				std::cout << dwRet << " Error" << std::endl;
				break;
			}
			Sleep(1);
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