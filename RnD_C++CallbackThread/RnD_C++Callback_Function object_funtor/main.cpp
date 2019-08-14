#include <iostream>
#include <string>

class Encryptor {
	bool m_isIncremental;
	int m_count;
public:
	Encryptor() {
		m_isIncremental = 0;
		m_count = 1;
	}
	Encryptor(bool isInc, int count) {
		m_isIncremental = isInc;
		m_count = count;
	}
	std::string operator()(std::string data) {
		for (int i = 0; i < data.size(); i++)
			if ((data[i] >= 'a' && data[i] <= 'z')
				|| (data[i] >= 'A' && data[i] <= 'Z'))
				if (m_isIncremental)
					data[i] = data[i] + m_count;
				else
					data[i] = data[i] - m_count;
		return data;
	}

};

std::string buildCompleteMessage(std::string rawData,
	Encryptor encyptorFuncObj) {
	// Add some header and footer to data to make it complete message
	rawData = "[HEADER]" + rawData + "[FooTER]";

	// Call the callBack provided i.e. function pointer to encrypt the
	rawData = encyptorFuncObj(rawData);

	return rawData;
}

int main() {

	//각 문자를 1씩 증가시켜 암호화(true -> 증가)
	std::string msg = buildCompleteMessage("SampleString", Encryptor(true, 1));
	std::cout << msg << std::endl;

	//각 문자를 2씩 증가시켜 암호화(true -> 증가)
	msg = buildCompleteMessage("SampleString", Encryptor(true, 2));
	std::cout << msg << std::endl;

	//각 문자를 1씩 감소시켜 암호화(false -> 감소)
	msg = buildCompleteMessage("SampleString", Encryptor(false, 1));
	std::cout << msg << std::endl;

	return 0;
}