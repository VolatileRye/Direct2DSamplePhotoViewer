#ifndef UNICODE
#define UNICODE
#endif 
#include <windows.h>
#include <wrl.h>
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_1.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <wincodec.h>
#include <magnification.h>

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "magnification")

using namespace Microsoft::WRL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ComPtr<ID3D11Device> direct3dDevice;
ComPtr<IDXGIDevice> dxgiDevice;
ComPtr<IDXGIFactory2> dxFactory;
ComPtr<IDXGISwapChain1> swapChain;
ComPtr<ID2D1Factory2> d2Factory;
ComPtr<ID2D1Device1> d2Device;
ComPtr<IDCompositionTarget> target;
ComPtr<ID2D1DeviceContext> dc;


//Direct Composition Device,Visual
ComPtr<IDCompositionDevice> dcompDevice;
ComPtr<IDCompositionDevice3> dcompDevice3;
ComPtr<IDCompositionVisual> visual;

//Direct Composition Effects
ComPtr<IDCompositionGaussianBlurEffect> blur;
ComPtr<IDCompositionSaturationEffect> saturation;

IWICImagingFactory* d2dWICFactory = NULL;
IWICBitmapDecoder* d2dDecoder = NULL;
IWICFormatConverter* d2dConverter = NULL;
IWICBitmapFrameDecode* d2dBmpSrc = NULL;
ID2D1Bitmap* d2dBmp = NULL;

HWND hwnd;
RECT windowRect, temp;
RECT* hostRect;

void LoadBackground();
void Render();

struct ComException
{
    HRESULT result;
    ComException(HRESULT const value) : result(value)
    {}
};
void HR(HRESULT const result)
{
    if (S_OK != result)
    {
        throw ComException(result);
    }
}

void CreateDevice(HWND hwnd)
{

    HR(D3D11CreateDevice(nullptr,    // Adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,    // Module
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, // Highest available feature level
        D3D11_SDK_VERSION,
        &direct3dDevice,
        nullptr,    // Actual feature level
        nullptr));  // Device context

    HR(direct3dDevice.As(&dxgiDevice));
    HR(CreateDXGIFactory2(
        DXGI_CREATE_FACTORY_DEBUG,
        __uuidof(dxFactory),
        reinterpret_cast<void**>(dxFactory.GetAddressOf())));

    DXGI_SWAP_CHAIN_DESC1 description = {};

    description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    description.BufferCount = 2;
    description.SampleDesc.Count = 1;
    description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    RECT rect = {};
    GetClientRect(hwnd, &rect);
    description.Width = rect.right - rect.left;
    description.Height = rect.bottom - rect.top;

    HR(dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(), &description, nullptr, swapChain.GetAddressOf()));

    D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };

    HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, d2Factory.GetAddressOf()));

    HR(d2Factory->CreateDevice(dxgiDevice.Get(), d2Device.GetAddressOf()));

    HR(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, dc.GetAddressOf()));

    ComPtr<IDXGISurface2> surface;
    HR(swapChain->GetBuffer(0, __uuidof(surface), reinterpret_cast<void**>(surface.GetAddressOf())));

    D2D1_BITMAP_PROPERTIES1 properties = {};
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    ComPtr<ID2D1Bitmap1> bitmap;
    HR(dc->CreateBitmapFromDxgiSurface(surface.Get(), properties, bitmap.GetAddressOf()));
    LoadBackground(); // loads my desktop background to d2dBmp using WIC
    dc->SetTarget(bitmap.Get());

    Render(); //render once

   //Creating Direct Compostion Devices and Visual

    HR(DCompositionCreateDevice(dxgiDevice.Get(), __uuidof(dcompDevice), reinterpret_cast<void**>(dcompDevice.GetAddressOf())));
    HR(DCompositionCreateDevice3(dxgiDevice.Get(), __uuidof(dcompDevice), reinterpret_cast<void**>(dcompDevice.GetAddressOf())));
    HR(dcompDevice->QueryInterface(__uuidof(IDCompositionDevice3), (LPVOID*)&dcompDevice3)); // use IDCompositionDevice3 here

    HR(dcompDevice3->CreateSaturationEffect(saturation.GetAddressOf()));
    HR(dcompDevice3->CreateGaussianBlurEffect(blur.GetAddressOf()));

    //setting effect properties
    blur->SetStandardDeviation(0.0f); // blur amount
    blur->SetBorderMode(D2D1_BORDER_MODE_HARD);
    saturation->SetSaturation(2.0f); //saturationamount

    HR(dcompDevice->CreateTargetForHwnd(hwnd, true, target.GetAddressOf()));

    blur->SetInput(NULL, bitmap.Get(), NULL);
    saturation->SetInput(NULL, blur.Get(), NULL);

    HR(dcompDevice->CreateVisual(visual.GetAddressOf()));
    HR(visual->SetContent(swapChain.Get()));
    visual->SetEffect(saturation.Get());
    HR(target->SetRoot(visual.Get()));
    HR(dcompDevice->Commit());

}

void LoadBackground()
{
    HR(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)(&d2dWICFactory)));
    HR(d2dWICFactory->CreateDecoderFromFilename(L"Untitled.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &d2dDecoder));
    HR(d2dWICFactory->CreateFormatConverter(&d2dConverter));
    HR(d2dDecoder->GetFrame(0, &d2dBmpSrc));
    HR(d2dConverter->Initialize(d2dBmpSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut));
    HR(dc->CreateBitmapFromWicBitmap(d2dConverter, NULL, &d2dBmp));
}

void Render()
{
    if (dc)
    {
        dc->BeginDraw();
        dc->Clear();
        if (hostRect)
        {
            D2D1_POINT_2F offset = D2D1::Point2F(0, 0);
            D2D1_RECT_F imgRect = D2D1::RectF(hostRect->left, hostRect->top, hostRect->right, hostRect->bottom);
            dc->DrawImage(d2dBmp, offset, imgRect, D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
        }
        HR(dc->EndDraw());
        HR(swapChain->Present(1, 0));
    }

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    hwnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName, L"Sample",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 500,
        nullptr, nullptr, hInstance, nullptr);

    if (hwnd == NULL)
    {
        return 0;
    }

    CreateDevice(hwnd);
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_MOVING:
    {
        hostRect = reinterpret_cast<RECT*>(lParam);
        windowRect.left = hostRect->left;
        windowRect.top = hostRect->top;
        windowRect.right = hostRect->right;
        windowRect.bottom = hostRect->bottom;
        if (hostRect->left == 0 || hostRect->top == 0)
        {
            GetWindowRect(hwnd, &temp);
            hostRect->left = temp.left;
            hostRect->top = temp.top;
        }
        DwmFlush();
        Render();
        return 0;
    }


    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        d2dWICFactory->Release();
        d2dDecoder->Release();
        d2dConverter->Release();
        d2dBmpSrc->Release();
        d2dBmp->Release();
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}