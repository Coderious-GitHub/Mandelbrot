#pragma once

#include <stdio.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <WICTextureLoader.h>
#include <wrl/client.h>


//include the Direct3D Library File
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;

class Direct3D
{
private:
	//global declaration
	ComPtr<IDXGISwapChain> swapchain;			//the pointer to the swapchain
	ComPtr<ID3D11Device> dev;					//the pointer to our Direct3D device interface
	ComPtr<ID3D11DeviceContext> devcon;			//the pointer to our Direct3D device context
	ComPtr<ID3D11RenderTargetView> backbuffer;	//the pointer to our backbuffer
	ComPtr<ID3D11DepthStencilView> zbuffer;		//the pointer to our depth buffer
	ComPtr<ID3D11InputLayout> pLayout;			//the pointer to the input layout
	ComPtr<ID3D11VertexShader> pVS;				//the pointer to the vertex shader
	ComPtr<ID3D11PixelShader> pPS;				//the pointer to the pixel shader
	ComPtr<ID3D11Buffer> pVBuffer;				//the pointer to the vertex buffer
	ComPtr<ID3D11Buffer> pIBuffer;				//the pointer to the index buffer	
	ComPtr<ID3D11Buffer> pCBuffer;				//the pointer to the constant buffer

	float Time = 0.0f;
	int indexCount = 0;


	//single vertex struct
	struct VERTEX
	{
		float X, Y, Z;
		float U, V;
	};

	// a struct to represent the constant buffers
	struct CBUFFER
	{
		double rStart, iStart;
		double width, height;
		float screenWidth, screenHeight;
		int maxIt;
	};

public:
	Direct3D(HWND hWnd, int width, int height);
	Direct3D();
	~Direct3D();

	ComPtr<IDXGISwapChain> GetSwapChain() { return swapchain; };
	void Present(); //present the swapchain
	void RenderFrame(); //render a single frame
	void OnResize(int width, int height);
	void OnResize();
	void CenterScreen(int xDelta, int yDelta);
	void Zoom(int xPos, int yPos, int wheelDelta);

	//function prototypes
	void InitD3D();
	void InitGraphics();		//creates the shape to render
	void InitPipeline();		//loads and prepare the shaders

	float GetScreenRatio();

	int height, width;
	double realStart, imagStart;
	double mandelWidth, mandelHeight;
	float startRatio, currentRatio;
	int iterations;
	float timeFrame;
	bool origColor;

	HWND hWnd;
	BOOL isFullScreen;
};




