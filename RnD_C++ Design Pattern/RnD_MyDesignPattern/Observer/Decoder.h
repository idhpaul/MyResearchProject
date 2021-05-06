#pragma once

#include "Observer.h"

#include <cstdint>
#include <memory>

class Decoder : public Observer
{
public:
	Decoder();
	Decoder(int width, int height);
	virtual ~Decoder();

	void RecvNotiResolutionChanged(int width, int height);

	std::shared_ptr<uint8_t> doDecode(const uint8_t* encodedData);

private:
	int width_;
	int height_;
};

