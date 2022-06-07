#pragma once
#include <d2d1.h>

namespace PhotoViewer { // Simple Direct2D

	typedef D2D1_COLOR_F ColorF;

	class Brush {
	public:
		Brush(ID2D1HwndRenderTarget* const pD2D1HwndRenderTarget,
			const ColorF& const color = D2D1::ColorF(D2D1::ColorF::Black)) :
			pSolidColorBrush_(nullptr),
			color_(color),
			pD2D1HwndRenderTarget_(pD2D1HwndRenderTarget)
		{
			HRESULT hr = S_OK;
			hr = pD2D1HwndRenderTarget_->CreateSolidColorBrush(
				color,
				&pSolidColorBrush_
			);
			if (FAILED(hr)) { 
				throw L"ID2D1RenderTarget::CreateSolidColorBrush method failed.\n";
			}

		}

		void SetColor(const ColorF& const color) {
			color_ = color;
		}

		auto GetColor() const {
			return pSolidColorBrush_->GetColor();
		}

		ID2D1SolidColorBrush& GetBrush() const{
			return *pSolidColorBrush_;
		}

		ID2D1HwndRenderTarget& GetPRenderTarget() const {
			return *pD2D1HwndRenderTarget_;
		}

	private:

		ColorF color_;
		CComPtr<ID2D1SolidColorBrush> pSolidColorBrush_;
		CComPtr<ID2D1HwndRenderTarget> pD2D1HwndRenderTarget_;
	};

}