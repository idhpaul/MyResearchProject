#include <iostream>
#include <string>

#ifndef MY_CURSOR_STRUCT_H
#define MY_CURSOR_STRUCT_H

typedef struct cursor_data {
	uint32_t width;
	uint32_t height;
	uint32_t xHotspot;
	uint32_t yHotspot;
	uint32_t cursor_size;
	const std::string cursor_image;

	cursor_data() 
	{ 
		memset(this, 0x00, sizeof(struct cursor_data)); 
	}
	cursor_data(uint32_t w, uint32_t h, uint32_t x, uint32_t y, uint32_t size, std::string image)
		: width(w), height(h), xHotspot(x), yHotspot(y), cursor_size(size), cursor_image(image) {}
}CursorData;

#endif
