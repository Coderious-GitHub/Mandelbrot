#include "Direct2D.h"

Direct2D::Direct2D(HWND hWindow, std::shared_ptr<Direct3D> pD3D)
{
	HRESULT hr;

	D3D = pD3D;
    hWnd = hWindow;

	swapchain = D3D->GetSwapChain();

	DXGI_SWAP_CHAIN_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	hr = swapchain->GetDesc(&pDesc);
	width = pDesc.BufferDesc.Width;
	height = pDesc.BufferDesc.Height;

	Init2D();
}


Direct2D::~Direct2D()
{


}


HRESULT Direct2D::Init2D()
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pFactory.GetAddressOf());

	HRESULT hr = S_OK;

	if (!pBackBuffer2d)
	{
		// Get a surface in the swap chain
		hr = swapchain->GetBuffer(
			0,
			IID_PPV_ARGS(pBackBuffer2d.GetAddressOf())
		);
			
		//create a Direct2D render target
		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				width,
				height);

		// Create a Direct2D render target which can draw into the surface in the swap chain
		hr = pFactory->CreateDxgiSurfaceRenderTarget(
			pBackBuffer2d.Get(),
			&props,
			bbOverlay.GetAddressOf()
		);

		//create a background brush
		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
			hr = bbOverlay->CreateSolidColorBrush(color, pBrush.GetAddressOf());
		}
	}

	hr = InitWrite();

	return hr;

}

HRESULT Direct2D::InitWrite()
{
	HRESULT hr;

	//create the factory
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(pIDWriteFactory.GetAddressOf()));


	//create DirectWrite format
	if (SUCCEEDED(hr))
	{
		hr = pIDWriteFactory->CreateTextFormat(
			L"Helvetica",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			2.0f,
			L"en-US",
			pITextFormat.GetAddressOf()
		);
	}

	//create a brush
	if (SUCCEEDED(hr))
	{
		hr = bbOverlay->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			pITextBrush.GetAddressOf()
		);
	}

	return hr;
}

void Direct2D::OnPaint()
{
	HRESULT hr;

	bbOverlay->BeginDraw();

	D2D1_SIZE_F rtSize = bbOverlay->GetSize();

	D2D1_RECT_F rectangle1 = D2D1::RectF(
		0.0f, 0.0f, 50.0f, 20.0f
	);

	bbOverlay->FillRectangle(&rectangle1, pBrush.Get());

	std::wstringstream wss;
	wss << "RealStart: ";
	wss << D3D->realStart;
	wss << "\n";
	wss << "ImagStart: ";
	wss << D3D->imagStart;
	wss << "\n";
	wss << "Width: ";
	wss << D3D->mandelWidth;
	wss << "\n";
	wss << "Height: ";
	wss << D3D->mandelHeight;
	wss << "\n";
	wss << "Max Iterations: ";
	wss << D3D->iterations;
	wss << "\n";
	wss << "Frame (ms): ";
	wss << D3D->timeFrame;
	wss << "\n";


	bbOverlay->DrawText(
		wss.str().c_str(),
		wss.str().length(),
		pITextFormat.Get(),
		rectangle1,
		pITextBrush.Get());

	hr = bbOverlay->EndDraw();

	//swapchain->Present(0, 0);
}

void Direct2D::OnResize()
{
	HRESULT hr;

	// Get a surface in the swap chain
	hr = swapchain->GetBuffer(
		0,
		IID_PPV_ARGS(pBackBuffer2d.GetAddressOf())
	);

	//create a Direct2D render target
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			width,
			height);

	// Create a Direct2D render target which can draw into the surface in the swap chain
	hr = pFactory->CreateDxgiSurfaceRenderTarget(
		pBackBuffer2d.Get(),
		&props,
		bbOverlay.GetAddressOf()
	);
}


//clear shared resources, needed for the OnResize in Direct3D
void Direct2D::Clear()
{
	pBackBuffer2d = nullptr;
	bbOverlay = nullptr;
}