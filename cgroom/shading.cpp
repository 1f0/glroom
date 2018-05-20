#include "shading.h"
#include <cmath>
void toUnit(float vec[3]){
	float length = (float)sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	if (length == 0)length = 1;
	vec[0] /= length;
	vec[1] /= length;
	vec[2] /= length;
}

void calcNormal(float v[3][3],float out[3]){
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	//平面上向量的叉积
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];
	toUnit(out);
}

void makeShadowMat(GLfloat pts[3][3], GLfloat lgtPos[4], GLfloat dstMat[4][4]){
	GLfloat planeCoef[4];
	GLfloat dot;
	calcNormal(pts, planeCoef);

	planeCoef[3] = -(planeCoef[0] * pts[2][0] + planeCoef[1] * pts[2][1] + planeCoef[2] * pts[2][2]);
	dot = planeCoef[0] * lgtPos[0] +
		planeCoef[1] * lgtPos[1] +
		planeCoef[2] * lgtPos[2] +
		planeCoef[3] * lgtPos[3];

	dstMat[0][0] = dot-lgtPos[0] * planeCoef[0];
	dstMat[1][0] = 0 - lgtPos[0] * planeCoef[1];
	dstMat[2][0] = 0 - lgtPos[0] * planeCoef[2];
	dstMat[3][0] = 0 - lgtPos[0] * planeCoef[3];

	dstMat[0][1] = 0 - lgtPos[1] * planeCoef[0];
	dstMat[1][1] = dot-lgtPos[1] * planeCoef[1];
	dstMat[2][1] = 0 - lgtPos[1] * planeCoef[2];
	dstMat[3][1] = 0 - lgtPos[1] * planeCoef[3];

	dstMat[0][2] = 0 - lgtPos[2] * planeCoef[0];
	dstMat[1][2] = 0 - lgtPos[2] * planeCoef[1];
	dstMat[2][2] = dot-lgtPos[2] * planeCoef[2];
	dstMat[3][2] = 0 - lgtPos[2] * planeCoef[3];

	dstMat[0][3] = 0 - lgtPos[3] * planeCoef[0];
	dstMat[1][3] = 0 - lgtPos[3] * planeCoef[1];
	dstMat[2][3] = 0 - lgtPos[3] * planeCoef[2];
	dstMat[3][3] = dot-lgtPos[3] * planeCoef[3];
}
