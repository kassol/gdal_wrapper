#ifndef GDAL_WRAPPER_CPP_
#define GDAL_WRAPPER_CPP_

#define GDAL_WRAPPER_EXPORT

#include "gdal_wrapper.h"
#include <io.h>
#include "FmTiff.h"
#include "FmJPEG.h"

FmMosaic::FmMosaic(void)
{
	pRaster = NULL;
}

FmMosaic::~FmMosaic(void)
{
	if (pRaster != NULL)
	{
		pRaster->Close();
		delete pRaster;
		pRaster = NULL;
	}
}

HRESULT FmMosaic::Open(BSTR bstrPathName, UINT uMode)
{
	CString strPathName;
	strPathName = bstrPathName;

	if (_access(strPathName, 0) == -1)
	{
		return S_FALSE;
	}

	CString strExt = strPathName.Right(strPathName.GetLength()-strPathName.ReverseFind('.')-1);

	if (strExt.CompareNoCase("tif") == 0 || strExt.CompareNoCase("tiff") == 0)
	{
		pRaster = new FmTiff();
	}
	else if (strExt.CompareNoCase("jpg") == 0 || strExt.CompareNoCase("jpeg") == 0)
	{
		pRaster = new FmJPEG();
	}
	else
	{
		pRaster = new FmBase();
	}

	return pRaster->Open(bstrPathName, uMode);
}

HRESULT FmMosaic::CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows,
	UINT DataType, int nBandNum, UINT BandType, 
	DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize)
{
	CString strPathName;
	strPathName = bstrFilePath;
	CString strExt = strPathName.Right(strPathName.GetLength()-strPathName.ReverseFind('.')-1);

	if (strExt.CompareNoCase("tif") == 0 || strExt.CompareNoCase("tiff") == 0)
	{
		pRaster = new FmTiff();
	}
	else if (strExt.CompareNoCase("jpg") == 0 || strExt.CompareNoCase("jpeg") == 0)
	{
		pRaster = new FmJPEG();
	}
	else
	{
		pRaster = new FmBase();
	}

	return pRaster->CreateImg(bstrFilePath, uMode, Cols, Rows, DataType, nBandNum, BandType, xStart, yStart, cellSize);
}

HRESULT FmMosaic::Close()
{
	if (pRaster != NULL)
	{
		pRaster->Close();
		delete pRaster;
		pRaster = NULL;
	}
	return S_OK;
}

HRESULT FmMosaic::IsGeoCoded()
{
	return pRaster->IsGeoCoded();
}

HRESULT FmMosaic::GetBandFormat(UINT* pBandFormat)
{
	return pRaster->GetBandFormat(pBandFormat);
}

HRESULT FmMosaic::GetBandNum(int* pBandNum)
{
	return pRaster->GetBandNum(pBandNum);
}

HRESULT FmMosaic::GetRows(int* pRows)
{
	return pRaster->GetRows(pRows);
}

HRESULT FmMosaic::GetCols(int* pCols)
{
	return pRaster->GetCols(pCols);
}

HRESULT FmMosaic::GetDataType(UINT* pDataType)
{
	return pRaster->GetDataType(pDataType);
}

HRESULT FmMosaic::GetGrdInfo(DOUBLE* xStart, DOUBLE* yStart, DOUBLE* cellSize)
{
	return pRaster->GetGrdInfo(xStart, yStart, cellSize);
}

HRESULT FmMosaic::SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize)
{
	return pRaster->SetGrdInfo(xStart, yStart, cellSize);
}

HRESULT FmMosaic::GetBPB(int* pBPB)
{
	return pRaster->GetBPB(pBPB);
}

HRESULT FmMosaic::GetBPP(int* pBPP)
{
	return pRaster->GetBPP(pBPP);
}

HRESULT FmMosaic::GetPathName(BSTR* bstrPathName)
{
	return pRaster->GetPathName(bstrPathName);
}

HRESULT FmMosaic::ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom,
	BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop,
	int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip)
{
	return pRaster->ReadImg(nSrcLeft, nSrcTop, nSrcRight, nSrcBottom, pBuf, nBufWid, nBufHeight, nBandNum,
		nDestLeft, nDestTop, nDestRight, nDestBottom, nSrcSkip, nDestSkip);
}

HRESULT FmMosaic::WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom,
	BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop,
	int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip)
{
	return pRaster->WriteImg(nSrcLeft, nSrcTop, nSrcRight, nSrcBottom, pBuf, nBufWid, nBufHeight, nBandNum, 
		nDestLeft, nDestTop, nDestRight, nDestBottom, nSrcSkip, nDestSkip);
}

HRESULT FmMosaic::GetSupExts(BYTE* lpszExts, UINT flags)
{
	return FmBase::GetSupExts(lpszExts, flags);
}

HRESULT FmMosaic::Image2World(FLOAT x, FLOAT y, DOUBLE* pX, DOUBLE* pY)
{
	return pRaster->Image2World(x, y, pX, pY);
}

HRESULT FmMosaic::World2Image(DOUBLE X, DOUBLE Y, FLOAT* px, FLOAT* py)
{
	return pRaster->World2Image(X, Y, px, py);
}

HRESULT FmMosaic::GetTiledSize(int* nXBlockSize, int* nYBlockSize)
{
	return pRaster->GetTiledSize(nXBlockSize, nYBlockSize);
}
#endif