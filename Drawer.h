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
	//��һ����̬�ı��������������������Աߵ������Ե͵�������Ƭ
	//Ҫ����ֱ�ӵ����������������ʾ������
	void Draw_Background();

	//Draw the force strip
	void Draw_Force_Strip(double force);

	//Draw Direction Line
	void Draw_Direction_Line(double direc);

	//��ʾ����
	void Display(double s1, double s2, double fraction);


	//Draw one curling
	//����һ�������Ĳ�������ָ��λ�ý��仭����
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