#include "Encoder.h"

#include <iostream>

Encoder::Encoder()
	:width_(1920), height_(1080), Observer()
{
}

Encoder::Encoder(int width, int height)
	: width_(width), height_(height), Observer()
{
}

Encoder::~Encoder()
{
}

void Encoder::RecvNotiResolutionChanged(int width, int height)
{
	std::cout << "[Encoder] recv api" << std::endl;
	std::cout << "Recv WxH : " << width << "x" << height << std::endl;

	width_ = width;
	height_ = height;
}

std::shared_ptr<uint8_t> Encoder::doEncode(const uint8_t* rawData)
{
	std::shared_ptr<uint8_t> outputData(new uint8_t[1500], std::default_delete<uint8_t[]>());

	memcpy(outputData.get(), "This is Encoded Data (H264 Format)", 35);

	std::cout << "Encoding done" << std::endl;

	return outputData;
}
