// ConsoleApplication3.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <windows.h>
#include <GL\glut.h>
#include <cstring>
#include "button.h"
#include "Texture.h"
#include "off.h"
//如果用全局坐标系的思想来考虑问题，那么必须要注意矩阵乘法的顺序和代码中的顺序是相反的。而用局部坐标系来思考的话，所有的操作都是针对于当前不断变化的坐标系，因此，矩阵乘法很自然的与他们再代码中出现的顺序一样。
//glPushMatrix()和glPopMatrix()可以消除上一次变换对本次变换的影响，使得本次变换都是基于世界坐标系的原点为参考点进行的。因此，实现太阳系，只需要把太阳先画出（默认在世界坐标系原点），然后保存当前视图，绘制行星，恢复视图，这种思路采用了全局坐标系的思想。
using std::vector;
using std::pair;

#define MAX_PATH_NAME 128

Off cone;
Off z;
const char *filename1 = "cone.off";
const char *filename2 = "z.off";
bool flag = 0;
bool xifen = 0;

GLfloat windowLeft, windowDown;
void reshape(int w, int h)
{//自动传入新窗口的大小（像素）
	glViewport(0, 0, w, h);//在窗口中显示的区域：显示区域左下角坐标显示区域的宽度与高度
	glMatrixMode(GL_PROJECTION);//申明将要进行投影设置操作
	glLoadIdentity();//重置变换矩阵

					 //使用正交投影观察物体，观察方式是平行修剪空间
	if (w <= h) {//这里使无论宽高比如何，始终观察窗口那么大的一块区域
		glOrtho(-2, 2, -2.0*(GLfloat)h / (GLfloat)w, 2.0*(GLfloat)h / (GLfloat)w, 0, 20);
		windowLeft = -2;
		windowDown = -2.0*(GLfloat)h / (GLfloat)w;
		//定义观察空间的：左、右、下、上、前、后
	}
	else {
		glOrtho(-2.0*(GLfloat)w / (GLfloat)h, 2.0*(GLfloat)w / (GLfloat)h, -2.0, 2.0, 0, 20);
		windowLeft = -2.0*(GLfloat)w / (GLfloat)h;
		windowDown = -2;
	}

	//也可以使用透视投影投影观察体
	//gluPerspective(90, w / h, 0.1, 6);//这里参数还需要调整
	//定义观察空间的垂直角度，宽高比，前截面，后界面

	glMatrixMode(GL_MODELVIEW);//申明将进行视景设置操作
	glLoadIdentity();//重置变换矩阵
					 //ps.当参数是GL_TEXTURE时可以进行纹理操作
}



void setLight() {//添加光照
	GLfloat lmodel_ambient[] = { 1,1,1,1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	GLfloat light_position[] = { 2,1,1,0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light[] = { 1,1,1,1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, light);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
}



void showHint() {//显示提示
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glRotatef(90, 1, 0, 0);
	glRotatef(90, 0, 1, 0);
	button hint(windowLeft, windowDown, 2.26, 0.7);
	hint.setTex(1);
	char localPath[MAX_PATH_NAME];
	GetCurrentDirectoryA(MAX_PATH_NAME, localPath);
	strcat(localPath, "\\");
	strcat(localPath, "set.bmp");
	hint.setTexImage(localPath);
	hint.show();
	glPopMatrix();
}


enum Model { CUBE_SLOID, CUBE_LINE, CUBE_POINT, BALL, EARTH}model;
vector<pair<int, GLfloat>> rotateLog;
const GLfloat axis[3][3] = { { 1.0,0.0,0.0 },{ 0.0,1.0,0.0 },{ 0.0,0.0,1.0 } };
GLfloat bigger = 1.0;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空颜色、三维缓存？
	glMatrixMode(GL_MODELVIEW);//申明将进行视景设置操作
	glLoadIdentity();//重置变换矩阵
	gluLookAt(15, 0, 0, 0, 0, 0, 0, 0, 1);//摄像机的xyz，观察点的xyz，相机上方向的xyz
	setLight();//设置光照
	glPushMatrix();
	for (int i = rotateLog.size() - 1; i >= 0; i--)
		glRotatef(rotateLog[i].second, axis[rotateLog[i].first][0], axis[rotateLog[i].first][1], axis[rotateLog[i].first][2]);
	//设置旋转

	glScalef(bigger, bigger, bigger);//设置放大
	switch (model) {//显示模型
	case EARTH:
		if (flag)flag = 0;
		else flag = 1;
		xifen = 0;
	case BALL://设置为球观察光照
		if (flag && xifen)
		{
			//z.show();
			z.loop();
			//z.show();
		}
		else if(xifen)
		{
			//cone.show();
			cone.loop();
			//cone.show();
		}
		model = CUBE_SLOID;
	case CUBE_SLOID://设置面着色模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(0.2, 0.2, 0.8);
		if (flag)z.draw(1);
		else cone.draw(1);
	case CUBE_LINE://设置边着色模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.5, 0.5, 0.5);
		glColor3f(0.5, 0.5, 0.5);
		if (flag)z.draw(1);
		else cone.draw(1);
	case CUBE_POINT://设置点着色模式
		glColor3f(1, 1, 1);
		glPointSize(2);
		if (flag)z.draw(0);
		else cone.draw(0);
		break;
	}
	glPopMatrix();
	showHint();//显示提示
	glutSwapBuffers();//置换缓存（刷新画面，因为使用了双缓存）
}


void logRotate(int axis, int count) {//记录旋转情况
	if (rotateLog.size() == 0)
		rotateLog.push_back(pair<int, GLfloat>(axis, count));
	else if (rotateLog.back().first == axis) {
		rotateLog.back().second += count;
		if ((int)rotateLog.back().second % 360 == 0)
			rotateLog.pop_back();
	}
	else rotateLog.push_back(pair<int, GLfloat>(axis, count));
}

void keyboard(unsigned char key, int _x, int _y) {
	if (key == 'u')
		rotateLog.clear();
	if (key == 'i')// && model != BALL)
		rotateLog.clear();
	if ((key == 'j' || key == 'k' || key == 'l') && (model == EARTH || model == BALL))
		rotateLog.clear();
	//model = CUBE_SLOID;
	switch (key)
	{//响应切换模型事件
	case 'j':
		model = CUBE_SLOID;
		break;
	case 'k':
		model = CUBE_LINE;
		break;
	case 'l':
		model = CUBE_POINT;
		break;
	case 'u':
		model = EARTH;
		break;
	case 'i':
		model = BALL;
		xifen = 1;
		break;
	}
	switch (key)
	{//响应旋转事件
	case 'q':
		logRotate(0, 1);
		break;
	case 'e':
		logRotate(0, -1);
		break;
	case 'w':
		logRotate(1, -1);
		break;
	case 's':
		logRotate(1, 1);
		break;
	case 'a':
		logRotate(2, -1);
		break;
	case 'd':
		logRotate(2, 1);
		break;
	}
	switch (key)
	{
	case 'r':
		bigger *= 1.01;
		break;
	case 'f':
		bigger /= 1.01;
		if (bigger == 0)
			bigger = 1;
		break;
	}
	glutPostRedisplay();//刷新视图
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);//初始化GLUT库；
	cone.readoff(filename1);
	z.readoff(filename2);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//双缓存模式、RGB模式、三维模式？
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1200, 1000);//设置窗口大小
	glutCreateWindow("Loop subdivision");//设置窗口名称
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//双面光栅化
	glEnable(GL_DEPTH_TEST);//开启三维模式？
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(3);//使用3维光栅化
	glutReshapeFunc(reshape);//设置当窗口大小发生改变时调用的函数
	glutDisplayFunc(display);//设置刷新函数
	glutKeyboardFunc(keyboard);//设置键盘响应事件
	glutMainLoop();//进入自动刷新循环
	return 0;
}