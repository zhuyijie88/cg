#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include "gl/glut.h"
#include <gl/glext.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "curling.h"
#include "CurlingManager.h"
#include "Drawer.h"
#include "Timer.h"
using namespace std;
#define pi 3.1415926
#define BMP_Header_Length 54  

CurlingManager cm;
Drawer dr;
Timer timer;

float fTranslate;
float fRotate;
float fScale = 1.0f;	// set inital scale value to 1.0f


double Frac = 0.6;
bool Allstop = true;
bool bPersp = true;
bool bAnim = false;
bool bWire = false;
bool is_Mix = false;
bool my_tex = false;
int wHeight = 0;
int wWidth = 0;
double Force = 2;
double Theta = -pi/2;


enum STATE{
	newturn,
	ready,
	moving,
	finish,
	movingtofinish,
	allfinish
};

STATE Game_State = newturn;

bool movebegin = false;
bool viewequal;

float eye[] = { 0, 3, 8 };
float center[] = { 0, 0, 0 };
float lasteye[] = { 0, 3, 8 };
float lastcenter[] = { 0, 0, 0 };
float dirc[] = { 0, 1, 0 };
float lastdirc[] = { 0, 0, -1};
GLfloat light_pos[] = { 0, 3, 6, 1 };

GLuint texture[9];

GLint CurlingList = 1;
extern short face_indicies[20288][6];
extern GLfloat vertices[10148][3];
extern  GLfloat normals[9096][3];
static void init_Curling_List()
{
	CurlingList = glGenLists(1);
	glNewList(CurlingList, GL_COMPILE);
	glTranslatef(0, 0.1, 6.7);
	glScalef(1.5, 1, 1.5);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i<(sizeof(face_indicies) / sizeof(face_indicies[0])); i++)
	{
		for (int j = 0; j<3; j++)
		{
			int vi = face_indicies[i][j];
			int ni = face_indicies[i][j + 3];//Normal index
			//glNormal3fv(&normals[ni][0]);
			//glVertex3fv(&vertices[vi][0]);
			GLfloat glTemp[3];
			GLfloat glScale = 2.0f;
			glTemp[0] = normals[ni][0] / glScale;
			glTemp[1] = normals[ni][1] / glScale;
			glTemp[2] = normals[ni][2] / glScale;
			glNormal3fv(&glTemp[0]);
			glTemp[0] = vertices[vi][0] / glScale;
			glTemp[1] = vertices[vi][1] / glScale;
			glTemp[2] = vertices[vi][2] / glScale;
			glVertex3fv(&glTemp[0]);
		}
	}
	glEnd();
	glEndList();

}


void readPixels() {

	BITMAPFILEHEADER bitmapFileHeader;
	memset(&bitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = 0x4d42;	//BM
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	//填充BITMAPINFOHEADER
	BITMAPINFOHEADER bitmapInfoHeader;
	memset(&bitmapInfoHeader, 0, sizeof(BITMAPINFOHEADER));
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = wWidth;
	bitmapInfoHeader.biHeight = wHeight;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = wWidth * abs(wHeight) * 3;

	FILE * filePtr;			//连接要保存的bitmap文件用
	unsigned char tempRGB;	//临时色素
	int imageIdx;

	unsigned char* bitmapData = new unsigned char[wWidth*wHeight*3];

	glReadPixels(0, 0, wWidth, wHeight, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

	//交换R、B的像素位置,bitmap的文件放置的是BGR,内存的是RGB
	for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapData[imageIdx];
		bitmapData[imageIdx] = bitmapData[imageIdx + 2];
		bitmapData[imageIdx + 2] = tempRGB;
	}
	int nametime = timer.getTime();
	char str[20];
	sprintf(str, "%d", nametime);
	string name = string(str);
	name += ".bmp";
	cout << "Screen Shot: " <<  name.c_str() << endl;
	filePtr = fopen(name.c_str(), "wb");


	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	fwrite(bitmapData, bitmapInfoHeader.biSizeImage, 1, filePtr);

	fclose(filePtr);

	delete bitmapData;
}


void updateView(int width, int height)
{
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	if (bPersp) {
		gluPerspective(60.0f, whRatio, 0.1f, 100.0f);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	wHeight = height;
	wWidth = width;

	updateView(wHeight, wWidth);
}

void idle()
{
	glutPostRedisplay();
}




void key(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 27:
	case 'q': {exit(0); break; }
	case ' ': {
				  if (!Allstop)
					  break;
				  else{
					  Allstop = false;
					  movebegin = true;
					  break;
				  }
	}

	case 'c': {
				  readPixels();
				  break;
	}
	case  'w':{
				  Force += 0.1;
				  //cout << "Force=" << Force << endl;
				  break;
	}
	case  's':{
				  Force -= 0.1;
				  //cout << "Force=" << Force << endl;
				  break;
	}

	case  'a':{
				  Theta -= 0.01;
				  //cout << "Theta=" << Theta << endl;
				  break;
	}
	case  'd':{
				  Theta += 0.01;
				  //cout << "Theta=" << Theta << endl;
				  break;
	}

	case 'f':{
				 if (Frac == 0.6)
					 Frac = 0.2;
				 else Frac = 0.6;
				 break;
	}

		/*************** 2017.01.11 ***************/

	case  'i':{
				  light_pos[2] -= 0.5;
				  break;
	}
	case  'k':{
				  light_pos[2] += 0.5;
				  break;
	}
	case  'j':{
				  light_pos[0] -= 0.5;
				  break;
	}
	case  'l':{
				  light_pos[0] += 0.5;
				  break;
	}
		/*************** 2017.01.11 ***************/

	}
	updateView(wHeight, wWidth);
}

void smooth(){
	double eps1 = 0.01;
	double eps2 = 0.01;
	double eps3 = 0.3;
	double gap1, gap2, gap3;
	viewequal = true;

	gap1 = (eye[0] - lasteye[0])*(eye[0] - lasteye[0]) +
		(eye[1] - lasteye[1])*(eye[1] - lasteye[1]) +
		(eye[2] - lasteye[2])*(eye[2] - lasteye[2]);
	gap1 = sqrt(gap1);

	gap2 = (center[0] - lastcenter[0])*(center[0] - lastcenter[0]) +
		(center[1] - lastcenter[1])*(center[1] - lastcenter[1]) +
		(center[2] - lastcenter[2])*(center[2] - lastcenter[2]);
	gap2 = sqrt(gap2);

	gap3 = (dirc[0] - lastdirc[0])*(dirc[0] - lastdirc[0]) +
		(dirc[1] - lastdirc[1])*(dirc[1] - lastdirc[1]) +
		(dirc[2] - lastdirc[2])*(dirc[2] - lastdirc[2]);
	gap3 = sqrt(gap3);

	if (gap1 > eps1){
		eye[0] = lasteye[0] + eps1 / gap1*(eye[0] - lasteye[0]);
		eye[1] = lasteye[1] + eps1 / gap1*(eye[1] - lasteye[1]);
		eye[2] = lasteye[2] + eps1 / gap1*(eye[2] - lasteye[2]);
		viewequal = false;
	}

	if (gap2 > eps2){
		center[0] = lastcenter[0] + eps2 / gap2*(center[0] - lastcenter[0]);
		center[1] = lastcenter[1] + eps2 / gap2*(center[1] - lastcenter[1]);
		center[2] = lastcenter[2] + eps2 / gap2*(center[2] - lastcenter[2]);
		viewequal = false;
	}
	/*
	if (gap3 > eps3){
		dirc[0] = lastdirc[0] + eps3 / gap3*(dirc[0] - lastdirc[0]);
		dirc[1] = lastdirc[1] + eps3 / gap3*(dirc[1] - lastdirc[1]);
		dirc[2] = lastdirc[2] + eps3 / gap3*(dirc[2] - lastdirc[2]);
		viewequal = false;
	}
	*/
	

	lasteye[0] = eye[0];
	lastcenter[0] = center[0];
	lastdirc[0] = dirc[0];
	lasteye[1] = eye[1];
	lastcenter[1] = center[1];
	lastdirc[1] = dirc[1];
	lasteye[2] = eye[2];
	lastcenter[2] = center[2];
	lastdirc[2] = dirc[2];
}

void view_ready(){
	eye[0] = 0;
	eye[1] = 2.5;
	eye[2] = 7;
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;
	dirc[0] = 0;
	dirc[1] = 1;
	dirc[2] = 0;
	lasteye[0] = eye[0];
	lastcenter[0] = center[0];
	lastdirc[0] = dirc[0];
	lasteye[1] = eye[1];
	lastcenter[1] = center[1];
	lastdirc[1] = dirc[1];
	lasteye[2] = eye[2];
	lastcenter[2] = center[2];
	lastdirc[2] = dirc[2];
	viewequal = true;
}

void view_moving(int index){
	curling c = cm.getcurling(index);
	center[0] = c.get_X()*0.2;
	center[1] = 0.1*0.4;
	center[2] = (c.get_Z() + 6.7)*0.33;
	double vx = c.get_VX()*0.2;
	double vz = c.get_VZ()*0.33;
	if (vz > 0) vz = -vz;
	if (vx == 0 && vz == 0){
		eye[0] = lasteye[0];
		eye[1] = lasteye[1];
		eye[2] = lasteye[2];
	}
	else{
		eye[1] = 1;
		double dist = 1.5;
		eye[0] = center[0] - dist*vx / sqrt(vx*vx + vz*vz);
		eye[2] = center[2] - dist*vz / sqrt(vx*vx + vz*vz);
	}
	smooth();
}

void view_movingtofinish(){
	center[0] = 0;
	center[1] = 0.1*0.4;
	center[2] = -6.7*0.33;
	eye[0] = 0;
	eye[1] = 1.5;
	eye[2] = -6.7*0.33;
	dirc[0] = 0;
	dirc[1] = 0;
	dirc[2] = -1;
	/*
	lastcenter[0] = center[0];
	lastcenter[1] = center[1];
	lastcenter[2] = center[2];
	lasteye[0] = eye[0];
	lasteye[1] = eye[1];
	lasteye[2] = eye[2];
	viewequal = true;
	*/
	smooth();
}




void redraw()
{
	static int curling_index = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		dirc[0], dirc[1], dirc[2]);				// 场景（0，0，0）的视点中心 (0,4,10)，Y轴向上

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glEnable(GL_LIGHT0);

	//Display the score
	glScalef(0.2, 0.4, 0.33);
	dr.Draw_Background();
	cm.show_all_curlings();
	dr.Display(cm.getscore0(), cm.getscore1(), Frac);
	//glLoadIdentity();

	double costtime = 0;
	static int disponce = 1;

	switch (Game_State){
	case newturn:
		cm.addCurling(0, 0, cm.getState());
		Game_State = ready;
		Force = 2.0; Theta = -pi / 2;
		break;
	case ready:
		view_ready();
		dr.Draw_Direction_Line(Theta);
		dr.Draw_Force_Strip(Force);
		if (movebegin){
			curling_index = cm.size() - 1;
			cm.SetInitSpeed(curling_index, Theta, Force);
			Allstop = false;
			movebegin = false;
			timer.Start();
			Game_State = moving;
		}
		break;
	case moving:
		timer.End();
		view_moving(curling_index);
		costtime = timer.costTime;
		timer.Reset();
		timer.Start();
		cm.update(costtime, Frac);
			
		if (Allstop){
			Sleep(1000);
			Game_State = movingtofinish;
		}
		break;
	case movingtofinish:
		view_movingtofinish();
		if (viewequal){
			Game_State = finish;
		}
		break;
	case finish:
		timer.Reset();
		cm.Calculate_Scores();
		Sleep(3000);
		if (cm.size() == 8)
			Game_State = allfinish;
		else {
			cm.teamchange();
			Game_State = newturn;
		}
		break;

	case allfinish:
		view_ready();
		if (disponce == 1){
			disponce--;
			cout << "Game Over!" << endl;
			cout << "Team1: " << cm.getscore0() << "分" << endl;
			cout << "Team2: " << cm.getscore1() << "分" << endl;
			if (cm.getscore0() > cm.getscore1())
				cout << "Team1 获胜!" << endl;
			else if (cm.getscore0() == cm.getscore1())
				cout << "平局!" << endl;
			else
				cout << "Team2 获胜!" << endl;

		}
		
	}
	


	glutSwapBuffers();

}


int main(int argc, char *argv[])
{
	cout << "************冰壶游戏**************" << endl;
	cout << "\n\n";
	cout << "******游戏规则********" << endl;
	cout << "" << endl;
	cout << "游戏一共分四轮，每轮两个玩家轮流操作一个冰壶，通过W和S键调节初始速度的大小，通过A和D键调整初始速度方向；\n";
	cout << "在冰壶滑动过程中可以通过按F键来模拟摩擦，切换摩擦力状态；\n\n";
	cout << "最终目标是使自己的四个冰壶尽可能靠近目标圆心，根据与目标圆心的距离排序从小到大进行排序，最近的冰壶获得8分，其次7分，以此类推。\n\n";
	cout << "注意：滑出或者被装出冰壶赛道的冰壶不参与计分！\n\n";
	cout <<	"按回车键开始游戏。" << endl;
	if (getchar() == 'q')
		return 0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(480, 480);
	int windowHandle = glutCreateWindow("BINGHU App");
	init_Curling_List();
	dr.tex_init();
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}


