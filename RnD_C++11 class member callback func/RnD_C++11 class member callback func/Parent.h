#ifndef FUNCTIONCALLBACK_PARENT_H
#define FUNCTIONCALLBACK_PARENT_H

#include "Child.h"

#include <iostream>
#include <functional>

using namespace std;

class Parent {
public:
	Parent() {}
	virtual ~Parent() {}

	void hello() {
		cout << "Hello" << endl;
	}

	void sum(int a, int b) {
		cout << a << " + " << b << " = " + (a + b) << endl;
	}

	void test() {
		Child child;

		child.setHandler1(bind(&Parent::hello, this));
		child.setHandler2(bind(&Parent::sum, this, placeholders::_1, placeholders::_2));
		child.exec();
	}
};

#endif //FUNCTIONCALLBACK_PARENT_H