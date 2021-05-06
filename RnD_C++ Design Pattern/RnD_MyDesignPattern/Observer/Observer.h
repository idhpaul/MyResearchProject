#pragma once
class Observer
{
public:
	virtual ~Observer() {};

	virtual void RecvNotiResolutionChanged(int width, int height) = 0;

protected:
	Observer() {};
};

