#pragma once

#include "Observer.h"

#include <cstdint>
#include <memory>

class Encoder : public Observer
{
public:
	Encoder();
	Encoder(int width, int height);
	virtual ~Encoder();

	void RecvNotiResolutionChanged(int width, int height);

	std::shared_ptr<uint8_t> doEncode(const uint8_t* rawData);

private:
	int width_;
	int height_;
};

