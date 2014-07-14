#define GDAL_WRAPPER_EXPORT

#include "FmTiff.h"

#include <fstream>
#include <io.h>

using namespace std;

FmTiff::FmTiff()
{
}


FmTiff::~FmTiff()
{
}

HRESULT FmTiff::Open(BSTR bstrPathPathName, UINT uMode)
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

void FmTiff::InitGeoInfo()
{
	CString strExt = m_strPathName.Left(m_strPathName.ReverseFind('.'))+_T(".tfw");
	fstream infile;
	infile.open(strExt.GetBuffer(0), ios::in);
	if (infile != NULL)
	{
		infile>>m_lfCellSize;
		double temp;
		infile>>temp;
		infile>>temp;
		infile>>temp;
		infile>>m_pGeoTrans[0];
		infile>>m_pGeoTrans[3];
		m_pGeoTrans[1] = m_lfCellSize;
		m_pGeoTrans[2] = 0;
		m_pGeoTrans[4] = 0;
		m_pGeoTrans[5] = -m_lfCellSize;
		m_bIsGeoCoded = true;
	}
	else
	{
		if (m_poDataset->GetGeoTransform(m_pGeoTrans) == CE_None)
		{
			m_bIsGeoCoded = true;
		}
		else
		{
			m_bIsGeoCoded = false;
		}
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
	infile.clear();
	infile.close();
}

HRESULT FmTiff::CreateImg(BSTR bstrFilePath, UINT uMode, int Cols, int Rows,
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

	char* pszDriverName = "GTIFF";

	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if( poDriver == NULL )
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

	//ppszOptions = CSLSetNameValue(ppszOptions, "TFW", "YES");
	ppszOptions = CSLSetNameValue(ppszOptions, "BIGTIFF", "IF_NEEDED");
	//ppszOptions = CSLSetNameValue(ppszOptions, "TILED", "YES");
	//ppszOptions = CSLSetNameValue(ppszOptions, "BLOCKXSIZE", "128");
	//ppszOptions = CSLSetNameValue(ppszOptions, "BLOCKYSIZE", "128");
	switch(BandType)
	{
	case BIL:
		break;
	case BSQ:
		ppszOptions = CSLSetNameValue(ppszOptions, "INTERLEAVE", "BAND");
		break;
	case BIP:
		ppszOptions = CSLSetNameValue(ppszOptions, "INTERLEAVE", "PIXEL");
		break;
	}

	m_poDataset = poDriver->Create(m_strPathName, Cols, Rows, nBandNum, m_eGDALType, ppszOptions);


	if ( m_poDataset == NULL )
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

	CString strTFWPath = m_strPathName.Left(m_strPathName.ReverseFind('.'));

	strTFWPath += _T(".tfw");
	fstream outfile;
	outfile.open(strTFWPath.GetBuffer(0), ios::out);
	char *temp = new char[50];
	memset(temp, 0, 50*sizeof(char));
	sprintf(temp, "%.6f\n", m_lfCellSize);
	outfile<<temp;
	sprintf(temp, "%.6f\n", 0.0);
	outfile<<temp;
	sprintf(temp, "%.6f\n", 0.0);
	outfile<<temp;
	sprintf(temp, "%.6f\n", -m_lfCellSize);
	outfile<<temp;
	sprintf(temp, "%.6f\n", m_lfxStart);
	outfile<<temp;
	sprintf(temp, "%.6f\n", m_lfyStart+m_lfCellSize*m_nHeight);
	outfile<<temp;
	outfile.clear();
	outfile.close();
	delete []temp;
	temp = NULL;

	m_poDataset->SetGeoTransform(m_pGeoTrans);

	return S_OK;
}

HRESULT FmTiff::SetGrdInfo(DOUBLE xStart, DOUBLE yStart, DOUBLE cellSize)
{
	FmBase::SetGrdInfo(xStart, yStart, cellSize);


	CString strTFW = m_strPathName.Left(m_strPathName.ReverseFind('.'))+_T(".tfw");

	fstream outfile;
	outfile.open(strTFW.GetBuffer(0), ios::out);
	char* temp = new char[50];
	memset(temp, 0, 50*sizeof(char));
	sprintf(temp, "%.6f\n", m_lfCellSize);
	outfile<<temp;
	sprintf(temp, "%.6f\n", 0.0);
	outfile<<temp;
	sprintf(temp, "%.6f\n", 0.0);
	outfile<<temp;
	sprintf(temp, "%.6f\n", -m_lfCellSize);
	outfile<<temp;
	sprintf(temp, "%.6f\n", m_lfxStart);
	outfile<<temp;
	sprintf(temp, "%.6f\n", m_pGeoTrans[3]);
	outfile<<temp;
	outfile.clear();
	outfile.close();
	delete []temp;
	temp = NULL;

	return S_OK;
}
