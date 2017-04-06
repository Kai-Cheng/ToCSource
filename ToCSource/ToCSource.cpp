// ToCSource.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "stdlib.h"
#include <windows.h>

_TCHAR *get_filename_extT(_TCHAR* filename) {
	_TCHAR *dot = _tcsrchr(filename, _T('.'));
	if (!dot || dot == filename) return NULL;
	return dot + 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfile;
	unsigned long  datasize, retSize, i, j;
	TCHAR newDir[MAX_PATH * 10];
	TCHAR *pDir;
	DWORD dwError = 0;
	char cTemp0 = 0;
	char cTemp1 = 0;
	char cTemp2 = 0;

	// If the directory is not specified as a command-line argument,
	// print usage.

#if _DEBUG//[.4test.
	argc = 2;
	argv[0] = _T("ToCSource.exe");
	argv[1] = _T("+ W23& H23_R.raw");
	//argv[2] = _T("2");
	if (argc != 2)
	{
		_tprintf(TEXT("\nUsage: %s <file path>\n"), argv[0]);
		return (-1);
	}
#else
	if (argc == 2 && (!_tcscmp(argv[1], _T("-?")) || !_tcscmp(argv[1], _T("-h")) || !_tcscmp(argv[1], _T("-help"))))
	{
		printf("Version 1.00\r\n");
		_tprintf(TEXT("\nUsage: %s <file path>\n"), argv[0]);
		//printf("This program built for Win8.1x64\n");
		printf("Report error to kai.cheng.wang@gmail.com\n");
		return 0;
	}
	if (argc != 2)
	{
		printf("Command error!\r\n");
		_tprintf(TEXT("\nUsage: %s <file path>\n"), argv[0]);
		return (-1);
	}
#endif//].

	_tcscpy(newDir, argv[1]);
	pDir = get_filename_extT(newDir);
	_tcscpy(pDir, _T("c")); // *.c

	// Open file and reading data to pBuf
	dwError = _wfopen_s(&pfile, argv[1], _T("rb"));
	if (pfile == NULL)
	{
		_tprintf(TEXT("\nOpen Error:%s, %s\n"), argv[1], TEXT("rb"));
		return (-1);
	}

	fseek(pfile, 0, SEEK_END);
	datasize = ftell(pfile);

	unsigned char *pbuf = new unsigned char[datasize + 4];
	if (pbuf == NULL)
	{
		printf("Malloc memory fail.");
		return (-1);
	}

	fseek(pfile, 0, SEEK_SET);
	retSize = fread(pbuf, sizeof(unsigned char), datasize, pfile);
	fclose(pfile);
	if (retSize != datasize)
	{

		delete[] pbuf;
		pbuf = NULL;
		printf("Read file fail.");
		return (-1);
	}
	// Convert to C source data
	char dLenStr[10];
	unsigned long  cSrcdataLLen = 0;
	unsigned long  cSrcdataSize = 35;

	cSrcdataSize += (datasize * 6); // "0xFF, "
	cSrcdataSize += ((datasize + 15) / 16) * 3; // "\t\r\n"

	char *pCSrcbuf = new char[cSrcdataSize];
	if (pCSrcbuf == NULL)
	{
		delete[] pbuf;
		pbuf = NULL;
		printf("Malloc memory fail [C].");
		return (-1);
	}
	strcpy(pCSrcbuf, "unsigned char data[");
	_itoa(datasize, dLenStr, 10);
	strcat(pCSrcbuf, dLenStr);
	strcat(pCSrcbuf, "] =\r\n{");
	for (i = 0, j = 1; i < datasize; i++, j++)
	{
		if ((i % 16) == 0)
		{
			strcat(pCSrcbuf, "\r\n\t");
		}
		strcat(pCSrcbuf, "0x");
		_itoa(pbuf[i], dLenStr, 16);
		if (strlen(dLenStr) == 1) // Align two digits
		{
			strcat(pCSrcbuf, "0");
		}
		// to Uppercase
		if (dLenStr[0] >= 0x61 && dLenStr[0] <= 0x6F) /* a ~ f */
		{
			dLenStr[0] -= 0x20;
		}
		if (dLenStr[1] >= 0x61 && dLenStr[1] <= 0x6F) /* a ~ f */
		{
			dLenStr[1] -= 0x20;
		}

		strcat(pCSrcbuf, dLenStr);
		if (j < datasize)
		{
			strcat(pCSrcbuf, ", ");
		}
	}
	strcat(pCSrcbuf, "\r\n};");
	cSrcdataLLen = strlen(pCSrcbuf);

	/* open file with w+b and write pBuf to file */
	//dwError = _wfopen_s(&pfile, argv[1], _T("w+b"));
	dwError = _wfopen_s(&pfile, newDir, _T("w+b"));

	if (pfile == NULL)
	{
		delete[] pbuf;
		pbuf = NULL;
		delete[] pCSrcbuf;
		pCSrcbuf = NULL;
		_tprintf(TEXT("\nOpen Error:%s, %s\n"), argv[1], TEXT("w+b"));
		return (-1);
	}
	retSize = fwrite(pCSrcbuf, sizeof(unsigned char), cSrcdataLLen, pfile);
	fclose(pfile);

	if (retSize != cSrcdataLLen)
	{
		delete[] pbuf;
		pbuf = NULL;
		delete[] pCSrcbuf;
		pCSrcbuf = NULL;
		printf("Write file fail.");
		return (-1);
	}

	if (pbuf)
	{
		delete[] pbuf;
		pbuf = NULL;
	}
	if (pCSrcbuf)
	{
		delete[] pCSrcbuf;
		pCSrcbuf = NULL;
	}
	printf("Convert complete!");
	return 0;
}
