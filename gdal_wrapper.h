#ifndef GDAL_WRAPPER_H_
#define GDAL_WRAPPER_H_

#define _AFXDLL
#include <afx.h>

#include "gdal_priv.h"
#pragma comment(lib, "gdal_dhj_win64_i.lib")

#ifdef  GDAL_WRAPPER_EXPORT
#define GDAL_WRAPPER_API __declspec(dllexport)
#else
#define GDAL_WRAPPER_API __declspec(dllimport)
#endif

class GDAL_WRAPPER_API ImageDriver
{
public:
	ImageDriver(void);
	~ImageDriver(void);

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
	virtual HRESULT GetByteOrder(UINT* pByteOrder);
	virtual HRESULT GetScale(FLOAT* pScale);
	virtual HRESULT SetScale(FLOAT Scale);
	virtual HRESULT GetGrdInfo(DOUBLE* xStart, DOUBLE* yStart, DOUBLE* cellSize);
	virtual HRESULT SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize);
	virtual HRESULT HaveColorTable(void);
	virtual HRESULT GetEntryNum(int* pEntryNum);
	virtual HRESULT GetColorTable(BYTE* pColorTable);
	virtual HRESULT SetColorTable(BYTE* pColorTable, int nEntryNum);
	virtual HRESULT GetResampleMethod(int* pResampleMethod);
	virtual HRESULT SetResampleMethod(int nResampleMethod);
	virtual HRESULT GetScanSize(FLOAT* pScanSize);
	virtual HRESULT SetScanSize(FLOAT ScanSize);
	virtual HRESULT GetBPB(int* pBPB);
	virtual HRESULT GetBPP(int* pBPP);
	virtual HRESULT GetPathName(BSTR* bstrPathName);
	virtual HRESULT GetPixel(int nRows, int nCols, BYTE* pPixel);
	virtual HRESULT SetPixel(int nRows, int nCols, BYTE* pPixel);
	virtual HRESULT GetGray(int nRows, int nCols, int nBand, BYTE* gray);
	virtual HRESULT SetGray(int nRows, int nCols, int nBand, BYTE* gray);
	virtual HRESULT GetPixelF(FLOAT x, FLOAT y, BYTE* pPixel, UINT nResampleMethod);
	virtual HRESULT GetGrayF(FLOAT x, FLOAT y, int nBand, BYTE* pGray, int nResampleMethod);
	virtual HRESULT ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	virtual HRESULT WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom, BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop, int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip);
	virtual HRESULT SetProgressInterface(void* pIUnknown);
	static HRESULT GetSupExts(BYTE* lpszExts, UINT flags);
	virtual HRESULT Image2World(FLOAT x, FLOAT y, DOUBLE* pX, DOUBLE* pY);
	virtual HRESULT World2Image(DOUBLE X, DOUBLE Y, FLOAT* px, FLOAT* py);
	virtual HRESULT GetDefaultBand(int* nBandIndex);
	virtual HRESULT GetCamera(BSTR* bstrCamera);
	virtual HRESULT SetCamera(BSTR bstrCamera);
	virtual HRESULT GetSensorType(int* pSensorType);
	virtual HRESULT SetSensorType(int nSensorType);
	virtual HRESULT GetDPI(FLOAT* pxDPI, FLOAT* pyDPI);
	virtual HRESULT SetDPI(FLOAT xDPI, FLOAT yDPI);
	virtual HRESULT Tiff2JPG(BSTR bstrTiffPath, BSTR bstrJPGPath);
	virtual HRESULT GetTiledSize(int* nXBlockSize, int* nYBlockSize);
	virtual HRESULT Translate(BSTR bstrImgPath);

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


#endif