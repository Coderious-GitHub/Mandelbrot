#include "Window.h"
#include "resource.h"

LRESULT CALLBACK Window::msgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pThis;

	if (msg == WM_NCCREATE)
	{
		pThis = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		SetLastError(0);
		if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis)
	{
		switch (msg)
		{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				int width = LOWORD(lParam);  // Macro to get the low-order word.
				int height = HIWORD(lParam); // Macro to get the high-order word.

				pThis->D2D->Clear();

				pThis->D3D->OnResize(width, height);
				pThis->D2D->OnResize();
				pThis->D2D->OnPaint();
				pThis->D3D->Present();
			}

			break;
		}
		case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			pThis->D3D->CenterScreen(xPos, yPos);
			pThis->D2D->OnPaint();
			pThis->D3D->Present();

			break;
		}
		case WM_MOUSEWHEEL:
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);
			int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			pThis->D3D->Zoom(xPos, yPos, wheelDelta);
			pThis->D2D->OnPaint();
			pThis->D3D->Present();
			break;
		}
		case WM_KEYDOWN:
			switch (wParam)
			{
			case 0x43:
				bool temp = pThis->D3D->origColor;
				pThis->D3D->origColor = !temp;

				pThis->D2D->Clear();

				pThis->D3D->OnResize();
				pThis->D2D->OnResize();
				pThis->D2D->OnPaint();
				pThis->D3D->Present();
			}
			break;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
};

//window class 
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = msgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = wbIcon;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = wbIcon;
	RegisterClassEx(&wc);

};

Window::WindowClass::~WindowClass()
{
	UnregisterClass(className, GetInstance());
};

LPCWSTR Window::WindowClass::GetName() noexcept
{
	return className;
};

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


//window stuff
Window::Window(int width, int height, LPCWSTR name) {
	//calculate window size based on desired client size
	RECT wr = { 0, 0, width, height };	//set the size but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);  //adjust the size

		//create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		WindowClass::GetName(),			//name of the window class
		name,							//title of the window
		WS_OVERLAPPEDWINDOW,			//window style
		CW_USEDEFAULT,					//x-position of the window
		CW_USEDEFAULT,					//y-position of the window
		wr.right - wr.left,				//width
		wr.bottom - wr.top,				//height
		nullptr,						//no parent window
		nullptr,						//no menu
		WindowClass::GetInstance(),		//application handle
		this);							//used for passing on information in msgProc

	D3D = std::shared_ptr<Direct3D>(new Direct3D(hWnd, width, height));
	D2D = std::shared_ptr<Direct2D>(new Direct2D(hWnd, D3D));

	//display the window on the screen
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
}

void Window::Run()
{
	//D3D->RenderFrame();
	//D2D->OnPaint();
}


Window::~Window()
{
	DestroyWindow(hWnd);
};
