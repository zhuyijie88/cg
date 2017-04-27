#ifndef _DRAWER_
#define _DRAWER_
#include "curling.h"
#include "gl/glut.h"
#include <Windows.h>

class Drawer
{
public:
	Drawer();
	~Drawer();

	//load the texture from bmp file.
	unsigned char* LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);

	//load the mixing texture
	void texloadmix(int i, char *filename1, char *filename2);

	//load the texture
	void texload(int i, char *filename);

	//Initialize the texture
	void tex_init();



	

	
	//Draw the constant background
	//画一个静态的背景，包括冰壶赛道、旁边的两个稍低的立方体片
	//要求我直接调用这个函数就能显示出背景
	void Draw_Background();

	//Draw the force strip
	void Draw_Force_Strip(double force);

	//Draw Direction Line
	void Draw_Direction_Line(double direc);

	//显示分数
	void Display(double s1, double s2, double fraction);


	//Draw one curling
	//输入一个冰壶的参数，在指定位置将其画出，
	void Draw_Curling(curling* c);
	void drawString(char* str);


private:
	void Tex_Cube(GLfloat n);
	void Draw_Border(float x);
	void Draw_Sheet(float x);
	void Draw_Carpet(float x, float z);
	void Draw_Plank(float l, float w);
	void Draw_Audience(float l, float w);

};

extern GLint CurlingList;
extern short face_indicies[20288][6];
extern  GLfloat vertices[10148][3];
extern  GLfloat normals[9096][3];

#endif