#include "ConcreteSubject.h"

#include "Capture.h"
#include "Decoder.h"
#include "Encoder.h"

#include <future>
#include <iostream>
#include <memory>

int main()
{
	std::unique_ptr<ConcreteSubject> target = std::make_unique<ConcreteSubject>();

	std::unique_ptr<Capture> observerCapture = std::make_unique<Capture>(1280,720);
	std::cout << "Capture width : " << observerCapture->GetCaptureWidth() << std::endl;
	std::cout << "Capture height : " << observerCapture->GetCaptureHeight() << std::endl;

	std::unique_ptr<Decoder> observerDecoder = std::make_unique<Decoder>(1920, 1080);
	auto asyncDecodeHandle = std::async(std::launch::async, [&]() {
		
		uint8_t encodedData[1500] = { 0, };

		std::shared_ptr<uint8_t> decodedData = observerDecoder->doDecode(encodedData);
		std::cout << "Decode data : " << decodedData << std::endl;
		
		});

	std::unique_ptr<Encoder> observerEncoder = std::make_unique<Encoder>(1920, 1080);
	auto asyncEncodeHandle = std::async(std::launch::async, [&]() {

		uint8_t rawData[1500] = { 0, };

		std::shared_ptr<uint8_t> encodedData = observerEncoder->doEncode(rawData);
		std::cout << "Encode data : " << encodedData << std::endl;

		});


	std::cout << "Attach!" << std::endl;
	target->Attach(observerCapture.get());
	target->Attach(observerDecoder.get());
	target->Attach(observerEncoder.get());


	system("pause");
	target->NotifyResolutionChanged(3840, 2160);


	std::cout << "Dettach!" << std::endl;
	target->Dettach(observerCapture.get());
	target->Dettach(observerDecoder.get());
	target->Dettach(observerEncoder.get());


	return 0;
}
