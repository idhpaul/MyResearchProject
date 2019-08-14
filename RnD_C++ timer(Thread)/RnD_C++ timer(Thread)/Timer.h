#pragma once
#include <iostream>
#include <thread>


class Timer {
	bool clear = false;

public:
	template<typename Function>
	void setTimeout(Function function, int delay);

	template<typename Function>
	void setInterval(Function function, int interval);

	void stop();
};