#pragma once
class ImageProcess
{
public:
	ImageProcess();
	~ImageProcess();

	int SobelXY(LPBYTE fmSour, LPBYTE fmDest, int nWidth, int nHeight, int nPitch,
		int* pKernelX, int* pKernelY, int nKernelSizeX, int nKernelSizeY, int sttX, int endX);

	void Binarization(unsigned char* pSrc, unsigned char* pDst, int width, int height, int th, bool bWhite);

	void BinarizationEdgeRange(unsigned char* pSrc, unsigned char* pDst,
		int width, int sttY, int endY, int thW, int thB, int* edge, int rangeX, bool reverseX, int skipStart);
};

