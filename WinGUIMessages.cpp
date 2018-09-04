#include <windows.h>

#define E_IMAGE_NOT_LOADED L"Can't load image"
#define ERROR_CAPTION L"Error"

#define WINDOW_CLASS_NAME L"WinGUIMessagesMainWindow"
#define WINDOW_TITLE L"Windows GUI Messages Lab"
#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_HEIGHT 500
#define MIN_WINDOW_WIDTH 400
#define MIN_WINDOW_HEIGHT 400

#define IDM_OBJECT_TYPE 0x00
#define IDM_RECTANGLE 0x01
#define IDM_ELLIPSE 0x02
#define IDM_SPRITE 0x03
#define IDM_HELP 0x04
#define IDM_AUTHOR 0x05

#define OBJECT_TYPE_MENU_ITEM_TITLE L"Object Type"
#define HELP_MENU_ITEM_TITLE L"Help"
#define AUTHOR_MENU_ITEM_TITLE L"Author"
#define RECTANGLE_MENU_ITEM_TITLE L"Rectangle"
#define ELLIPSE_MENU_ITEM_TITLE L"Ellipse"
#define SPRITE_MENU_ITEM_TITLE L"Custom sprite"

#define TEXT_HELP L"Use MOUSE WHEEL and SHIFT to move object.\r\nObject type can be changed in menu."
#define TEXT_ABOUT_AUTHOR L"Arseni Rynkevich\r\nStudent group ¹651003\r\nhttps://github.com/NRGb3nder"

#define SPRITE_PATH L"sprite.bmp"

#define MOVE_NOT 0x00
#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_LEFT 0x03
#define MOVE_RIGHT 0x04
#define OBJECT_MOVEMENT_STEP 5
#define IDT_SPRITE_TIMER 0xFF
#define OBJECT_TIMER_INTERVAL 50

#define MASK_TRANSPARENT RGB(255, 0, 255)

#define SHAPE_TYPE_RECTANGLE 0x00
#define SHAPE_TYPE_ELLIPSE 0x01
#define SHAPE_WIDTH 200
#define SHAPE_HEIGHT 100
#define SHAPE_BRUSH_COLOR RGB(63, 136, 143)
#define SHAPE_PEN_COLOR RGB(0, 0, 0)
#define SHAPE_PEN_WIDTH 1
#define SHAPE_PEN_TYPE PS_SOLID

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID AddMenu(HWND);
VOID RespawnObject();
BOOL LoadSprite();
VOID DrawSprite(HWND);
VOID ProtectBorders(INT, INT, INT, INT, INT, INT);
VOID MoveSpriteMW(WPARAM);
VOID MoveSpriteOnMouseWheel(WPARAM);
VOID CALLBACK MoveSpriteOnTimer(HWND, UINT, UINT_PTR, DWORD);

HMENU hMenu;
HMENU hObjectTypeSubMenu;
HBITMAP hBitmap;

INT xObjectOffset = 0;
INT yObjectOffset = 0;
SHORT nObjectDirection = MOVE_NOT;

BOOL bSpriteOff = TRUE;
SHORT nShapeType = SHAPE_TYPE_RECTANGLE;

INT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
    WNDCLASS wndc = { 0 };
    wndc.style = CS_HREDRAW | CS_VREDRAW;
    wndc.lpfnWndProc = WndProc;
    wndc.hInstance = hInstance;
    wndc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
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

    return 0;
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
        case IDM_RECTANGLE:
            bSpriteOff = TRUE;
            nShapeType = SHAPE_TYPE_RECTANGLE;
            RespawnObject();
            break;
        case IDM_ELLIPSE:
            bSpriteOff = TRUE;
            nShapeType = SHAPE_TYPE_ELLIPSE;
            RespawnObject();
            break;
        case IDM_SPRITE:
            bSpriteOff = FALSE;
            RespawnObject();
            break;
        case IDM_HELP:
            MessageBox(NULL, TEXT_HELP, HELP_MENU_ITEM_TITLE, MB_OK);
            break;
        case IDM_AUTHOR:
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
        SetTimer(hWnd, IDT_SPRITE_TIMER, OBJECT_TIMER_INTERVAL, MoveSpriteOnTimer);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, IDT_SPRITE_TIMER);
        PostQuitMessage(0);
        break;
    case WM_GETMINMAXINFO:
        lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
        lpMMI->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

VOID AddMenu(HWND hWnd)
{
    hMenu = CreateMenu();
    hObjectTypeSubMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hObjectTypeSubMenu, OBJECT_TYPE_MENU_ITEM_TITLE);
    AppendMenu(hObjectTypeSubMenu, MF_STRING, IDM_RECTANGLE, RECTANGLE_MENU_ITEM_TITLE);
    AppendMenu(hObjectTypeSubMenu, MF_STRING, IDM_ELLIPSE, ELLIPSE_MENU_ITEM_TITLE);
    AppendMenu(hObjectTypeSubMenu, MF_STRING, IDM_SPRITE, SPRITE_MENU_ITEM_TITLE);
    AppendMenu(hMenu, MF_STRING, IDM_HELP, HELP_MENU_ITEM_TITLE);
    AppendMenu(hMenu, MF_STRING, IDM_AUTHOR, AUTHOR_MENU_ITEM_TITLE);
    SetMenu(hWnd, hMenu);
}

VOID RespawnObject()
{
    nObjectDirection = MOVE_NOT;
    xObjectOffset = 0;
    yObjectOffset = 0;
}

BOOL LoadSprite()
{
    return (hBitmap = (HBITMAP)LoadImage(NULL, SPRITE_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)) ? TRUE : FALSE;
}

VOID DrawSprite(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hWndDc;
    HDC hMemDc;
    BITMAP bmp;
    HBITMAP hOldBmp;
    RECT wndRect;
    INT xDest;
    INT yDest;

    GetClientRect(hWnd, &wndRect);
    INT widthWnd = wndRect.right - wndRect.left;
    INT heightWnd = wndRect.bottom - wndRect.top;

    hWndDc = BeginPaint(hWnd, &ps);

    if (bSpriteOff) {
        HBRUSH hBr = CreateSolidBrush(SHAPE_BRUSH_COLOR);
        HBRUSH hOldBr = (HBRUSH)SelectObject(hWndDc, hBr);
        HPEN hPn = CreatePen(SHAPE_PEN_TYPE, SHAPE_PEN_WIDTH, SHAPE_PEN_COLOR);
        HPEN hOldPn = (HPEN)SelectObject(hWndDc, hPn);

        xDest = ((widthWnd - SHAPE_WIDTH) / 2) + xObjectOffset;
        yDest = ((heightWnd - SHAPE_HEIGHT) / 2) + yObjectOffset;
        ProtectBorders(xDest, yDest, SHAPE_WIDTH, SHAPE_HEIGHT, widthWnd, heightWnd);
        switch (nShapeType) {
        case SHAPE_TYPE_RECTANGLE:
            Rectangle(hWndDc, xDest, yDest, xDest + SHAPE_WIDTH, yDest + SHAPE_HEIGHT);
            break;
        case SHAPE_TYPE_ELLIPSE:
            Ellipse(hWndDc, xDest, yDest, xDest + SHAPE_WIDTH, yDest + SHAPE_HEIGHT);
            break;
        }

        SelectObject(hWndDc, hOldBr);
        DeleteObject(hBr);
        SelectObject(hWndDc, hOldPn);
        DeleteObject(hPn);
    }
    else {
        GetObject(hBitmap, sizeof(BITMAP), &bmp);
        xDest = ((widthWnd - bmp.bmWidth) / 2) + xObjectOffset;
        yDest = ((heightWnd - bmp.bmHeight) / 2) + yObjectOffset;
        hMemDc = CreateCompatibleDC(hWndDc);
        hOldBmp = (HBITMAP)SelectObject(hMemDc, hBitmap);
        ProtectBorders(xDest, yDest, bmp.bmWidth, bmp.bmHeight, widthWnd, heightWnd);
        TransparentBlt(hWndDc, xDest, yDest, bmp.bmWidth, bmp.bmHeight, hMemDc,
            0, 0, bmp.bmWidth, bmp.bmHeight, MASK_TRANSPARENT);
        SelectObject(hMemDc, hOldBmp);
        DeleteDC(hMemDc);
        DeleteObject(hOldBmp);
    }

    EndPaint(hWnd, &ps);
}

VOID ProtectBorders(INT xDest, INT yDest, INT widthDest, INT heightDest, INT widthWnd, INT heightWnd)
{
    if (xDest <= 0) {
        nObjectDirection = MOVE_RIGHT;
        xObjectOffset = -(widthWnd - widthDest) / 2;
    } else if (xDest + widthDest >= widthWnd) {
        nObjectDirection = MOVE_LEFT;
        xObjectOffset = (widthWnd - widthDest) / 2;
    } else if (yDest <= 0) {
        nObjectDirection = MOVE_DOWN;
        yObjectOffset = -(heightWnd - heightDest) / 2;
    } else if (yDest + heightDest >= heightWnd) {
        nObjectDirection = MOVE_UP;
        yObjectOffset = (heightWnd - heightDest) / 2;
    }
}

VOID MoveSpriteOnMouseWheel(WPARAM wParam)
{
    if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
            nObjectDirection = MOVE_RIGHT;
            xObjectOffset += OBJECT_MOVEMENT_STEP;
        } else {
            nObjectDirection = MOVE_LEFT;
            xObjectOffset -= OBJECT_MOVEMENT_STEP;
        }
    } else {
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
            nObjectDirection = MOVE_DOWN;
            yObjectOffset += OBJECT_MOVEMENT_STEP;
        } else {
            nObjectDirection = MOVE_UP;
            yObjectOffset -= OBJECT_MOVEMENT_STEP;
        }
    }
}

VOID CALLBACK MoveSpriteOnTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    switch (nObjectDirection) {
    case MOVE_UP:
        yObjectOffset -= OBJECT_MOVEMENT_STEP;
        break;
    case MOVE_DOWN:
        yObjectOffset += OBJECT_MOVEMENT_STEP;
        break;
    case MOVE_LEFT:
        xObjectOffset -= OBJECT_MOVEMENT_STEP;
        break;
    case MOVE_RIGHT:
        xObjectOffset += OBJECT_MOVEMENT_STEP;
        break;
    }

    RECT wndRect;
    GetClientRect(hWnd, &wndRect);
    InvalidateRect(hWnd, &wndRect, TRUE);
}