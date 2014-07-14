#define GDAL_WRAPPER_EXPORT

#include "FmBase.h"
#include <io.h>


HANDLE hmutex2 = CreateMutex(NULL, FALSE, _T("RW"));


#define BiLinear_img(Type) void Type##_BiLinear_img(BYTE *buf11,BYTE *buf21, \
	BYTE *buf22,BYTE *buf12, \
	double Dx,double Dy,     \
	BYTE *pOut)				\
{																\
	*((Type *)pOut) = (Type)((1-Dx) * (1-Dy) * (*((Type*)buf11))			\
	+ Dx     * (1-Dy) * (*((Type*)buf21))				\
	+ Dx     * Dy     * (*((Type*)buf22))				\
	+ (1-Dx) * Dy     * (*((Type*)buf12)));			\
}

inline BiLinear_img(BYTE)
	inline BiLinear_img(USHORT)
	inline BiLinear_img(short)
	inline BiLinear_img(UINT)
	inline BiLinear_img(int)
	inline BiLinear_img(float)
	inline BiLinear_img(double)
	inline BiLinear_img(__int64)

#define FuncName_img(Type) Type##_BiLinear_img

	typedef void (* BiLinearFunc_img)(BYTE *buf11,BYTE *buf21, BYTE *buf22,BYTE *buf12,double Dx,double Dy, BYTE *pOut);

static BiLinearFunc_img FuncTable_img[9] = {FuncName_img(BYTE),
	FuncName_img(USHORT),
	FuncName_img(short),
	FuncName_img(UINT),
	FuncName_img(int),
	FuncName_img(float),
	FuncName_img(double),
	FuncName_img(__int64),
	FuncName_img(__int64)//the singed int64 is same as int64
};


FmBase::FmBase(void)
	:m_poDataset(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nBandNum(0)
	, m_nDataType(-1)
	, m_bIsGeoCoded(false)
	, m_nBytesPerBand(0)
	, m_nOldBytesPerBand(0)
	, m_nResampleMethod(0)
	, m_nBPB(0)
	, m_nBPP(0)
	, m_strPathName(_T(""))
	, m_lfxStart(0)
	, m_lfyStart(0)
	, m_lfCellSize(1)
	, m_uMode(0)
	, m_fxDPI(0)
	, m_fyDPI(0)
	,m_fResolution(0)
	, m_fXResolution(0)
	, m_fYResolution(0)
	, m_bTranto8bit(false)
	, m_plut(NULL)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
	GDALAllRegister();
}


FmBase::~FmBase(void)
{
}

HRESULT FmBase::Open(BSTR bstrPathPathName, UINT uMode)
{
	if ((uMode & modeAqlut) == modeAqlut)
	{
		m_bTranto8bit = true;
	}
	else
	{
		m_bTranto8bit = false;
	}
	m_strPathName = bstrPathPathName;
	m_uMode = uMode;

	CString strExt = m_strPathName.Right(m_strPathName.GetLength()-m_strPathName.ReverseFind('.')-1);
	if (strExt.CompareNoCase("hdr") == 0)
	{
		m_strPathName = m_strPathName.Left(m_strPathName.ReverseFind('.'));
		m_strPathName += _T(".img");
	}

	if ((uMode & modeReadWrite) == modeReadWrite)
	{
		if (_access(m_strPathName.GetBuffer(0), 2) == -1)
		{
			return S_FALSE;
		}
		m_poDataset = (GDALDataset*)GDALOpen(m_strPathName.GetBuffer(0), GA_Update);
	}
	else if (((uMode & modeRead) == modeRead) && ((uMode & modePyramidCreate) == modePyramidCreate))
	{
		m_poDataset = (GDALDataset*)GDALOpen(m_strPathName.GetBuffer(0), GA_Update);
	}
	else if (((uMode & modeRead) == modeRead) && ((uMode & modePyramidCreate) != modePyramidCreate))
	{
		m_poDataset = (GDALDataset*)GDALOpen(m_strPathName.GetBuffer(0), GA_ReadOnly);
	}
	else
	{
		return S_FALSE;
	}

	if (m_poDataset == NULL)
	{
		return S_FALSE;
	}

	InitImgInfo();

	if (m_nDataType == Pixel_Byte)
	{
		m_bTranto8bit = false;
	}

	if (m_bTranto8bit)
	{
		if(S_FALSE == InitColorInfo())
		{
			return S_FALSE;
		}
	}

	m_nBPB = m_nBytesPerBand;
	m_nBPP = m_nBandNum*m_nBPB;

	if (m_nWidth == 0 || m_nHeight == 0 || m_nBandNum == 0 || m_nDataType == -1)
	{
		return S_FALSE;
	}

	InitGeoInfo();
	InitExtInfo();
	if(S_FALSE == InitPyramid())
	{
		return S_FALSE;
	}

	return S_OK;
}

void FmBase::InitImgInfo()
{
	m_nWidth = m_poDataset->GetRasterXSize();
	m_nHeight = m_poDataset->GetRasterYSize();
	m_nBandNum = m_poDataset->GetRasterCount();
	m_eGDALType = m_poDataset->GetRasterBand(1)->GetRasterDataType();

	switch(m_eGDALType)
	{
	case 1:
		{
			m_nDataType = Pixel_Byte;
			m_nBytesPerBand = 1;
			break;
		}
	case 2:
		{
			m_nDataType = Pixel_Int16;
			m_nBytesPerBand = 2;
			break;
		}
	case 3:
		{
			m_nDataType = Pixel_SInt16;
			m_nBytesPerBand = 2;
			break;
		}
	case 4:
		{
			m_nDataType = Pixel_Int32;
			m_nBytesPerBand = 4;
			break;
		}
	case 5:
		{
			m_nDataType = Pixel_SInt32;
			m_nBytesPerBand = 4;
			break;
		}
	case 6:
		{
			m_nDataType = Pixel_Float;
			m_nBytesPerBand = 4;
			break;
		}
	case 7:
		{
			m_nDataType = Pixel_Double;
			m_nBytesPerBand = 8;
			break;
		}
	case 0:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		{
			m_nDataType = -1;
			m_nBytesPerBand = 0;
			break;
		}
	default:
		{
			m_nDataType = -1;
			m_nBytesPerBand = 0;
		}
	}
}

HRESULT FmBase::InitColorInfo()
{
	m_plut = new BYTE[m_nBandNum*65536];
	m_nOldBytesPerBand = m_nBytesPerBand;
	m_nBytesPerBand = 1;
	WaitForSingleObject(hmutex2, INFINITE);
	FILE* stream;
	CString strLutPath = m_strPathName+_T(".lut");
	if (_access(strLutPath, 0) != -1)
	{
		if (fopen_s(&stream, strLutPath.GetBuffer(0), "rb") == 0)
		{
			fread(m_plut, sizeof(BYTE), 65536*m_nBandNum, stream);
		}
		fclose(stream);
	}
	else
	{
		const int nBlock = 1000;
		int nCols = m_nWidth>nBlock ? nBlock : m_nWidth;
		int nRows = m_nHeight> nBlock ? nBlock : m_nHeight;

		switch(m_nDataType)
		{
		case Pixel_Int16:
			{
				unsigned short* pOverview = new unsigned short[nCols*nRows*m_nBandNum];
				int* pBandMap = new int[m_nBandNum];
				for (int i = 0; i < m_nBandNum; ++i)
				{
					pBandMap[i] = i +1;
				}
				if (CE_Failure == m_poDataset->RasterIO(GF_Read, 0, 0, m_nWidth, m_nHeight, pOverview, nCols, nRows, m_eGDALType, m_nBandNum, pBandMap, 0, 0, 0))
				{
					return S_FALSE;
				}
				int nodata = (int)m_poDataset->GetRasterBand(1)->GetNoDataValue();
				delete []pBandMap;
				pBandMap = NULL;

				double* pHistogram = new double[65536];
				for (int nb = 0; nb < m_nBandNum; ++nb)
				{
					memset(pHistogram, 0, sizeof(double)*65536);
					for (int n = nb; n < nCols*nRows*m_nBandNum; n += m_nBandNum)
					{
						pHistogram[pOverview[n]]++;
					}

					double SamplesOfPixel = 0;
					for (int n = 1; n < 65535; ++n)
					{
						SamplesOfPixel += pHistogram[n];
					}

					double lfNoiseLimitl = 0.0001, lfNoise = 0, lfNoiseLimitr = 0.001;
					int nBandMin = 0, nBandMax = 65535;

					for (int n = 1; n < 65535; ++n)
					{
						lfNoise += pHistogram[n];
						if (lfNoise/SamplesOfPixel > lfNoiseLimitl)
						{
							nBandMin = n;
							break;
						}
					}

					lfNoise = 0;
					for (int n = 65534; n >= 1; --n)
					{
						lfNoise += pHistogram[n];
						if (lfNoise/SamplesOfPixel > lfNoiseLimitr)
						{
							nBandMax = n;
							break;
						}
					}

					for (int n = 0; n < 65535; ++n)
					{
						double temp = (n-nBandMin)/double(nBandMax-nBandMin)*255.0+0.5;
						if (temp > 255)
						{
							m_plut[nb*65536+n] = 255;
						}
						else if (temp < 0)
						{
							m_plut[nb*65536+n] = 0;
						}
						else
						{
							m_plut[nb*65536+n] = (BYTE)temp;
						}
					}
				}
				delete []pHistogram;
				pHistogram = NULL;
				delete []pOverview;
				pOverview = NULL;
				break;
			}
		case Pixel_SInt16:
			{
				break;
			}
		}
		if (fopen_s(&stream, strLutPath.GetBuffer(0), "wb") == 0)
		{
			fwrite(m_plut, sizeof(BYTE), 65536*m_nBandNum, stream);
		}
		fclose(stream);
	}
	ReleaseMutex(hmutex2);
	return S_OK;
}

void FmBase::InitGeoInfo()
{
	if (CE_None == m_poDataset->GetGeoTransform(m_pGeoTrans))
	{
		m_bIsGeoCoded = true;
	}
	else
	{
		m_bIsGeoCoded = false;
	}
	if (m_bIsGeoCoded == false)
	{
		m_lfxStart = 0;
		m_lfyStart = 0;
		m_lfCellSize = 1;
	}
	else
	{
		m_lfxStart = m_pGeoTrans[0];
		if (abs(m_pGeoTrans[1]) != abs(m_pGeoTrans[5]))
		{
			m_lfCellSize = 1;
		}
		else
		{
			m_lfCellSize = abs(m_pGeoTrans[1]);
		}
		m_lfyStart = m_pGeoTrans[3]-m_nHeight*m_lfCellSize;
	}
}

void FmBase::InitExtInfo()
{
	char** pszMetadata = m_poDataset->GetMetadata();
	const char* dpitmp = CSLFetchNameValue(pszMetadata, "TIFFTAG_RESOLUTIONUNIT");
	if (dpitmp == NULL)
	{
		m_fResolution = -1;
	}
	else if (dpitmp[0] == '2')//inch
	{
		const char* xdpi = CSLFetchNameValue(pszMetadata, "TIFFTAG_XRESOLUTION");
		const char* ydpi = CSLFetchNameValue(pszMetadata, "TIFFTAG_YRESOLUTION");
		m_fXResolution = (float)atof(xdpi);
		m_fYResolution = (float)atof(ydpi);
		m_fResolution = (float)25.4/m_fXResolution;
	}
	else if (dpitmp[0] == '3')//cm
	{
		const char* xdpi = CSLFetchNameValue(pszMetadata, "TIFFTAG_XRESOLUTION");
		const char* ydpi = CSLFetchNameValue(pszMetadata, "TIFFTAG_YRESOLUTION");
		m_fXResolution = (float)(atof(xdpi)*2.54);
		m_fYResolution = (float)(atof(ydpi)*2.54);
		m_fResolution = (float)(10.0/atof(xdpi));
	}
	else
	{
		m_fResolution = -1;
	}
}

HRESULT FmBase::InitPyramid()
{
	const char* pszResampling = "NEAREST";

	if ((m_uMode & modePyramidClean) == modePyramidClean)
	{
		if (CE_Failure == m_poDataset->BuildOverviews(pszResampling, 0, 0, 0, NULL, NULL, NULL))
		{
			return S_FALSE;
		}
	}

	if ((m_uMode & modePyramidCreate) == modePyramidCreate)
	{
		CPLSetConfigOption( "USE_RRD", "YES" );
		int nLevelCount = 0;
		int *pOverviewList = NULL;
		int i = 0;
		while(1)
		{
			int n = (int)pow(2.0, i);
			if (m_nWidth/n < 128 || m_nHeight/n <128)
			{
				nLevelCount = i;
				pOverviewList = new int [nLevelCount];
				for (int n = 0; n < nLevelCount; ++n)
				{
					pOverviewList[n] = (int)pow(2.0, n+1);
				}
				break;
			}
			++i;
		}
		if (CE_Failure == m_poDataset->BuildOverviews(pszResampling, nLevelCount, pOverviewList, 0, NULL, NULL, NULL))
		{
			return S_FALSE;
		}
		delete []pOverviewList;
		pOverviewList = NULL;
	}
	return S_OK;
}

HRESULT FmBase::CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows,
	UINT DataType, int nBandNum, UINT BandType,
	DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize)
{
	m_bTranto8bit = false;
	m_uMode = uMode;
	if ((m_uMode & modeCreate) != modeCreate)
	{
		return S_FALSE;
	}
	m_strPathName = bstrFilePath;

	CString strExt = m_strPathName.Right(m_strPathName.GetLength()-m_strPathName.ReverseFind('.')-1);

	char* pszDriverName = NULL;
	if (strExt.CompareNoCase("bmp") == 0)
	{
		pszDriverName = "BMP";
	}
	else if (strExt.CompareNoCase("img") == 0)
	{
		pszDriverName = "HFA";
	}
	else if (strExt.CompareNoCase("ntf") == 0)
	{
		pszDriverName = "NITF";
	}
	else if (strExt.CompareNoCase("hdr") == 0)
	{
		pszDriverName = "ENVI";
		m_strPathName = m_strPathName.Left(m_strPathName.ReverseFind('.'));
		m_strPathName += _T(".img");
	}
	else if (strExt.CompareNoCase("raw") == 0)
	{
		pszDriverName = "EIR";
	}
	else
	{
		return S_FALSE;
	}

	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)
	{
		return S_FALSE;
	}

	char** ppszMetaData = poDriver->GetMetadata();
	if (ppszMetaData == NULL)
	{
		return S_FALSE;
	}
	if (CSLFetchBoolean(ppszMetaData, GDAL_DCAP_CREATE, FALSE) != TRUE
		&&CSLFetchBoolean(ppszMetaData, GDAL_DCAP_CREATECOPY, FALSE) != TRUE)
	{
		return S_FALSE;
	}

	m_nWidth = Cols;
	m_nHeight = Rows;
	m_nBandNum = nBandNum;
	m_nDataType = DataType;

	switch(m_nDataType)
	{
	case Pixel_Byte:
		m_eGDALType = GDT_Byte;
		m_nBytesPerBand = 1;
		break;
	case Pixel_Int16:
		m_eGDALType = GDT_UInt16;
		m_nBytesPerBand = 2;
		break;
	case Pixel_SInt16:
		m_eGDALType = GDT_Int16;
		m_nBytesPerBand = 2;
		break;
	case Pixel_Int32:
		m_eGDALType = GDT_UInt32;
		m_nBytesPerBand = 4;
		break;
	case Pixel_SInt32:
		m_eGDALType = GDT_Int32;
		m_nBytesPerBand = 4;
		break;
	case Pixel_Float:
		m_eGDALType = GDT_Float32;
		m_nBytesPerBand = 4;
		break;
	case Pixel_Double:
		m_eGDALType = GDT_Float64;
		m_nBytesPerBand = 8;
		break;
	case Pixel_Int64:
	case Pixel_SInt64:
		m_eGDALType = GDT_Unknown;
		m_nBytesPerBand = 0;
		break;
	default:
		m_eGDALType = GDT_Byte;
		m_nBytesPerBand = 1;
	}
	m_nBPB = m_nBytesPerBand;
	m_nBPP = m_nBPB* m_nBandNum;

	char** ppszOptions = NULL;
	if (strcmp(pszDriverName, "ENVI") == 0)
	{
		switch(BandType)
		{
		case BIL:
			ppszOptions = CSLSetNameValue(ppszOptions, "INTERLEAVE", "BIL");
			break;
		case BSQ:
			ppszOptions = CSLSetNameValue(ppszOptions, "INTERLEAVE", "BSQ");
			break;
		case BIP:
			ppszOptions = CSLSetNameValue(ppszOptions, "INTERLEAVE", "BIP");
			break;
		}
	}
	else if (strcmp(pszDriverName, "HFA") == 0)
	{
		ppszOptions = CSLSetNameValue(ppszOptions, "BLOCKSIZE", "128");
	}
	else if (strcmp(pszDriverName, "NITF") == 0)
	{
		ppszOptions = CSLSetNameValue(ppszOptions, "BLOCKXSIZE", "128");
		ppszOptions = CSLSetNameValue(ppszOptions, "BLOCKYSIZE", "128");
	}

	m_poDataset = poDriver->Create(m_strPathName, Cols, Rows, nBandNum, m_eGDALType, ppszOptions);
	if (m_poDataset == NULL)
	{
		return S_FALSE;
	}

	m_pGeoTrans[0] = xStart;
	m_pGeoTrans[1] = cellSize;
	m_pGeoTrans[2] = 0;
	m_pGeoTrans[3] = yStart + cellSize * m_nHeight;
	m_pGeoTrans[4] = 0;
	m_pGeoTrans[5] = 0.0-cellSize;
	m_lfxStart = xStart;
	m_lfyStart = yStart;
	m_lfCellSize = cellSize;
	m_bIsGeoCoded = true;
	m_nResampleMethod = 0;

	m_poDataset->SetGeoTransform(m_pGeoTrans);

	if (strcmp(pszDriverName, "ENVI") == 0)
	{
		for (int n = 0; n < m_nBandNum; ++n)
		{
			if(CE_Failure == m_poDataset->GetRasterBand(n+1)->Fill(0))
			{
				return 0;
			}
		}
	}

	return S_OK;
}

HRESULT FmBase::Close()
{
	if (m_poDataset != NULL)
	{
		GDALClose((GDALDatasetH)m_poDataset);
		m_poDataset = NULL;
	}
	if (m_bTranto8bit)
	{
		delete []m_plut;
		m_plut = NULL;
	}
	m_bIsGeoCoded = false;
	m_bTranto8bit = false;
	return S_OK;
}

HRESULT FmBase::IsGeoCoded()
{
	if (m_bIsGeoCoded)
	{
		return S_OK;
	}
	return S_FALSE;
}

HRESULT FmBase::GetBandFormat(UINT* pBandFormat)
{
	char** pszMetaData = m_poDataset->GetMetadata("IMAGE_STRUCTURE");
	if (pszMetaData == NULL)
	{
		return S_FALSE;
	}
	const char* pszBand = CSLFetchNameValue(pszMetaData, "INTERLEAVE");
	if (pszBand == NULL)
	{
		return S_FALSE;
	}
	if (strcmp(pszBand, "LINE") == 0)
	{
		*pBandFormat = BIL;
	}
	else if (strcmp(pszBand, "BAND") == 0)
	{
		*pBandFormat = BSQ;
	}
	else if (strcmp(pszBand, "PIXEL")==0)
	{
		*pBandFormat = BIP;
	}
	else
	{
		*pBandFormat = BSQ;
	}
	return S_OK;
}

HRESULT FmBase::GetBandNum(int* pBandBum)
{
	*pBandBum = m_nBandNum;
	return S_OK;
}

HRESULT FmBase::GetRows(int* pRows)
{
	*pRows = m_nHeight;
	return S_OK;
}

HRESULT FmBase::GetCols(int* pCols)
{
	*pCols = m_nWidth;
	return S_OK;
}

HRESULT FmBase::GetDataType(UINT* pDataType)
{
	*pDataType = m_nDataType;
	return S_OK;
}

HRESULT FmBase::GetGrdInfo(DOUBLE* xStart, DOUBLE* yStart, DOUBLE* cellSize)
{
	*xStart = m_lfxStart;
	*yStart = m_lfyStart;
	*cellSize = m_lfCellSize;
	return S_OK;
}

HRESULT FmBase::SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize)
{
	m_lfxStart = xStart;
	m_lfyStart = yStart;
	m_lfCellSize = cellSize;
	m_pGeoTrans[0] = xStart;
	m_pGeoTrans[3] = yStart+m_nHeight*cellSize;
	m_pGeoTrans[1] = cellSize;
	m_pGeoTrans[5] = 0.0-cellSize;
	m_pGeoTrans[2] = m_pGeoTrans[4] = 0;
	m_bIsGeoCoded = true;
	if (CE_Failure == m_poDataset->SetGeoTransform(m_pGeoTrans))
	{
		return S_FALSE;
	}
	return S_OK;
}

HRESULT FmBase::GetBPB(int* pBPB)
{
	*pBPB = m_nBPB;
	return S_OK;
}

HRESULT FmBase::GetBPP(int* pBPP)
{
	*pBPP = m_nBPP;
	return S_OK;
}

HRESULT FmBase::GetPathName(BSTR* bstrPathName)
{
	*bstrPathName = m_strPathName.AllocSysString();
	return S_OK;
}

HRESULT FmBase::ReadImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom,
	BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop,
	int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip)
{
	double lfScale = (nDestRight-nDestLeft)/(double)(nSrcRight-nSrcLeft);

	if (nSrcLeft >= m_nWidth || nSrcTop >= m_nHeight || nSrcRight <= 0 || nSrcBottom <= 0)
	{
		return S_FALSE;
	}

	int nStartRowOffset = 0, nStartColOffset = 0, nEndRowOffset = 0, nEndColOffset = 0;

	if (nSrcLeft < 0)
	{
		nStartColOffset = -nSrcLeft;
	}
	if (nSrcTop < 0)
	{
		nStartRowOffset = -nSrcTop;
	}
	if (nSrcRight > m_nWidth)
	{
		nEndColOffset = m_nWidth - nSrcRight;
	}
	if (nSrcBottom > m_nHeight)
	{
		nEndRowOffset = m_nHeight - nSrcBottom;
	}

	nSrcLeft += nStartColOffset;
	nSrcRight += nEndColOffset;
	nSrcTop += nStartRowOffset;
	nSrcBottom += nEndRowOffset;

	nDestLeft += int(nStartColOffset*lfScale);
	nDestRight += int(nEndColOffset*lfScale);
	nDestTop += int(nStartRowOffset*lfScale);
	nDestBottom += int(nEndRowOffset*lfScale);


	if (nSrcSkip == -1)
	{
		if (nBandNum != m_nBandNum)
		{
			return S_FALSE;
		}

		int *bandmap = new int[nBandNum];
		for (int i = 0; i < nBandNum; ++i)
		{
			bandmap[i] = i + 1;
		}
		if (!m_bTranto8bit)
		{
			CPLErr er = m_poDataset->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				pBuf + (nDestBottom - 1) * nBufWid * m_nBPP + nDestLeft * m_nBPP, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, nBandNum, bandmap,
				m_nBPP, -m_nBPP * nBufWid, m_nBPB);
			if (CE_Failure == er)
			{
				return S_FALSE;
			}
		}
		else
		{
			BYTE *temBuf = new BYTE[m_nOldBytesPerBand*m_nBandNum*nBufHeight*nBufWid];
			CPLErr er = m_poDataset->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				temBuf + (nDestBottom - 1) * nBufWid * m_nOldBytesPerBand*nBandNum + nDestLeft * m_nOldBytesPerBand*nBandNum, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, nBandNum, bandmap,
				m_nOldBytesPerBand*nBandNum, -m_nOldBytesPerBand*nBandNum * nBufWid, m_nOldBytesPerBand);
			if (CE_Failure == er)
			{
				return S_FALSE;
			}
			switch(m_nDataType)
			{
			case Pixel_Int16:
				{
					unsigned short* pp = (unsigned short*)temBuf;
					for (int j = nDestTop; j < nDestBottom;++j)
					{
						for (int i = nDestLeft; i < nDestRight; ++i)
						{
							for (int nb = 0; nb < nBandNum; ++nb)
							{
								pBuf[j*nBufWid*nBandNum+i*nBandNum+nb] = m_plut[nb*65536+pp[j*nBufWid*nBandNum+i*nBandNum+nb]];
							}
						}
					}
					break;
				}
			case Pixel_SInt16:
				{
					short* pp = (short*)temBuf;
					for (int j = nDestTop; j < nDestBottom;++j)
					{
						for (int i = nDestLeft; i < nDestRight; ++i)
						{
							for (int nb = 0; nb < m_nBandNum; ++nb)
							{
								pBuf[j*nBufWid*nBandNum+i*nBandNum+nb] = m_plut[nb*65536+pp[j*nBufWid*nBandNum+i*nBandNum+nb]];
							}
						}
					}
					break;
				}
			}
			delete []temBuf;
			temBuf = NULL;
		}

		delete [] bandmap;
		bandmap = NULL;
	}
	else
	{
		if (nDestSkip > nBandNum - 1)
		{
			return S_FALSE;
		}
		if (!m_bTranto8bit)
		{
			CPLErr er = m_poDataset->GetRasterBand(nSrcSkip+1)->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				pBuf + (nDestBottom - 1) * nBufWid * nBandNum* m_nBPB + nDestLeft * nBandNum * m_nBPB + nDestSkip * m_nBPB, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, 
				nBandNum * m_nBPB, -nBandNum * m_nBPB * nBufWid);
			if (CE_Failure == er)
			{
				return S_FALSE;
			}
		}
		else
		{
			BYTE *temBuf = new BYTE[m_nOldBytesPerBand*nBufHeight*nBufWid];
			CPLErr er = m_poDataset->GetRasterBand(nSrcSkip+1)->RasterIO(GF_Read, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
				temBuf + (nDestBottom - 1) * nBufWid * m_nOldBytesPerBand + nDestLeft * m_nOldBytesPerBand, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, 
				m_nOldBytesPerBand, -m_nOldBytesPerBand * nBufWid);
			if (CE_Failure == er)
			{
				return S_FALSE;
			}
			switch(m_nDataType)
			{
			case Pixel_Int16:
				{
					unsigned short* pp = (unsigned short*)temBuf;
					for (int j = nDestTop; j < nDestBottom; ++j)
					{
						for(int i = nDestLeft; i < nDestRight; ++i)
						{
							pBuf[j*nBufWid*nBandNum+i*nBandNum+nDestSkip] = m_plut[nSrcSkip*65536+pp[j*nBufWid+i]];
						}
					}
					break;
				}
			case Pixel_SInt16:
				{
					short* pp = (short*)temBuf;
					for (int j = nDestTop; j < nDestBottom; ++j)
					{
						for(int i = nDestLeft; i < nDestRight; ++i)
						{
							pBuf[j*nBufWid*nBandNum+i*nBandNum+nDestSkip] = m_plut[nSrcSkip*65536+pp[j*nBufWid+i]];
						}
					}
					break;
				}
			}
			delete []temBuf;
			temBuf = NULL;
		}
	}
	return S_OK;
}

HRESULT FmBase::WriteImg(int nSrcLeft, int nSrcTop, int nSrcRight, int nSrcBottom,
	BYTE* pBuf, int nBufWid, int nBufHeight, int nBandNum, int nDestLeft, int nDestTop,
	int nDestRight, int nDestBottom, int nSrcSkip, int nDestSkip)
{
	double lfScale = (nDestRight-nDestLeft)/(double)(nSrcRight-nSrcLeft);
	if (nSrcLeft >= m_nWidth || nSrcTop >= m_nHeight || nSrcRight <= 0 || nSrcBottom <= 0)
	{
		return S_FALSE;
	}

	int nStartRowOffset = 0, nStartColOffset = 0, nEndRowOffset = 0, nEndColOffset = 0;
	if (nSrcLeft < 0)
	{
		nStartColOffset = -nSrcLeft;
	}
	if (nSrcTop < 0)
	{
		nStartRowOffset = -nSrcTop;
	}
	if (nSrcRight > m_nWidth)
	{
		nEndColOffset = m_nWidth - nSrcRight;
	}
	if (nSrcBottom > m_nHeight)
	{
		nEndRowOffset = m_nHeight - nSrcBottom;
	}

	nSrcLeft += nStartColOffset;
	nSrcRight += nEndColOffset;
	nSrcTop += nStartRowOffset;
	nSrcBottom += nEndRowOffset;

	nDestLeft += int(nStartColOffset*lfScale);
	nDestRight += int(nEndColOffset*lfScale);
	nDestTop += int(nStartRowOffset*lfScale);
	nDestBottom += int(nEndRowOffset*lfScale);

	if (nDestSkip > nBandNum - 1)
	{
		return S_FALSE;
	}
	if (nSrcSkip == -1)
	{
		if (nBandNum != m_nBandNum)
		{
			return S_FALSE;
		}

		int *bandmap = new int[nBandNum];
		for (int i = 0; i < nBandNum; ++i)
		{
			bandmap[i] = i+1;
		}

		CPLErr er = m_poDataset->RasterIO(GF_Write, nSrcLeft, m_nHeight - nSrcBottom, nSrcRight - nSrcLeft, nSrcBottom - nSrcTop,
			pBuf + (nDestBottom - 1) * nBufWid * m_nBPP + nDestLeft * m_nBPP, nDestRight - nDestLeft, nDestBottom - nDestTop, m_eGDALType, nBandNum, bandmap,
			m_nBPP, -m_nBPP * nBufWid, m_nBPB);
		if (CE_Failure == er)
		{
			return S_FALSE;
		}

		delete [] bandmap;
		bandmap = NULL;

	}
	else
	{
		CPLErr er = m_poDataset->GetRasterBand(nSrcSkip+1)->RasterIO(GF_Write, nSrcLeft, m_nHeight-nSrcBottom, nSrcRight-nSrcLeft, nSrcBottom-nSrcTop,
			pBuf + (nDestBottom - 1) * nBufWid * nBandNum * m_nBPB + nDestLeft * nBandNum * m_nBPB+nDestSkip*m_nBPB, nDestRight-nDestLeft, nDestBottom-nDestTop, m_eGDALType, 
			nBandNum*m_nBPB, -nBandNum*m_nBPB*nBufWid);
		if (CE_Failure == er)
		{
			return S_FALSE;
		}
	}
	return S_OK;
}

HRESULT FmBase::GetSupExts(BYTE* lpszExts, UINT flags)
{
	if ((flags & modeReadWrite) == modeReadWrite)
	{
		strcpy((char*)lpszExts, "Tiff file(*.tif;*.tiff)|*.tif; *.tiff\
								|Erdas file(*.img)|*.img|Hdr file(*.hdr)|*.hdr\
								|Nitf file(*.ntf)|*.ntf|Bitmap file(*.bmp)|*.bmp|All file (*.*)|*.*||");
	}
	else if ((flags & modeRead) == modeRead)
	{
		strcpy((char*)lpszExts, "Tiff file (*.tif;*.tiff)|*.tif; *.tiff|Erdas file (*.img)|*.img|\
								Hdr file (*.hdr)|*.hdr|Jpeg file (*.jpg;*.jpeg)|*.jpg; *.jpeg\
								|Nitf file (*.ntf)|*.ntf|Bitmap file (*.bmp)|*.bmp|Sat file (*.sat)|*.sat|ADS40 file (*.sup)|*.ads|All file (*.*)|*.*||");
	}
	else if ((flags & modeCreate) == modeCreate)
	{
		strcpy((char*)lpszExts, "Tiff file (*.tif;*.tiff)|*.tif; *.tiff|Erdas file (*.img)|*.img|\
								Hdr file (*.hdr)|*.hdr|Jpeg file (*.jpg;*.jpeg)|*.jpg; *.jpeg\
								|Nitf file (*.ntf)|*.ntf|Bitmap file (*.bmp)|*.bmp|All file (*.*)|*.*||");
	}
	return S_OK;
}

HRESULT FmBase::Image2World(FLOAT x, FLOAT y, DOUBLE* pX, DOUBLE* pY)
{
	*pX = DOUBLE(x) * m_lfCellSize + m_lfxStart;
	*pY = DOUBLE(y) * m_lfCellSize + m_lfyStart;
	return S_OK;
}

HRESULT FmBase::World2Image(DOUBLE X, DOUBLE Y, FLOAT* px, FLOAT* py)
{
	*px = FLOAT((X-m_lfxStart)/m_lfCellSize);
	*py = FLOAT((Y-m_lfyStart)/m_lfCellSize);
	return S_OK;
}

HRESULT FmBase::GetTiledSize(int* nXBlockSize, int* nYBlockSize)
{
	m_poDataset->GetRasterBand(1)->GetBlockSize(nXBlockSize, nYBlockSize);
	return S_OK;
}
