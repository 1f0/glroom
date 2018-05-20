#ifndef _DSPMAP_H_
#define _DSPMAP_H_
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glut.h>
#include <GL/glext.h>
#define MAP_SIZE 512
#define	STEP_SIZE	  16	
#define	HEIGHT_RATIO  1.5f
bool bRender = true;
unsigned char* hMap;
float scaleValue = 0.15f;
void RenderHeightMap(unsigned char pHeightMap[]);
#endif