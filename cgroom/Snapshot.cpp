//
//  Snapshot.cpp
//  TestPLOYGON
//
//  Created by Gyuu on 16/1/9.
//  Copyright © 2016年 Gyuu. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS

#include "Snapshot.h"
#include <atlimage.h>
#define BYTE unsigned char

#define BYTES_PER_PIXEL 3

int WriteBMPImage(std::string filename, int width, int height, BYTE * bitmapData)
{
	// actual data to use.
	BYTE *imageData = NULL;
	int dataLength = width * height * BYTES_PER_PIXEL;

	// fix BMP memory alignment issue.
	//
	int bytesInRow = width * 3;
	// if not multiple of 4, turn it to.
	int realBytesInRow = ((bytesInRow)+3) / 4 * 4;
	if (realBytesInRow != bytesInRow) {
		int bytesToAdd = realBytesInRow - bytesInRow;
		printf("width:%d, bytes of a row:%d, %d bytes to add.\n", width, bytesInRow, bytesToAdd);

		int realLen = realBytesInRow * height;
		BYTE *realData = (BYTE *)malloc(realLen);
		if (!realData) {
			printf("out of memory\n");
			return EXIT_FAILURE;
		}

		for (int srcIdx = 0, destIdx = 0; srcIdx < dataLength; srcIdx += bytesInRow) {
			// copy old data to new place, adding aligning bytes.
			memcpy(&realData[destIdx], &bitmapData[srcIdx], bytesInRow);
			destIdx += bytesInRow;
			for (int i = 0; i < bytesToAdd; i++) {
				realData[destIdx++] = 0;
			}
		}
		imageData = realData;
		// update dataLength.
		dataLength = realLen;
	}
	else {
		imageData = bitmapData;
	}

	BYTE tempRGB;	//临时色素
	int imageIdx;

	// in memory there is RGB, in image it's BGR.
	for (imageIdx = 0; imageIdx < dataLength; imageIdx += 3)
	{
		tempRGB = imageData[imageIdx];
		imageData[imageIdx] = imageData[imageIdx + 2];
		imageData[imageIdx + 2] = tempRGB;
	}

	//填充BITMAPFILEHEADER
	BITMAPFILEHEADER bitmapFileHeader;
	memset(&bitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));
	bitmapFileHeader.bfType = 0x4d42;	//BM
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//填充BITMAPINFOHEADER
	BITMAPINFOHEADER bitmapInfoHeader;
	memset(&bitmapInfoHeader, 0, sizeof(BITMAPINFOHEADER));
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 8 * BYTES_PER_PIXEL; // meaning 24.
	bitmapInfoHeader.biCompression = 0; // BI_RGB
	bitmapInfoHeader.biSizeImage = dataLength;

	FILE * filePtr;
	filePtr = fopen(filename.c_str(), "wb");
	if (NULL == filePtr)
	{
		printf("error writing image.");
		return -1;
	}

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	fwrite(imageData, dataLength, 1, filePtr);
	fclose(filePtr);
	return 0;
}

void SaveScreenShot(int w, int h)
{
	static int cnt = 0;
    BYTE * screenData;
    int len = w * h * 3;
    screenData = (BYTE *)malloc(len);
    memset(screenData,0,len);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, screenData);
    
    //生成文件名字符串，以时间命名
    time_t tm = 0;
    tm = time(NULL);
	char num[20];
	sprintf(num, "%d", cnt++);
	std::string lpstrFilename = "Snap/Screenshot ";
	lpstrFilename += num;
	lpstrFilename += ".bmp";
    WriteBMPImage(lpstrFilename, w, h, (BYTE *)screenData);
    //释放内存
    free(screenData);
}

std::string GetTimeStampString() {
    time_t rawtime;
    char buffer[80];
    
    time (&rawtime);
    struct tm *timeInfo = localtime(&rawtime);
    
    strftime(buffer, 80,"%Y-%m-%d %I:%M:%S",timeInfo);
    std::string timestamp(buffer);
    return timestamp;
}
