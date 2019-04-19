#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib,"ws2_32")

#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#define __SOCKET 1

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC 10000

#define EXIT_ON_ERROR(hres) \
    if (FAILED(hres)) { goto Exit; }

#define SAFE_RELEASE(punk) \
    if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

//!FLAC Encode (Extern)
extern int flac_encode(char *wasapi_pData, unsigned int wasapi_size);

FILE* fp;

WSADATA wsadata;
int opt = 0;

SOCKET sock;
SOCKET clientsock;

struct sockaddr_in sockinfo;
struct sockaddr_in clientsockinfo;

int clientsize;

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

class MyAudioSink {
public:
	int SetFormat(WAVEFORMATEX *pwfx);
	int CopyData(char *pData, UINT32 numFramesAvailable, BOOL *pbDone);
};

int MyAudioSink::SetFormat(WAVEFORMATEX *pwfx)
{
	FILE *fp = fopen("capture_format.txt", "w");
	char str[128];
	sprintf(str, "wFormatTag \t\tis %x\nnChannels \t\tis %d\nnSamplesPerSec \tis %ld\nnAvgBytesPerSec is %ld\nwBitsPerSample \tis %d",
		pwfx->wFormatTag, pwfx->nChannels, pwfx->nSamplesPerSec, pwfx->nAvgBytesPerSec, pwfx->wBitsPerSample);
	fwrite(str, 1, strlen(str), fp);
	fclose(fp);
	return 0;
}

int MyAudioSink::CopyData(char *pData, UINT32 numFramesAvailable, BOOL *pbDone)
{

	printf("size : %d\n", numFramesAvailable);

	/////////////////////////////////////////////////////////
	//!FLAC Encoding

	/*if (flac_encode(pData, numFramesAvailable) < 0) {
		printf("failed to encode flac");
		return -1;
	}*/

	/////////////////////////////////////////////////////////

	send(clientsock, (char *)pData, numFramesAvailable, 0);
	//printf("%s\n", pData);

	if (pData != NULL)
		fwrite(pData, numFramesAvailable, 1, fp);
	return 0;
}

// pwfx->nSamplesPerSec = 44100;
BOOL AdjustFormatTo16Bits(WAVEFORMATEX *pwfx)
{
	BOOL bRet(FALSE);
	if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
		pwfx->wFormatTag = WAVE_FORMAT_PCM;
	else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
		if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
		{
			pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			pEx->Samples.wValidBitsPerSample = 16;
		}
	}
	else
		return bRet;
	pwfx->wBitsPerSample = 16;
	pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
	pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
	bRet = TRUE;
	return bRet;
}

HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
	HRESULT         hr;
	//    REFERENCE_TIME  hnsActualDuration;
	UINT32          bufferFrameCount;
	UINT32          numFramesAvailable;
	BYTE *          pData;
	DWORD           flags;
	REFERENCE_TIME  hnsDefaultDevicePeriod(0);

	IMMDeviceEnumerator *   pEnumerator = NULL;
	IMMDevice *             pDevice = NULL;
	IAudioClient *          pAudioClient = NULL;
	IAudioCaptureClient *   pCaptureClient = NULL;
	WAVEFORMATEX *          pwfx = NULL;
	UINT32                  packetLength = 0;
	BOOL                    bDone = FALSE;
	HANDLE                  hTimerWakeUp = NULL;

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
	//EXIT_ON_ERROR(hr)

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	//EXIT_ON_ERROR(hr)

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
	//EXIT_ON_ERROR(hr)

		//hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);

	hr = pAudioClient->GetMixFormat(&pwfx);
	//EXIT_ON_ERROR(hr)

	AdjustFormatTo16Bits(pwfx);

	//hTimerWakeUp = CreateWaitableTimer(NULL, FALSE, NULL);

	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, NULL);
	//EXIT_ON_ERROR(hr)

		// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	//EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
	//EXIT_ON_ERROR(hr)

	//	LARGE_INTEGER liFirstFire;
	//liFirstFire.QuadPart = -hnsDefaultDevicePeriod / 2; // negative means relative time
	//LONG lTimeBetweenFires = (LONG)hnsDefaultDevicePeriod / 2 / (10 * 1000);

	//SetWaitableTimer(hTimerWakeUp, &liFirstFire, lTimeBetweenFires, NULL, NULL, FALSE);

	// Notify the audio sink which format to use.
	hr = pMySink->SetFormat(pwfx);
	//EXIT_ON_ERROR(hr)

		// Calculate the actual duration of the allocated buffer.
	//    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start recording.
//EXIT_ON_ERROR(hr)

//HANDLE waitArray[1] = { hTimerWakeUp };

// Each loop fills about half of the shared buffer.
	while (bDone == FALSE)
	{
		//WaitForMultipleObjects(sizeof(waitArray) / sizeof(waitArray[0]), waitArray, FALSE, INFINITE);

		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		EXIT_ON_ERROR(hr)

			while (packetLength != 0)
			{
				// Get the available data in the shared buffer.
				hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
				EXIT_ON_ERROR(hr)

					// Copy the available capture data to the audio sink.
					hr = pMySink->CopyData((char *)pData, numFramesAvailable * pwfx->nBlockAlign, &bDone);
				EXIT_ON_ERROR(hr)

					hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
				EXIT_ON_ERROR(hr)

					hr = pCaptureClient->GetNextPacketSize(&packetLength);
				EXIT_ON_ERROR(hr)
			}
	}

	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr)

		Exit:
	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pCaptureClient)

		return hr;
}

int main()
{
#if __SOCKET

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("windows socket initialize failed\n");
		return -1;
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("failed to create socket");
		return -1;
	}


	memset(&sockinfo, 0, sizeof(sockinfo));

	sockinfo.sin_family = AF_INET;
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockinfo.sin_port = htons(9999);

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	if (bind(sock, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR) {
		printf("PARMS bind() error");
		return -1;
	}

	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		printf("PARMS listen() error");
		return -1;
	}


	printf("Waiting for Client\n");

	clientsize = sizeof(clientsockinfo);

	clientsock = accept(sock, (SOCKADDR*)&clientsockinfo, &clientsize);
	if (clientsock == INVALID_SOCKET) {
		printf("PARMS accept() error");
		return -1;
	}

	printf("Á¢¼Ó\n");
#endif //__SOCKET

	///////////////////////////////////////////////////////////////////////////////////
	fp = fopen("record.pcm", "wb");

	CoInitialize(NULL);
	MyAudioSink test;

	RecordAudioStream(&test);

	fclose(fp);
	///////////////////////////////////////////////////////////////////////////////////
#if __SOCKET
	closesocket(sock);
	closesocket(clientsock);

	WSACleanup();
#endif //__SOCKET

	return 0;
	return 0;
}