// adapted code from http://www.codeproject.com/Tips/1116253/Desktop-Screen-Capture-on-Windows-via-Windows-Desk

#define WIN32_LEAN_AND_MEAN    

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <memory>
#include <algorithm>
#include <string>

#pragma comment(lib, "D3D11.lib")

template <typename T>
class CComPtrCustom
{
public:

	CComPtrCustom(T *aPtrElement)
		:element(aPtrElement)
	{
	}

	CComPtrCustom()
		:element(nullptr)
	{
	}

	virtual ~CComPtrCustom()
	{
		Release();
	}

	T* Detach()
	{
		auto lOutPtr = element;

		element = nullptr;

		return lOutPtr;
	}

	T* detach()
	{
		return Detach();
	}

	void Release()
	{
		if (element == nullptr)
			return;

		auto k = element->Release();

		element = nullptr;
	}

	CComPtrCustom& operator = (T *pElement)
	{
		Release();

		if (pElement == nullptr)
			return *this;

		auto k = pElement->AddRef();

		element = pElement;

		return *this;
	}

	void Swap(CComPtrCustom& other)
	{
		T* pTemp = element;
		element = other.element;
		other.element = pTemp;
	}

	T* operator->()
	{
		return element;
	}

	operator T*()
	{
		return element;
	}

	operator T*() const
	{
		return element;
	}


	T* get()
	{
		return element;
	}

	T* get() const
	{
		return element;
	}

	T** operator &()
	{
		return &element;
	}

	bool operator !()const
	{
		return element == nullptr;
	}

	operator bool()const
	{
		return element != nullptr;
	}

	bool operator == (const T *pElement)const
	{
		return element == pElement;
	}


	CComPtrCustom(const CComPtrCustom& aCComPtrCustom)
	{
		if (aCComPtrCustom.operator!())
		{
			element = nullptr;

			return;
		}

		element = aCComPtrCustom;

		auto h = element->AddRef();

		h++;
	}

	CComPtrCustom& operator = (const CComPtrCustom& aCComPtrCustom)
	{
		Release();

		element = aCComPtrCustom;

		auto k = element->AddRef();

		return *this;
	}

	_Check_return_ HRESULT CopyTo(T** ppT) throw()
	{
		if (ppT == NULL)
			return E_POINTER;

		*ppT = element;

		if (element)
			element->AddRef();

		return S_OK;
	}

	HRESULT CoCreateInstance(const CLSID aCLSID)
	{
		T* lPtrTemp;

		auto lresult = ::CoCreateInstance(aCLSID, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&lPtrTemp));

		if (SUCCEEDED(lresult))
		{
			if (lPtrTemp != nullptr)
			{
				Release();

				element = lPtrTemp;
			}

		}

		return lresult;
	}

protected:

	T* element;
};

// Driver types supported
D3D_DRIVER_TYPE gDriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE
};
UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

// Feature levels supported
D3D_FEATURE_LEVEL gFeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_1
};

UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);

class ScreenCaptureProcessorGDI {
private:
	CComPtrCustom<ID3D11Device> lDevice;
	CComPtrCustom<ID3D11DeviceContext> lImmediateContext;
	CComPtrCustom<IDXGIOutputDuplication> lDeskDupl;
	CComPtrCustom<ID3D11Texture2D> lAcquiredDesktopImage;
	CComPtrCustom<ID3D11Texture2D> lGDIImage;
	CComPtrCustom<ID3D11Texture2D> lDestImage;
	DXGI_OUTPUT_DESC lOutputDesc;
	DXGI_OUTDUPL_DESC lOutputDuplDesc;
	unsigned int frame_count;
	int lresult;
	D3D_FEATURE_LEVEL lFeatureLevel;
	HRESULT hr;
	CComPtrCustom<IDXGIResource> lDesktopResource;
	DXGI_OUTDUPL_FRAME_INFO lFrameInfo;

public:
	ScreenCaptureProcessorGDI() : frame_count(0), lresult(-1), hr(E_FAIL) {}
	bool init() {
		// Create device
		for (UINT DriverTypeIndex = 0; DriverTypeIndex < gNumDriverTypes; ++DriverTypeIndex)
		{
			hr = D3D11CreateDevice(
				nullptr,
				gDriverTypes[DriverTypeIndex],
				nullptr,
				0,
				gFeatureLevels,
				gNumFeatureLevels,
				D3D11_SDK_VERSION,
				&lDevice,
				&lFeatureLevel,
				&lImmediateContext);

			if (SUCCEEDED(hr))
			{
				// Device creation success, no need to loop anymore
				break;
			}

			lDevice.Release();

			lImmediateContext.Release();
		}

		if (FAILED(hr))
			return false;

		Sleep(100);

		if (lDevice == nullptr)
			return false;

		// Get DXGI device
		CComPtrCustom<IDXGIDevice> lDxgiDevice;

		hr = lDevice->QueryInterface(IID_PPV_ARGS(&lDxgiDevice));

		if (FAILED(hr))
			return false;

		// Get DXGI adapter
		CComPtrCustom<IDXGIAdapter> lDxgiAdapter;
		hr = lDxgiDevice->GetParent(
			__uuidof(IDXGIAdapter),
			reinterpret_cast<void**>(&lDxgiAdapter));

		if (FAILED(hr))
			return false;

		lDxgiDevice.Release();

		UINT Output = 0;

		// Get output
		CComPtrCustom<IDXGIOutput> lDxgiOutput;
		hr = lDxgiAdapter->EnumOutputs(
			Output,
			&lDxgiOutput);

		if (FAILED(hr))
			return false;

		lDxgiAdapter.Release();

		hr = lDxgiOutput->GetDesc(
			&lOutputDesc);

		if (FAILED(hr))
			return false;

		// QI for Output 1
		CComPtrCustom<IDXGIOutput1> lDxgiOutput1;

		hr = lDxgiOutput->QueryInterface(IID_PPV_ARGS(&lDxgiOutput1));

		if (FAILED(hr))
			return false;

		lDxgiOutput.Release();

		// Create desktop duplication
		hr = lDxgiOutput1->DuplicateOutput(
			lDevice,
			&lDeskDupl);

		if (FAILED(hr))
			return false;

		lDxgiOutput1.Release();

		// Create GUI drawing texture
		lDeskDupl->GetDesc(&lOutputDuplDesc);

		D3D11_TEXTURE2D_DESC desc;

		desc.Width = lOutputDuplDesc.ModeDesc.Width;

		desc.Height = lOutputDuplDesc.ModeDesc.Height;

		desc.Format = lOutputDuplDesc.ModeDesc.Format;

		desc.ArraySize = 1;

		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;

		desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

		desc.SampleDesc.Count = 1;

		desc.SampleDesc.Quality = 0;

		desc.MipLevels = 1;

		desc.CPUAccessFlags = 0;

		desc.Usage = D3D11_USAGE_DEFAULT;

		hr = lDevice->CreateTexture2D(&desc, NULL, &lGDIImage);

		if (FAILED(hr))
			return false;

		if (lGDIImage == nullptr)
			return false;


		// Create CPU access texture

		desc.Width = lOutputDuplDesc.ModeDesc.Width;

		desc.Height = lOutputDuplDesc.ModeDesc.Height;

		desc.Format = lOutputDuplDesc.ModeDesc.Format;

		desc.ArraySize = 1;

		desc.BindFlags = 0;

		desc.MiscFlags = 0;

		desc.SampleDesc.Count = 1;

		desc.SampleDesc.Quality = 0;

		desc.MipLevels = 1;

		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_STAGING;

		hr = lDevice->CreateTexture2D(&desc, NULL, &lDestImage);

		if (FAILED(hr))
			return false;

		if (lDestImage == nullptr)
			return false;

		return true;
	}

	bool grabImage() {
		int lTryCount = 4;

		do
		{
			Sleep(8); // not sure if required

			// Get new frame
			hr = lDeskDupl->AcquireNextFrame(
				33,
				&lFrameInfo,
				&lDesktopResource);

			if (SUCCEEDED(hr))
				break;

			if (hr == DXGI_ERROR_WAIT_TIMEOUT)
			{
				continue;
			}
			else if (FAILED(hr))
				break;

		} while (--lTryCount > 0);

		if (FAILED(hr))
			return false;

		// QI for ID3D11Texture2D
		hr = lDesktopResource->QueryInterface(IID_PPV_ARGS(&lAcquiredDesktopImage));

		if (FAILED(hr))
			return false;

		lDesktopResource.Release();

		if (lAcquiredDesktopImage == nullptr)
			return false;

		// Copy image into GDI drawing texture
		lImmediateContext->CopyResource(lGDIImage, lAcquiredDesktopImage);

		// Draw cursor image into GDI drawing texture
		CComPtrCustom<IDXGISurface1> lIDXGISurface1;

		hr = lGDIImage->QueryInterface(IID_PPV_ARGS(&lIDXGISurface1));

		if (FAILED(hr))
			return false;

		CURSORINFO lCursorInfo = { 0 };

		lCursorInfo.cbSize = sizeof(lCursorInfo);

		auto lBoolres = GetCursorInfo(&lCursorInfo);

		if (lBoolres == TRUE)
		{
			if (lCursorInfo.flags == CURSOR_SHOWING)
			{
				auto lCursorPosition = lCursorInfo.ptScreenPos;

				auto lCursorSize = lCursorInfo.cbSize;

				HDC  lHDC;

				lIDXGISurface1->GetDC(FALSE, &lHDC);

				DrawIconEx(
					lHDC,
					lCursorPosition.x,
					lCursorPosition.y,
					lCursorInfo.hCursor,
					0,
					0,
					0,
					0,
					DI_NORMAL | DI_DEFAULTSIZE);

				lIDXGISurface1->ReleaseDC(nullptr);
			}

		}

		// Copy image into CPU access texture
		lImmediateContext->CopyResource(lDestImage, lGDIImage);

		// Copy from CPU access texture to bitmap buffer
		D3D11_MAPPED_SUBRESOURCE resource;
		UINT subresource = D3D11CalcSubresource(0, 0, 0);
		lImmediateContext->Map(lDestImage, subresource, D3D11_MAP_READ_WRITE, 0, &resource);

		BITMAPINFO	lBmpInfo;

		// BMP 32 bpp
		ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

		lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

		lBmpInfo.bmiHeader.biBitCount = 32;

		lBmpInfo.bmiHeader.biCompression = BI_RGB;

		lBmpInfo.bmiHeader.biWidth = lOutputDuplDesc.ModeDesc.Width;

		lBmpInfo.bmiHeader.biHeight = lOutputDuplDesc.ModeDesc.Height;

		lBmpInfo.bmiHeader.biPlanes = 1;

		lBmpInfo.bmiHeader.biSizeImage = lOutputDuplDesc.ModeDesc.Width
			* lOutputDuplDesc.ModeDesc.Height * 4;

		std::unique_ptr<BYTE> pBuf(new BYTE[lBmpInfo.bmiHeader.biSizeImage]);

		UINT lBmpRowPitch = lOutputDuplDesc.ModeDesc.Width * 4;

		BYTE* sptr = reinterpret_cast<BYTE*>(resource.pData);
		BYTE* dptr = pBuf.get() + lBmpInfo.bmiHeader.biSizeImage - lBmpRowPitch;

		UINT lRowPitch = std::min<UINT>(lBmpRowPitch, resource.RowPitch);

		for (size_t h = 0; h < lOutputDuplDesc.ModeDesc.Height; ++h)
		{
			memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
			sptr += resource.RowPitch;
			dptr -= lBmpRowPitch;
		}

		// Save bitmap buffer into the file ScreenShot.bmp
		WCHAR lMyDocPath[MAX_PATH];

		hr = SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, lMyDocPath);

		if (FAILED(hr))
			return false;

		std::wstring lFilePath = L"ScreenShot" + std::to_wstring(frame_count) + L".bmp"; /*std::wstring(lMyDocPath) + L"\\ScreenShot.bmp";*/

		FILE* lfile = nullptr;
		auto lerr = _wfopen_s(&lfile, lFilePath.c_str(), L"wb");

		if (lerr != 0)
			return false;

		if (lfile != nullptr)
		{
			BITMAPFILEHEADER bmpFileHeader;

			bmpFileHeader.bfReserved1 = 0;
			bmpFileHeader.bfReserved2 = 0;
			bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lBmpInfo.bmiHeader.biSizeImage;
			bmpFileHeader.bfType = 'MB';
			bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, lfile);
			fwrite(&lBmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, lfile);
			fwrite(pBuf.get(), lBmpInfo.bmiHeader.biSizeImage, 1, lfile);

			fclose(lfile);

			//ShellExecute(0, 0, lFilePath.c_str(), 0, 0, SW_SHOW);

			lresult = 0;
			frame_count++;
		}

		return lresult;
	}

	bool release() {
		return true;
	}
};

int main()
{
	ScreenCaptureProcessorGDI capture;
	capture.init();
	capture.grabImage();
	capture.release();
}