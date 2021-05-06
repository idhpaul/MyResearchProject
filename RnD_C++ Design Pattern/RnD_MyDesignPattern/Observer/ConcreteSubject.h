#pragma once
#include "Subject.h"

#include <list>

class ConcreteSubject : public Subject
{
public:
	ConcreteSubject();
	virtual ~ConcreteSubject();

	void Attach(Observer* observer);
	void Dettach(Observer* observer);

	void NotifyResolutionChanged(int width, int height);

private:
	std::list<Observer*> observers_;
};

