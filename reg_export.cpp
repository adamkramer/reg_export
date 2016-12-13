/*  This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Created by Adam Kramer [2016] - Email: adamkramer at hotmail dot com */

#include "stdafx.h"
#include "stdio.h"
#include "windows.h"


int _tmain(int argc, _TCHAR* argv[])
{

	/*  SECTION 1 - Check arguments are valid, else display welcome message */
	printf("      *** reg_export - Created by Adam Kramer [2016] ***\n");
	printf("-----------------------------------------------------------\n");

	if (argc < 4) {

		printf("This program exports the raw content of a registry value to a file\n");
		printf("Usage: reg_export.exe <registry key> <value name> <file>\n");
		printf("[ Example: reg_export.exe HKEY_CURRENT_USER\\Console CursorSize C:\\output.raw ]\n\n");
		printf("N.B. If you want the default value for a subkey, enter the value name (default)\n");

		return -1;
	}

	for (int i = 1; i < argc; i++)
		if (!wcslen(argv[i]))
		{
			printf("Error: One of the arguments has length zero\n");
			return -1;
		}
	
	/* SECTION 2 - Read data from registry key */
	
	/* Variables for registry key & handle */
	HKEY hKey, phKey;

	/* Copy registry key to 'wRegKey' and make it uppercase */
	wchar_t* wRegKey = new wchar_t[(wcslen(argv[1]))];
	wcscpy_s(wRegKey, wcslen(argv[1])*sizeof(wchar_t), argv[1]);
	CharUpper(wRegKey);

	/* Identify which registry key is being requested */
	if ((wRegKey == wcsstr(wRegKey, L"HKEY_CLASSES_ROOT")) ||
			(wRegKey == wcsstr(wRegKey, L"HKCR")))
		hKey = HKEY_CLASSES_ROOT;
	else if ((wRegKey == wcsstr(wRegKey, L"HKEY_CURRENT_CONFIG")) ||
			(wRegKey == wcsstr(wRegKey, L"HKCC")))
		hKey = HKEY_CURRENT_CONFIG;
	else if ((wRegKey == wcsstr(wRegKey, L"HKEY_CURRENT_USER")) ||
			(wRegKey == wcsstr(wRegKey, L"HKCU")))
		hKey = HKEY_CURRENT_USER;
	else if ((wRegKey == wcsstr(wRegKey, L"HKEY_LOCAL_MACHINE")) ||
			(wRegKey == wcsstr(wRegKey, L"HKLM")))
		hKey = HKEY_LOCAL_MACHINE;
	else if ((wRegKey == wcsstr(wRegKey, L"HKEY_USERS")) ||
			(wRegKey == wcsstr(wRegKey, L"HKU")))
		hKey = HKEY_USERS;
	else { 
		printf("Error: Argument 1 does not appear to be a registry key\n"); 
		return -1;  
		}

	/* Split subkey out into "wSubKey" */
	wchar_t* wSubKey = wcschr(wRegKey, L'\\');

	if (!wSubKey) {
		printf("Error: Invalid registry key\n");
		return -1;
	}

	wSubKey++;

	/* dValueSize will store size of requested value data */
	DWORD dValueSize;

	/* Copy requested value to 'wValue' and make it uppercase */
	wchar_t* wValue = new wchar_t[(wcslen(argv[2]))];
	wcscpy_s(wValue, wcslen(argv[2])*sizeof(wchar_t), argv[2]);
	CharUpper(wValue);

	/* If user requests default value, set wValue to NULL */
	if (wcsstr(wValue, L"(DEFAULT)"))
		wValue = NULL;

	/* Open handle to registry key - phKey will hold handle */
	if (RegOpenKeyEx(hKey, wSubKey, 0, KEY_READ, &phKey))
	{
		printf("Error: Cannot open handle to registry key\n");
		return -1;
	}

	/* Obtain size of value data */
	if (RegQueryValueEx(phKey, wValue, NULL, NULL, NULL, &dValueSize))
	{
		printf("Error: Cannot obtain size of requested registry value\n");
		return -1;
	}

	/* Allocate space to hold value data, and read it from the registry */
	TCHAR *bData = (TCHAR *)malloc(dValueSize * sizeof(TCHAR));

	if (RegQueryValueEx(phKey, wValue, NULL, NULL, (LPBYTE)bData, &dValueSize))
	{
		printf("Error: Cannot read specified registry value\n");
		return -1;
	}

	/* Close handle to registry key */
	CloseHandle(phKey);

	/* SECTION 3 - Write data to file */

	/* Write value data (bData) to the requested file */
	DWORD dBytesWritten;
	HANDLE hFile = CreateFile(argv[3], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Error: Cannot open handle to specified file\n");

		CloseHandle(hFile);
		return -1;
	}

	if (!WriteFile(hFile, bData, dValueSize, &dBytesWritten, NULL))
	{
		printf("Error: Cannot write data to specified file\n");

		CloseHandle(hFile);
		return -1;
	}

	CloseHandle(hFile);

	/* Inform user that action has been successfully completed */
	printf("Success: Selected registry value has been saved to the file\n");

	return 0;
}
