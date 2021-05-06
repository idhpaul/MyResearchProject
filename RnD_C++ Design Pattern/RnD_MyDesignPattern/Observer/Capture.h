#pragma once
#include "Observer.h"

class Capture : public Observer
{
public:
	Capture();
	Capture(int width, int height);
	virtual ~Capture();

	void RecvNotiResolutionChanged(int width, int height);

	int GetCaptureWidth();
	int GetCaptureHeight();

	int GetFrame();

private:
	int width_;
	int height_;
};

