/*
	`std::bind()` is a Standard Fuction Objects that acts as a Functional Adaptor
*/

#include <iostream>
#include <functional> //bind

#include <memory>
#include <algorithm>

using namespace std::placeholders;

int add(int first, int second)
{
	return first + second;
}

int main(void)
{
	auto a = true;
	auto b = 'A';
	auto c = L'A';
	auto d = "문자열";
	auto e = 3;
	auto f = 3.0;

	std::cout << "값\t크기\t자료형" << std::endl;
	std::cout << a << '\t' << sizeof(a) << '\t' << typeid(a).name() << std::endl;
	std::cout << b << '\t' << sizeof(b) << '\t' << typeid(b).name() << std::endl;
	std::cout << c << '\t' << sizeof(c) << '\t' << typeid(c).name() << std::endl;
	std::cout << d << '\t' << sizeof(d) << '\t' << typeid(d).name() << std::endl;
	std::cout << e << '\t' << sizeof(e) << '\t' << typeid(e).name() << std::endl;
	std::cout << f << '\t' << sizeof(f) << '\t' << typeid(f).name() << std::endl;

	////////////////////////////////////////////////////////////////////

	//typeid(add_func).name = class std::_Binder<struct std::_Unforced,int (__cdecl*)(int,int),struct std::_Ph<1> const &,struct std::_Ph<2> const &>
	auto add_func = std::bind(&add, _1, _2);

	std::cout << a << '\t' << sizeof(add_func) << '\t' << typeid(add_func).name() << std::endl;


	return 0;
}