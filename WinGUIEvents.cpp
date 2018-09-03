#include <windows.h>

#define E_IMAGE_NOT_LOADED L"Can't load image"
#define ERROR_CAPTION L"Error"

#define WINDOW_CLASS_NAME L"WinGUIEventsMainWindow"
#define WINDOW_TITLE L"Windows GUI Events Lab"
#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_HEIGHT 500
#define MIN_WINDOW_WIDTH 400
#define MIN_WINDOW_HEIGHT 400

#define HELP_MENU_ITEM 0x01
#define AUTHOR_MENU_ITEM 0x02
#define HELP_MENU_ITEM_TITLE L"Help"
#define AUTHOR_MENU_ITEM_TITLE L"Author"

#define TEXT_HELP L"No help here right now"
#define TEXT_ABOUT_AUTHOR L"Arseni Rynkevich\r\nStudent group ¹651003\r\nhttps://github.com/NRGb3nder"

#define SPRITE_PATH L"sprite.bmp"

#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_LEFT 0x03
#define MOVE_RIGHT 0x04
#define SPRITE_MOVEMENT_STEP 5
#define IDT_SPRITE_TIMER 0xFF
#define SPRITE_TIMER_INTERVAL 50

#define MASK_TRANSPARENT RGB(255, 0, 255)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID AddMenu(HWND);
BOOL LoadSprite();
VOID DrawSprite(HWND);
VOID ProtectBorders(INT, INT, INT, INT, INT, INT);
VOID MoveSpriteMW(WPARAM);
VOID MoveSpriteOnMouseWheel(WPARAM);
VOID CALLBACK MoveSpriteOnTimer(HWND, UINT, UINT_PTR, DWORD);

HMENU hMenu;
HBITMAP hBitmap;

INT xSpriteOffset = 0;
INT ySpriteOffset = 0;
SHORT nSpriteDirection = MOVE_UP;

INT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASS wndc = { 0 };
	wndc.style = CS_HREDRAW | CS_VREDRAW;
	wndc.lpfnWndProc = WndProc;
	wndc.hInstance = hInstance;
	wndc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wndc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndc.lpszClassName = WINDOW_CLASS_NAME;

	RegisterClass(&wndc);

	HWND hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT wndRect;
	LPMINMAXINFO lpMMI;

	switch (message) {
	case WM_PAINT:
		DrawSprite(hWnd);
		break;
	case WM_MOUSEWHEEL:
		MoveSpriteOnMouseWheel(wParam);
		GetClientRect(hWnd, &wndRect);
		InvalidateRect(hWnd, &wndRect, TRUE);
		break;
	case WM_COMMAND:
		switch (wParam) {
		case HELP_MENU_ITEM:
			MessageBox(NULL, TEXT_HELP, HELP_MENU_ITEM_TITLE, MB_OK);
			break;
		case AUTHOR_MENU_ITEM:
			MessageBox(NULL, TEXT_ABOUT_AUTHOR, AUTHOR_MENU_ITEM_TITLE, MB_OK);
			break;
		}
		break;
	case WM_CREATE:
		if (!LoadSprite()) {
			MessageBox(NULL, E_IMAGE_NOT_LOADED, ERROR_CAPTION, MB_OK);
			PostQuitMessage(0);
		}
		AddMenu(hWnd);
		SetTimer(hWnd, IDT_SPRITE_TIMER, SPRITE_TIMER_INTERVAL, MoveSpriteOnTimer);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, IDT_SPRITE_TIMER);
		PostQuitMessage(0);
		break;
	case WM_GETMINMAXINFO:
		lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
		lpMMI->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

VOID AddMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, HELP_MENU_ITEM, HELP_MENU_ITEM_TITLE);
	AppendMenu(hMenu, MF_STRING, AUTHOR_MENU_ITEM, AUTHOR_MENU_ITEM_TITLE);
	SetMenu(hWnd, hMenu);
}


BOOL LoadSprite()
{
	return (BOOL)(hBitmap = (HBITMAP)LoadImage(NULL, SPRITE_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
}

VOID DrawSprite(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hWndDc;
	HDC hMemDc;
	BITMAP bmp;
	HBITMAP hOldBmp;
	RECT wndRect;

	hWndDc = BeginPaint(hWnd, &ps);

	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	GetClientRect(hWnd, &wndRect);
	INT widthWnd = wndRect.right - wndRect.left;
	INT heightWnd = wndRect.bottom - wndRect.top;
	INT xDest = ((widthWnd - bmp.bmWidth) / 2) + xSpriteOffset;
	INT yDest = ((heightWnd - bmp.bmHeight) / 2) + ySpriteOffset;

	hMemDc = CreateCompatibleDC(hWndDc);
	hOldBmp = (HBITMAP)SelectObject(hMemDc, hBitmap);
	ProtectBorders(xDest, yDest, bmp.bmWidth, bmp.bmHeight, widthWnd, heightWnd);
	TransparentBlt(hWndDc, xDest, yDest, bmp.bmWidth, bmp.bmHeight, hMemDc, 
		0, 0, bmp.bmWidth, bmp.bmHeight, MASK_TRANSPARENT);
	SelectObject(hMemDc, hOldBmp);
	DeleteDC(hMemDc);
	DeleteObject(hOldBmp);

	EndPaint(hWnd, &ps);
}

VOID ProtectBorders(INT xDest, INT yDest, INT widthDest, INT heightDest, INT widthWnd, INT heightWnd)
{
	if (xDest <= 0) {
		nSpriteDirection = MOVE_RIGHT;
		xSpriteOffset = -(widthWnd - widthDest) / 2;
	} else if (xDest + widthDest >= widthWnd) {
		nSpriteDirection = MOVE_LEFT;
		xSpriteOffset = (widthWnd - widthDest) / 2;
	} else if (yDest <= 0) {
		nSpriteDirection = MOVE_DOWN;
		ySpriteOffset = -(heightWnd - heightDest) / 2;
	} else if (yDest + heightDest >= heightWnd) {
		nSpriteDirection = MOVE_UP;
		ySpriteOffset = (heightWnd - heightDest) / 2;
	}
}

VOID MoveSpriteOnMouseWheel(WPARAM wParam)
{
	if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			nSpriteDirection = MOVE_RIGHT;
			xSpriteOffset += SPRITE_MOVEMENT_STEP;
		} else {
			nSpriteDirection = MOVE_LEFT;
			xSpriteOffset -= SPRITE_MOVEMENT_STEP;
		}
	} else {
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			nSpriteDirection = MOVE_DOWN;
			ySpriteOffset += SPRITE_MOVEMENT_STEP;
		} else {
			nSpriteDirection = MOVE_UP;
			ySpriteOffset -= SPRITE_MOVEMENT_STEP;
		}
	}
}

VOID CALLBACK MoveSpriteOnTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	switch (nSpriteDirection) {
	case MOVE_UP:
		ySpriteOffset -= SPRITE_MOVEMENT_STEP;
		break;
	case MOVE_DOWN:
		ySpriteOffset += SPRITE_MOVEMENT_STEP;
		break;
	case MOVE_LEFT:
		xSpriteOffset -= SPRITE_MOVEMENT_STEP;
		break;
	case MOVE_RIGHT:
		xSpriteOffset += SPRITE_MOVEMENT_STEP;
		break;
	}
	RECT wndRect;
	GetClientRect(hWnd, &wndRect);
	InvalidateRect(hWnd, &wndRect, TRUE);
}