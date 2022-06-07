#pragma once
#include "text.hpp"
#include "resource.h"
#include "framework.h"
#include "image.hpp"

//constexpr int maxLoadString = 100;


class Viewer
{
public:
	Viewer(const HINSTANCE& hInstance, const std::wstring& szWindowClass, const std::wstring& szTitle);

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();

	// Process and dispatch messages
	void RunMessageLoop();


private:
	// --- Initializer ---
	// Initialize device-independent resources.
	HRESULT CreateDeviceIndependentResources();
	// Initialize device-dependent resources.
	HRESULT CreateDeviceResources(const HWND& const hWnd);

	// --- Procedures ---
	// Draw content.
	HRESULT OnRender(const HWND& const hWnd, const LPARAM& const lParam);
	void GridRender();
	void ImageRender(const LPARAM& lparem);
	void TextRender();

	// Resize the render target.
	void OnResize(const LPARAM& const lParam);

	// --- Image Processing ---
	void ModifyScale(const WPARAM& const wParam);

	// The windows procedure.
	virtual LRESULT WndProc(
		const HWND& const hWnd,
		const UINT& const message,
		const WPARAM& const wParam,
		const LPARAM& const lParam);

	static LRESULT StaticWndProc(
		const HWND hWnd,
		const UINT message,
		const WPARAM wParam,
		const LPARAM lParam);


	//const int m_maxLoadString = 100;
	PhotoViewer::Image m_image;

	// Render
	D2D1_SIZE_F m_rtSize; // Render Target Size
	D2D_POINT_2F m_cePoint; // Center Point of the Render Target Rect
	D2D1_SIZE_F m_bSize; // Original Bitmap Size (width, height)

	// Mouse
	POINT m_prevCursorPoint;
	bool m_lButton;
	bool m_rButton;

	//MenuOption
	bool m_enableGrid;
	bool m_enableText;
	bool m_enableDarkMode;


	// Window
	const HINSTANCE m_hInst;
	const WCHAR m_szWindowClass[100];
	WCHAR m_szTitle[100];

	// Direct2D
	CComPtr<ID2D1Factory> m_pD2D1Factory;
	CComPtr<ID2D1HwndRenderTarget> m_pD2D1HwndRenderTarget;

};