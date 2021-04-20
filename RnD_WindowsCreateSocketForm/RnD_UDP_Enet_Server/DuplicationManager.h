// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#ifndef _DUPLICATIONMANAGER_H_
#define _DUPLICATIONMANAGER_H_

#include <d3d11.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <sal.h>
#include <new>
#include <stdio.h>

#include <vector>

#include "PixelShaderLuminance.h"
#include "PixelShaderChrominance.h"
#include "PixelShaderCbCr.h"
#include "VertexShader.h"


#define NUMVERTICES 6

extern HRESULT SystemTransitionsExpectedErrors[];
extern HRESULT CreateDuplicationExpectedErrors[];
extern HRESULT FrameInfoExpectedErrors[];
extern HRESULT AcquireFrameExpectedError[];
extern HRESULT EnumOutputsExpectedErrors[];

typedef _Return_type_success_(return == DUPL_RETURN_SUCCESS) enum
{
	DUPL_RETURN_SUCCESS = 0,
	DUPL_RETURN_GET_FRAME_TIME_OUT = 1,
	DUPL_RETURN_ERROR_ACCESS_LOST = 2,
	DUPL_RETURN_ERROR_EXPECTED = 3,
	DUPL_RETURN_ERROR_UNEXPECTED = 4
}DUPL_RETURN;

//
// A vertex with a position and texture coordinate
//
typedef struct _VERTEX
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
} VERTEX;

//
// Handles the task of duplicating an output.
//
class DUPLICATIONMANAGER
{
public:

	DUPLICATIONMANAGER();
	~DUPLICATIONMANAGER();

	/**
	 * DirectX11 Device DXGI �ʱ�ȭ.
	 * ������ ������(width, height)�� ������
	 * 
	 * \return DUPL_RETURN_SUCCESS �̿��� ���ϰ��� ��� ������ ó����
	 */
	DUPL_RETURN InitDupl();

	/**
	 * BGRA �������� ������
	 * BGRA ������ �������⸦ �����ϸ� ���������� ������ ������(BGRA)�� ������ ����.
	 * 
	 * \return					DUPL_RETURN_SUCCESS (�������� ���������� ������)
	 *							DUPL_RETURN_GET_FRAME_TIME_OUT (���� ȭ�� ��ȭ�� ���� ��ȯ��)
	 *							�� ���� �̿��� ��� ������ ó����
	 */
	_Success_(*Timeout == false && return == DUPL_RETURN_SUCCESS)
	DUPL_RETURN GetFrame();

	/**
	 * BGRA �������� NV12 ���������� ��ȯ.
	 * BGRA �������� ���������� ������ ����
	 * 
	 * \param nv12_image			����ȯ�� �Ϸ�� ������(NV12) ����� ���� ����
	 * \return					DUPL_RETURN_SUCCESS �̿��� ���� ��� ������ ó����
	 */
	DUPL_RETURN TransformFrame(_Inout_ std::vector<uint8_t>& nv12_image);

	/**
	 * NV12 �����ӿ� ���콺 �����͸� �׸��� ���� �ؽ�ó ���� �Լ�.
	 */
	void CrateTexture();
	void SurfaceResize();
	int GetImageHeight();
	int GetImageWidth();
	int GetImagePitch();

private:

	// vars
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	IDXGIOutputDuplication* m_DeskDupl;
	ID3D11Texture2D* m_AcquiredDesktopImage;
	ID3D11Texture2D* m_GdiTexture;
	ID3D11Texture2D* m_CPUAccessibleLuminanceSurf;
	ID3D11Texture2D* m_CPUAccessibleChrominanceSurf;
	ID3D11Texture2D* m_ScaleSrcSurf;
	ID3D11Texture2D* m_TempSharedSurf;
	ID3D11Texture2D* m_BeforeSharedSurf;
	ID3D11Texture2D* m_LuminanceSurf;
	ID3D11Texture2D* m_CbSurf;
	ID3D11Texture2D* m_CrSurf;
	ID3D11Texture2D* m_ChrominanceSurf;
	ID3D11Buffer* m_VertexBuffer;
	DXGI_OUTPUT_DESC m_OutputDesc;

	int m_ImagePitch;
	bool m_NeedsResize;
	ID3D11RenderTargetView* m_LuminanceRTV;
	ID3D11RenderTargetView* m_CbRTV;
	ID3D11RenderTargetView* m_CrRTV;
	ID3D11RenderTargetView* m_ChrominanceRTV;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShaderLuminance;
	ID3D11PixelShader* m_PixelShaderCbCr;
	ID3D11PixelShader* m_PixelShaderChrominance;
	ID3D11InputLayout* m_InputLayout;
	ID3D11SamplerState* m_SamplerLinear;
	D3D11_VIEWPORT m_VPLuminance;
	D3D11_VIEWPORT m_VPChrominance;

	//HANDLE GuardSurface;
	bool CaptureFrameAvailable;

	//methods
	DUPL_RETURN InitDevice();
	DUPL_RETURN InitDxgi();
	_Post_satisfies_(return != DUPL_RETURN_SUCCESS)
		DUPL_RETURN ProcessFailure(_In_opt_ ID3D11Device* Device, _In_ LPCWSTR Str, HRESULT hr, _In_opt_z_ HRESULT* ExpectedErrors = nullptr);
	void DisplayMsg(_In_ LPCWSTR Str, HRESULT hr);
	void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr);
	DUPL_RETURN DoneWithFrame();

	void CopyNV12Image(std::vector<uint8_t>& nv12_image);

	DUPL_RETURN DrawNV12Frame();
	DUPL_RETURN DrawYCbCrFrame();
	DUPL_RETURN ResizeNV12Surfaces();
	DUPL_RETURN ResizeYCbCrSurfaces();
	DUPL_RETURN MakeRTV(ID3D11RenderTargetView** pRTV, ID3D11Texture2D* pSurf);
	void SetViewPort(D3D11_VIEWPORT* VP, UINT Width, UINT Height);
	DUPL_RETURN InitShaders();
	void ReleaseSurfaces();
};

#endif
