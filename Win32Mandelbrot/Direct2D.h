// Windows Header Files:
#include <windows.h>
#include <wrl/client.h>
#include <sstream>
#include <d2d1.h>
#include <dwrite.h>

#include "Direct3D.h"

#pragma comment (lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

using namespace Microsoft::WRL;

class Direct2D
{
public:
    Direct2D(HWND hWindow, std::shared_ptr<Direct3D> D3D);
    ~Direct2D();

    //draw content
    void OnPaint();
    void Clear();

    //resize the render target
    void OnResize();

private:
    HWND hWnd;
    int width, height;

    std::shared_ptr<Direct3D> D3D;

    //Direct2D stuff
    ComPtr<ID2D1Factory> pFactory;
    ComPtr<IDXGISwapChain> swapchain;			
    ComPtr<ID2D1HwndRenderTarget> pRenderTarget;
    ComPtr<IDXGISurface> pBackBuffer2d;
    ComPtr<ID2D1SolidColorBrush> pBrush;
    ComPtr<ID2D1RenderTarget> bbOverlay;

    //write stuff
    ComPtr<ID2D1SolidColorBrush> pITextBrush;
    ComPtr<IDWriteTextFormat> pITextFormat;
    ComPtr<IDWriteFactory> pIDWriteFactory;

    //initialize device-dependent resources
    HRESULT Init2D();
    HRESULT InitWrite();

};


