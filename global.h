#ifndef GLOBAL_H_
#define GLOBAL_H_ 


enum OpenFlags
{
	modeRead = 0,
	modeWrite = 0x1,
	modeReadWrite = 0x2,
	modeJpegCompress = 0x4,
	modePyramidCreate = 0x100,
	modePyramidClean = 0x200,
	modeAqlut = 0x400,
	modeEnhance = 0xc,
	modePyramidRead = 0x800,
	shareCompat = 0,
	shareExclusive = 0x10,
	shareDenyWrite = 0x20,
	shareDenyRead = 0x30,
	shareDenyNone = 0x40,
	modeNoInherit = 0x80,
	modeCreate = 0x1000,
	modeNoTruncate = 0x2000,
	modePropertyOnly = 0x4000,
	modeDemHeaderOnly = 0x8000
};

enum DataType
{
	Pixel_Byte,
	Pixel_Int16,
	Pixel_SInt16,
	Pixel_Int32,
	Pixel_SInt32,
	Pixel_Float,
	Pixel_Double, 
	Pixel_Int64,
	Pixel_SInt64
};

enum BandFormat
{
	BIP,
	BIL,
	BSQ
};

#endif
