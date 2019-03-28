#include "stdafx.h"
#include "DXGIManager.h"
#include <time.h>

DXGIManager g_DXGIManager;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WNDCLASSEX g_wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Foo", NULL };
HWND g_hWnd;
HBITMAP g_hbm;
int hDC;
int g_screenX, g_screenY;

int _tmain(int argc, _TCHAR* argv[])
{
    CoInitialize(NULL);

    g_DXGIManager.SetCaptureSource(CaptureSource::CSMonitor1);

    RECT rcDim;
    g_DXGIManager.GetOutputRect(rcDim);

    DWORD dwWidth = rcDim.right - rcDim.left;
    DWORD dwHeight = rcDim.bottom - rcDim.top;

    g_screenX = dwWidth;
    g_screenY = dwHeight;

    RegisterClassEx(&g_wc);
    g_hWnd = CreateWindow(L"Foo", L"Foo", WS_OVERLAPPEDWINDOW, 0, 0, dwWidth, dwHeight, NULL, NULL, g_wc.hInstance, NULL);
    ::ShowWindow(g_hWnd, SW_SHOW);

    printf("dwWidth=%d dwHeight=%d\n", dwWidth, dwHeight);

    HDC hdcWin = GetDC(g_hWnd);
    RECT rc = { 0, 0, dwWidth, dwHeight };
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = dwWidth;
    bmi.bmiHeader.biHeight = -dwHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    RGBQUAD *prgbBits;
    g_hbm = CreateDIBSection(hdcWin, &bmi, DIB_RGB_COLORS, &reinterpret_cast<void*&>(prgbBits), NULL, 0);

    DWORD dwBufSize = dwWidth*dwHeight * 4;

    vector<BYTE> buf(dwBufSize);
    BYTE *pBuf = buf.data();

    MSG msg;

    while (1)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            HRESULT hr = g_DXGIManager.GetOutputBits(pBuf, rcDim);

            if (hr != S_OK)
            {
                continue;
            }

            memcpy(prgbBits, pBuf, dwWidth * dwHeight * 4);
            ::InvalidateRect(g_hWnd, nullptr, TRUE);
        }
    }

    CoUninitialize();

    return msg.wParam;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        return 0;
    }

    if (msg == WM_PAINT)
    {
        PAINTSTRUCT paint;
        HDC hDC = ::BeginPaint(hWnd, &paint);

        HDC hdcMem = CreateCompatibleDC(hDC);
        HBITMAP hbmPrev = (HBITMAP)::SelectObject(hdcMem, g_hbm);

        BitBlt(hDC, 0, 0, g_screenX, g_screenY, hdcMem, 0, 0, SRCCOPY);
        
        SelectObject(hdcMem, hbmPrev);
        DeleteDC(hdcMem);

        ::EndPaint(hWnd, &paint);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
