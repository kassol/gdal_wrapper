#pragma once

#ifdef  GDAL_WRAPPER_EXPORT
#define GDAL_WRAPPER_API __declspec(dllexport)
#else
#define GDAL_WRAPPER_API __declspec(dllimport)
#endif

#include "fmbase.h"
class FmJPEG :
	public FmBase
{
public:
	FmJPEG(void);
	~FmJPEG(void);

public:
	HRESULT Open(BSTR bstrPathPathName, UINT uMode);
	HRESULT CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows, UINT DataType, int nBandNum, UINT BandType, DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	HRESULT Close(void);
};

