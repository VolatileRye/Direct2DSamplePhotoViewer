#pragma once
#include "utility.hpp"
#include "framework.h"

namespace PhotoViewer { // Simple Direct2D


	class Image {
	public:
		Image(ID2D1HwndRenderTarget* const pD2D1HwndRenderTarget,
			const std::wstring& const filename)
			: filename_(filename),
			m_scale(1.0f),
			m_rotate(0.0f),
			m_translation({ 0.0f,0.0f }),
			enableLinear_(true),
			pD2D1HwndRenderTarget_(nullptr),
			pD2D1Bitmap_(nullptr)

		{
			HRESULT hr = S_OK;
		}

		void Load() {
			Load(pD2D1HwndRenderTarget_);
		}

		void Load(ID2D1HwndRenderTarget* const pD2D1HwndRenderTarget) {
			HRESULT hr = S_OK;

			static bool firstInit = false;

			if (firstInit == false) {
				if (pD2D1HwndRenderTarget_ == nullptr) {
					if (pD2D1HwndRenderTarget == nullptr) throw L"D2D1HwndRenderTarget is null pointer";
					pD2D1HwndRenderTarget_ = pD2D1HwndRenderTarget;
				}


				hr = ::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<LPVOID*>(&pWICImagingFactory_));
				if (FAILED(hr)) { throw L"CoCreateInstance function failed.\n"; }
				firstInit = true;
			}

			hr = pWICImagingFactory_->CreateDecoderFromFilename(filename_.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pWICBitmapDecoder_);

			if (FAILED(hr)) {
				MessageBox(nullptr, L"Failed to read a file.", filename_.c_str(), MB_OK);
				return;
			}

			hr = pWICBitmapDecoder_->GetFrame(0, &pWICBitmapFrameDecode_);
			if (FAILED(hr)) { throw L"IWICBitmapDecoder::GetFrame method failed.\n"; }

			hr = pWICImagingFactory_->CreateFormatConverter(&pWICFormatConverter_);
			if (FAILED(hr)) { throw L"IWICImagingFactory::CreateFormatConverter method failed.\n"; }

			hr = pWICFormatConverter_->Initialize(pWICBitmapFrameDecode_, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 1.0f, WICBitmapPaletteTypeMedianCut);
			if (FAILED(hr)) { throw L"IWICFormatConverter::Initialize method failed.\n"; }

			hr = pD2D1HwndRenderTarget_->CreateBitmapFromWicBitmap(pWICFormatConverter_, nullptr, &pD2D1Bitmap_);
			if (FAILED(hr)) { throw L"ID2D1RenderTarget::CreateBitmapFromWicBitmap failed.\n"; }
		}
				
		void Open(const std::wstring& const filename) {
			filename_ = filename;
			Load(this->pD2D1HwndRenderTarget_);
			m_scale = 1.0f;
			m_rotate = 0.0f;
			m_translation = { 0.0f,0.0f };
		}

		const std::wstring& filename()const { return filename_; }

		float scale() const { return m_scale; }
		float rotate() const { return m_rotate; }
		PhotoViewer::SizeF translation() const { return m_translation; }

		void scale(const float& scale) { m_scale = scale; }
		void rotate(const float& rotate) { m_rotate = rotate; }
		void translation(const PhotoViewer::SizeF& translation) { m_translation = translation; }

		bool Draw() {
			// If you failed to read a image file
			if (pD2D1Bitmap_ == nullptr) return false;

			const PhotoViewer::SizeF rtSize = pD2D1HwndRenderTarget_->GetSize();
			const PhotoViewer::SizeF bSize = pD2D1Bitmap_->GetSize();

			const D2D_POINT_2F cePoint = D2D1::Point2F(rtSize.width / 2.0f, rtSize.height / 2.0f);

			const PhotoViewer::RectF drawRect = D2D1::RectF(
				cePoint.x - bSize.width / 2.0f,  // left
				cePoint.y - bSize.height / 2.0f, // top
				cePoint.x + bSize.width / 2.0f,   // right
				cePoint.y + bSize.height / 2.0f // bottom
			);

			PhotoViewer::Matrix3x2F matrix(PhotoViewer::Matrix3x2F::Identity());
			matrix = matrix * PhotoViewer::Matrix3x2F::Scale({ m_scale, m_scale }, cePoint);
			matrix = matrix * PhotoViewer::Matrix3x2F::Rotation(m_rotate, cePoint);
			matrix = matrix * PhotoViewer::Matrix3x2F::Translation(m_translation);
			pD2D1HwndRenderTarget_->SetTransform(PhotoViewer::Matrix3x2F(matrix));

			if (enableLinear_ == true) {
				pD2D1HwndRenderTarget_->DrawBitmap(pD2D1Bitmap_, drawRect, 1.0f,
					D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
			}
			else {
				pD2D1HwndRenderTarget_->DrawBitmap(pD2D1Bitmap_, drawRect, 1.0f,
					D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}

			return true;
		}

	public:
		// Affine Rransform
		float m_scale;
		float m_rotate;
		PhotoViewer::SizeF m_translation;
		bool enableLinear_;

	private:
		// Image Filename
		std::wstring filename_;

		ID2D1HwndRenderTarget* pD2D1HwndRenderTarget_;
		ID2D1Bitmap* pD2D1Bitmap_;

		static IWICImagingFactory* pWICImagingFactory_;
		static IWICBitmapDecoder* pWICBitmapDecoder_;
		static IWICBitmapFrameDecode* pWICBitmapFrameDecode_;
		static IWICFormatConverter* pWICFormatConverter_;
	};
}