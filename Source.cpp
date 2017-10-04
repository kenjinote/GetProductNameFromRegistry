#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

BOOL GetProductNameFromRegistry(LPWSTR lpszProductName, INT nStringLength)
{
	BOOL bSuccess = FALSE;
	HKEY hKey;
	if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ | KEY_WOW64_64KEY, &hKey))
	{
		DWORD dwSize = 0;
		if (ERROR_SUCCESS == RegQueryValueExW(hKey, L"ProductName", 0, 0, 0, &dwSize) && dwSize != 0)
		{
			LPWSTR lpszValue = (LPWSTR)GlobalAlloc(GMEM_FIXED, dwSize);
			if (lpszValue)
			{
				if (ERROR_SUCCESS == RegQueryValueExW(hKey, L"ProductName", 0, 0, (LPBYTE)lpszValue, &dwSize) && dwSize != 0)
				{
					if (lstrcpynW(lpszProductName, lpszValue, nStringLength))
					{
						bSuccess = TRUE;
					}
				}
				GlobalFree(lpszValue);
			}
		}
		RegCloseKey(hKey);
	}
	return bSuccess;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	switch (msg)
	{
	case WM_CREATE:
		hEdit = CreateWindow(TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_READONLY | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		{
			WCHAR szProductName[256] = { 0 };
			if (GetProductNameFromRegistry(szProductName, _countof(szProductName)))
			{
				SetWindowTextW(hEdit, szProductName);
			}
			else
			{
				SetWindowTextW(hEdit, L"取得に失敗しました");
			}
		}
		break;
	case WM_SIZE:
		MoveWindow(hEdit, 10, 10, LOWORD(lParam) - 20, 32, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	TCHAR szClassName[] = TEXT("Window");
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("レジストリーから OS の製品名を取得する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
