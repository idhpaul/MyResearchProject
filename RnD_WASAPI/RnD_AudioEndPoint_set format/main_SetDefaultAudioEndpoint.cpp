/*
	Q. Why passive reason to replace for default audio device setting format(like. 16bit/44100, 24bit/48000)

	A. Unfortunately, there is no method in Core Auido API which returns a list of supported audio format list.
      So I passived changing audio device format setting.
*/


#include <initguid.h> // if this file is included
#include <Windows.h>

#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#include <Functiondiscoverykeys_devpkey.h>


#include <iostream>


WAVEFORMATEXTENSIBLE *deviceFormatProperties;

void Get_audio_format(int bitrate, int hz)
{

	if (bitrate == 16 && hz == 44100)
	{
		//This is for 16bit/44100Hz
		deviceFormatProperties->Format.nChannels = 2;
		deviceFormatProperties->Format.nSamplesPerSec = 44100;
		deviceFormatProperties->Format.wBitsPerSample = 16;

		deviceFormatProperties->Format.nBlockAlign = (deviceFormatProperties->Format.nChannels*deviceFormatProperties->Format.wBitsPerSample) / 8;
		deviceFormatProperties->Format.nAvgBytesPerSec = deviceFormatProperties->Format.nSamplesPerSec*deviceFormatProperties->Format.nBlockAlign;

		deviceFormatProperties->Samples.wReserved = 16;
		deviceFormatProperties->Samples.wSamplesPerBlock = 16;
		deviceFormatProperties->Samples.wValidBitsPerSample = 16;

		deviceFormatProperties->dwChannelMask = 0x00000003;
		deviceFormatProperties->Format.cbSize = 22;
	}

	else if (bitrate == 16 && hz == 48000)
	{
		//This is for 16bit/48000Hz
		deviceFormatProperties->Format.nChannels = 2;
		deviceFormatProperties->Format.nSamplesPerSec = 48000;
		deviceFormatProperties->Format.wBitsPerSample = 16;

		deviceFormatProperties->Format.nBlockAlign = (deviceFormatProperties->Format.nChannels*deviceFormatProperties->Format.wBitsPerSample) / 8;
		deviceFormatProperties->Format.nAvgBytesPerSec = deviceFormatProperties->Format.nSamplesPerSec*deviceFormatProperties->Format.nBlockAlign;

		deviceFormatProperties->Samples.wReserved = 16;
		deviceFormatProperties->Samples.wSamplesPerBlock = 16;
		deviceFormatProperties->Samples.wValidBitsPerSample = 16;

		deviceFormatProperties->dwChannelMask = 0x00000003;
		deviceFormatProperties->Format.cbSize = 22;
	}

	else if (bitrate == 24 && hz == 44100)
	{
		//This is for 24bit/44100Hz
		deviceFormatProperties->Format.nChannels = 2;
		deviceFormatProperties->Format.nSamplesPerSec = 44100;
		deviceFormatProperties->Format.wBitsPerSample = 32;

		deviceFormatProperties->Format.nBlockAlign = (deviceFormatProperties->Format.nChannels*deviceFormatProperties->Format.wBitsPerSample) / 8;
		deviceFormatProperties->Format.nAvgBytesPerSec = deviceFormatProperties->Format.nSamplesPerSec*deviceFormatProperties->Format.nBlockAlign;

		deviceFormatProperties->Samples.wReserved = 24;
		deviceFormatProperties->Samples.wSamplesPerBlock = 24;
		deviceFormatProperties->Samples.wValidBitsPerSample = 24;

		deviceFormatProperties->dwChannelMask = 0x00000003;
		deviceFormatProperties->Format.cbSize = 22;
	}

	else if (bitrate == 24 && hz == 48000)
	{
		//This is for 24bit/48000Hz
		deviceFormatProperties->Format.nChannels = 2;
		deviceFormatProperties->Format.nSamplesPerSec = 48000;
		deviceFormatProperties->Format.wBitsPerSample = 32;

		deviceFormatProperties->Format.nBlockAlign = (deviceFormatProperties->Format.nChannels*deviceFormatProperties->Format.wBitsPerSample) / 8;
		deviceFormatProperties->Format.nAvgBytesPerSec = deviceFormatProperties->Format.nSamplesPerSec*deviceFormatProperties->Format.nBlockAlign;

		deviceFormatProperties->Samples.wReserved = 24;
		deviceFormatProperties->Samples.wSamplesPerBlock = 24;
		deviceFormatProperties->Samples.wValidBitsPerSample = 24;

		deviceFormatProperties->dwChannelMask = 0x00000003;
		deviceFormatProperties->Format.cbSize = 22;
	}

	else
	{
		std::cout << "invalid parameter for audio format";
		system("pause");

	}

}

int main()
{

	HRESULT hr_READ, hr_WRITE;
	IMMDevice * pDevice_READ = NULL, *pDevice_WRITE = NULL;
	IMMDeviceEnumerator * pEnumerator_READ = NULL, *pEnumerator_WRITE = NULL;
	IPropertyStore* store_READ = nullptr, *store_WRITE = nullptr;
	IAudioClient   *audioClient = NULL;
	PROPVARIANT prop_READ, prop_WRITE;
	CoInitialize(NULL);
	//get the device enumerator
	hr_READ = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID *)&pEnumerator_READ);

	// get default audio endpoint
	hr_READ = pEnumerator_READ->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice_READ);

	hr_READ = pDevice_READ->OpenPropertyStore(STGM_READ, &store_READ);
	if (FAILED(hr_READ))
	{
		std::cout << "OpenPropertyStore failed!" << std::endl;
	}

	hr_READ = store_READ->GetValue(PKEY_AudioEngine_DeviceFormat, &prop_READ);
	if (FAILED(hr_READ))
	{
		std::cout << "GetValue failed!" << std::endl;
	}

	deviceFormatProperties = (WAVEFORMATEXTENSIBLE *)prop_READ.blob.pBlobData;
	std::cout << "Channels    = " << deviceFormatProperties->Format.nChannels << std::endl;
	std::cout << "Sample rate = " << deviceFormatProperties->Format.nSamplesPerSec << std::endl;
	std::cout << "Bit depth   = " << deviceFormatProperties->Format.wBitsPerSample << std::endl;
	std::cout << "Format Tag   = " << deviceFormatProperties->Format.wFormatTag << std::endl;
	std::cout << "nBlockAlign   = " << deviceFormatProperties->Format.nBlockAlign << std::endl;
	std::cout << "Size   = " << deviceFormatProperties->Format.cbSize << std::endl;
	std::cout << "AvgBytesPerSec   = " << deviceFormatProperties->Format.nAvgBytesPerSec << std::endl;
	std::cout << "Samples.wReserved    = " << deviceFormatProperties->Samples.wReserved << std::endl;
	std::cout << "Samples.wSamplesPerBlock = " << deviceFormatProperties->Samples.wSamplesPerBlock << std::endl;
	std::cout << "Samples.wValidBitsPerSample   = " << deviceFormatProperties->Samples.wValidBitsPerSample << std::endl;
	std::cout << "dwChannelMask   = " << deviceFormatProperties->dwChannelMask << std::endl;

	
	//Set Audio Device format
	Get_audio_format(24, 48000);

	hr_READ = pDevice_READ->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient);
	if (FAILED(hr_READ))
	{
		std::cout << "pDevice->Activate failed!" << std::endl;
	}


	hr_READ = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, (PWAVEFORMATEX)&deviceFormatProperties->Format, NULL);
	if (FAILED(hr_READ))
	{
		std::cout << "IsFormatSupported failed" << std::endl;
	}
	hr_WRITE = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID *)&pEnumerator_WRITE);

	// get default audio endpoint
	hr_WRITE = pEnumerator_WRITE->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice_WRITE);

	hr_WRITE = pDevice_WRITE->OpenPropertyStore(STGM_WRITE, &store_WRITE);
	if (FAILED(hr_WRITE))
	{
		std::cout << "OpenPropertyStore failed!" << std::endl;
	}
	hr_WRITE = store_WRITE->SetValue(PKEY_AudioEngine_DeviceFormat, prop_READ);
	if (FAILED(hr_WRITE))
	{
		std::cout << "SetValue failed!" << std::endl;
	}
	std::cout << "\nCHANGED VALUES\n";
	std::cout << "Channels    = " << deviceFormatProperties->Format.nChannels << std::endl;
	std::cout << "Sample rate = " << deviceFormatProperties->Format.nSamplesPerSec << std::endl;
	std::cout << "Bit depth   = " << deviceFormatProperties->Format.wBitsPerSample << std::endl;
	std::cout << "Format Tag   = " << deviceFormatProperties->Format.wFormatTag << std::endl;
	std::cout << "nBlockAlign   = " << deviceFormatProperties->Format.nBlockAlign << std::endl;
	std::cout << "Size   = " << deviceFormatProperties->Format.cbSize << std::endl;
	std::cout << "AvgBytesPerSec   = " << deviceFormatProperties->Format.nAvgBytesPerSec << std::endl;
	std::cout << "Samples.wReserved    = " << deviceFormatProperties->Samples.wReserved << std::endl;
	std::cout << "Samples.wSamplesPerBlock = " << deviceFormatProperties->Samples.wSamplesPerBlock << std::endl;
	std::cout << "Samples.wValidBitsPerSample   = " << deviceFormatProperties->Samples.wValidBitsPerSample << std::endl;
	std::cout << "dwChannelMask   = " << deviceFormatProperties->dwChannelMask << std::endl;
	hr_WRITE = store_WRITE->Commit();
	pDevice_WRITE->Release();
	pEnumerator_WRITE->Release();
	store_WRITE->Release();
	PropVariantClear(&prop_WRITE);
	pDevice_READ->Release();
	pEnumerator_READ->Release();
	store_READ->Release();
	pDevice_READ = nullptr;
	PropVariantClear(&prop_READ);
	CoUninitialize();
	system("pause");
	return 0;
}