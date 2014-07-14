#ifndef GDAL_WRAPPER_H_
#define GDAL_WRAPPER_H_

#ifdef  GDAL_WRAPPER_EXPORT
#define GDAL_WRAPPER_API __declspec(dllexport)
#else
#define GDAL_WRAPPER_API __declspec(dllimport)
#endif

#include "FmBase.h"

class GDAL_WRAPPER_API FmMosaic
{
public:
	FmMosaic(void);
	~FmMosaic(void);

public:
public:
	HRESULT Open(BSTR bstrPathName, UINT uMode);
	HRESULT CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows, UINT DataType, int nBandNum, UINT BandType, DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	HRESULT Close(void);
	HRESULT IsGeoCoded(void);
	HRESULT GetBandFormat(UINT* pBandFormat);
	HRESULT GetBandNum(int* pBandNum);
	HRESULT GetRows(int* pRows);
	HRESULT GetCols(int* pCols);
	HRESULT GetDataType(UINT* pDataType);
	HRESULT GetGrdInfo(DOUBLE* xStart, DOUBLE* yStart, DOUBLE* cellSize);
	HRESULT SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	HRESULT GetBPB(int* pBPB);
	HRESULT GetBPP(int* pBPP);
	HRESULT GetPathName(BSTR* bstrPathName);
	HRESULT ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	HRESULT WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	HRESULT GetSupExts(BYTE* lpszExts, UINT flags);
	HRESULT Image2World(FLOAT x, FLOAT y, DOUBLE* pX, DOUBLE* pY);
	HRESULT World2Image(DOUBLE X, DOUBLE Y, FLOAT* px, FLOAT* py);
	HRESULT GetTiledSize(int* nXBlockSize, int* nYBlockSize);

private:
	FmBase* pRaster;
public:
};


#endif