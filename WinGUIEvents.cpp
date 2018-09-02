#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPCWSTR e_lpszRegisterClassFailed = L"Call to RegisterClass failed.";
	LPCWSTR e_lpszCreateWindowFailed = L"Call to CreateWindow failed.";
	LPCWSTR lpszErrorCaption = L"Error";
	LPCWSTR lpszWndClassName = L"WinGUIEventsMainWindow";
	LPCWSTR lpszWndTitle = L"Windows GUI Events Lab";
	UINT iWndWidth = 1000;
	UINT iWndHeight = 500;

	WNDCLASS wndc = { 0 };
	wndc.style = CS_HREDRAW | CS_VREDRAW;
	wndc.lpfnWndProc = WndProc;
	wndc.hInstance = hInstance;
	wndc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wndc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndc.hInstance = hInstance;
	wndc.lpszClassName = lpszWndClassName;

	if (!RegisterClass(&wndc)) {
		MessageBox(NULL, e_lpszRegisterClassFailed, lpszErrorCaption, NULL);
		return 1;
	}

	HWND hWnd = CreateWindow(lpszWndClassName, lpszWndTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, iWndWidth, iWndHeight, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		MessageBox(NULL, e_lpszCreateWindowFailed, lpszErrorCaption, NULL);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPCWSTR lpszHelloWorld = L"Hello, World!";

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 10, lpszHelloWorld, wcslen(lpszHelloWorld));
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
