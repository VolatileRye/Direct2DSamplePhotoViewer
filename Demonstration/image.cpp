#include "image.hpp"

namespace PhotoViewer{
IWICImagingFactory* Image::pWICImagingFactory_ = nullptr;
IWICBitmapDecoder* Image::pWICBitmapDecoder_ = nullptr;
IWICBitmapFrameDecode* Image::pWICBitmapFrameDecode_ = nullptr;
IWICFormatConverter* Image::pWICFormatConverter_ = nullptr;
}