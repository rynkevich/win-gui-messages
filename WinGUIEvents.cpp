#include <windows.h>

#define MENU_ITEM_HELP 0x01
#define MENU_ITEM_AUTHOR 0x02

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID AddMenus(HWND);

LPCWSTR lpszHelpMenuItemTitle = L"Help";
LPCWSTR lpszAuthorMenuItemTitle = L"Author";

HMENU g_hMenu;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPCWSTR e_lpszRegisterClassFailed = L"Call to RegisterClass failed.";
	LPCWSTR e_lpszCreateWindowFailed = L"Call to CreateWindow failed.";
	LPCWSTR lpszErrorCaption = L"Error";
	LPCWSTR lpszWndClassName = L"WinGUIEventsMainWindow";
	LPCWSTR lpszWndTitle = L"Windows GUI Events Lab";
	CONST UINT iWndWidth = 1000;
	CONST UINT iWndHeight = 500;

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
	LPCWSTR lpszAboutAuthor = L"Arseni Rynkevich\r\nStudent group ¹651003\r\nhttps://github.com/NRGb3nder";
	LPCWSTR lpszHelp = L"No help here right now";

	switch (message) {
	case WM_COMMAND:
		switch (wParam) {
		case MENU_ITEM_HELP:
			MessageBox(NULL, lpszHelp, lpszHelpMenuItemTitle, NULL);
			break;
		case MENU_ITEM_AUTHOR:
			MessageBox(NULL, lpszAboutAuthor, lpszAuthorMenuItemTitle, NULL);
			break;
		}
	case WM_CREATE:
		AddMenus(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

VOID AddMenus(HWND hWnd)
{
	g_hMenu = CreateMenu();
	AppendMenu(g_hMenu, MF_STRING, MENU_ITEM_HELP, lpszHelpMenuItemTitle);
	AppendMenu(g_hMenu, MF_STRING, MENU_ITEM_AUTHOR, lpszAuthorMenuItemTitle);
	SetMenu(hWnd, g_hMenu);
}
