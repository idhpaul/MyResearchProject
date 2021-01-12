#include "MouseCursor.h"

#include "scoped_gdi_object.h"

#include <iostream>

CURSORINFO MouseCursor::prevCursorInfo_ = { 0, };

constexpr uint32_t RGBA(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    return (((a << 24) & 0xFF000000) | ((b << 16) & 0xFF0000) | ((g << 8) & 0xFF00) | (r & 0xFF));
}

constexpr int kBytesPerPixel = 4;

// Pixel colors used when generating cursor outlines.
constexpr uint32_t kPixelRgbaBlack = RGBA(0, 0, 0, 0xFF);
constexpr uint32_t kPixelRgbaWhite = RGBA(0xFF, 0xFF, 0xFF, 0xFF);
constexpr uint32_t kPixelRgbaTransparent = RGBA(0, 0, 0, 0);

constexpr uint32_t kPixelRgbWhite = RGB(0xFF, 0xFF, 0xFF);

// Scans a 32bpp bitmap looking for any pixels with non-zero alpha component.
// Returns true if non-zero alpha is found. |stride| is expressed in pixels.
static bool hasAlphaChannel(const uint32_t* data, int width, int height)
{
    const RGBQUAD* plane = reinterpret_cast<const RGBQUAD*>(data);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (plane->rgbReserved != 0)
                return true;

            ++plane;
        }
    }

    return false;
}

// Expands the cursor shape to add a white outline for visibility against
// dark backgrounds.
static void addCursorOutline(int width, int height, uint32_t* data)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // If this is a transparent pixel (bgr == 0 and alpha = 0), check
            // the neighbor pixels to see if this should be changed to an
            // outline pixel.
            if (*data == kPixelRgbaTransparent)
            {
                // Change to white pixel if any neighbors (top, bottom, left,
                // right) are black.
                if ((y > 0 && data[-width] == kPixelRgbaBlack) ||
                    (y < height - 1 && data[width] == kPixelRgbaBlack) ||
                    (x > 0 && data[-1] == kPixelRgbaBlack) ||
                    (x < width - 1 && data[1] == kPixelRgbaBlack))
                {
                    *data = kPixelRgbaWhite;
                }
            }

            ++data;
        }
    }
}

// Premultiplies RGB components of the pixel data in the given image by
// the corresponding alpha components.
static void alphaMul(uint32_t* data, int width, int height)
{
    static_assert(sizeof(uint32_t) == kBytesPerPixel,
        "size of uint32 should be the number of bytes per pixel");

    for (uint32_t* data_end = data + width * height; data != data_end; ++data)
    {
        RGBQUAD* from = reinterpret_cast<RGBQUAD*>(data);
        RGBQUAD* to = reinterpret_cast<RGBQUAD*>(data);

        to->rgbBlue = (static_cast<uint16_t>(from->rgbBlue) * from->rgbReserved) / 0xFF;
        to->rgbGreen = (static_cast<uint16_t>(from->rgbGreen) * from->rgbReserved) / 0xFF;
        to->rgbRed = (static_cast<uint16_t>(from->rgbRed) * from->rgbReserved) / 0xFF;
    }
}

MouseCursor::MouseCursor()
{
    // 스크린 크기를 저장하기 위한 변수
    RECT desktop;
    // 데스크 탑의 핸들
    const HWND hDesktop = GetDesktopWindow();
    // 핸들로 부터 크기를 받는다.
    GetWindowRect(hDesktop, &desktop);
    // 너비
    int width = desktop.right;
    // 높이
    int height = desktop.bottom;

    // 스크린 핸들러 취득
    //HDC hScreen = GetDC(NULL);
    // 핸들러 생성
    //HDC hDC = CreateCompatibleDC(hScreen);

    // @note : GetDC는 놔둔채로 호출할때마다 CreateCompatibleDC() 호출했을 경우 상태 확인
    ScreenDC_ = GetDC(NULL);
    //hDC_ = CreateCompatibleDC(ScreenDC_);

}

MouseCursor::~MouseCursor()
{
    // 핸들러 삭제 (리소스 해제)
    //DeleteDC(hDC_);
    // 리소스 해제
    ReleaseDC(NULL, ScreenDC_);
}

CursorPayload MouseCursor::CaptureCursor()
{

    CursorPayload payloadData;

    if (ScreenDC_ == NULL)
    {
        std::cerr << "Failed to get dc" << std::endl;

        payloadData.cursorsize = -1;
        return payloadData;
    }
    
    hDC_ = CreateCompatibleDC(ScreenDC_);

    CURSORINFO cursorInfo = { 0 };
    cursorInfo.cbSize = sizeof(CURSORINFO);

    if (GetCursorInfo(&cursorInfo) == TRUE)
    {

        if (!isSameCursorShape(cursorInfo, prevCursorInfo_))
        {
            if (cursorInfo.flags == 0)
            {
                std::cout << "None system cursor" << std::endl;
                cursorInfo.hCursor = LoadCursorW(nullptr, IDC_ARROW);
            }

            if (mouseCursorFromHCursor(hDC_, cursorInfo.hCursor, payloadData))
            {
                prevCursorInfo_ = cursorInfo;
            }
            else
            {
                std::cerr << "Failed to get cursor" << std::endl;
                payloadData.cursorsize = -1;
            }
        }
        else
        {
            //std::cout << "Cursor is same" << std::endl;
            payloadData.cursorsize = 0;
        }
    }

    DeleteDC(hDC_);

    return payloadData;
}

bool MouseCursor::isSameCursorShape(const CURSORINFO& nowCursorInfo, const CURSORINFO& prevCursorInfo)
{
    return nowCursorInfo.flags == prevCursorInfo.flags && (nowCursorInfo.flags != CURSOR_SHOWING ||
        nowCursorInfo.hCursor == prevCursorInfo.hCursor);
}

bool MouseCursor::mouseCursorFromHCursor(HDC& dc, HCURSOR& cursor, CursorPayload& payloadData)
{
    ICONINFO icon_info = { 0 };

    if (!GetIconInfo(cursor, &icon_info))
    {
        std::cerr << "GetIconInfo failed" << std::endl;
        return false;
    }

    // Make sure the bitmaps will be freed.
    ScopedHBITMAP scoped_mask(icon_info.hbmMask);
    ScopedHBITMAP scoped_color(icon_info.hbmColor);

    bool is_color = (icon_info.hbmColor != nullptr);

    // Get |scoped_mask| dimensions.
    BITMAP bitmap_info;

    if (!GetObjectW(scoped_mask, sizeof(bitmap_info), &bitmap_info))
    {
        std::cerr << "GetObjectW failed" << std::endl;
        //return nullptr;
    }

    int width = bitmap_info.bmWidth;
    int height = bitmap_info.bmHeight;

    std::unique_ptr<uint32_t[]> mask_data = std::make_unique<uint32_t[]>(width * height);

    // Get pixel data from |scoped_mask| converting it to 32bpp along the way.
    // GetDIBits() sets the alpha component of every pixel to 0.
    BITMAPV5HEADER bmi = { 0 };

    bmi.bV5Size = sizeof(bmi);
    bmi.bV5Width = width;
    bmi.bV5Height = -height; // request a top-down bitmap.
    bmi.bV5Planes = 1;
    bmi.bV5BitCount = kBytesPerPixel * 8;
    bmi.bV5Compression = BI_RGB;
    bmi.bV5AlphaMask = 0xFF000000;
    bmi.bV5CSType = LCS_WINDOWS_COLOR_SPACE;
    bmi.bV5Intent = LCS_GM_BUSINESS;

    if (!GetDIBits(dc,
        scoped_mask,
        0,
        height,
        mask_data.get(),
        reinterpret_cast<BITMAPINFO*>(&bmi),
        DIB_RGB_COLORS))
    {
        std::cerr << "GetDIBits failed" << std::endl;
        //return nullptr;
    }

    uint32_t* mask_plane = mask_data.get();

    std::vector<uint8_t> image;

    bool has_alpha = false;

    if (is_color)
    {
       // std::cout << "has color" << std::endl;

        image.resize(width * height * kBytesPerPixel);

        // Get the pixels from the color bitmap.
        if (!GetDIBits(dc,
            scoped_color,
            0,
            height,
            image.data(),
            reinterpret_cast<BITMAPINFO*>(&bmi),
            DIB_RGB_COLORS))
        {
            std::cerr << "GetDIBits failed" << std::endl;
            //return nullptr;
        }

        // GetDIBits() does not provide any indication whether the bitmap has
        // alpha channel, so we use HasAlphaChannel() below to find it out.
        has_alpha = hasAlphaChannel(reinterpret_cast<const uint32_t*>(image.data()), width, height);


    }
    else
    {
        //std::cout << "no color" << std::endl;


        // For non-color cursors, the mask contains both an AND and an XOR mask
        // and the height includes both. Thus, the width is correct, but we
        // need to divide by 2 to get the correct mask height.
        height /= 2;

        image.resize(width * height * kBytesPerPixel);

        // The XOR mask becomes the color bitmap.
        memcpy(image.data(), mask_plane + (width * height), image.size());
    }


    //Reconstruct transparency from the mask if the color image does not has
    //alpha channel.

    if (!has_alpha)
    {
        //std::cout << "no alpha" << std::endl;

        bool add_outline = false;
        uint32_t* dst = reinterpret_cast<uint32_t*>(image.data());
        uint32_t* mask = mask_plane;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // The two bitmaps combine as follows:
                //  mask  color   Windows Result   Our result    RGB   Alpha
                //   0     00      Black            Black         00    ff
                //   0     ff      White            White         ff    ff
                //   1     00      Screen           Transparent   00    00
                //   1     ff      Reverse-screen   Black         00    ff
                //
                // Since we don't support XOR cursors, we replace the "Reverse
                // Screen" with black. In this case, we also add an outline
                // around the cursor so that it is visible against a dark
                // background.
                if (*mask == kPixelRgbWhite)
                {
                    if (*dst != 0)
                    {
                        add_outline = true;
                        *dst = kPixelRgbaBlack;
                    }
                    else
                    {
                        *dst = kPixelRgbaTransparent;
                    }
                }
                else
                {
                    *dst = kPixelRgbaBlack ^ *dst;
                }

                ++dst;
                ++mask;
            }
        }

        if (add_outline)
        {
            std::cout << "add outline" << std::endl;

            addCursorOutline(width, height, reinterpret_cast<uint32_t*>(image.data()));
        }
    }

    // Pre-multiply the resulting pixels since MouseCursor uses premultiplied images.
    alphaMul(reinterpret_cast<uint32_t*>(image.data()), width, height);



    //cursordata->data.resize(image.size());
    //std::copy(image.begin(), image.end(), cursordata->data.begin());
    //or
    //cursordata->data.assign(image.begin(), image.end());
    payloadData.cursordata.assign(image.begin(), image.end());
    payloadData.cursorsize = image.size();
    payloadData.width = width;
    payloadData.height = height;
    payloadData.xHotspot = icon_info.xHotspot;
    payloadData.yHotspot = icon_info.yHotspot;

    return true;
}
