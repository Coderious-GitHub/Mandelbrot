#include "Direct3D.h"

Direct3D::Direct3D(HWND hWindow, int w, int h)
{
	width = w;
	height = h;
	hWnd = hWindow;

	realStart = -2.0;
	imagStart = -1.25;
	mandelWidth = 2.5;
	startRatio = (float)w / h;
	currentRatio = 1;
	mandelHeight = mandelWidth / GetScreenRatio();
	iterations = 100;
	origColor = true;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if(SUCCEEDED(hr))
		InitD3D();
}

Direct3D::Direct3D()
{

}


void Direct3D::InitD3D()
{
	//create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	//clear out the struc for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//fill the swap chain description struct
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//create a device, device context and swap chain using the above info
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		swapchain.GetAddressOf(),
		dev.GetAddressOf(),
		NULL,
		devcon.GetAddressOf());

	//create the depth buffer texture
	D3D11_TEXTURE2D_DESC texd;
	ZeroMemory(&texd, sizeof(texd));

	texd.Width = width;
	texd.Height = height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 4;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;


	//get the address of the backbuffer
	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//use the backbuffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, backbuffer.GetAddressOf());
	pBackBuffer->Release();

	//set the render target as the backbuffer
	devcon->OMSetRenderTargets(1, backbuffer.GetAddressOf(), zbuffer.Get());

	//set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	devcon->RSSetViewports(1, &viewport);

	InitGraphics();
	InitPipeline();
	RenderFrame();
}


//this is the function used to render a single frame
void Direct3D::RenderFrame()
{
	HRESULT hr;

	D3D11_QUERY_DESC qDesTimestampDisjoint;
	qDesTimestampDisjoint.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	qDesTimestampDisjoint.MiscFlags = 0;

	D3D11_QUERY_DESC qDesStartFrame;
	qDesStartFrame.Query = D3D11_QUERY_TIMESTAMP;
	qDesStartFrame.MiscFlags = 0;

	D3D11_QUERY_DESC qDesEndFrame;
	qDesEndFrame.Query = D3D11_QUERY_TIMESTAMP;
	qDesEndFrame.MiscFlags = 0;

	ComPtr<ID3D11Query> pQueryTimestampDisjoint;
	ComPtr<ID3D11Query> pQueryStartFrame;
	ComPtr<ID3D11Query> pQueryEndFrame;

	hr = dev->CreateQuery(&qDesTimestampDisjoint, pQueryTimestampDisjoint.GetAddressOf());
	hr = dev->CreateQuery(&qDesStartFrame, pQueryStartFrame.GetAddressOf());
	hr = dev->CreateQuery(&qDesEndFrame, pQueryEndFrame.GetAddressOf());

	devcon->Begin(pQueryTimestampDisjoint.Get());
	devcon->End(pQueryStartFrame.Get());

	CBUFFER cBuffer;
	cBuffer.rStart = realStart;
	cBuffer.iStart = imagStart;
	cBuffer.width = mandelWidth;
	cBuffer.height = mandelHeight;
	cBuffer.screenWidth = width;
	cBuffer.screenHeight = height;
	cBuffer.maxIt = iterations;

	const float COLOR[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	//clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(backbuffer.Get(), COLOR);

	//clear the depth buffer
	devcon->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//do the 3D render here
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, pVBuffer.GetAddressOf(), &stride, &offset);
	devcon->IASetIndexBuffer(pIBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//select the type of primitive
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//draw the vertex buffer to the back buffer
	devcon->UpdateSubresource(pCBuffer.Get(), 0, 0, &cBuffer, 0, 0);
	devcon->DrawIndexed(indexCount, 0, 0);

	//Present();
	
	devcon->End(pQueryEndFrame.Get());
	devcon->End(pQueryTimestampDisjoint.Get());

	while (S_OK != devcon->GetData(pQueryTimestampDisjoint.Get(), NULL, 0, 0))
	{
	}

	// Check whether timestamps were disjoint during the last frame
	D3D10_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
	devcon->GetData(pQueryTimestampDisjoint.Get(), &tsDisjoint, sizeof(tsDisjoint), 0);
	if (tsDisjoint.Disjoint)
	{
		//nothing
	}
	else
	{
		UINT64 tsBeginFrame, tsEndFrame;
		devcon->GetData(pQueryStartFrame.Get(), &tsBeginFrame, sizeof(UINT64), 0);
		devcon->GetData(pQueryEndFrame.Get(), &tsEndFrame, sizeof(UINT64), 0);
		timeFrame = float(tsEndFrame - tsBeginFrame) / float(tsDisjoint.Frequency) * 1000.0f;
	}
}


void Direct3D::InitGraphics()
{
	//create a triangle
	VERTEX OurVertices[] =
	{
		{ -1.0f,  1.0f, 0.0f,   0.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f,   1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.0f,   0.0f, 1.0f },
		{  1.0f,  1.0f, 0.0f,	1.0f, 0.0f },
	};

	//create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(OurVertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&bd, NULL, pVBuffer.GetAddressOf());

	//copy the vertice into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));
	devcon->Unmap(pVBuffer.Get(), NULL);

	DWORD OurIndices[] =
	{
		0, 1, 2,    // side 1
		0, 3, 1,
	};

	indexCount = ARRAYSIZE(OurIndices);

	//create the index buffer
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	dev->CreateBuffer(&bd, NULL, &pIBuffer);

	devcon->Map(pIBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, OurIndices, sizeof(OurIndices));
	devcon->Unmap(pIBuffer.Get(), NULL);
}

//preparing the shader
void Direct3D::InitPipeline()
{
	//load and compile the two shaders
	ID3DBlob* VS, * PS, * vsError, * psError;
	HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL,
		"main", "vs_5_0", NULL, NULL, &VS, &vsError);

	if (vsError) {
		MessageBox(NULL, L"Failed compiling vertex shader", L"Error", MB_OK);
		return;
	}

	if (origColor)
	{
		hr = D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL,
			"main", "ps_5_0", NULL, NULL, &PS, &psError);

		if (psError) {
			MessageBox(NULL, L"Failed compiling pixel shader", L"Error", MB_OK);
			return;
		}
	}
	else
	{
		hr = D3DCompileFromFile(L"PixelShader2.hlsl", NULL, NULL,
			"main", "ps_5_0", NULL, NULL, &PS, &psError);

		if (psError) {
			MessageBox(NULL, L"Failed compiling pixel shader", L"Error", MB_OK);
			return;
		}
	}



	//encapsulate shader into shader object
	dev->CreateVertexShader(VS->GetBufferPointer(),
		VS->GetBufferSize(), NULL, pVS.GetAddressOf());
	dev->CreatePixelShader(PS->GetBufferPointer(),
		PS->GetBufferSize(), NULL, pPS.GetAddressOf());

	//set the sahder object
	devcon->VSSetShader(pVS.Get(), 0, 0);
	devcon->PSSetShader(pPS.Get(), 0, 0);

	//create the input layout object;
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	dev->CreateInputLayout(ied, ARRAYSIZE(ied), VS->GetBufferPointer(), VS->GetBufferSize(), pLayout.GetAddressOf());
	devcon->IASetInputLayout(pLayout.Get());

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ied) * indexCount;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	dev->CreateBuffer(&bd, NULL, pCBuffer.GetAddressOf());
	devcon->VSSetConstantBuffers(0, 1, pCBuffer.GetAddressOf());
	devcon->PSSetConstantBuffers(0, 1, pCBuffer.GetAddressOf());
}

void Direct3D::OnResize(int w, int h)
{
	HRESULT hr;

	mandelWidth *= (double)w / width;
	mandelHeight *= (double)h / height;

	width = w;
	height = h;

	DXGI_MODE_DESC mDesc;
	mDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mDesc.Width = w;
	mDesc.Height = h;
	mDesc.RefreshRate.Denominator = 1;
	mDesc.RefreshRate.Numerator = 0;
	mDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		// unspecified scan line ordering
	mDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;

	hr = swapchain->ResizeTarget(&mDesc);

	// Release all outstanding references to the swap chain's buffers.
	backbuffer = nullptr;
	zbuffer = nullptr;

	devcon->Flush();
	devcon->ClearState();

	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	//create the depth buffer texture
	D3D11_TEXTURE2D_DESC texd;
	ZeroMemory(&texd, sizeof(texd));

	texd.Width = width;
	texd.Height = height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 4;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	
	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**)&pBuffer);
	// Perform error handling here!

	hr = dev->CreateRenderTargetView(pBuffer, NULL,
		backbuffer.GetAddressOf());
	// Perform error handling here!
	pBuffer->Release();

	devcon->OMSetRenderTargets(1, backbuffer.GetAddressOf(), zbuffer.Get());

	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	devcon->RSSetViewports(1, &vp);

	InitGraphics();
	InitPipeline();
	RenderFrame();
}

//overload without size, keep size unchanged
void Direct3D::OnResize()
{
	HRESULT hr;

	DXGI_MODE_DESC mDesc;
	mDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mDesc.Width = width;
	mDesc.Height = height;
	mDesc.RefreshRate.Denominator = 1;
	mDesc.RefreshRate.Numerator = 0;
	mDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		// unspecified scan line ordering
	mDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;

	hr = swapchain->ResizeTarget(&mDesc);

	// Release all outstanding references to the swap chain's buffers.
	backbuffer = nullptr;
	zbuffer = nullptr;

	devcon->Flush();
	devcon->ClearState();

	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	//create the depth buffer texture
	D3D11_TEXTURE2D_DESC texd;
	ZeroMemory(&texd, sizeof(texd));

	texd.Width = width;
	texd.Height = height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 4;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**)&pBuffer);
	// Perform error handling here!

	hr = dev->CreateRenderTargetView(pBuffer, NULL,
		backbuffer.GetAddressOf());
	// Perform error handling here!
	pBuffer->Release();

	devcon->OMSetRenderTargets(1, backbuffer.GetAddressOf(), zbuffer.Get());

	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	devcon->RSSetViewports(1, &vp);

	InitGraphics();
	InitPipeline();
	RenderFrame();
}

void Direct3D::Present()
{
	//switch the backbuffer and the front buffer
	swapchain->Present(0, 0);
}

void Direct3D::CenterScreen(int xPos, int yPos)
{
	int xDelta = (xPos - width / 2);
	int yDelta = (yPos - height / 2);

	realStart += mandelWidth * ((double)xDelta / width);
	imagStart += mandelHeight * ((double)yDelta / height);

	RenderFrame();
}

void Direct3D::Zoom(int xPos, int yPos, int wheelDelta)
{
	//get 1 or -1 and multiply by a number to increase the iteration
	int iDelta = (float)wheelDelta / 120;
	iterations += iDelta * 100;
	iterations = max(iterations, 100);
	
	double wFactor = mandelWidth * (double)wheelDelta / 500;
	double hFactor = mandelHeight * (double)wheelDelta / 500;
	mandelWidth -= wFactor;
	mandelHeight -= hFactor;
	realStart += wFactor / 2.0;
	imagStart += hFactor / 2.0;

	RenderFrame();
}


float Direct3D::GetScreenRatio()
{
	return (float)width / height;
}


Direct3D::~Direct3D()
{

}




//std::chrono::time_point<std::chrono::system_clock> start, end;
//start = std::chrono::system_clock::now();
//
//end = std::chrono::system_clock::now();
//std::chrono::duration<float> elapsed_seconds = end - start;
//
//std::wstringstream wss;
//wss << "Elapsed time: : ";
//wss << elapsed_seconds.count();
//wss << "\n";
//
//OutputDebugString(wss.str().c_str());
