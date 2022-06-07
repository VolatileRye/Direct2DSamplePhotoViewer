#include "viewer.hpp"
#include <commdlg.h>
namespace {
	const std::wstring HowToControlString =
		L"Direct2D Sample Photo Viewer\n"
		//L"volatile rye (2021-)\n\n"
		L"------- 🖱️ MOUSE CONTROL --------\n"
		L"  Left\t\tMove\t(UNSTABLE)\n"
		L"  Right\tRotate\t(NOT IMPLEMENTED)\n"
		L"  Middle\tReset\n"
		L"  Wheel\tZoom in/out\n\n"
		L"------ ⌨️ KEYBOARD CONTROL ------\n"
		L"  WASD\t\tMove\n"
		L"  Left/Right\tRotate\n"
		L"  Up/Down\tZoom in/out\n"
		L"  R\t\tReset\n"
		L"  O\t\tOpen a image file...\n\n"
		L"  G\t\tGrid\n"
		L"  M\t\tDark mode\n"
		L"  L\t\tLinear interpolation\n"
		L"  C\t\tControl Guide\n"
		L"  F4\t\tFullscreen\t(NOT IMPLEMENTED)\n"
		L"  Esc\t\tExit\n\n"
		L"------- ⌨️ OMAKE CONTROL --------\n"
		L"  ?\t\t???\n\n"
		L"---------------------------------"
		;
}

Viewer::Viewer(const HINSTANCE& hInstance, const std::wstring& szWindowClass, const std::wstring& szTitle) :
	m_prevCursorPoint({ 0,0 }),
	m_lButton(false),
	m_rButton(false),
	m_enableGrid(true),
	m_enableText(true),
	m_enableDarkMode(true),
	m_hInst(hInstance), // Window
	m_szWindowClass(),
	m_szTitle(),
	m_pD2D1Factory(nullptr), // Direct2D
	m_pD2D1HwndRenderTarget(nullptr),
	m_image(m_pD2D1HwndRenderTarget, L"./sample.tiff")
{
	StringCchCatW(const_cast<WCHAR*>(m_szWindowClass), 100, szTitle.c_str());
	StringCchCatW(m_szTitle, 100, szTitle.c_str());
}

void Viewer::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT Viewer::Initialize()
{

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	HRESULT hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Viewer::StaticWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = m_hInst;
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr; // MAKEINTRESOURCE(IDR_MENU1);
		wcex.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
		wcex.lpszClassName = m_szWindowClass;

		RegisterClassEx(&wcex);

		// Create the window.
		const HWND hWnd = CreateWindow(
			//WS_EX_NOREDIRECTIONBITMAP,
			m_szWindowClass,
			m_szTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			m_hInst,
			this
		);
		hr = hWnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(hWnd, SW_SHOWNORMAL);
			UpdateWindow(hWnd);
		}
	}

	return hr;
}

HRESULT Viewer::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2D1Factory);
	if (FAILED(hr)) { return hr; }

	return hr;
}

HRESULT Viewer::CreateDeviceResources(const HWND& const hWnd)
{
	HRESULT hr = S_OK;

	if (!m_pD2D1HwndRenderTarget)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		const D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// Direct2D
		hr = m_pD2D1Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&m_pD2D1HwndRenderTarget
		);

		// m_image
		m_image.Load(m_pD2D1HwndRenderTarget);

		if (FAILED(hr)) { return hr; }
	}


	return hr;
}

BOOL _stdcall DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	UINT wmId;

	switch (message) {
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDOK:
			EndDialog(hWnd, IDOK);
			return TRUE;
		}
	case WM_CLOSE:
		EndDialog(hWnd, IDOK);
		return TRUE;
	}
	return FALSE;
}

LRESULT Viewer::WndProc(
	const HWND& const hWnd,
	const UINT& const message,
	const WPARAM& const wParam,
	const LPARAM& const lParam)
{
	Viewer* pViewer = reinterpret_cast<Viewer*>(static_cast<LONG_PTR>(
		::GetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA
		)));

	bool wasHandled = false;
	static OPENFILENAME oFileName;
	WCHAR path[256] = L"";

	if (pViewer)
	{
		switch (message)
		{
			// Window events
		case WM_SIZE:
		{
			pViewer->OnResize(lParam);
		}
		break;
		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		case WM_PAINT:
		{
			pViewer->OnRender(hWnd, lParam);
		}
		break;

		// Mouse events
		case WM_MOUSEMOVE:
		{
			POINT nowCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if (pViewer->m_lButton == true) {
				//pViewer
			}
			if (pViewer->m_rButton == true) {
			}
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		case WM_LBUTTONDOWN:
		{
			pViewer->m_lButton = true;

			POINT nowCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_image.m_translation.width += static_cast<float>(nowCursorPoint.x - pViewer->m_prevCursorPoint.x);
			m_image.m_translation.height += static_cast<float>(nowCursorPoint.y - pViewer->m_prevCursorPoint.y);
			pViewer->m_prevCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		case WM_LBUTTONUP:
		{
			pViewer->m_lButton = true;
			POINT nowCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			m_image.m_translation.width += static_cast<float>(nowCursorPoint.x - pViewer->m_prevCursorPoint.x);
			m_image.m_translation.height += static_cast<float>(nowCursorPoint.y - pViewer->m_prevCursorPoint.y);
			pViewer->m_prevCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		case WM_RBUTTONDOWN:
		{
			pViewer->m_rButton = true;
			pViewer->m_prevCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		}
		break;
		case WM_RBUTTONUP:
		{
			pViewer->m_rButton = false;
			POINT nowCursorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		}
		break;
		case WM_MBUTTONDOWN:
		{
			m_image.m_scale = 1.0f;
			m_image.m_rotate = 0.0f;
			m_image.m_translation = { 0.0f,0.0f };
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			m_image.m_scale += 0.001f * GET_WHEEL_DELTA_WPARAM(wParam);
			if (m_image.m_scale < 0.05) { m_image.m_scale = 0.05f; }
			if (m_image.m_scale > 20) { m_image.m_scale = 20.0f; }
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		
		// Keyboard Events
		case WM_CHAR:
		{
			switch (wParam) {
			case 'w':
				m_image.m_translation.height -= 10.0f;
				break;
			case 'a':
				m_image.m_translation.width -= 10.0f;
				break;
			case 's':
				m_image.m_translation.height += 10.0f;
				break;
			case 'd':
				m_image.m_translation.width += 10.0f;
				break;
			case 'r':
				m_image.m_scale = 1.0f;
				m_image.m_rotate = 0.0f;
				m_image.m_translation = { 0.0f,0.0f };
				break;
			case 'o':
				oFileName.lStructSize = sizeof(OPENFILENAME);
				oFileName.hwndOwner = hWnd;
				oFileName.lpstrInitialDir = L".";       // 初期フォルダ位置
				oFileName.lpstrFile = path;       // 選択ファイル格納
				oFileName.nMaxFile = 256;
				oFileName.lpstrFilter = L"Bitmap (*.bmp,*.dib)\0*.bmp;*.dib\0"
					L"JPEG (*.jpg;*.jpeg;*.jpe;*.jfif)\0*.jpg;*.jpeg;*.jpe;*.jfif\0"
					L"GIF (*.gif)\0*.gif\0"
					L"TIFF (*.tif;*.tiff)\0*.tif;*.tiff\0"
					L"PNG (*.png)\0*.png\0"
					L"HEIC (*.heic;*.hif)\0*.heic;*.hif\0"
					L"All files (*.*)\0*.bmp;*.dib;*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.tif;*.tiff;*.png;*.heic;*.hif\0\0";
				oFileName.nFilterIndex = 7; // All files
				oFileName.lpstrTitle = L"Choose a image file";
				oFileName.Flags = OFN_FILEMUSTEXIST;
				
				GetOpenFileName(&oFileName);
				if (oFileName.lpstrFile[0]) {
					m_image.Open(path);
				}
				break;
			case 'g':
				m_enableGrid = !m_enableGrid;
				break;
			case 'l':
				m_image.enableLinear_ = !m_image.enableLinear_;
				break;
			case 'c':
				m_enableText = !m_enableText;
				break;
			case 'm':
				m_enableDarkMode = !m_enableDarkMode;
				break;
			}
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
		case WM_KEYDOWN:
		{
			switch (wParam){
			case VK_UP:
				m_image.m_scale *= 1.25f;
				if (m_image.m_scale < 0.05) { m_image.m_scale = 0.05f; }
				if (m_image.m_scale > 20) { m_image.m_scale = 20.0f; }
				break;
			case VK_DOWN:
				m_image.m_scale *= 0.80f;
				if (m_image.m_scale < 0.05) { m_image.m_scale = 0.05f; }
				if (m_image.m_scale > 20) { m_image.m_scale = 20.0f; }
				break;
			case VK_LEFT:
				m_image.m_rotate -= 5;
				break;
			case VK_RIGHT:
				m_image.m_rotate += 5;
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);
				return 0;
			}
			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;

		// End & Default
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
		default: {
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		}
		wasHandled = true;
	}

	if (!wasHandled)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT _stdcall Viewer::StaticWndProc(
	const HWND hWnd,
	const UINT message,
	const WPARAM wParam,
	const LPARAM lParam)
{
	Viewer* This = (Viewer*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!This) {//取得できなかった(ウィンドウ生成中)場合
		if (message == WM_CREATE) {
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			Viewer* pViewer = (Viewer*)pcs->lpCreateParams;

			::SetWindowLongPtrW(
				hWnd,
				GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(pViewer)
			);

			This = (Viewer*)((LPCREATESTRUCT)lParam)->lpCreateParams;
			if (This) {
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)This);
				return This->WndProc(hWnd, message, wParam, lParam);
			}
		}
	}
	else {//取得できた場合(ウィンドウ生成後)
		return This->WndProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT Viewer::OnRender(const HWND& const hWnd, const LPARAM& const lParam)
{
	HRESULT hr = CreateDeviceResources(hWnd);
	if (FAILED(hr)) { return hr; }

	m_pD2D1HwndRenderTarget->BeginDraw();
	m_pD2D1HwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	if (m_enableDarkMode == true) {
		m_pD2D1HwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	}
	else {
		m_pD2D1HwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}

	if(m_enableGrid == true) GridRender();
	ImageRender(lParam);
	if(m_enableText == true) TextRender();

	hr = m_pD2D1HwndRenderTarget->EndDraw();
	if (FAILED(hr)) { return hr; }

	const auto linear = m_image.enableLinear_ == true ? L"on" : L"off";
	SetWindowText(hWnd, std::format(L"Linear is {} | Scale: {}% | Rotate: {}° | Trans: {}, {}", linear, 100 * m_image.m_scale, m_image.m_rotate, m_image.m_translation.width, m_image.m_translation.height).c_str());
	ValidateRect(hWnd, nullptr);

	return hr;
}

void Viewer::GridRender() {

	static PhotoViewer::Brush brush(m_pD2D1HwndRenderTarget, D2D1::ColorF(D2D1::ColorF::Gray));

	m_rtSize = m_pD2D1HwndRenderTarget->GetSize();
	m_cePoint = D2D1::Point2F(m_rtSize.width / 2.0f, m_rtSize.height / 2.0f);

	for (int x = 0; x <= static_cast<int>(m_rtSize.width / 2.0f); x += 10)
	{
		m_pD2D1HwndRenderTarget->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(m_cePoint.x - x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(m_cePoint.x - x), m_rtSize.height),
			&brush.GetBrush(),
			0.5f
		);
		m_pD2D1HwndRenderTarget->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(m_cePoint.x + x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(m_cePoint.x + x), m_rtSize.height),
			&brush.GetBrush(),
			0.5f
		);
	}

	for (int y = 0; y < static_cast<int>(m_rtSize.height / 2.0f); y += 10)
	{
		m_pD2D1HwndRenderTarget->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(m_cePoint.y - y)),
			D2D1::Point2F(m_rtSize.width, static_cast<FLOAT>(m_cePoint.y - y)),
			&brush.GetBrush(),
			0.5f
		);

		m_pD2D1HwndRenderTarget->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(m_cePoint.y + y)),
			D2D1::Point2F(m_rtSize.width, static_cast<FLOAT>(m_cePoint.y + y)),
			&brush.GetBrush(),
			0.5f
		);
	}
}

void Viewer::ImageRender(const LPARAM& lParam) {
	m_image.Draw();
}

void Viewer::TextRender() {

	static PhotoViewer::Brush brush(m_pD2D1HwndRenderTarget, D2D1::ColorF(D2D1::ColorF::LightSlateGray));

	static const auto darkModeColor = PhotoViewer::Brush(m_pD2D1HwndRenderTarget, D2D1::ColorF(D2D1::ColorF::LightSlateGray)).GetColor();
	const auto currentColor = brush.GetColor();

	if (m_enableDarkMode == true) {
		if (&currentColor != &darkModeColor) {
			brush.SetColor(D2D1::ColorF(D2D1::ColorF::LightSlateGray));
		}
	}
	else {
		if (&currentColor == &darkModeColor) {
			brush.SetColor(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));
		}
	}

	const std::wstring string = ::HowToControlString;
	const wchar_t* const string2 = L"http://sipi.usc.edu/database/preview/misc/house.png";

	static PhotoViewer::Text text(string, brush, 15.0f, L"Cascadia Code");
	text.Draw();
}



void Viewer::OnResize(const LPARAM& const lParam)
{
	if (m_pD2D1HwndRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.

		const unsigned int width = LOWORD(lParam);
		const unsigned int height = HIWORD(lParam);
		m_pD2D1HwndRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}