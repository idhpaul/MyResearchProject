#include "Capture.h"

#include <iostream>
Capture::Capture()
	: width_(1920), height_(1080), Observer()
{
}

Capture::Capture(int width, int height)
	:width_(width), height_(height), Observer()
{
}

Capture::~Capture()
{
}

void Capture::RecvNotiResolutionChanged(int width, int height)
{
	std::cout << "[Capture] recv api" << std::endl;
	std::cout << "Recv WxH : " << width << "x" << height << std::endl;

	width_ = width;
	height_ = height;
}

int Capture::GetCaptureWidth()
{
	return width_;
}

int Capture::GetCaptureHeight()
{
	return height_;
}

int Capture::GetFrame()
{
	std::cout << "Todo implement Get DXGI Capture" << std::endl;
	return 0;
}
