#ifndef CURSR_STRUCT_H
#define CURSR_STRUCT_H

#include <iostream>
#include <vector>

class CursorPayload
{
public:
    CursorPayload()
        : width(),height(), xHotspot(), yHotspot(), cursorsize(), cursordata()
    {}

    uint32_t width;
    uint32_t height;
    uint32_t xHotspot;
    uint32_t yHotspot;
    int32_t cursorsize;
    std::vector<uint8_t> cursordata;
};

#endif // !CURSR_STRUCT_H

