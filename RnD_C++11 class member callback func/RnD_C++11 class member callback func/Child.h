#ifndef FUNCTIONCALLBACK_CHILD_H
#define FUNCTIONCALLBACK_CHILD_H

#include <functional>

using namespace std;

class Child {
public:
	Child() {}
	virtual ~Child() {}

private:
	typedef function<void()> func1;
	typedef function<void(int, int)> func2;

	func1 handler1;
	func2 handler2;

public:
	void setHandler1(func1 func) {
		handler1 = move(func);
	}

	void setHandler2(func2 func) {
		handler2 = move(func);
	}

	void exec() {
		handler1();
		handler2(100, 200);
	}
};

#endif //FUNCTIONCALLBACK_CHILD_H