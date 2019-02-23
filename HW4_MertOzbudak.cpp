
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "veclib.h"

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500
#define BALL_SPEED 5
#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1    // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool invalid = false;
bool flag = false;
vec2_t R;

typedef struct {
	vec2_t pos;
	float x,y;
} point_t;
typedef struct{
	point_t C1, C2, C3, C4;
	vec2_t pos;
	float angle;
}wall_t;
typedef struct {
	float rad;
	vec2_t pos;
	vec2_t vel;  //velocity
	bool  visibility; // visibility: true if the player sees the enemy

} ball_t;
typedef struct {
	float angle;
}ust_t;

wall_t wall = {{ -75, 150 }, { 75, 150 }, { -75, -150 }, { 75, -150 }, { 0, -150 }, {0} };
ball_t  B = {5};
ust_t s = { 90 };//angle for validation


//vector hesaplayýcý
/*vec2_t CalculateNormalVector(point_t P1, point_t P2)
{
	vec2_t N;

	N.x = (P1.x + P2.x) / 2;
	N.y = (P1.y + P2.y) / 2;
	return N;
}*/   
//L = unitV(subV(B.vel, mulV(BALL_SPEED, unitV(subV(wall.pos, B.pos)))));
float calDistance(wall_t wall, ball_t B)
{
	float distance = sqrtf(powf(B.pos.x - wall.pos.x, 2) + powf(B.pos.y - wall.pos.y, 2));
	if (distance < 5)
		distance = 0;
	return distance;
}
//
// to draw circle, center at (x,y)
//  radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}
void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}
void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}
// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}
void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i<len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}
//
// To display onto window using OpenGL commands
//
void point(point_t max){

	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(max.x, max.y);
	glEnd();

}
void drawBall(ball_t b){
	
	vec2_t N = pol2rec(-20, s.angle);
	vec2_t L = unitV(subV(B.pos, wall.pos));
	
	glColor3f(1, 1, 1);
	if (dotP(subV(wall.pos, B.pos),N ) < wall.angle)
	{
		glColor3f(1, 0, 0);
		invalid = true;
	}
	else
		glColor3f(0, 1, 0);

	circle(b.pos.x, b.pos.y, B.rad);

}
void display()
{
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//INFORMATIONS
	glLoadIdentity();
	glColor3f(1, 1, 1);
	vprint(-200, 210, GLUT_BITMAP_8_BY_13, "Mert Ozbudak");
	glColor3f(1, 0, 0);
	vprint(100, -210, GLUT_BITMAP_8_BY_13, "N= <%.2f, %.2f>", -sin((180 - wall.angle) * D2R), -cos((180 - wall.angle)*D2R));
	vprint(100, -230, GLUT_BITMAP_8_BY_13, "Angle: %.0f", wall.angle);
	//IN ORDER TO WRITE WITH GREEN COLOUR or RED COLOUR WHEN USER CLICK
	if (flag == true )
	{	
		
		if (invalid == false)
		{
			glColor3f(0, 1, 0);
			vprint(100, -190, GLUT_BITMAP_8_BY_13, "dist: %.0f", calDistance(wall, B));
			vec2_t N = pol2rec(B.rad, s.angle);
			vec2_t L = unitV(subV(B.pos, wall.pos));
			R=subV(mulV(2 * dotP(N, L), N), L);
			vprint(100, -170, GLUT_BITMAP_8_BY_13, "Re:<%.0f %.0f>", R.x, R.y);
		}
		else
		{
			vec2_t N = pol2rec(-20, s.angle);
			vec2_t L = unitV(subV(B.pos, wall.pos));
			if (dotP(subV((wall.pos), B.pos), N) < 0 && calDistance(wall, B) != 0)
			{
				glColor3f(1, 0, 0);
				vprint(100, -190, GLUT_BITMAP_8_BY_13, "Invalid");
			}
		}
	}
	
	glLoadIdentity();
	glColor3f(0.4, 0.4, 0.4);//GREY
	glBegin(GL_LINES);
	// Y
	glVertex2f(-400, -150);
	glVertex2f(400, -150);
	// X
	glVertex2f(0,300);
	glVertex2f(0, -300);
	glEnd();
	
	if (flag == true)
	{
		drawBall(B);
	}

	
	//DRAW WALL
	glLoadIdentity();
	glColor4f(0, 0, 1.0, 0);
	glTranslatef(0, -150, 0);
	glRotatef(wall.angle, 0, 0, 1);
	glTranslatef(0, -12, 0);
	glRectf(wall.C1.x, wall.C1.y, wall.C2.x, wall.C3.y);
	//RECTANGULAR BORDER
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	glColor3f(0.2, 0, 1);
	
	glVertex2f(wall.C1.x - 75, wall.C1.y+10);
	glVertex2f(wall.C2.x + 75, wall.C1.y + 10);
	
	glVertex2f(wall.C2.x + 75, wall.C1.y + 10);
	glVertex2f(wall.C2.x + 75, wall.C3.y - 10);
	
	glVertex2f(wall.C2.x + 75, wall.C3.y - 10);
	glVertex2f(wall.C1.x - 75, wall.C3.y - 10);
	
	glVertex2f(wall.C1.x - 75, wall.C3.y - 10);
	glVertex2f(wall.C1.x - 75, wall.C1.y + 10);
	//NORMAL VECTOR OF WALL
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex2f(0, 80);
	glVertex2f(0, 10);
	glEnd();

	glutSwapBuffers();


}
//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int state, int x, int y)
{
	int lx = x - winWidth / 2;
	int ly = winHeight / 2 - y;
	// Write your codes here.
	B.pos.x = lx;
	B.pos.y = ly;
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	
		flag = true;
		//B.vel = { wall.pos.x-lx, wall.pos.y-ly };
		invalid = false;
	}
	
	
	
	// to refresh the window it calls display() function
	glutPostRedisplay();
}
//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}
void onMoveDown(int x, int y) {
	// Write your codes here.


	// to refresh the window it calls display() function	
	glutPostRedisplay();
}
void onMove(int x, int y) {
	// Write your codes here.
	int lx = x - winWidth / 2;
	int ly = winHeight / 2 - y;
	
	printf("x:%d y:%d\n", lx, ly);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}
#if TIMER_ON == 1
void onTimer(int v) {
	
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (left && wall.angle != 60){
		wall.angle += 3;
		s.angle += 3;
	}
	if (right && wall.angle != -60){
		wall.angle -= 3;
		s.angle -= 3;
	}

	vec2_t N = pol2rec(1, wall.angle);
	vec2_t L = unitV(subV(B.pos, wall.pos));
	
		if (flag == true)
		{
			B.vel = mulV(BALL_SPEED, unitV(subV(wall.pos, B.pos)));
			if (invalid != true)
			B.pos = addV(B.pos, B.vel);
			if (B.pos.x > 300 || B.pos.x < -300 || B.pos.y>300 || B.pos.y < -300)
				flag = false;
		}
	
	
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif
void Init() {
	
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);
}
void main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CTIS164 - Hw#4");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}