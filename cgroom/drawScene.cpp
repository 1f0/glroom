#define _CRT_SECURE_NO_WARNINGS
#include "drawScene.h"
#include "collision.h"
#include <iostream>
#include <atlimage.h>
#include <tchar.h>
#include "shading.h"
#define ALLFRAME 160

const float PI = 3.1415926539;
extern AABB aabox[4];
extern GLuint object;
extern GLuint object;
extern GLuint boy;
extern GLuint sofa;
extern GLuint sidesofa;
extern GLuint table1;
extern GLuint door;
extern GLuint tv;
extern float cubex;
extern float cubez;
extern float cubey;
extern float movDirection;//initial direction
extern float objtheta;//initial direction
extern float theta;
extern GLfloat light_pos[];
extern bool playVideo;
extern int wWidth, wHeight;
extern GLfloat shdMat[4][4];
extern bool inroom;
#define checkImgWid 64
#define checkImgHgt 64

#define BITMAP_ID 0x4D42

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader){
	FILE *filePtr;	// 文件指针
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap文件头
	unsigned char	*bitmapImage;		// bitmap图像数据
	int	imageIdx = 0;		// 图像位置索引
	unsigned char	tempRGB;	// 交换变量

	// 以“二进制+读”模式打开文件filename 
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL) return NULL;
	// 读入bitmap文件图
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// 验证是否为bitmap文件
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// 读入bitmap信息头
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// 将文件指针移至bitmap数据
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// 为装载图像数据创建足够的内存
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// 验证内存是否创建成功
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// 读入bitmap图像数据
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// 确认读入成功
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//由于bitmap中保存的格式是BGR，下面交换R和B的值，得到RGB格式
	for (imageIdx = 0;
		imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// 关闭bitmap图像文件
	fclose(filePtr);
	return bitmapImage;
}

void loadTex(GLuint txname, char*fileName){
	BITMAPINFOHEADER bmpInfoHeader;
	unsigned char* bmpData = LoadBitmapFile(fileName, &bmpInfoHeader);
	//glColor4f(0.8f, 0.8f, 0.8f, 0.5f);		// 全亮度， 50% Alpha 混合
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);		// 基于源象素alpha通道值的半透明混合函数

	glBindTexture(GL_TEXTURE_2D, txname);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,			//mipmap level
		GL_RGB,	//tex color
		bmpInfoHeader.biWidth,
		bmpInfoHeader.biHeight,
		0,			//no border
		GL_RGB,	//bitmap data
		GL_UNSIGNED_BYTE,	//color data type
		bmpData);
}


GLuint ATLLoadTexture(const char *fileName)
{
	BITMAP bm;
	GLuint idTexture = 0;
	CImage img;             //需要头文件atlimage.h
	HRESULT hr = img.Load(fileName);
	if (!SUCCEEDED(hr))	    //文件加载失败  
	{
		MessageBox(NULL, "文件加载失败", "ERROR", 0);
		return NULL;
	}
	HBITMAP hbmp = img;
	if (!GetObject(hbmp, sizeof(bm), &bm))
		return 0;

	glGenTextures(1, &idTexture);
	if (idTexture)
	{
		glBindTexture(GL_TEXTURE_2D, idTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glPixelStoref(GL_PACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, bm.bmWidth, bm.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bm.bmBits); //这里不是GL_RGB  
	}
	return idTexture;
}

static GLuint texName[11];

static GLuint avi[60];
static GLuint tvTex;

void init(){
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);			// Really Nice Point Smoothing
	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(11, texName);
	loadTex(texName[0], "pic/Monet.bmp");
	loadTex(texName[1], "pic/floor2.bmp");
	loadTex(texName[2], "pic/wall.bmp");
	loadTex(texName[3], "pic/top.bmp");
	loadTex(texName[4], "obj/Wooden_door/Wooden_door_d.bmp");
	loadTex(texName[5], "pic/desk.bmp");
	loadTex(texName[6], "pic/door1.bmp");
	loadTex(texName[7], "pic/door.bmp");
	loadTex(texName[8], "pic/sky.bmp");
	loadTex(texName[9], "pic/deng.bmp");
	loadTex(texName[10], "pic/light.bmp");
	char buf[50];
	//glGenTextures(60, avi);
	for (int i = 0; i < ALLFRAME; i++){
		sprintf(buf, "avi/img-%03d.jpg", i);
		avi[i]=ATLLoadTexture(buf);
	}
	tvTex = ATLLoadTexture("obj/tv/Moc.jpg");
}

void drawAB(int i){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Vec3 tmp;
	glPushMatrix();
	tmp = aabox[i].getCenter();
	glTranslatef(tmp.x, tmp.y, tmp.z);
	float xt = fabs(aabox[i]._max.x - aabox[i]._min.x);
	float yt = fabs(aabox[i]._max.y - aabox[i]._min.y);
	float zt = fabs(aabox[i]._max.z - aabox[i]._min.z);
	glScalef(xt, yt, zt);
	glutSolidCube(1);
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static const GLfloat vertex_list[8][3] = {
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	-0.5, 0.5, -0.5,
	0.5, 0.5, -0.5,
	-0.5, -0.5, 0.5,
	0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
};

void DrawCube()
{
	glBegin(GL_QUADS);
	//底
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[2]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[3]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[1]);

	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[1]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[5]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[4]);

	glTexCoord2f(0, 0); glVertex3fv(vertex_list[4]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[5]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[6]);

	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[4]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[6]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[2]);

	glTexCoord2f(0, 0); glVertex3fv(vertex_list[2]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[6]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[3]);

	glTexCoord2f(0, 0); glVertex3fv(vertex_list[1]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[3]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[5]);

	glEnd();
}

void DrawDoor()
{	
	glPushMatrix();
	glTranslatef(0, 0, 2.2);
	glRotatef(-1*theta, 0, 1, 0);
	glScalef(0.2, 8, 4.4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]);  
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[2]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[3]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[1]);
	glEnd();
	glDisable(GL_TEXTURE_2D);	

	//底
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]); 
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[1]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[5]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[4]);
	glEnd();
	glDisable(GL_TEXTURE_2D);	

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]); 
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[4]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[5]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[6]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//门反面
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[7]); 
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[4]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[6]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[2]);
	glEnd();
	glDisable(GL_TEXTURE_2D);	

	//上
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]); 
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[2]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[6]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[3]);
	glEnd();
	glDisable(GL_TEXTURE_2D);	

	//门正面
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[7]); 
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[1]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[3]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[5]);
	glEnd();
	glDisable(GL_TEXTURE_2D);	

	glPopMatrix();
}

void DrawPaint()
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[2]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[3]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[1]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[1]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[5]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[4]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[4]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[5]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[6]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[0]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[4]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[6]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[2]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[2]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[6]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[3]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//画正面
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3fv(vertex_list[1]);
	glTexCoord2f(1, 0); glVertex3fv(vertex_list[3]);
	glTexCoord2f(1, 1); glVertex3fv(vertex_list[7]);
	glTexCoord2f(0, 1); glVertex3fv(vertex_list[5]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}

void Draw_Leg()
{
	glScalef(0.8,1, 0.8);
	DrawCube();
}

void Draw_desk() // This function draws a triangle with RGB colors
{
	//桌面

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texName[5]);  //选择纹理texture[5]
	glTranslatef(0, 1.7, 0);
	glScalef(5, 0.4, 4);
	DrawCube();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);	//关闭纹理texture[5]

	//桌脚
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[5]);  //选择纹理texture[5]
	glPushMatrix();
	glTranslatef(1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, 1, -1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, -1.5);
	Draw_Leg();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);	//关闭纹理texture[5]

}

int frameNum = 0;
extern bool fogstat;

void DrawScene(){
	if (fogstat) {
		glEnable(GL_FOG);
	}
	else {
		glDisable(GL_FOG);
	}

	float rmsize = 1;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	////////////////////////
	glPushMatrix();
		glScalef(rmsize, rmsize, rmsize);
		//地板
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texName[1]);
		glColor3f(0.8f, 1.0f, 0.8f);
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);                   //用于定义法线向量  	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 0.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, 0.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(10.0f, 0.0f, 10.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);


		//天花板
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texName[3]);
		glColor3f(0.8f, 1.0f, 0.8f);
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, 15.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(10.0f, 15.0f, 10.0f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	/////////////////////////////////////////////
	//front wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	glPushMatrix();
		glScalef(rmsize, rmsize, rmsize);
		glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);

		glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, 0.0f, -10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, -10.0f);

		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//back
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	glPushMatrix();
		glScalef(rmsize, rmsize, rmsize);
		glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 15.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 15.0f, 10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, 10.0f);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//right
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	glPushMatrix();
		glScalef(rmsize, rmsize, rmsize);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(10.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, 15.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(10.0f, 0.0f, -10.0f);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//left
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[2]);
	glPushMatrix();
		glScalef(rmsize, rmsize, rmsize);
		glColor3f(1.0f, 1.0f, 1.0f);

		//////////////////////////////////
		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, 8.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 15.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 8.0f, -10.0f);
		glEnd();


		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 8.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 8.0f, 2.2f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, 2.2f);
		glEnd();


		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, 0.0f, -2.2f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 8.0f, -2.2f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 8.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, -10.0f);
		glEnd();


	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	////////////////////////

	//***********************
	//门外的星空
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	GLUquadricObj *quadObj1 = gluNewQuadric();
	gluQuadricTexture(quadObj1, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texName[8]);
	gluSphere(quadObj1, 30.0f, 100, 100);  

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	//***********************

	//fighter
	glPushMatrix();
		glTranslatef(cubex, cubey, cubez);
		float objtheta2 = 2*movDirection - objtheta;
		glRotatef(-objtheta2 * 180 / PI, 0, 1, 0);
		glScalef(0.05, 0.05, 0.05);
		glCallList(boy);
	glPopMatrix();

	//fighter shadow
	if (inroom) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glColor3f(0.1, 0.1, 0.1);
		glPushMatrix();
		glMultMatrixf((GLfloat*)shdMat);
		glTranslatef(cubex, 0.05, cubez);
		glRotatef(-objtheta2 * 180 / PI, 0, 1, 0);
		glScalef(0.05, 0.05, 0.05);
		glCallList(boy);
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}

	//台灯
	glPushMatrix();
		glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
		glScalef(0.3, 0.3, 0.3);
		glCallList(object);
	glPopMatrix();

	//台灯桌
	glPushMatrix();
		glTranslatef(4.5, 1.0, 4);
		glScalef(0.25, 0.25, 0.25);
		glCallList(table1);
	glPopMatrix();


	//侧沙发
	glPushMatrix();
		glTranslatef(6, 1.2, 7);
		glScalef(0.8, 0.8, 0.8);
		glRotatef(200, 0, 1, 0);
		glCallList(sidesofa);
	glPopMatrix();


	//沙发
	glPushMatrix();
		glTranslatef(0, 1.5, 8);
		glScalef(1.8, 1.8, 1.8);
		glRotatef(180, 0, 1, 0);
		glCallList(sofa);
	glPopMatrix();

	//tv
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tvTex);
	glPushMatrix();
	glTranslatef(0.8, 3.4, -6);
	glScalef(0.6, 0.6, 0.6);
	glCallList(tv);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//电视桌
	glPushMatrix();
	glTranslatef(0, -0.55, -6);
	glScalef(1.2, 1.2, 1.2);
	Draw_desk();
	glPopMatrix();

	//门
	glPushMatrix();
	glTranslatef(-1 * (10 - 2.2*sin((180 - theta) / 180.0*PI)), 4, 2.2 *cos((180 - theta) / 180.0*PI));
	DrawDoor();
	glPopMatrix();

	//画
	glPushMatrix();
	glTranslatef(9.9, 10, 0);
	glRotatef(180, 0, 1, 0);
	glRotatef(-90, 1 ,0, 0);
	glScalef(0.1,4.0, 4.0); 
	DrawPaint();
	glPopMatrix();

	//大灯
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	GLUquadricObj *lquadratic;
	lquadratic = gluNewQuadric();
	gluQuadricTexture(lquadratic, TRUE);
	glBindTexture(GL_TEXTURE_2D, texName[9]);
	glTranslatef(0, 15, 0);
	glRotatef(90, 1, 0, 0);
	gluCylinder(lquadratic, 1.0f, 1.0f, 1.0f, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//灯泡
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	GLUquadricObj *quadObj2 = gluNewQuadric();
	gluQuadricTexture(quadObj2, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texName[10]);
	glTranslatef(0, 14.4, 0);
	gluSphere(quadObj2, 0.6f, 100, 100);  
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	static int slow = 0;
	if (playVideo){
		float x = 2, y = 1.1;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, avi[frameNum]);
		glPushMatrix();
		glTranslatef(0.1, 3.7, -5.85);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-x, -y);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( x, -y);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( x,  y);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-x,  y);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		if (slow < 1){
			slow++;
		}
		else{
			slow = 0;
			if (frameNum <ALLFRAME-1)frameNum++;
			else frameNum = 0;
		}
	}

	// 显示AB碰撞盒的代码
	//for (int i = 0; i < 7; i++){
	//	drawAB(i);
	//}

}