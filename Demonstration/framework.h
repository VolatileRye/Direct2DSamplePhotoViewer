#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Resource File


// Windows/DirectX or Microsoft's Header Files
#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wincodecsdk.h>

#include <atlbase.h>
#include <strsafe.h>

#include <wrl.h>
using namespace Microsoft::WRL;
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")

// C/C++ Header Files
#include <wchar.h>
#include <cmath>
#include <array>
#include <string>
#include <iterator>
#include <format>

// Need the Following Library Files.
//#pragma comment (lib, "d2d1.lib")
//#pragma comment (lib, "Dwrite.lib")
//#pragma comment (lib, "windowscodecs.lib")
#pragma comment (lib, "Comdlg32.lib")

