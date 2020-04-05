#pragma once

// Windows header
#include <Windows.h>
#include <Windowsx.h>
#include "resource.h"
#include "Direct3D.h"
#include "Direct2D.h"


class Window
{
private:
	//window class singleton
	class WindowClass
	{
	public:
		static LPCWSTR GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		static WindowClass wndClass;
		HINSTANCE hInst;
		static constexpr LPCWSTR className = L"WindowClass";

		//load icon
		HICON mbIcon = static_cast<HICON>(LoadImage(GetInstance(),
			MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0));

		WindowClass() noexcept;
		~WindowClass();
	};

private:
	HWND hWnd;

public:
	//constructor and destruction
	Window(int width, int height, LPCWSTR name);
	~Window();

	std::shared_ptr<Direct3D> D3D;
	std::shared_ptr<Direct2D> D2D;

	//initialize external components
	void Run();

	//the callback function
	static LRESULT CALLBACK msgProc(HWND hWnd, UINT msgm, WPARAM wParam, LPARAM lParam);

};
