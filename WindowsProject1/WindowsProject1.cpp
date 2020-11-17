#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <cmath>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

#pragma comment (lib,"Gdiplus.lib")

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("Application");

HINSTANCE hInst;
int spriteWidth;
int spriteHeight;
bool started = false;
bool paused = false;
bool mousePressed = false;

int clientWidth;
int clientHeight;

int step = 5;
int x, y;
double dX, dY;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

void InitializeSpriteSizes()
{
    Image temp(L"sprite.bmp");
    spriteHeight = temp.GetHeight();
    spriteWidth = temp.GetWidth();
}

void ResizeWindow(LPARAM lParam)
{
    clientWidth = LOWORD(lParam);
    clientHeight = HIWORD(lParam);
}

void MouseDown(HWND hWnd, LPARAM lParam)
{
    KillTimer(hWnd, 1);
    started = true;
    paused = true;
    mousePressed = true;

    int angle = rand() % 360;

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    dX = 5 * cos(angle);
    dY = 5 * sin(angle);

    InvalidateRect(hWnd, NULL, TRUE);
}

void MouseMove(HWND hWnd, LPARAM lParam)
{
    if (mousePressed)
    {
        x = LOWORD(lParam);
        y = HIWORD(lParam);

        if (y < spriteHeight / 2 + step)
        {
            y = spriteHeight / 2 + step;
        }

        if (y > clientHeight - spriteHeight / 2 - step)
        {
            y = clientHeight - spriteHeight / 2;
        }

        if (x < spriteWidth / 2 - step)
        {
            x = spriteWidth / 2 - step;
        }

        if (x > clientWidth - spriteWidth / 2 + step)
        {
            x = clientWidth - spriteWidth / 2 + step;
        }

        InvalidateRect(hWnd, NULL, true);
    }
}

void KeyDown(HWND hWnd, WPARAM wParam)
{

    switch (wParam)
    {
    case VK_UP:
    {
        if (y > spriteHeight / 2 + step)
        {
            y -= step;
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    case VK_DOWN:
    {
        if (y < clientHeight - spriteHeight / 2 - step)
        {
            y += step;
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    case VK_LEFT:
    {
        if (x > spriteWidth / 2 - step)
        {
            x -= step;
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    case VK_RIGHT:
    {
        if (x < clientWidth - spriteWidth / 2 + step)
        {
            x += step;
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    break;
    }

}

void MouseWheel(HWND hWnd, WPARAM wParam)
{
    int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (LOWORD(wParam) == MK_CONTROL)
    {
        for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
        {
            if (x < clientWidth - spriteWidth / 2)
            {
                x += step;
                InvalidateRect(hWnd, NULL, true);
            }
        }
        for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
        {
            if (x > step)
            {
                x -= step;
                InvalidateRect(hWnd, NULL, true);
            }
        }
    }
    else
    {
        for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
        {
            if (y < clientHeight - spriteHeight / 2 - step)
            {
                y += step;
                InvalidateRect(hWnd, NULL, true);
            }
        }
        for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
        {
            if (y > spriteHeight / 2 + step)
            {
                y -= step;
                InvalidateRect(hWnd, NULL, true);
            }
        }
    }
}

void OnPaint(HWND hWnd, WPARAM wParam, int x, int y, int w, int h)
{
    RECT rect;
    if (!GetClientRect(hWnd, &rect))
    {
        GetLastError();
    }
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP mOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    Graphics graphics(memDC);
    Image image(L"sprite.bmp");
    Rect destRect(x, y, w, h);
    graphics.DrawImage(&image, destRect);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, mOldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        ResizeWindow(lParam);
        break;
    case WM_LBUTTONDOWN:
        MouseDown(hWnd, lParam);
        break;
    case WM_MOUSEMOVE:
        MouseMove(hWnd, lParam);
        break;
    case WM_LBUTTONUP:
        mousePressed = false;
        break;
    case WM_KEYDOWN:
        KeyDown(hWnd, wParam);
        break;
    case WM_MOUSEWHEEL:
        MouseWheel(hWnd, wParam);
        break;
    case WM_PAINT:
        if (started)
        {
            OnPaint(hWnd, wParam, x - spriteWidth / 2, y - spriteHeight / 2, spriteWidth, spriteHeight);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        InitializeSpriteSizes();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (LRESULT)NULL;
}
