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

FmDem::FmDem()
	: pAltitude(NULL)
	, m_X0(0)
	, m_Y0(0)
	, m_XCellSize(0)
	, m_YCellSize(0)
	, m_Kappa(0)
	, m_nRows(0)
	, m_nCols(0)
	, m_AveAltitude(0)
	, m_MaxAltitude(0)
	, m_MinAltitude(0)
	, m_lfAltitudeOffset(0)
	, m_strDEMPathName("")
{
}

FmDem::~FmDem()
{
	if (pAltitude != NULL)
	{
		delete []pAltitude;
		pAltitude = NULL;
	}
}

HRESULT FmDem::Open(BSTR bstrPathName, DOUBLE lfAltitudeOffset, UINT accMode)
{
	CString strPathName=CString(bstrPathName);
	m_strDEMPathName=strPathName;
	m_lfAltitudeOffset=lfAltitudeOffset;

	if(IsSupported(bstrPathName,accMode))
	{
		return S_FALSE;
	}

	if(_access(strPathName,0)==-1)
	{
		return S_FALSE;
	}

	//¶ÁÈ¡Êý¾Ý
	FILE* fp=fopen(strPathName,"rt");
	if(fp==NULL)
	{
		return S_FALSE;
	}

	char NSDTFHEADER[20];
	char VERSION[20];
	char UNIT[20];
	char COMPRESS[20];
	int	nHZoom=1;
	fscanf(fp,"%s%s%s%lf%s",NSDTFHEADER,VERSION,UNIT,&m_Kappa,COMPRESS);
	fscanf(fp,"%lf%lf%lf%lf%d%d%d",
		&m_X0,
		&m_Y0,
		&m_XCellSize,
		&m_YCellSize,
		&m_nRows,
		&m_nCols,
		&nHZoom);

	m_AveAltitude=0;
	m_MaxAltitude=-99999.9;
	m_MinAltitude=99999.9;

	pAltitude=new double[m_nRows*m_nCols];

	int index = 1;

	if (fabs(m_Kappa) < 1e-5)
	{
		double lfAAA = 0;
		for (int i = m_nRows-1; i >= 0; --i)
		{
			double temp=0;
			int nCount=0;
			double* pAltitudeIndex=pAltitude+m_nCols*i;
			for(int j=0;j<m_nCols;j++)
			{
				fscanf(fp,"%lf",pAltitudeIndex+j);

				lfAAA += pAltitudeIndex[j];
				continue;
				if(fabs(pAltitudeIndex[j]+99999.0)<1e-5)
				{
					pAltitudeIndex[j]=DBL_MAX;
				}
				else
				{
					pAltitudeIndex[j]/=nHZoom;
					pAltitudeIndex[j]+=m_lfAltitudeOffset;

					if(pAltitudeIndex[j]>m_MaxAltitude)
					{
						m_MaxAltitude=pAltitudeIndex[j];
					}
					if(pAltitudeIndex[j]<m_MinAltitude)
					{
						m_MinAltitude=pAltitudeIndex[j];
					}

					temp+=pAltitudeIndex[j];
					nCount++;
				}
			}
			if(nCount>0)
			{
				m_AveAltitude+=(temp/nCount);
			}
		}
		int iiii = 0;
	}
	else
	{
		for (int i = 0; i < m_nRows; ++i)
		{
			double temp=0;
			int nCount=0;
			double* pAltitudeIndex=pAltitude+m_nCols*i;
			for(int j=0;j<m_nCols;j++)
			{
				fscanf(fp,"%lf",pAltitudeIndex+j);

				if(fabs(pAltitudeIndex[j]+99999.0)<1e-5)
				{
					pAltitudeIndex[j]=DBL_MAX;
				}
				else
				{
					pAltitudeIndex[j]/=nHZoom;
					pAltitudeIndex[j]+=m_lfAltitudeOffset;

					if(pAltitudeIndex[j]>m_MaxAltitude)
					{
						m_MaxAltitude=pAltitudeIndex[j];
					}
					if(pAltitudeIndex[j]<m_MinAltitude)
					{
						m_MinAltitude=pAltitudeIndex[j];
					}

					temp+=pAltitudeIndex[j];
					nCount++;
				}
			}
			if(nCount>0)
			{
				m_AveAltitude+=(temp/nCount);
			}
		}
	}

	m_AveAltitude/=m_nRows;
	fclose(fp);
	return S_OK;
}

HRESULT FmDem::IsSupported(BSTR bstrPathName, UINT accMode)
{
	if(accMode&modeWrite||accMode&modeCreate)
	{
		return S_FALSE;
	}

	CString strPathName=CString(bstrPathName);
	CString strExt=strPathName.Right(strPathName.GetLength()-strPathName.ReverseFind('.')-1);
	if(strExt.CompareNoCase(_T("dem"))!=0)
	{
		return S_FALSE;
	}

	FILE* fp=fopen(strPathName,"rt");
	if(fp==NULL)
	{
		return S_FALSE;
	}

	char NSDTFHEADER[20];
	fscanf(fp,"%s",NSDTFHEADER);
	fclose(fp);

	if(strcmp(NSDTFHEADER,"NSDTF-DEM")==0)
	{
		return S_OK;
	}
	return S_FALSE;
}

HRESULT FmDem::GetAverageAltitude(double* pZ)
{
	if(pZ==NULL)
	{
		return S_FALSE;
	}

	*pZ=m_AveAltitude;

	return S_OK;
}

HRESULT FmDem::GetAltitude(double X, double Y, double* pZ, UINT uResampleMethod)
{
	if (X < m_X0 || X > m_X0+m_nCols*m_XCellSize || Y > m_Y0 || Y < m_Y0-m_nRows*m_YCellSize)
	{
		*pZ = INVALID_ALTITUDE;
		return S_FALSE;
	}

	int nx = int((X-m_X0)/m_XCellSize);
	int ny = int((m_Y0-Y)/m_YCellSize);
	*pZ = pAltitude[ny*m_nCols+nx];

	return S_OK;
}

HRESULT FmDem::Close()
{
	if (pAltitude != NULL)
	{
		delete []pAltitude;
		pAltitude = NULL;
		m_X0 = 0;
		m_Y0 = 0;
		m_XCellSize = 0;
		m_YCellSize = 0;
		m_Kappa = 0;
		m_nRows = 0;
		m_nCols = 0;
		m_AveAltitude = 0;
		m_MaxAltitude = 0;
		m_MinAltitude = 0;
		m_lfAltitudeOffset = 0;
		m_strDEMPathName = "";
	}
	return S_OK;
}

HRESULT FmDem::GetRows(int* pRows)
{
	*pRows=m_nRows;
	return S_OK;
}

HRESULT FmDem::GetCols(int* pCols)
{
	*pCols=m_nCols;
	return S_OK;
}

HRESULT FmDem::GetStartPos(double* pX0, double* pY0)
{
	*pX0 = m_X0;
	*pY0 = m_Y0-m_nRows*m_YCellSize;
	return S_OK;
}

HRESULT FmDem::GetCellSize(double* pXCellSize, double* pYCellSize)
{
	*pXCellSize = m_XCellSize;
	*pYCellSize = m_YCellSize;
	return S_OK;
}

HRESULT FmDem::GetRange(double* pLBX, double* pLBY, double* pRTX, double* pRTY)
{
	*pLBX = m_X0;
	*pRTY = m_Y0;
	*pRTX = m_X0+m_nCols*m_XCellSize;
	*pLBY = m_Y0-m_nRows*m_YCellSize;
	return S_OK;
}

#endif