#include "md5.h"
#include <iostream>

int main()
{
    std::string MD5_PlainStr = "New1234!";
    std::string MD5_output(md5(MD5_PlainStr));

    std::cout << "md5 of 'grape': " << MD5_output << std::endl;

	return 0;
}