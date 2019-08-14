#include <iostream>
#include <string>


//This encrypt function increment all letters in string by 1.
std::string encryptDataByLetterInc(std::string data)
{
	for (int i = 0; i < data.size(); i++)
	{
		if ((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z'))
			data[i]++;
	}
	return data;
}

//This encrypt function decrement  all letters in string by 1.
std::string encryptDataByLetterDec(std::string data)
{
	for (int i = 0; i < data.size(); i++)
	{
		if ((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z'))
			data[i]--;
	}
	return data;
}
std::string buildCompleteMessage(std::string rawData, std::string(*encrypterFunPtr)(std::string))
{
	// Add some header and footer to data to make it complete message
	rawData = "[HEADER]" + rawData + "[FooTER]";
	// Call the callBack provided i.e. function pointer to encrypt the
	rawData = encrypterFunPtr(rawData);
	return rawData;
}
int main(void)
{

	std::string msg1 = buildCompleteMessage("SampleString", &encryptDataByLetterInc);
	std::cout << msg1 << std::endl;


	std::string msg2 = buildCompleteMessage("SampleString", &encryptDataByLetterDec);
	std::cout << msg2 << std::endl;


	return 0;
}