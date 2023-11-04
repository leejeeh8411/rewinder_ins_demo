#include "stdafx.h"
#include "ImageProcess.h"


ImageProcess::ImageProcess()
{
}


ImageProcess::~ImageProcess()
{
}

void ImageProcess::Binarization(unsigned char* pSrc, unsigned char* pDst, int width, int height, int th, bool bWhite)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int val = *(pSrc + y * width + x);

			if (bWhite == true) {
				if (val > th) {
					*(pDst + y * width + x) = 0xff;
				}
				else {
					*(pDst + y * width + x) = 0x00;
				}
			}
			else if(bWhite == false) {
				if (val < th) {
					*(pDst + y * width + x) = 0xff;
				}
				else {
					*(pDst + y * width + x) = 0x00;
				}
			}
		}
	}
}

int ImageProcess::SobelXY(LPBYTE fmSour, LPBYTE fmDest, int nWidth, int nHeight, int nPitch,
	int* pKernelX, int* pKernelY, int nKernelSizeX, int nKernelSizeY, int sttX, int endX)
{
	// 변수 체크 
	if (nWidth>nPitch) return 0;

	int i, j, k, l;
	int nOrgX, nOrgY;
	int Kernel_ALLZero = 0;
	int MaxValue;

	nOrgX = (int)(nKernelSizeX / 2.0 - 0.5);
	nOrgY = (int)(nKernelSizeY / 2.0 - 0.5);

	for (k = 0; k<nKernelSizeY; k++)
		for (l = 0; l<nKernelSizeX; l++)
			if (*(pKernelX + nKernelSizeX*k + l) > 0) Kernel_ALLZero = 1;

	if (Kernel_ALLZero == 0) // 모든 kernel의 값이 ZERO이면 
	{
		for (i = 0; i<nHeight - nKernelSizeY - 1; i++)
			for (j = sttX; j<endX - nKernelSizeX - 1; j++)
			{
				MaxValue = 0;
				for (k = 0; k<nKernelSizeY; k++)
					for (l = 0; l<nKernelSizeX; l++)
						if (*(fmSour + (i + k)*nPitch + j + l) > MaxValue)
						{
							MaxValue = *(fmSour + (i + k)*nPitch + j + l);
						}

				*(fmDest + (i + nOrgY)*nPitch + j + nOrgX) = MaxValue;
			}
	}
	else  //kernel이 어떤 값을 가지고 있을때
	{
		for (i = nOrgY; i < nHeight - nKernelSizeY; i++)
			for (j = sttX + nOrgX; j < nWidth - nKernelSizeX; j++)
			{
				int sumX = 0;
				int sumY = 0;
				for (k = 0; k < nKernelSizeY; k++) {
					for (l = 0; l < nKernelSizeX; l++) {
						sumX = sumX + *(fmSour + (i + k)*nPitch + j + l) * *(pKernelX + nKernelSizeX*k + l);
						sumY = sumY + *(fmSour + (i + k)*nPitch + j + l) * *(pKernelY + nKernelSizeX*k + l);
					}
				}

				int sum = sqrt((sumX * sumX) + (sumY * sumY));

				if (sum <= 0) {
					sum = abs(sum);
					//sum = 0;
				}
				else if (sum >= 255) sum = 255;
				*(fmDest + (i + nOrgX)*nPitch + (j + nOrgY)) = sum;
			}
	}

	return 1;
}

void ImageProcess::BinarizationEdgeRange(unsigned char* pSrc, unsigned char* pDst,
	int width, int sttY, int endY, int thW, int thB, int* edge, int rangeX, bool reverseX, int skipStart)
{
	for (int y = sttY; y < endY; y++) {
		if (reverseX == true) {
			for (int x = edge[y] - rangeX; x < edge[y] - skipStart; x++) {
				int val = *(pSrc + y * width + x);

				if (val > thW || val < thB) {
					*(pDst + y * width + x) = 0xff;
				}
				else {
					*(pDst + y * width + x) = 0x00;
				}
			}
		}
		else {
			for (int x = edge[y] + skipStart; x < edge[y] + rangeX; x++) {
				int val = *(pSrc + y * width + x);

				if (val > thW || val < thB) {
					*(pDst + y * width + x) = 0xff;
				}
				else {
					*(pDst + y * width + x) = 0x00;
				}
			}
		}
	}
}
