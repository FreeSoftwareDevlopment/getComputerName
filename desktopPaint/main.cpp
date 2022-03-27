#include <Windows.h>

bool shouldClose();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#define bgcolor RGB(0x17, 0x17, 0x17)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow) {
	if(shouldClose()){
		MessageBoxA(NULL, "GetComputerName", "Get Computer Name is already running!", MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
		return;
	}
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));

	wcex.cbSize = sizeof(WNDCLASSEX);
	const wchar_t stricl[] = L"DeskClass";
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(bgcolor);
	wcex.lpszClassName = stricl;
	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE, stricl, L"Computer Name:", WS_DISABLED,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		break;
	}
	case WM_PAINT:
	{
		unsigned long s = 16;
		wchar_t hostname[16 + 1];
		GetComputerNameW(&hostname[0], &s);
#define beginat 7
#define wbeg  7
#define width 9
#define height 50
		size_t winwidth = (width * s) + 9;
		SetWindowPos(hWnd, HWND_TOPMOST, -wbeg, -beginat, winwidth + 8, height + 7, SWP_SHOWWINDOW);
		SetWindowRgn(hWnd, CreateRectRgn(wbeg, beginat, winwidth, height), 0);

		{
			RECT aRect;
			int taskb = 0;
			HWND taskBar = FindWindow(L"Shell_traywnd", NULL);
			if (taskBar && GetWindowRect(taskBar, &aRect) != 0)
				taskb = aRect.bottom - aRect.top;//GET TASKBAR
			if (GetWindowRect(GetDesktopWindow(), &aRect) != 0)
				SetWindowPos(hWnd, HWND_TOPMOST, 
					-wbeg, aRect.bottom - height - beginat - ((taskb < (aRect.bottom - aRect.top) + 3) ? taskb : 0),
					NULL, NULL, SWP_SHOWWINDOW | SWP_NOSIZE);
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SetBkColor(hdc, bgcolor);
		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
		TextOutW(
			hdc,
			0,
			0,
			&hostname[0],
			s
		);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool shouldClose() {
	//Make sure at most one instance of the tool is running
	HANDLE hMutexOneInstance(::CreateMutexA(NULL, TRUE, "{a1097153-3429-4248-931d-8560cb14aef8}"));
	bool bAlreadyRunning((::GetLastError() == ERROR_ALREADY_EXISTS));
	if (hMutexOneInstance == NULL || bAlreadyRunning)
	{
		if (hMutexOneInstance)
		{
			::ReleaseMutex(hMutexOneInstance);
			::CloseHandle(hMutexOneInstance);
		}
		return true;
	}
	return false;
}
