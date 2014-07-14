#pragma once

#ifdef  GDAL_WRAPPER_EXPORT
#define GDAL_WRAPPER_API __declspec(dllexport)
#else
#define GDAL_WRAPPER_API __declspec(dllimport)
#endif

#define _AFXDLL
#include <afx.h>

#include "gdal_priv.h"

#include "global.h"

class GDAL_WRAPPER_API FmBase
{
public:
	FmBase(void);
	~FmBase(void);

public:
	virtual HRESULT Open(BSTR bstrPathPathName, UINT uMode);
	virtual HRESULT CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows, UINT DataType, int nBandNum, UINT BandType, DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	virtual HRESULT Close(void);
	virtual HRESULT IsGeoCoded(void);
	virtual HRESULT GetBandFormat(UINT* pBandFormat);
	virtual HRESULT GetBandNum(int* pBandBum);
	virtual HRESULT GetRows(int* pRows);
	virtual HRESULT GetCols(int* pCols);
	virtual HRESULT GetDataType(UINT* pDataType);
	virtual HRESULT GetGrdInfo(DOUBLE* xStart, DOUBLE* yStart, DOUBLE* cellSize);
	virtual HRESULT SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	virtual HRESULT GetBPB(int* pBPB);
	virtual HRESULT GetBPP(int* pBPP);
	virtual HRESULT GetPathName(BSTR* bstrPathName);
	virtual HRESULT ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	virtual HRESULT WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	static HRESULT GetSupExts(BYTE* lpszExts, UINT flags);
	virtual HRESULT Image2World(FLOAT x, FLOAT y, DOUBLE* pX, DOUBLE* pY);
	virtual HRESULT World2Image(DOUBLE X, DOUBLE Y, FLOAT* px, FLOAT* py);
	virtual HRESULT GetTiledSize(int* nXBlockSize, int* nYBlockSize);

protected:
	void InitImgInfo();
	HRESULT InitColorInfo();
	void InitGeoInfo();
	void InitExtInfo();
	HRESULT InitPyramid();

protected:
	GDALDataset* m_poDataset;
	int m_nWidth;
	int m_nHeight;
	int m_nBandNum;
	unsigned int m_nDataType;
	double m_pGeoTrans[6];
	bool m_bIsGeoCoded;
	GDALDataType m_eGDALType;
	int m_nBytesPerBand;
	int m_nOldBytesPerBand;
	int m_nResampleMethod;
	int m_nBPB;
	int m_nBPP;
	CString m_strPathName;
	double m_lfxStart;
	double m_lfyStart;
	double m_lfCellSize;
	UINT m_uMode;
	float m_fxDPI;
	float m_fyDPI;
	float m_fResolution, m_fXResolution, m_fYResolution;
	bool m_bTranto8bit;
	BYTE* m_plut;
};

