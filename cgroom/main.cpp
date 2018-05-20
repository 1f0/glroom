#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include "obj.h"
#include <math.h>
#include <ctime>
#include "drawScene.h"
#include "global.h"
#include "collision.h"
#include "Snapshot.h"
#include "shading.h"


#define ABNUM 10
using namespace std;

bool keystates[256];

GLfloat shdMat[4][4];

float center2[3];
float eye2[3];

bool coliSw = true;
float theta = 0.0;
float elevation = 0.4;
bool bPersp = true;
bool bWire = false;
bool desklamp = false;
bool playVideo = true;
bool fogstat = false;

int mode = 0;

// global variable recording window size.
int wHeight = 0;
int wWidth = 0;

float EYESIGHT = 2;
float eye[] = { 0, 5, EYESIGHT };
float center[] = { 0, 5, 0 };

GLfloat light_pos0[] = { 0, 15, 0, 1 };
GLfloat light_pos[] = { 4.5, 2.3, 4, 1 };

AABB aabox[ABNUM];

void updateView(int width, int height)
{
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix
	glLoadIdentity();										// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	if (bPersp) {
		gluPerspective(70.0f, whRatio, 0.01f, 800.0f);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height == 0)									// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	wHeight = height;
	wWidth = width;

	updateView(wHeight, wWidth);
}

const float flighterH = 0.05;
const float flighterW = 0.2;

bool inroom = true;
bool notInRoom(){
	//cout << eye[0] << endl;cubex cubez
	if (cubey < 0.1 || cubey > 14.8) return true;
	if (cubez < -9.8 || cubez>9.8 || cubex > 9.8)return true;
	if (cubex < -9.8){
		if (cubey > 7.8)return true;
		if (cubez > 1.8 || cubez < -1.8){
			return true;
		}
		else if (cubex < -10.3)inroom = false;
	}
	return false;
}

bool coliWhenIn(){
	Vec3 min, max;
	min.set(-10, 0, -10);
	max.set(10, 15, 10);
	AABB tmp;
	tmp.set(min, max);
	if (aabox[0].intersects(tmp)){
		if (cubex > -10.2){
			if (cubey > 7.8)return true;
			if (cubez < 1.8 && cubez > -1.8){
				if (cubex > -9.9)inroom = true;
				return false;
			}
		}
		return true;
	}
	return false;
}

bool lineWithLine(float x1,float y1,float x2,float y2,float a1,float b1,float a2,float b2){
	//(x1,y1)
	float norm[2];//y1,-x1
	norm[0] = y2 - y1;
	norm[1] = x1 - x2;
	float v1[2];
	v1[0] = a2 - x1;
	v1[1] = b2 - y1;
	float inner = v1[0] * norm[0] + v1[1] * norm[1];
	v1[0] = a1 - x1;//redup used
	v1[1] = b1 - y1;
	float inner2 = v1[0] * norm[0] + v1[1] * norm[1];
	if (inner > 0 && inner2 > 0)return false;
	if (inner < 0 && inner2 < 0)return false;
	norm[0] = b2 - b1;
	norm[1] = a1 - a2;
	inner = v1[0] * norm[0] + v1[1] * norm[1];
	v1[0] = a1 - x2;
	v1[1] = b1 - y2;
	inner2 = v1[0] * norm[0] + v1[1] * norm[1];
	if (inner > 0 && inner2 > 0)return false;
	if (inner < 0 && inner2 < 0)return false;
	return true;
}

bool collisionDetect(){
	if (!coliSw)return false;
	if (inroom){
		if (cubey < 7.9&&cubex>-10.1){
			if ((cubex + 10)*(cubex + 10) + (cubez - 2)*(cubez - 2) < 19.36){
				//cout << "in" << endl;
				float tmp2 = tan((180-theta)*PI / 180);
				float tmp = (cubex + 10 - tmp2*(cubez - 2));
				tmp = tmp*tmp;
				if (tmp < (1 + tmp2*tmp2)*0.25)
					return true;
			}
		}
		if (notInRoom())return true;

		//detect collision with door
		//start from real box
		for (int i = 1; i < 7; i++){
			if (aabox[0].intersects(aabox[i])) {
				return true;
			}
		}
	}
	else{
		if (coliWhenIn())return true;
	}
	//detect collision with door
	return false;
}

void TimerFunction(int  value) {
	static int rotstate = 0;
	static float rotheta = 1.0;
	static float speed = 0.2;
	static bool speedstate = false;
	float para = 0.2;
	Vec3 tmp, tmp2;
	if (mode == 0){
		if (keystates['W'] || keystates['w']) {
			speedstate = true;
			if (speed < 0)speed = 0;
			else if (speed < 2)speed += 0.01;
			if (speed>0)rotstate = 1;
			//collision predict
			cubex -= speed*cos(movDirection);
			cubez -= speed*sin(movDirection);
			tmp.set(cubex + flighterW, cubey + flighterH, cubez + flighterW);
			tmp2.set(cubex - flighterW, cubey - flighterH, cubez - flighterW);
			aabox[0].set(tmp2, tmp);
			//Collision Test, now with exhaustive method
			if (collisionDetect()){
				speed = -speed;
				cubex -= 2 * speed*cos(movDirection);
				cubez -= 2 * speed*sin(movDirection);
			}
			center[0] = cubex;
			center[2] = cubez;
			eye[0] -= speed*cos(movDirection);
			eye[2] -= speed*sin(movDirection);
		}
		else if (keystates['S'] || keystates['s']) {
			speedstate = true;
			if (speed > 0)speed = 0;
			else if (speed > -1)speed -= 0.01;
			if (speed < 0) rotstate = -1;
			//collision predict
			cubex -= speed*cos(movDirection);
			cubez -= speed*sin(movDirection);
			tmp.set(cubex + flighterW, cubey + flighterH, cubez + flighterW);
			tmp2.set(cubex - flighterW, cubey - flighterH, cubez - flighterW);
			aabox[0].set(tmp2, tmp);
			if (collisionDetect()){
				speed = -speed;
				cubex -= 2 * speed*cos(movDirection);
				cubez -= 2 * speed*sin(movDirection);
			}
			center[0] = cubex;
			center[2] = cubez;
			eye[0] -= speed*cos(movDirection);
			eye[2] -= speed*sin(movDirection);
		}
		else if (speedstate){
			if (fabs(speed) > 0.05)speed *= 0.95;
			else {
				speedstate = false;
				speed = 0;
			}
			//collision predict
			cubex -= speed*cos(movDirection);
			cubez -= speed*sin(movDirection);
			tmp.set(cubex + flighterW, cubey + flighterH, cubez + flighterW);
			tmp2.set(cubex - flighterW, cubey - flighterH, cubez - flighterW);
			aabox[0].set(tmp2, tmp);
			if (collisionDetect()){
				speed = -speed;
				cubex -= 2 * speed*cos(movDirection);
				cubez -= 2 * speed*sin(movDirection);
			}
			center[0] = cubex;
			center[2] = cubez;
			eye[0] -= speed*cos(movDirection);
			eye[2] -= speed*sin(movDirection);
		}
		if (keystates['a'] || keystates['A']) {
			//if(speed<0)movDirection += 0.02;
			movDirection -= 0.02;
			eye[0] = center[0] + EYESIGHT*cos(movDirection);
			eye[2] = center[2] + EYESIGHT*sin(movDirection);
			rotheta = 0.1;
		}
		if (keystates['D'] || keystates['d']) {
			//if (speed<0)movDirection -= 0.02;
			movDirection += 0.02;
			eye[0] = center[0] + EYESIGHT*cos(movDirection);
			eye[2] = center[2] + EYESIGHT*sin(movDirection);
			rotheta = 0.1;
		}
		if (keystates['t'] || keystates['T']) {
			//collision predict
			cubey += 0.1f;
			eye[1] += 0.1f;
			tmp.set(cubex + flighterW, cubey + flighterH, cubez + flighterW);
			tmp2.set(cubex - flighterW, cubey - flighterH, cubez - flighterW);
			aabox[0].set(tmp2, tmp);
			if (collisionDetect()){
				cubey -= 0.1f;
				eye[1] -= 0.1f;
			}
			center[1] = cubey;
		}
		else if (keystates['G'] || keystates['g']) {
			cubey -= 0.1f;
			eye[1] -= 0.1;
			tmp.set(cubex + flighterW, cubey + flighterH, cubez + flighterW);
			tmp2.set(cubex - flighterW, cubey - flighterH, cubez - flighterW);
			aabox[0].set(tmp2, tmp);
			if (collisionDetect()){
				cubey += 0.1f;
				eye[1] += 0.1f;
			}
			center[1] = cubey;
		}
		if (fabs(objtheta - movDirection) > 0.01) {
			objtheta = objtheta + (movDirection - objtheta)*rotheta;
			if (rotheta < 1.0) rotheta += 0.05;
		}
	}
	//cout << cubex <<":"<<cubey<<":"<<cubez<< endl;
	//cout << eye[0] << ":" << eye[1] << ":" << eye[2] << endl;
	if (keystates['b']) {
		glEnable(GL_BLEND);			// 打开混合
		glDisable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_BLEND);		// 关闭混合
		glEnable(GL_DEPTH_TEST);	// 打开深度测试
	}
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}

void keyUp(unsigned char k, int x, int y) {
	keystates[k] = false;
	if (k >= 'A' && k <= 'Z') {
		keystates[k] = false;
		keystates[k + 'a' - 'A'] = false;
	}
	else if (k >= 'a' && k <= 'z') {
		keystates[k] = false;
		keystates[k - 'a' + 'A'] = false;
	}
	else keystates[k] = false;
}

int intensity = 0;
GLfloat dlcolor[] = { 0.0, 0.0, 0.0, 0.1 };

bool inCircle(){
	if ((light_pos[0] - 4.5)*(light_pos[0] - 4.5) + (light_pos[2] - 4)*(light_pos[2] - 4) < 1)
		return true;
	return false;
}

void key(unsigned char k, int x, int y)
{
	keystates[k] = true;
	float para = 0.2;
	switch (k)
	{
	case 27 : {exit(0); break; }
	case'O':case 'o': {bWire = !bWire; break; }

	case'n':case 'N': {
		if(theta<135)
		theta+=5; break; 
	}

	case'm':case 'M': {
		if (theta >0)
		theta-=5; break; 
	}
	case 'v':case 'V':{
		if (mode < 1){
			mode++;
		}
		else mode = 0;
	}
	case'Y':case'y': {
		intensity++;
		if (intensity == 1) {
			dlcolor[0] = 0.2;
			dlcolor[1] = 0.0;
			dlcolor[2] = 0.0;
			desklamp = !desklamp;
		}

		if (intensity == 2) {
			dlcolor[0] = 0.0;
			dlcolor[1] = 0.2;
			dlcolor[2] = 0.0;
		}

		if (intensity == 3) {
			dlcolor[0] = 0.0;
			dlcolor[1] = 0.0;
			dlcolor[2] = 0.2;
		}

		if (intensity == 4) {
			desklamp = !desklamp;
			intensity = 0;
		}
		break;
	}

	case 'J':case'j': {
		light_pos[0] -= 0.2*sin(movDirection);
		light_pos[2] += 0.2*cos(movDirection);
		if (!inCircle()){
			light_pos[0] += 0.2*sin(movDirection);
			light_pos[2] -= 0.2*cos(movDirection);
		}
		break;
	}
	case 'L':case'l': {
		light_pos[0] += 0.2*sin(movDirection);
		light_pos[2] -= 0.2*cos(movDirection);
		if (!inCircle()){
			light_pos[0] -= 0.2*sin(movDirection);
			light_pos[2] += 0.2*cos(movDirection);
		}
		break;
	}
	case 'I':case'i': {
		light_pos[0] -= 0.2*cos(movDirection);
		light_pos[2] -= 0.2*sin(movDirection);
		if (!inCircle()){
			light_pos[0] += 0.2*cos(movDirection);
			light_pos[2] += 0.2*sin(movDirection);
		}
		break;
	}
	case 'K':case'k': {
		light_pos[0] += 0.2*cos(movDirection);
		light_pos[2] += 0.2*sin(movDirection);
		if (!inCircle()){
			light_pos[0] -= 0.2*cos(movDirection);
			light_pos[2] -= 0.2*sin(movDirection);
		}
		break;
	}
	case'Q':case 'q': {
		if (EYESIGHT > 1.0) {
			EYESIGHT /= 1.05;
			eye[0] = center[0] + EYESIGHT*cos(movDirection);
			eye[2] = center[2] + EYESIGHT*sin(movDirection);
		}break;
	}
	case 'E':case 'e': {
		if (EYESIGHT < 8.0) {
			EYESIGHT *= 1.05;
			eye[0] = center[0] + EYESIGHT*cos(movDirection);
			eye[2] = center[2] + EYESIGHT*sin(movDirection);
		}break;
	}
	case 'R':case'r': {
		EYESIGHT = 2.0;
		eye[0] = center[0] + EYESIGHT*cos(movDirection);
		eye[2] = center[2] + EYESIGHT*sin(movDirection);
	}
	case 'z':case 'Z': {
		if (elevation < 1.5)elevation += 0.005;
		break;
	}
	case 'c':case 'C': {
		if (elevation > -1.5)elevation -= 0.005;
		break;
	}
	case 'x':case 'X': {
		coliSw = !coliSw;
		break;
	}
	case 'F':case 'f':{
		fogstat = !fogstat;
		break;
	}
	}
	updateView(wHeight, wWidth);
}

void redraw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	if (mode == 0){
		eye2[1] = center[1] + EYESIGHT*sin(elevation);
		eye2[0] = center[0] + EYESIGHT*cos(movDirection)*cos(elevation);
		eye2[2] = center[2] + EYESIGHT*sin(movDirection)*cos(elevation);
		gluLookAt(eye2[0], eye2[1], eye2[2],
			center[0], center[1], center[2],
			0, 1, 0);
	}
	else{
		eye2[1] = 10;
		eye2[0] = 6;
		eye2[2] = 0;
		center2[0] = 10;
		center2[1] = 10;
		center2[2] = 0;
		gluLookAt(eye2[0], eye2[1], eye2[2],
			center2[0], center2[1], center2[2],
			0, 1, 0);
	}


	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//台灯
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, dlcolor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dlcolor);
	if (desklamp) {
		glEnable(GL_LIGHT1);
	}
	else {
		glDisable(GL_LIGHT1);
	}

	DrawScene();

	glutSwapBuffers();
}



void ScreenshotListener(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F10:
		SaveScreenShot(wWidth, wHeight);
		printf("screenshot saved.\n");
		break;
	case GLUT_KEY_F11:
		playVideo = !playVideo;
		break;
	default:
		break;
	}
}

GLuint filter;											// 使用哪一个纹理过滤器
GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };		// 雾气的模式
GLuint fogfilter = 1;									// 使用哪一种雾气
GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };		// 雾的颜色设为白色


int main()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	int windowHandle = glutCreateWindow("Simple OpenGL Room");

	glClearColor(0.2f, 0.2f, 0.2f, 0.8f);			// 设置背景的颜色为雾气的颜色
	glFogi(GL_FOG_MODE, fogMode[fogfilter]);		// 设置雾气的模式
	glFogfv(GL_FOG_COLOR, fogColor);				// 设置雾的颜色
	glFogf(GL_FOG_DENSITY, 0.1f);			// 设置雾的密度
	glHint(GL_FOG_HINT, GL_DONT_CARE);		// 设置系统如何计算雾气
	glFogf(GL_FOG_START, 1.0f);				// 雾气的开始位置
	glFogf(GL_FOG_END, 5.0f);				// 雾气的结束位置

	init();
	GLfloat white[] = { 0.2, 0.2, 0.2, 1.0 };
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	GLfloat pts[3][3] = { { -3, 0.2, -2 }, { -3, 0.2, 2 }, { 4, 0.2, 2 } };
	makeShadowMat(pts, light_pos0, shdMat);

	//collision box
	Vec3 min, max;
	//sofa
	min.set(-3.5, 0, 6.6);
	max.set(3.5, 1.4, 8);
	aabox[1].set(min,max);

	min.set(-3., 0, 8.3);
	max.set(3., 2.5, 9);
	aabox[2].set(min, max);

	//chair
	min.set(4.8, 0, 7.5);
	max.set(7, 2.3, 6.2);
	aabox[3].set(min, max);

	//desk
	min.set(3.8, 0, 3.2);
	max.set(5.3, 2.8, 4.8);
	aabox[4].set(min, max);

	//tv desk
	min.set(-3, 0, -8);
	max.set(3, 1.8, -3.6);
	aabox[5].set(min, max);
	
	//tv
	min.set(-2.1, 1.8, -6.2);
	max.set(2.3, 5, -5.8);
	aabox[6].set(min, max);

	//
	object = loadOBJ("obj/Lamp/Lamp.obj");
	boy = loadOBJ("obj/ship.obj");
	sofa = loadOBJ("obj/Sofa/Sofa.obj");
	table1 = loadOBJ("obj/SideTable1/SideTable1.obj");
	sidesofa = loadOBJ("obj/ChairArm/ChairArm.obj");
	tv = loadOBJ("obj/tv/tv.obj");

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutIgnoreKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(ScreenshotListener);
	glutTimerFunc(33, TimerFunction, 1);

	glutMainLoop();
	return 0;
}
