#pragma once
#include "gdal_wrapper.h"

class TiffDriver :
	public ImageDriver
{
public:
	TiffDriver();
	~TiffDriver();

	HRESULT Open(BSTR bstrPathPathName, UINT uMode);
	HRESULT CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows, UINT DataType, int nBandNum, UINT BandType, DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	HRESULT SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);

protected:
	void InitGeoInfo();
};

