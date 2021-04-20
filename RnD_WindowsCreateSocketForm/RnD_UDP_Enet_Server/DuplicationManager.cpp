// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
#include "DuplicationManager.h"

#include <wrl.h>
#include <time.h>

#include <fstream> 
#include <iostream>
#include <system_error>

#define DRAW_MOUSE_DX_SURFACE 0

using namespace DirectX;

// Below are lists of errors expect from Dxgi API calls when a transition event like mode change, PnpStop, PnpStart
// desktop switch, TDR or session disconnect/reconnect. In all these cases we want the application to clean up the threads that process
// the desktop updates and attempt to recreate them.
// If we get an error that is not on the appropriate list then we exit the application

// These are the errors we expect from general Dxgi API due to a transition
HRESULT SystemTransitionsExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	DXGI_ERROR_ACCESS_LOST,
	static_cast<HRESULT>(WAIT_ABANDONED),
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIOutput1::DuplicateOutput due to a transition
HRESULT CreateDuplicationExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	static_cast<HRESULT>(E_ACCESSDENIED),
	DXGI_ERROR_UNSUPPORTED,
	DXGI_ERROR_SESSION_DISCONNECTED,
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIOutputDuplication methods due to a transition
HRESULT FrameInfoExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	DXGI_ERROR_ACCESS_LOST,
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIAdapter::EnumOutputs methods due to outputs becoming stale during a transition
HRESULT EnumOutputsExpectedErrors[] = {
	DXGI_ERROR_NOT_FOUND,
	S_OK                                    // Terminate list with zero valued HRESULT
};

//
// Constructor sets up references / variables
//
DUPLICATIONMANAGER::DUPLICATIONMANAGER() : m_DeskDupl(nullptr),
m_AcquiredDesktopImage(nullptr),
m_GdiTexture(nullptr),
m_CPUAccessibleLuminanceSurf(nullptr),
m_CPUAccessibleChrominanceSurf(nullptr),
m_ImagePitch(0),
m_ScaleSrcSurf(nullptr),
m_TempSharedSurf(nullptr),
m_BeforeSharedSurf(nullptr),
m_LuminanceSurf(nullptr),
m_ChrominanceSurf(nullptr),
m_LuminanceRTV(nullptr),
m_ChrominanceRTV(nullptr),
m_VertexShader(nullptr),
m_PixelShaderLuminance(nullptr),
m_PixelShaderChrominance(nullptr),
m_PixelShaderCbCr(nullptr),
m_VertexBuffer(nullptr),
m_SamplerLinear(nullptr),
m_NeedsResize(true),
m_Device(nullptr),
m_DeviceContext(nullptr),
m_CbSurf(nullptr),
m_CrSurf(nullptr),
/* GuardSurface(nullptr),*/
CaptureFrameAvailable(false)
{
	RtlZeroMemory(&m_OutputDesc, sizeof(m_OutputDesc));
}

//
// Destructor simply calls CleanRefs to destroy everything
//
DUPLICATIONMANAGER::~DUPLICATIONMANAGER()
{
	ReleaseSurfaces();

	if (m_GdiTexture)
	{
		m_GdiTexture->Release();
		m_GdiTexture = nullptr;
	}
	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = nullptr;
	}
	if (m_PixelShaderLuminance)
	{
		m_PixelShaderLuminance->Release();
		m_PixelShaderLuminance = nullptr;
	}
	if (m_PixelShaderChrominance)
	{
		m_PixelShaderChrominance->Release();
		m_PixelShaderChrominance = nullptr;
	}
	if (m_PixelShaderCbCr)
	{
		m_PixelShaderCbCr->Release();
		m_PixelShaderCbCr = nullptr;
	}
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}
	if (m_SamplerLinear)
	{
		m_SamplerLinear->Release();
		m_SamplerLinear = nullptr;
	}
	if (m_DeskDupl)
	{
		m_DeskDupl->Release();
		m_DeskDupl = nullptr;
	}
	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}
	if (m_LuminanceRTV)
	{
		m_LuminanceRTV->Release();
		m_LuminanceRTV = nullptr;
	}
	if (m_ChrominanceRTV)
	{
		m_ChrominanceRTV->Release();
		m_ChrominanceRTV = nullptr;
	}
	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}
	if (m_DeviceContext)
	{
		m_DeviceContext->Release();
		m_DeviceContext = nullptr;
	}

}

//
// Indicates that screen resolution has been changed.
//
void DUPLICATIONMANAGER::SurfaceResize()
{
	m_NeedsResize = true;
}

//
// Initialize duplication interfaces
//
DUPL_RETURN DUPLICATIONMANAGER::InitDupl()
{

	DUPL_RETURN Ret = InitDevice();
	if (Ret != DUPL_RETURN_SUCCESS)
	{
		std::cout << "Failed to Init DirectX Device" << std::endl;

		return Ret;
	}

	Ret = InitDxgi();
	if (Ret != DUPL_RETURN_SUCCESS)
	{
		std::cout << "Failed to Init Dxgi" << std::endl;

		return Ret;
	}
	std::cout << "Success to Init Dxgi" << std::endl;

	HRESULT hr;
	D3D11_SAMPLER_DESC desc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	// Create the sample state
	hr = m_Device->CreateSamplerState(&desc, &m_SamplerLinear);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create sampler state in OUTPUTMANAGER", hr, SystemTransitionsExpectedErrors);
	}

	m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerLinear);

#if DRAW_MOUSE_DX_SURFACE
	CrateTexture();
#endif

	return InitShaders();
}

/// @brief Init DirectX Device
/// @return DUPL_RETURN
DUPL_RETURN DUPLICATIONMANAGER::InitDevice()
{
	HRESULT hr = S_OK;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

	// Feature levels supported
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

	D3D_FEATURE_LEVEL FeatureLevel;

	// Create device
	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
	{
		hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, D3D11_CREATE_DEVICE_SINGLETHREADED, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &m_Device, &FeatureLevel, &m_DeviceContext);
		if (SUCCEEDED(hr))
		{
			// Device creation success, no need to loop anymore
			break;
		}
	}
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to create device in InitializeDx", hr);
	}

	printf("The feature level - %04x\n", FeatureLevel);

	return DUPL_RETURN_SUCCESS;
}

/// @brief Init DXGI device and adapter for get DesktopDuication
/// @return DUPL_RETURN
DUPL_RETURN DUPLICATIONMANAGER::InitDxgi()
{
	// Get DXGI device
	IDXGIDevice* DxgiDevice = nullptr;
	HRESULT hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to QI for DXGI Device", hr);
	}

	// Get DXGI adapter
	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to get parent DXGI Adapter", hr, SystemTransitionsExpectedErrors);
	}

	// 어뎁터 정보 구조체 
	DXGI_ADAPTER_DESC desc;
	DxgiAdapter->GetDesc(&desc);

	// 어뎁터의 이름을 출력한다. (GPU 정보) 
	std::cout << "Adapter Name : " << desc.Description << std::endl;

	DxgiDevice->Release();
	DxgiDevice = nullptr;

	// Get output
	IDXGIOutput* DxgiOutput = nullptr;

	// Only Main monitor
	hr = DxgiAdapter->EnumOutputs(0, &DxgiOutput);
	if (hr == DXGI_ERROR_NOT_FOUND)
	{
		return ProcessFailure(m_Device, L"Failed to find adapter output", hr);
	}
	else if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
	{
		return ProcessFailure(m_Device, L"Failed to use current available output / it will be running in session 0", hr);
	}
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;

	// Describes an output
	DxgiOutput->GetDesc(&m_OutputDesc);
	std::cout << "Output Device name : " << m_OutputDesc.DeviceName << std::endl;;
	std::cout << "Output WidthxHeight : " << GetImageWidth() << "x" << GetImageHeight() << std::endl;

	// QI for Output 1
	IDXGIOutput1* DxgiOutput1 = nullptr;
	hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER", hr);
	}
	DxgiOutput->Release();
	DxgiOutput = nullptr;

	// Create desktop duplication
	hr = DxgiOutput1->DuplicateOutput(m_Device, &m_DeskDupl);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to get duplicate output", hr, CreateDuplicationExpectedErrors);
	}
	DxgiOutput1->Release();
	DxgiOutput1 = nullptr;


	return DUPL_RETURN_SUCCESS;
}

void DUPLICATIONMANAGER::CrateTexture()
{
	HRESULT hr;
	DXGI_OUTDUPL_DESC lOutputDuplDesc;
	m_DeskDupl->GetDesc(&lOutputDuplDesc);

	// Create shared texture for all duplication threads to draw into
	D3D11_TEXTURE2D_DESC DeskTexD;
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = lOutputDuplDesc.ModeDesc.Width;
	DeskTexD.Height = lOutputDuplDesc.ModeDesc.Height;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = lOutputDuplDesc.ModeDesc.Format;
	DeskTexD.SampleDesc.Count = 1;

	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET; // 0 // D3D11_BIND_RENDER_TARGET
	DeskTexD.Usage = D3D11_USAGE_DEFAULT;	//D3D11_USAGE_DEFAULT //D3D11_USAGE_STAGING
	DeskTexD.CPUAccessFlags = 0;
	DeskTexD.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_GdiTexture);
	if (FAILED(hr)) {
		ProcessFailure(m_Device, L"Failed to create GdiTexture texture", hr);
	}
}

//
// Get next frame and write it into Data
//
_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)
DUPL_RETURN DUPLICATIONMANAGER::GetFrame()
{
	IDXGIResource* DesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
	RtlZeroMemory(&FrameInfo, sizeof(FrameInfo));

	HRESULT hr;

	// Release duplication frame
	hr = m_DeskDupl->ReleaseFrame();

	// Get new frame
	hr = m_DeskDupl->AcquireNextFrame(0, &FrameInfo, &DesktopResource);
	/*if (FrameInfo.AccumulatedFrames == 0 || FrameInfo.LastPresentTime.QuadPart == 0 )
	{
		return DUPL_RETURN_GET_FRAME_TIME_OUT;
	}*/
	if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		//std::cout << "TimeOut" << std::endl;
		return DUPL_RETURN_GET_FRAME_TIME_OUT;
	}
	else if (hr == DXGI_ERROR_ACCESS_LOST)
	{
		//PLOGI.printf("Access Lost hr : %x\n", hr);
		std::cout << "ACCESS LOST" << std::endl;

		InitDxgi();
		hr = m_DeskDupl->AcquireNextFrame(5, &FrameInfo, &DesktopResource);
	}
	else if (hr == DXGI_ERROR_ACCESS_DENIED)
	{
		//PLOGI.printf("Access Denied hr : %x\n", hr);
		std::cout << "ACCESS Denied" << std::endl;

		InitDxgi();
		hr = m_DeskDupl->AcquireNextFrame(5, &FrameInfo, &DesktopResource);
	}
	else if (FAILED(hr))
	{
		InitDxgi();
		//PLOGI.printf("Access Denied hr : %x\n", hr);
		return ProcessFailure(m_Device, L"Failed to acquire next frame in DUPLICATIONMANAGER", hr, FrameInfoExpectedErrors);
	}
	/*std::cout << "Frame AccumulatedFrames : " << FrameInfo.AccumulatedFrames << std::endl;

	std::cout << "Frame Pointer is Visualble : " << FrameInfo.PointerPosition.Visible << std::endl;
	std::cout << "Frame Pointer Position x : " << FrameInfo.PointerPosition.Position.x << std::endl;
	std::cout << "Frame Pointer Position y : " << FrameInfo.PointerPosition.Position.y << std::endl;

	std::cout << "Frame LastPresentTime QuardPart : " << FrameInfo.LastPresentTime.QuadPart << std::endl;
	std::cout << "Frame LastPresentTime HighPart : " << FrameInfo.LastPresentTime.HighPart << std::endl;
	std::cout << "Frame LastPresentTime LowPart : " << FrameInfo.LastPresentTime.LowPart << std::endl;

	std::cout << "Frame LastMouseUpdateTime QuadPart : " << FrameInfo.LastMouseUpdateTime.QuadPart << std::endl;
	std::cout << "Frame LastMouseUpdateTime HighPart : " << FrameInfo.LastMouseUpdateTime.HighPart << std::endl;
	std::cout << "Frame LastMouseUpdateTime LowPart : " << FrameInfo.LastMouseUpdateTime.LowPart << std::endl;*/
	//if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	//{
	//	return DUPL_RETURN_GET_FRAME_TIME_OUT;
	//}
	//else if (m_DeskDupl == nullptr
	//	|| hr == DXGI_ERROR_ACCESS_LOST
	//	|| hr == DXGI_ERROR_INVALID_CALL
	//	|| hr == DXGI_ERROR_ACCESS_DENIED) {
	//	if (m_DeskDupl == nullptr) {
	//		std::cout << "Error getting next frame due to Desktop Duplication instance is NULL, reinitializing" << std::endl;;
	//	}
	//	else if (hr == DXGI_ERROR_ACCESS_LOST) {
	//		std::cout << "Error getting next frame due to error DXGI_ERROR_ACCESS_LOST, reinitializing" << std::endl;;
	//	}
	//	else if (hr == DXGI_ERROR_INVALID_CALL) {
	//		std::cout << "Error getting next frame due to error DXGI_ERROR_INVALID_CALL, reinitializing" << std::endl;;
	//	}
	//	else if (hr == DXGI_ERROR_ACCESS_DENIED) {
	//		std::cout << "Error getting next frame due to error DXGI_ERROR_ACCESS_DENIED, reinitializing" << std::endl;;
	//	}
	//	if (m_DeskDupl) {
	//		m_DeskDupl->ReleaseFrame();
	//		m_DeskDupl->Release();
	//	}
	//	hr = InitDxgi();
	//	//hr = m_DeskDupl->AcquireNextFrame(5, &FrameInfo, &DesktopResource);
	//}
	//else if (FAILED(hr) && hr != DXGI_ERROR_WAIT_TIMEOUT) {
	//	printf("Failed hr : %x\n", hr);
	//	return ProcessFailure(m_Device, L"Failed to acquire next frame in DUPLICATIONMANAGER", hr, FrameInfoExpectedErrors);
	//}


#if DRAW_MOUSE_DX_SURFACE

	//@note : Visual Mouse Cursor at DXGI Output surface
	ID3D11Texture2D* outputTexture;
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&outputTexture));
	if (FAILED(hr)) {
		PLOG_ERROR << "Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER (" << MAKE_DXGI_HRESULT(hr) << ")";
		return ProcessFailure(nullptr, L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", hr);
	}

	D3D11_MAPPED_SUBRESOURCE dsec = { 0 };
	m_DeviceContext->CopyResource(m_GdiTexture, outputTexture);

	IDXGISurface1* surface1;
	hr = m_GdiTexture->QueryInterface(__uuidof(IDXGISurface1), reinterpret_cast<void**>(&surface1));
	if (FAILED(hr)) {
		PLOG_ERROR << "Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER (" << MAKE_DXGI_HRESULT(hr) << ")";
		return ProcessFailure(nullptr, L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", hr);
	}

	HDC  hdc;
	CURSORINFO cursorInfo = { 0 };
	cursorInfo.cbSize = sizeof(CURSORINFO);
	if (GetCursorInfo(&cursorInfo) == TRUE) {
		if (cursorInfo.flags == CURSOR_SHOWING)
		{
			auto cursorPosition = cursorInfo.ptScreenPos;
			auto lCursorSize = cursorInfo.cbSize;

			int xhotspot = 0;
			int yhotspot = 0;

			//std::cout << "cursor Info hCursor : " << cursorInfo.hCursor << std::endl; // 커서에 대한 핸들

			ICONINFO info = { 0 };
			if (::GetIconInfo(cursorInfo.hCursor, &info) != 0)
			{
				xhotspot = info.xHotspot;
				yhotspot = info.yHotspot;
				/*bool bBWCursor = (info.hbmColor == NULL);
				BITMAP bmpinfo = { 0 };
				if (::GetObject(info.hbmMask, sizeof(BITMAP), &bmpinfo) != 0)
				{
					std::cout << "bmp Info bmWidth : " << bmpinfo.bmWidth << std::endl;
					std::cout << "bmp Info bmHeight : " << abs(bmpinfo.bmHeight) / (bBWCursor ? 2 : 1) << std::endl;
				}*/
				//::DeleteObject(info.hbmColor);
				//::DeleteObject(info.hbmMask);
			}

			HRESULT hres;
			hres = surface1->GetDC(FALSE, &hdc);
			if (hres == S_OK)
			{
				if (DrawIconEx(hdc, cursorPosition.x - xhotspot, cursorPosition.y - yhotspot, cursorInfo.hCursor, 0, 0, 0, 0, DI_NORMAL | DI_COMPAT))
				{

				}
				else
				{
					PLOG_WARNING << "****** Draw Cursor failed ****** : " << GetLastError();
				}

				surface1->ReleaseDC(nullptr);
			}
			else
			{
				PLOG_ERROR << "****** HRESULT : " << format_error(hres);
				PLOG_ERROR << "****** GetDC error ****** : " << GetLastError();

			}


		}
	}
	else
	{
		PLOG_WARNING << "****** No Cursor info ****** : " << GetLastError();
	}


	m_DeviceContext->CopyResource(m_TempSharedSurf, m_GdiTexture);

#else

	// QI for IDXGIResource
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_AcquiredDesktopImage));
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", hr);
	}
	DesktopResource->Release();
	DesktopResource = nullptr;

	m_DeviceContext->CopyResource(m_TempSharedSurf, m_AcquiredDesktopImage);

	DoneWithFrame();

#endif

	return DUPL_RETURN_SUCCESS;
}

DUPL_RETURN DUPLICATIONMANAGER::TransformFrame(std::vector<uint8_t>&nv12_image)
{
	m_DeviceContext->CopyResource(m_ScaleSrcSurf, m_TempSharedSurf);

	DUPL_RETURN Ret = DrawNV12Frame();

	if (Ret != DUPL_RETURN_SUCCESS)
		return DUPL_RETURN_ERROR_UNEXPECTED;

	CopyNV12Image(nv12_image);


	return DUPL_RETURN_SUCCESS;
}

void DUPLICATIONMANAGER::ReleaseSurfaces()
{
	if (m_ScaleSrcSurf)
	{
		m_ScaleSrcSurf->Release();
		m_ScaleSrcSurf = nullptr;
	}
	if (m_BeforeSharedSurf)
	{
		m_BeforeSharedSurf->Release();
		m_BeforeSharedSurf = nullptr;
	}
	if (m_TempSharedSurf)
	{
		m_TempSharedSurf->Release();
		m_TempSharedSurf = nullptr;
	}
	if (m_LuminanceSurf)
	{
		m_LuminanceSurf->Release();
		m_LuminanceSurf = nullptr;
	}
	if (m_ChrominanceSurf)
	{
		m_ChrominanceSurf->Release();
		m_ChrominanceSurf = nullptr;
	}
	if (m_CbSurf)
	{
		m_CbSurf->Release();
		m_CbSurf = nullptr;
	}
	if (m_CrSurf)
	{
		m_CrSurf->Release();
		m_CrSurf = nullptr;
	}
	if (m_CPUAccessibleLuminanceSurf)
	{
		m_CPUAccessibleLuminanceSurf->Release();
		m_CPUAccessibleLuminanceSurf = nullptr;
	}
	if (m_CPUAccessibleChrominanceSurf)
	{
		m_CPUAccessibleChrominanceSurf->Release();
		m_CPUAccessibleChrominanceSurf = nullptr;
	}
}

//
// Resize surfaces
//
DUPL_RETURN DUPLICATIONMANAGER::ResizeNV12Surfaces()
{
	ReleaseSurfaces();

	DXGI_OUTDUPL_DESC lOutputDuplDesc;
	m_DeskDupl->GetDesc(&lOutputDuplDesc);

	// Create shared texture for all duplication threads to draw into
	D3D11_TEXTURE2D_DESC DeskTexD;
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = lOutputDuplDesc.ModeDesc.Width;
	DeskTexD.Height = lOutputDuplDesc.ModeDesc.Height;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = lOutputDuplDesc.ModeDesc.Format;
	DeskTexD.SampleDesc.Count = 1;
	DeskTexD.Usage = D3D11_USAGE_DEFAULT;
	DeskTexD.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_ScaleSrcSurf);
	if (FAILED(hr) || !m_ScaleSrcSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create shared texture", hr);
	}
	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_TempSharedSurf);
	if (FAILED(hr) || !m_TempSharedSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create temp texture", hr);
	}
	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_BeforeSharedSurf);
	if (FAILED(hr) || !m_BeforeSharedSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create before texture", hr);
	}

	DeskTexD.Format = DXGI_FORMAT_R8_UNORM;
	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET;

	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_LuminanceSurf);
	if (FAILED(hr) || !m_LuminanceSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create luminance texture", hr);
	}

	DeskTexD.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	DeskTexD.Usage = D3D11_USAGE_STAGING;
	DeskTexD.BindFlags = 0;

	hr = m_Device->CreateTexture2D(&DeskTexD, NULL, &m_CPUAccessibleLuminanceSurf);

	if (FAILED(hr) || m_CPUAccessibleLuminanceSurf == nullptr)
	{
		ProcessFailure(nullptr, L"Creating cpu accessable luminance texture failed.", hr);
		return DUPL_RETURN_ERROR_UNEXPECTED;
	}

	SetViewPort(&m_VPLuminance, DeskTexD.Width, DeskTexD.Height);
	DUPL_RETURN Ret = MakeRTV(&m_LuminanceRTV, m_LuminanceSurf);
	if (Ret != DUPL_RETURN_SUCCESS)
		return Ret;

	DeskTexD.Width = lOutputDuplDesc.ModeDesc.Width / 2;
	DeskTexD.Height = lOutputDuplDesc.ModeDesc.Height / 2;
	DeskTexD.Format = DXGI_FORMAT_R8G8_UNORM;

	DeskTexD.Usage = D3D11_USAGE_DEFAULT;
	DeskTexD.CPUAccessFlags = 0;
	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET;

	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_ChrominanceSurf);
	if (FAILED(hr) || !m_ChrominanceSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create chrominance texture", hr);
	}

	DeskTexD.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	DeskTexD.Usage = D3D11_USAGE_STAGING;
	DeskTexD.BindFlags = 0;

	hr = m_Device->CreateTexture2D(&DeskTexD, NULL, &m_CPUAccessibleChrominanceSurf);

	if (FAILED(hr) || m_CPUAccessibleChrominanceSurf == nullptr)
	{
		ProcessFailure(nullptr, L"Creating cpu accessable chrominance texture failed.", hr);
		return DUPL_RETURN_ERROR_UNEXPECTED;
	}

	SetViewPort(&m_VPChrominance, DeskTexD.Width, DeskTexD.Height);
	return MakeRTV(&m_ChrominanceRTV, m_ChrominanceSurf);
}

//
// Resize surfaces
//
DUPL_RETURN DUPLICATIONMANAGER::ResizeYCbCrSurfaces()
{
	ReleaseSurfaces();

	DXGI_OUTDUPL_DESC lOutputDuplDesc;
	m_DeskDupl->GetDesc(&lOutputDuplDesc);

	// Create shared texture for all duplication threads to draw into
	D3D11_TEXTURE2D_DESC DeskTexD;
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = lOutputDuplDesc.ModeDesc.Width;
	DeskTexD.Height = lOutputDuplDesc.ModeDesc.Height;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = lOutputDuplDesc.ModeDesc.Format;
	DeskTexD.SampleDesc.Count = 1;
	DeskTexD.Usage = D3D11_USAGE_DEFAULT;
	DeskTexD.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_ScaleSrcSurf);
	if (FAILED(hr) || !m_ScaleSrcSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create shared texture", hr);
	}
	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_TempSharedSurf);
	if (FAILED(hr) || !m_TempSharedSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create temp texture", hr);
	}

	DeskTexD.Format = DXGI_FORMAT_R8_UNORM;
	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET;

	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_LuminanceSurf);
	if (FAILED(hr) || !m_LuminanceSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create luminance texture", hr);
	}

	DeskTexD.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	DeskTexD.Usage = D3D11_USAGE_STAGING;
	DeskTexD.BindFlags = 0;

	hr = m_Device->CreateTexture2D(&DeskTexD, NULL, &m_CPUAccessibleLuminanceSurf);

	if (FAILED(hr) || m_CPUAccessibleLuminanceSurf == nullptr)
	{
		ProcessFailure(nullptr, L"Creating cpu accessable luminance texture failed.", hr);
		return DUPL_RETURN_ERROR_UNEXPECTED;
	}

	SetViewPort(&m_VPLuminance, DeskTexD.Width, DeskTexD.Height);
	DUPL_RETURN Ret = MakeRTV(&m_LuminanceRTV, m_LuminanceSurf);
	if (Ret != DUPL_RETURN_SUCCESS)
		return Ret;

	DeskTexD.Width = lOutputDuplDesc.ModeDesc.Width / 2;
	DeskTexD.Height = lOutputDuplDesc.ModeDesc.Height / 2;

	DeskTexD.Usage = D3D11_USAGE_DEFAULT;
	DeskTexD.CPUAccessFlags = 0;
	DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET;

	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_CbSurf);
	if (FAILED(hr) || !m_CbSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create cb texture", hr);
	}

	hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_CrSurf);
	if (FAILED(hr) || !m_CrSurf)
	{
		return ProcessFailure(m_Device, L"Failed to create cr texture", hr);
	}

	DeskTexD.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	DeskTexD.Usage = D3D11_USAGE_STAGING;
	DeskTexD.BindFlags = 0;

	hr = m_Device->CreateTexture2D(&DeskTexD, NULL, &m_CPUAccessibleChrominanceSurf);

	if (FAILED(hr) || m_CPUAccessibleChrominanceSurf == nullptr)
	{
		ProcessFailure(nullptr, L"Creating cpu accessable chrominance texture failed.", hr);
		return DUPL_RETURN_ERROR_UNEXPECTED;
	}

	SetViewPort(&m_VPChrominance, DeskTexD.Width, DeskTexD.Height);
	Ret = MakeRTV(&m_CbRTV, m_CbSurf);
	if (Ret != DUPL_RETURN_SUCCESS)
		return Ret;
	return MakeRTV(&m_CrRTV, m_CrSurf);
}

DUPL_RETURN DUPLICATIONMANAGER::MakeRTV(ID3D11RenderTargetView * *pRTV, ID3D11Texture2D * pSurf)
{
	if (*pRTV)
	{
		(*pRTV)->Release();
		*pRTV = nullptr;
	}
	// Create a render target view
	HRESULT hr = m_Device->CreateRenderTargetView(pSurf, nullptr, pRTV);

	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create render target view in OUTPUTMANAGER", hr);
	}

	return DUPL_RETURN_SUCCESS;
}

void DUPLICATIONMANAGER::SetViewPort(D3D11_VIEWPORT * VP, UINT Width, UINT Height)
{
	VP->Width = static_cast<FLOAT>(Width);
	VP->Height = static_cast<FLOAT>(Height);
	VP->MinDepth = 0.0f;
	VP->MaxDepth = 1.0f;
	VP->TopLeftX = 0;
	VP->TopLeftY = 0;
}

DUPL_RETURN DUPLICATIONMANAGER::InitShaders()
{
	HRESULT hr;

	UINT Size = ARRAYSIZE(g_VS);
	hr = m_Device->CreateVertexShader(g_VS, Size, nullptr, &m_VertexShader);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create vertex shader in OUTPUTMANAGER", hr, SystemTransitionsExpectedErrors);
	}

	m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);

	// Vertices for drawing whole texture
	VERTEX Vertices[NUMVERTICES] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0), XMFLOAT2(1.0f, 0.0f) },
	};

	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;

	D3D11_BUFFER_DESC BufferDesc;
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(VERTEX) * NUMVERTICES;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;

	// Create vertex buffer
	hr = m_Device->CreateBuffer(&BufferDesc, &InitData, &m_VertexBuffer);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create vertex buffer when drawing a frame", hr, SystemTransitionsExpectedErrors);
	}
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &Stride, &Offset);

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT NumElements = ARRAYSIZE(Layout);
	hr = m_Device->CreateInputLayout(Layout, NumElements, g_VS, Size, &m_InputLayout);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create input layout in OUTPUTMANAGER", hr, SystemTransitionsExpectedErrors);
	}
	m_DeviceContext->IASetInputLayout(m_InputLayout);

	Size = ARRAYSIZE(g_PS_Y);
	hr = m_Device->CreatePixelShader(g_PS_Y, Size, nullptr, &m_PixelShaderLuminance);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create pixel shader in OUTPUTMANAGER", hr, SystemTransitionsExpectedErrors);
	}

	Size = ARRAYSIZE(g_PS_CbCr);
	hr = m_Device->CreatePixelShader(g_PS_CbCr, Size, nullptr, &m_PixelShaderCbCr);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create pixel CbCr in OUTPUTMANAGER", hr, SystemTransitionsExpectedErrors);
	}

	Size = ARRAYSIZE(g_PS_UV);
	hr = m_Device->CreatePixelShader(g_PS_UV, Size, nullptr, &m_PixelShaderChrominance);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create pixel UV in OUTPUTMANAGER", hr, SystemTransitionsExpectedErrors);
	}

	return DUPL_RETURN_SUCCESS;
}

//
// Draw frame for NV12 texture
//
DUPL_RETURN DUPLICATIONMANAGER::DrawNV12Frame()
{
	HRESULT hr;

	// If window was resized, resize swapchain
	if (m_NeedsResize)
	{
		DUPL_RETURN Ret = ResizeNV12Surfaces();
		if (Ret != DUPL_RETURN_SUCCESS)
		{
			return Ret;
		}
		m_NeedsResize = false;
	}

	D3D11_TEXTURE2D_DESC FrameDesc;
	m_ScaleSrcSurf->GetDesc(&FrameDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
	ShaderDesc.Format = FrameDesc.Format;
	ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderDesc.Texture2D.MostDetailedMip = FrameDesc.MipLevels - 1;
	ShaderDesc.Texture2D.MipLevels = FrameDesc.MipLevels;

	// Create new shader resource view
	ID3D11ShaderResourceView* ShaderResource = nullptr;
	hr = m_Device->CreateShaderResourceView(m_ScaleSrcSurf, &ShaderDesc, &ShaderResource);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create shader resource when drawing a frame", hr);
	}
	m_DeviceContext->PSSetShaderResources(0, 1, &ShaderResource);

	// Set resources
	m_DeviceContext->OMSetRenderTargets(1, &m_LuminanceRTV, nullptr);
	m_DeviceContext->PSSetShader(m_PixelShaderLuminance, nullptr, 0);
	m_DeviceContext->RSSetViewports(1, &m_VPLuminance);

	// Draw textured quad onto render target
	m_DeviceContext->Draw(NUMVERTICES, 0);

	m_DeviceContext->OMSetRenderTargets(1, &m_ChrominanceRTV, nullptr);
	m_DeviceContext->PSSetShader(m_PixelShaderChrominance, nullptr, 0);
	m_DeviceContext->RSSetViewports(1, &m_VPChrominance);

	// Draw textured quad onto render target
	m_DeviceContext->Draw(NUMVERTICES, 0);

	// Release shader resource
	ShaderResource->Release();
	ShaderResource = nullptr;

	return DUPL_RETURN_SUCCESS;
}

//
// Draw frame for NV12 texture
//
DUPL_RETURN DUPLICATIONMANAGER::DrawYCbCrFrame()
{
	HRESULT hr;

	// If window was resized, resize swapchain
	if (m_NeedsResize)
	{
		DUPL_RETURN Ret = ResizeYCbCrSurfaces();
		if (Ret != DUPL_RETURN_SUCCESS)
		{
			return Ret;
		}
		m_NeedsResize = false;
	}

	D3D11_TEXTURE2D_DESC FrameDesc;
	m_ScaleSrcSurf->GetDesc(&FrameDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
	ShaderDesc.Format = FrameDesc.Format;
	ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderDesc.Texture2D.MostDetailedMip = FrameDesc.MipLevels - 1;
	ShaderDesc.Texture2D.MipLevels = FrameDesc.MipLevels;

	// Create new shader resource view
	ID3D11ShaderResourceView* ShaderResource = nullptr;
	hr = m_Device->CreateShaderResourceView(m_ScaleSrcSurf, &ShaderDesc, &ShaderResource);
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to create shader resource when drawing a frame", hr);
	}
	m_DeviceContext->PSSetShaderResources(0, 1, &ShaderResource);

	// Set resources
	m_DeviceContext->OMSetRenderTargets(1, &m_LuminanceRTV, nullptr);
	m_DeviceContext->PSSetShader(m_PixelShaderLuminance, nullptr, 0);
	m_DeviceContext->RSSetViewports(1, &m_VPLuminance);

	// Draw textured quad onto Y render target
	m_DeviceContext->Draw(NUMVERTICES, 0);

	ID3D11RenderTargetView* pRenderTargetViews[2] = { m_CbRTV, m_CrRTV };

	m_DeviceContext->OMSetRenderTargets(2, pRenderTargetViews, nullptr);
	m_DeviceContext->PSSetShader(m_PixelShaderCbCr, nullptr, 0);
	m_DeviceContext->RSSetViewports(1, &m_VPChrominance);

	// Draw textured quad onto Cb, Cr render targets
	m_DeviceContext->Draw(NUMVERTICES, 0);

	// Release shader resource
	ShaderResource->Release();
	ShaderResource = nullptr;

	return DUPL_RETURN_SUCCESS;
}

void  DUPLICATIONMANAGER::CopyNV12Image(std::vector<uint8_t>&nv12_image)
{
	m_DeviceContext->CopyResource(m_CPUAccessibleLuminanceSurf, m_LuminanceSurf);
	D3D11_MAPPED_SUBRESOURCE resource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);

	HRESULT hr = m_DeviceContext->Map(m_CPUAccessibleLuminanceSurf, subresource, D3D11_MAP_READ, 0, &resource);

	BYTE* sptr = reinterpret_cast<BYTE*>(resource.pData);
	//BYTE* dptr = pSurface->Data.Y;
	BYTE* dptr = &nv12_image[0];

	//Store Image Pitch
	m_ImagePitch = resource.RowPitch;
	//m_ImagePitch = resource.RowPitch < pSurface->Data.Pitch ? resource.RowPitch : pSurface->Data.Pitch;
	int height = GetImageHeight();
	int width = GetImageWidth();

	//memcpy(dptr, sptr, height * width);

	for (int i = 0; i < height; i++)
	{
		if (memcpy_s(dptr, m_ImagePitch, sptr, m_ImagePitch))
		{
			printf("There is a problem transferring the luminance data to NV12 texture.\n");
		}
		sptr += resource.RowPitch;
		dptr += m_ImagePitch;
	}

	/*for (int i = 0; i < height; i++)
	{
		if (memcpy_s(dptr, m_ImagePitch, sptr, m_ImagePitch))
		{
			printf("There is a problem transferring the luminance data to NV12 texture.\n");
		}
		sptr += resource.RowPitch;
		dptr += pSurface->Data.Pitch;
	}*/
	m_DeviceContext->Unmap(m_CPUAccessibleLuminanceSurf, subresource);

	m_DeviceContext->CopyResource(m_CPUAccessibleChrominanceSurf, m_ChrominanceSurf);
	hr = m_DeviceContext->Map(m_CPUAccessibleChrominanceSurf, subresource, D3D11_MAP_READ, 0, &resource);

	sptr = reinterpret_cast<BYTE*>(resource.pData);
	//dptr = pSurface->Data.UV;
	dptr = &nv12_image[1920 * 1080];

	//m_ImagePitch = resource.RowPitch < pSurface->Data.Pitch ? resource.RowPitch : pSurface->Data.Pitch;
	m_ImagePitch = resource.RowPitch;
	height /= 2;
	width /= 2;

	//memcpy(dptr, sptr, 1920 * 1080 /2);

	for (int i = 0; i < height; i++)
	{
		if (memcpy_s(dptr, m_ImagePitch, sptr, m_ImagePitch))
		{
			printf("There is a problem transferring the chrominance data to NV12 texture.\n");
		}
		sptr += resource.RowPitch;
		dptr += m_ImagePitch;
	}

	m_DeviceContext->Unmap(m_CPUAccessibleChrominanceSurf, subresource);

}


int DUPLICATIONMANAGER::GetImageHeight()
{
	return m_OutputDesc.DesktopCoordinates.bottom - m_OutputDesc.DesktopCoordinates.top;
}

int DUPLICATIONMANAGER::GetImageWidth()
{
	return m_OutputDesc.DesktopCoordinates.right - m_OutputDesc.DesktopCoordinates.left;
}

int DUPLICATIONMANAGER::GetImagePitch()
{
	return m_ImagePitch;
}


//
// Release frame
//
DUPL_RETURN DUPLICATIONMANAGER::DoneWithFrame()
{
	HRESULT hr;

	if (m_AcquiredDesktopImage)
	{
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	hr = m_DeskDupl->ReleaseFrame();
	if (FAILED(hr))
	{
		return ProcessFailure(m_Device, L"Failed to release frame in DUPLICATIONMANAGER", hr, FrameInfoExpectedErrors);
	}


	return DUPL_RETURN_SUCCESS;
}

//
// Gets output desc into DescPtr
//
void DUPLICATIONMANAGER::GetOutputDesc(_Out_ DXGI_OUTPUT_DESC * DescPtr)
{
	*DescPtr = m_OutputDesc;
}

_Post_satisfies_(return != DUPL_RETURN_SUCCESS)
DUPL_RETURN DUPLICATIONMANAGER::ProcessFailure(_In_opt_ ID3D11Device * Device, _In_ LPCWSTR Str, HRESULT hr, _In_opt_z_ HRESULT * ExpectedErrors)
{
	HRESULT TranslatedHr;
	std::string message = std::system_category().message(hr);

	std::cerr << "Error Message : " << Str << std::endl;
	std::cerr << "Hr : " << hr << std::endl;

	// On an error check if the DX device is lost
	if (Device)
	{
		HRESULT DeviceRemovedReason = Device->GetDeviceRemovedReason();

		switch (DeviceRemovedReason)
		{
		case DXGI_ERROR_DEVICE_REMOVED:
		case DXGI_ERROR_DEVICE_RESET:
			case static_cast<HRESULT>(E_OUTOFMEMORY) :
			{
				// Our device has been stopped due to an external event on the GPU so map them all to
				// device removed and continue processing the condition
				TranslatedHr = DXGI_ERROR_DEVICE_REMOVED;
				break;
			}

			case S_OK:
			{
				// Device is not removed so use original error
				TranslatedHr = hr;
				break;
			}

			default:
			{
				// Device is removed but not a error we want to remap
				TranslatedHr = DeviceRemovedReason;
			}
		}
	}
	else
	{
		TranslatedHr = hr;
	}

	// Check if this error was expected or not
	if (ExpectedErrors)
	{
		HRESULT* CurrentResult = ExpectedErrors;

		while (*CurrentResult != S_OK)
		{
			if (*(CurrentResult++) == TranslatedHr)
			{
				return DUPL_RETURN_ERROR_EXPECTED;
			}
		}
	}

	// Error was not expected so display the message box
	//DisplayMsg(Str, TranslatedHr);

	return DUPL_RETURN_ERROR_UNEXPECTED;
}

//
// Displays a message
//
void DUPLICATIONMANAGER::DisplayMsg(_In_ LPCWSTR Str, HRESULT hr)
{
	if (SUCCEEDED(hr))
	{
		printf("%ls\n", Str);
		return;
	}

	const UINT StringLen = (UINT)(wcslen(Str) + sizeof(" with HRESULT 0x########."));
	wchar_t* OutStr = new wchar_t[StringLen];
	if (!OutStr)
	{
		return;
	}

	INT LenWritten = swprintf_s(OutStr, StringLen, L"%s with 0x%X.", Str, hr);
	if (LenWritten != -1)
	{
		printf("%ls\n", OutStr);

	}

	delete[] OutStr;
}




