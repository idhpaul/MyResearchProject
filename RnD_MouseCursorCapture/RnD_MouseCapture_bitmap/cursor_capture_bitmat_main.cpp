#include <iostream>
#include <Windows.h>

bool SaveBmp(const char* filename, byte* pImage, int width, int height)
{


	int bpp=32;// 이미지의 Bit per pixel

	BYTE *pData;// RGB 데이터를 저장할 버퍼 포인터

	BITMAPINFOHEADER *info; // 헤더 정보 저장할 포인터

	BITMAPFILEHEADER finfo;

	int imgsize; // RGB 데이터의 크기

	DWORD dw;
	char *pstr;



	//BITMAPINFOHEADER 포인터 변수에 메모리 할당
	info = (BITMAPINFOHEADER*)GlobalAlloc(GMEM_FIXED, sizeof(BITMAPFILEHEADER) + (1024 * sizeof(RGBQUAD)));
	info->biSize = sizeof(BITMAPINFOHEADER); // 크기 저장
	info->biBitCount = bpp; // BPP 저장

	info->biCompression = BI_RGB;
	info->biWidth = width;
	info->biHeight = height;


	imgsize = info->biWidth*info->biHeight*bpp/8;

	pData = (unsigned char*)GlobalAlloc(GMEM_FIXED, imgsize); // RGB 데이터가 들어갈 곳..

	 //pData 에 RGB 데이터를 복사한다.

	//이미지의 위, 아래를 뒤집어서 넣어 준다.



	pstr = (char *)&finfo.bfType;
	pstr[0] = 'B';
	pstr[1] = 'M';

	finfo.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	finfo.bfSize = finfo.bfOffBits + imgsize;
	finfo.bfReserved1 = 0;
	finfo.bfReserved2 = 0;



	FILE *fp = NULL;
	fopen_s(&fp,filename, "wb"); // 파일 오픈

	int rr = fwrite(&finfo, 1, sizeof(BITMAPFILEHEADER), fp); // 헤더 저장
	rr = fwrite(info, 1, sizeof(BITMAPINFOHEADER), fp);


	rr = fwrite(pData, 1, imgsize, fp); // RGB 데이터 저장
	fclose(fp);
	GlobalFree(info);
	GlobalFree(pData);

	return true;
}

static uint8_t *get_bitmap_data(HBITMAP hbmp, BITMAP *bmp)
{
	if (GetObject(hbmp, sizeof(*bmp), bmp) != 0) {
		uint8_t *output;
		unsigned int size =
			(bmp->bmHeight * bmp->bmWidth * bmp->bmBitsPixel) / 8;

		output = (uint8_t*)malloc(size);
		GetBitmapBits(hbmp, size, output);
		return output;
	}

	return NULL;
}

static inline uint8_t bit_to_alpha(uint8_t *data, long pixel, bool invert)
{
	uint8_t pix_byte = data[pixel / 8];
	bool alpha = (pix_byte >> (7 - pixel % 8) & 1) != 0;

	if (invert) {
		return alpha ? 0xFF : 0;
	}
	else {
		return alpha ? 0 : 0xFF;
	}
}

static inline bool bitmap_has_alpha(uint8_t *data, long num_pixels)
{
	for (long i = 0; i < num_pixels; i++) {
		if (data[i * 4 + 3] != 0) {
			return true;
		}
	}

	return false;
}

static inline void apply_mask(uint8_t *color, uint8_t *mask, long num_pixels)
{
	for (long i = 0; i < num_pixels; i++)
		color[i * 4 + 3] = bit_to_alpha(mask, i, false);
}

static inline uint8_t *copy_from_color(ICONINFO *ii, uint32_t *width, uint32_t *height)
{
	BITMAP bmp_color;
	BITMAP bmp_mask;
	uint8_t *color;
	uint8_t *mask;

	color = get_bitmap_data(ii->hbmColor, &bmp_color);
	if (!color) {
		return NULL;
	}

	if (bmp_color.bmBitsPixel < 32) {
		free(color);
		return NULL;
	}

	SaveBmp("Save_bitmap_cursor.bmp", color, bmp_color.bmWidth, bmp_color.bmHeight);

	mask = get_bitmap_data(ii->hbmMask, &bmp_mask);
	if (mask) {
		long pixels = bmp_color.bmHeight * bmp_color.bmWidth;

		if (!bitmap_has_alpha(color, pixels))
			apply_mask(color, mask, pixels);

		free(mask);
	}

	*width = bmp_color.bmWidth;
	*height = bmp_color.bmHeight;
	return color;
}

static inline uint8_t *copy_from_mask(ICONINFO *ii, uint32_t *width,uint32_t *height)
{
	uint8_t *output;
	uint8_t *mask;
	long pixels;
	long bottom;
	BITMAP bmp;

	mask = get_bitmap_data(ii->hbmMask, &bmp);
	if (!mask) {
		return NULL;
	}

	bmp.bmHeight /= 2;

	pixels = bmp.bmHeight * bmp.bmWidth;
	output = (uint8_t*)malloc(pixels * 4);

	bottom = bmp.bmWidthBytes * bmp.bmHeight;

	for (long i = 0; i < pixels; i++) {
		uint8_t alpha = bit_to_alpha(mask, i, false);
		uint8_t color = bit_to_alpha(mask + bottom, i, true);

		if (!alpha) {
			output[i * 4 + 3] = color;
		}
		else {
			*(uint32_t *)&output[i * 4] = !!color ? 0xFFFFFFFF
				: 0xFF000000;
		}
	}

	free(mask);

	*width = bmp.bmWidth;
	*height = bmp.bmHeight;
	return output;
}

static inline uint8_t *cursor_capture_icon_bitmap(ICONINFO *ii, uint32_t *width, uint32_t *height)
{
	uint8_t *output;

	output = copy_from_color(ii, width, height);
	if (!output)
		output = copy_from_mask(ii, width, height);

	return output;
}

static inline bool cursor_capture_icon(struct cursor_data *data, HICON icon)
{
	uint8_t *bitmap;
	uint32_t height;
	uint32_t width;
	ICONINFO ii;

	if (!icon) {
		return false;
	}
	if (!GetIconInfo(icon, &ii)) {
		return false;
	}

	bitmap = cursor_capture_icon_bitmap(&ii, &width, &height);
	/*if (bitmap) {
		if (data->last_cx != width || data->last_cy != height)
		{
			data->texture = get_cached_texture(data, width, height);
			data->last_cx = width;
			data->last_cy = height;
		}
		gs_texture_set_image(data->texture, bitmap, width * 4, false);
		free(bitmap);

		data->x_hotspot = ii.xHotspot;
		data->y_hotspot = ii.yHotspot;
	}*/

	free(bitmap);


	DeleteObject(ii.hbmColor);
	DeleteObject(ii.hbmMask);
	return true;//!!data->texture;
}

int main(void)
{

	//HCURSOR current_cursor;
	POINT cursor_pos;
	long x_hotspot;
	long y_hotspot;
	bool visible;

	uint32_t last_cx;
	uint32_t last_cy;

	CURSORINFO ci = { 0 };
	HICON icon;

	ci.cbSize = sizeof(ci);

	if (!GetCursorInfo(&ci)) {
		std::cout << "No Cursor" << std::endl;
		visible = false;
		return -1;
	}

	memcpy(&cursor_pos, &ci.ptScreenPos, sizeof(cursor_pos));

	/*if (current_cursor == ci.hCursor) {
		return -1;
	}*/

	icon = CopyIcon(ci.hCursor);
	visible = cursor_capture_icon(NULL,icon);
	//data->current_cursor = ci.hCursor;
	//if ((ci.flags & CURSOR_SHOWING) == 0)
	//	data->visible = false;
	DestroyIcon(icon);


	return 0;
}