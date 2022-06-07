#include "framework.h"
#include "viewer.hpp"

int WINAPI wWinMain(
    HINSTANCE  _In_ hInstance ,
    HINSTANCE  _In_opt_ hPrevInstance ,
    LPWSTR _In_ lpCmdLine ,
    int _In_  nCmdShow 
)
{

    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

    if (SUCCEEDED(CoInitialize(nullptr)))
    {
        {
            Viewer viewer(hInstance, L"DemoApp", L"Direct2D Sample Photo Viewer");

            if (SUCCEEDED(viewer.Initialize()))
            {
                viewer.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}