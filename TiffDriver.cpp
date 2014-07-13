#include "TiffDriver.h"

#include <fstream>
#include <io.h>

using namespace std;

TiffDriver::TiffDriver()
{
}


TiffDriver::~TiffDriver()
{
}

HRESULT TiffDriver::Open(BSTR bstrPathName, UINT uMode)
{
	if ((uMode & modeAqlut) == modeAqlut) 
	{
		m_bTranto8bit = true;
	}
	else
	{
		m_bTranto8bit = false;
	}
	m_strPathName = bstrPathName;
	m_uMode = uMode;

	if ((uMode & modeReadWrite) == modeReadWrite)
	{
		if (_access(m_strPathName.GetBuffer(0), 2) == -1)
		{
			m_poDataset = (GDALDataset*)GDALOpen(m_strPathName.GetBuffer(0), GA_Update);
		}
	}
}
