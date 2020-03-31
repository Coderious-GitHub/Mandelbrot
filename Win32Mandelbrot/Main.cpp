//include the basic windows header file
#include <stdio.h>
#include <Windows.h>
#include <exception>
#include <iostream>
#include <string>

#include "Window.h"

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    PWSTR pCmdLine,
    int nCmdShow)
{
    try {
        //Create the window
        Window wnd(800, 800, L"Mandelbrot");

        // Main message loop:
        MSG msg;
        while (true)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                //translate keystroke messages into the right format
                TranslateMessage(&msg);

                //send the message to the WindowProc function
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT)
                    break;
            }
            else
            {
                wnd.Run();
            }

        }
    }
    catch (...)
    {
        std::cout << "Damn";
    }

}
