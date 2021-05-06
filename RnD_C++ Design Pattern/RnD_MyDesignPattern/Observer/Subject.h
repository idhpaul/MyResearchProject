#pragma once

#include "Observer.h"

class Subject
{

public:
	
	virtual ~Subject() {};

	virtual void Attach(Observer* observer) = 0;
	virtual void Dettach(Observer* observer) = 0;

	virtual void NotifyResolutionChanged(int width, int height) = 0;

protected:
	Subject() {};

};

