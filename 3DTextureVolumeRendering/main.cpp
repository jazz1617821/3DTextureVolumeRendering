#include "GLSLSetup.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <iostream>

using namespace std;

#define FileString "engine.raw"
#define DATA_X 149
#define DATA_Y 208
#define DATA_Z 110
#define FileType unsigned char

FileType buf[DATA_Z*DATA_Y*DATA_X];
float texture3D[DATA_Z*DATA_Y*DATA_X*4];
float MaxValue = 0.0;

float eye_x, eye_y = -300, eye_z;
float pi = 3.1415926;
int eyeRot = 0;
int ceta = 90;
int R = DATA_Z*4;
float mvMatrix[16];
float u[3], v[3];
float wMax = -1000.0, wMin = 1000.0;
float sampleRate = 200, dd = 0;
float bbSide = 0.0;

/*-----Create image space for textures -----*/
unsigned int   textName[2];

float mColor[256*4];

void fileOpen() {
	cout << "Openong file begin." << endl;

	FILE* pInput = fopen(FileString, "rb");
	//FILE* pOutput = NULL;
	if (!pInput) {
		cout << "ÀÉ®×¶}±Ò¥¢±Ñ...\n" << endl;
		exit(1);
	}

	fread(buf, sizeof(FileType), DATA_X*DATA_Y*DATA_Z, pInput);

	fclose(pInput);

	for (int i = 0; i < DATA_X*DATA_Y*DATA_Z; i++) {
		if (buf[i] > MaxValue) {
			MaxValue = buf[i];
		}
	}

	for (int i = 0; i < DATA_X*DATA_Y*DATA_Z; i++) {
		float v = buf[i] / MaxValue;
		texture3D[i * 4 + 0] = v;
		texture3D[i * 4 + 1] = v;
		texture3D[i * 4 + 2] = v;
		texture3D[i * 4 + 3] = v;
	}
	

	cout << "Opening file done." << endl;

}

void makeColorMap() {
	for (int i = 0; i < 256; i++) {
		mColor[i * 4 + 0] = (i >= 0 && i <= 91) ? sin(i*pi / 91) : 0.0;
		mColor[i * 4 + 1] = (i >= 82 && i <= 173) ? sin((i - 82)*pi / 91) : 0.0;
		mColor[i * 4 + 2] = (i >= 164 && i < 256) ? sin((i - 164)*pi / 91) : 0.0;
		if (i > 15 && i <= 91) {
			mColor[i * 4 + 3] = sin(i*pi / 91) * (10 / sampleRate) * 2;
		}
		else if (i >= 82 && i <= 173) {
			mColor[i * 4 + 3] = sin((i - 82)*pi / 91) * (10.0 / sampleRate) * 3;
		}
		else if(i >= 164 && i < 256) {
			mColor[i * 4 + 3] = sin((i - 164)*pi / 91) * (10.0 / sampleRate) * 4;
		}
		else {
			mColor[i * 4 + 3] = 0;
		}
	}
}

void Texture()
{
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_1D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(2, textName);

	glUniform1i(glGetUniformLocation(ReturnProgramID(0), "volume_texunit"), 0);
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_3D, textName[0]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, DATA_X, DATA_Y, DATA_Z, 0, GL_RGBA, GL_FLOAT, texture3D);
	
	glUniform1i(glGetUniformLocation(ReturnProgramID(0), "trfunc_texunit"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, textName[1]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, mColor);
	
	glDisable(GL_TEXTURE_3D);
	glDisable(GL_TEXTURE_1D);
}

void Light() {

	float light0_amb[4] = { 0.0,0.0,0.0,1 };
	float light0_dif[4] = { 1.0,1.0,1.0,1 };
	float light0_spe[4] = { 0.3,0.3,0.3,1 };
	float light0_dir[4] = { 0, 0, 600, 0.0 };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_spe);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_dir);

	glEnable(GL_COLOR_MATERIAL);
}

void Init()
{
	ceta = (ceta + 1) % 360;
	eye_x = R*cos(ceta*(pi / 180));
	eye_z = R*sin(ceta*(pi / 180));
	bbSide = sqrt(pow(DATA_X, 2) + pow(DATA_Y, 2) + pow(DATA_Z, 2)) + 1;

	fileOpen();
	makeColorMap();
	Texture();
	//Light();
	glClearColor(0.0, 0.0, 0.0, 1);
	//projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 2500);
	//Blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

void drawBound() {
	glColor4f(1.0, 1.0, 1.0,1.0);
	glBegin(GL_LINES);

	glVertex3d(0, 0, 0);
	glVertex3d(DATA_X, 0, 0);

	glVertex3d(DATA_X, 0, 0);
	glVertex3d(DATA_X, DATA_Y, 0);

	glVertex3d(DATA_X, DATA_Y, 0);
	glVertex3d(0, DATA_Y, 0);

	glVertex3d(0, DATA_Y, 0);
	glVertex3d(0, 0, 0);

	glVertex3d(0, 0, DATA_Z);
	glVertex3d(DATA_X, 0, DATA_Z);

	glVertex3d(DATA_X, 0, DATA_Z);
	glVertex3d(DATA_X, DATA_Y, DATA_Z);

	glVertex3d(DATA_X, DATA_Y, DATA_Z);
	glVertex3d(0, DATA_Y, DATA_Z);

	glVertex3d(0, DATA_Y, DATA_Z);
	glVertex3d(0, 0, DATA_Z);

	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, DATA_Z);

	glVertex3d(DATA_X, 0, 0);
	glVertex3d(DATA_X, 0, DATA_Z);

	glVertex3d(DATA_X, DATA_Y, 0);
	glVertex3d(DATA_X, DATA_Y, DATA_Z);

	glVertex3d(0, DATA_Y, 0);
	glVertex3d(0, DATA_Y, DATA_Z);
	glEnd();
}

void drawBB(float x, float y, float z) {
	u[0] = mvMatrix[0];
	u[1] = mvMatrix[4];
	u[2] = mvMatrix[8];
	
	v[0] = mvMatrix[4] * mvMatrix[10] - mvMatrix[8] * mvMatrix[6];
	v[1] = mvMatrix[8] * mvMatrix[2] - mvMatrix[0] * mvMatrix[10];
	v[2] = mvMatrix[0] * mvMatrix[6] - mvMatrix[4] * mvMatrix[2];
	
	float p0[3], p1[3], p2[3], p3[3];
	p0[0] = x - (bbSide / 2)*u[0] - (bbSide / 2)*v[0];
	p0[1] = y - (bbSide / 2)*u[1] - (bbSide / 2)*v[1];
	p0[2] = z - (bbSide / 2)*u[2] - (bbSide / 2)*v[2];
	p1[0] = x + (bbSide / 2)*u[0] - (bbSide / 2)*v[0];
	p1[1] = y + (bbSide / 2)*u[1] - (bbSide / 2)*v[1];
	p1[2] = z + (bbSide / 2)*u[2] - (bbSide / 2)*v[2];
	p2[0] = x + (bbSide / 2)*u[0] + (bbSide / 2)*v[0];
	p2[1] = y + (bbSide / 2)*u[1] + (bbSide / 2)*v[1];
	p2[2] = z + (bbSide / 2)*u[2] + (bbSide / 2)*v[2];
	p3[0] = x - (bbSide / 2)*u[0] + (bbSide / 2)*v[0];
	p3[1] = y - (bbSide / 2)*u[1] + (bbSide / 2)*v[1];
	p3[2] = z - (bbSide / 2)*u[2] + (bbSide / 2)*v[2];

	glEnable(GL_TEXTURE_3D);
	glBegin(GL_POLYGON);
	glTexCoord3f(((p0[0] + (DATA_X / 2))*1.0 / DATA_X), ((p0[1] + (DATA_Y / 2)) / DATA_Y), ((p0[2] + (DATA_Z / 2)) / DATA_Z));
	glVertex3fv(p0);
	glTexCoord3f(((p1[0] + (DATA_X / 2))*1.0 / DATA_X), ((p1[1] + (DATA_Y / 2)) / DATA_Y), ((p1[2] + (DATA_Z / 2)) / DATA_Z));
	glVertex3fv(p1);
	glTexCoord3f(((p2[0] + (DATA_X / 2))*1.0 / DATA_X), ((p2[1] + (DATA_Y / 2)) / DATA_Y), ((p2[2] + (DATA_Z / 2)) / DATA_Z));
	glVertex3fv(p2);
	glTexCoord3f(((p3[0] + (DATA_X / 2))*1.0 / DATA_X), ((p3[1] + (DATA_Y / 2)) / DATA_Y), ((p3[2] + (DATA_Z / 2)) / DATA_Z));
	glVertex3fv(p3);
	glEnd();
	glDisable(GL_TEXTURE_3D);
}

float distance(float a, float b, float c) {
	return fabs((a - eye_x)* mvMatrix[2] + (b - eye_y) * mvMatrix[6] + (c - eye_z) * mvMatrix[10]);
}

void computeWmaxWmin() {
	float p0[3] = { -DATA_X / 2, -DATA_Y / 2, -DATA_Z / 2 };
	float p1[3] = { +DATA_X / 2, -DATA_Y / 2, -DATA_Z / 2 };
	float p2[3] = { +DATA_X / 2, -DATA_Y / 2, +DATA_Z / 2 };
	float p3[3] = { -DATA_X / 2, -DATA_Y / 2, +DATA_Z / 2 };
	float p4[3] = { -DATA_X / 2, +DATA_Y / 2, -DATA_Z / 2 };
	float p5[3] = { +DATA_X / 2, +DATA_Y / 2, -DATA_Z / 2 };
	float p6[3] = { +DATA_X / 2, +DATA_Y / 2, +DATA_Z / 2 };
	float p7[3] = { -DATA_X / 2, +DATA_Y / 2, +DATA_Z / 2 };

	float v[8] = { distance(p0[0], p0[1], p0[2]),
				distance(p1[0], p1[1], p1[2]),
				distance(p2[0], p2[1], p2[2]),
				distance(p3[0], p3[1], p3[2]),
				distance(p4[0], p4[1], p4[2]),
				distance(p5[0], p5[1], p5[2]),
				distance(p6[0], p6[1], p6[2]),
				distance(p7[0], p7[1], p7[2]) };
	for (int i = 0; i < 8; i++) {
		if (v[i] > wMax) {
			wMax = v[i];
		}
		if (v[i] < wMin) {
			wMin = v[i];
		}
	}
	dd = (wMax - wMin) / sampleRate;
}

void volumeRendering() {
	
	for (float i = wMax; i >= wMin + dd; i -= dd) {
		drawBB(eye_x - i*mvMatrix[2], eye_y - i*mvMatrix[6], eye_z - i*mvMatrix[10]);
	}
	
	//drawBB(0.0, 0.0, 0.0);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	//Camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye_x, eye_y, eye_z, 0, 0, 0, 0, -1, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, mvMatrix);
	//glUseProgram(0);
	
	glUseProgram(ReturnProgramID(0));
	computeWmaxWmin();
	volumeRendering();

	/*
	glUseProgram(0);
	glPushMatrix();
	glTranslated(-DATA_X / 2, -DATA_Y / 2, -DATA_Z / 2);
	drawBound();
	glPopMatrix();
	*/

	glutSwapBuffers();
}

void eyeRotate() {
	ceta = (ceta + 1) % 360;
	eye_x = R*cos(ceta*(pi / 180));
	eye_z = R*sin(ceta*(pi / 180));
}

void Timer(int c)
{
	if (eyeRot) {
		eyeRotate();
	}
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 0);
}

void KeyboardDown(unsigned char c, int x, int y)
{
	if (c == ' ') {
		eyeRot = (eyeRot + 1) % 2;
	}
	glFinish();
}

void Mouse(int button, int state, int x, int y)
{
	glFinish();
}

void Motion(int x, int y)
{
	glutPostRedisplay();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Volume Rendering");
	glutDisplayFunc(Display);

	glutKeyboardFunc(KeyboardDown);
	glutMotionFunc(Motion);
	glutMouseFunc(Mouse);

	glutTimerFunc(50, Timer, 0);
	glewInit();
	

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
	}
	SetGLShaderLanguage("GLSL.vert", "GLSL.frag");

	Init();
	glutMainLoop();
}