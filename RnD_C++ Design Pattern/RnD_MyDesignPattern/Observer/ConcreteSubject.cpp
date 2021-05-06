#include "ConcreteSubject.h"

ConcreteSubject::ConcreteSubject()
	:Subject()
{
}

ConcreteSubject::~ConcreteSubject()
{
}

void ConcreteSubject::Attach(Observer* observer)
{
	observers_.push_back(observer);
}

void ConcreteSubject::Dettach(Observer* observer)
{
	std::remove(observers_.begin(), observers_.end(), observer);
}

void ConcreteSubject::NotifyResolutionChanged(int width, int height)
{
	for (Observer* observer : observers_)
		observer->RecvNotiResolutionChanged(width, height);
}
