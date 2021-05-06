#include "Decoder.h"

#include <iostream>

Decoder::Decoder()
    :width_(1920), height_(1080), Observer()
{
}

Decoder::Decoder(int width, int height)
    : width_(1920), height_(1080), Observer()
{
}

Decoder::~Decoder()
{
}

void Decoder::RecvNotiResolutionChanged(int width, int height)
{
    std::cout << "[Decoder] recv api" << std::endl;
    std::cout << "Recv WxH : " << width << "x" << height << std::endl;

    width_ = width;
    height_ = height;
}

std::shared_ptr<uint8_t> Decoder::doDecode(const uint8_t* encodedData)
{
    std::shared_ptr<uint8_t> outputData(new uint8_t[1500], std::default_delete<uint8_t[]>());

    memcpy(outputData.get(), "This is Decoded Data (NV12 Format)", 35);

    std::cout << "Decoding done" << std::endl;
    
    return outputData;
}
