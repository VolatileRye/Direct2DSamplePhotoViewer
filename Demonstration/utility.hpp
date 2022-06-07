#pragma once
#include "framework.h"
#include <fstream>
namespace PhotoViewer { // Simple Direct2D

	typedef D2D1_POINT_2F PointF;
	typedef D2D1_SIZE_F SizeF;
	typedef D2D1_SIZE_U SizeU;
	typedef D2D1_RECT_F RectF;
	typedef D2D1::Matrix3x2F Matrix3x2F;

	// TODO : Create a DumpFile.
	//void malfunctionAbort(const std::wstring& errorString) {
	//		OutputDebugString(errorString.c_str());
	//		std::wofstream ofs(L"CrashReport.log");
	//		ofs << errorString.c_str() << std::endl;
	//		throw errorString.c_str();
	//}
}