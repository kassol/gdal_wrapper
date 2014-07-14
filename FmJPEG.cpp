#define GDAL_WRAPPER_EXPORT

#include "FmJPEG.h"


FmJPEG::FmJPEG(void)
{
}


FmJPEG::~FmJPEG(void)
{
}

HRESULT FmJPEG::Open(BSTR bstrPathPathName, UINT uMode)
{
	if ((uMode & modeReadWrite) == modeReadWrite)
	{
		uMode = uMode^modeReadWrite;
		uMode |= modeRead;
	}
	return FmBase::Open(bstrPathPathName, uMode);
}

HRESULT FmJPEG::CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows,
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

	CString temPathName = m_strPathName.Left(m_strPathName.ReverseFind('.'))+_T("_temp.tif");
	GDALDriver* tempoDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	if (tempoDriver == NULL)
	{
		return S_FALSE;
	}
	m_poDataset = tempoDriver->Create(temPathName.GetBuffer(0), Cols, Rows, nBandNum, m_eGDALType, NULL);
	if (m_poDataset == NULL)
	{
		return S_FALSE;
	}


	m_pGeoTrans[0] = xStart;
	m_pGeoTrans[1] = cellSize;
	m_pGeoTrans[2] = 0;
	m_pGeoTrans[3] = yStart+cellSize*m_nHeight;
	m_pGeoTrans[4] = 0;
	m_pGeoTrans[5] = 0.0-cellSize;
	m_lfxStart = xStart;
	m_lfyStart = yStart;
	m_lfCellSize = cellSize;
	m_bIsGeoCoded = true;
	m_nResampleMethod = 0;

	m_poDataset->SetGeoTransform(m_pGeoTrans);
	return S_OK;
}

HRESULT FmJPEG::Close()
{
	if ((m_uMode & modeCreate) == modeCreate)
	{
		GDALDriver* tempoDriver = GetGDALDriverManager()->GetDriverByName("JPEG");
		if (tempoDriver == NULL)
		{
			FmBase::Close();
			return S_FALSE;
		}
		GDALDataset* tempoDataset = tempoDriver->CreateCopy(m_strPathName, m_poDataset, true, NULL, NULL, NULL);
		if (tempoDataset == NULL)
		{
			FmBase::Close();
			return S_FALSE;
		}
		GDALClose((GDALDatasetH)tempoDataset);

		FmBase::Close();

		CString temPathName = m_strPathName.Left(m_strPathName.ReverseFind('.'))+_T("_temp.tif");
		DeleteFile(temPathName);
		// 		temPathName = m_strPathName.Left(m_strPathName.ReverseFind('.'))+_T("_temp.tfw");
		// 		DeleteFile(temPathName);
	}
	else
	{
		FmBase::Close();
	}
	return S_OK;
}
