#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

#include "octree.h"

#define KEY_ESC 27
#define SPACEBAR 32
#define minimum -25.0f
#define maximum 25.0f
#define ESCALAR 8 //for .obj's
#define RANDOM_POINTS 500000

using namespace std;


///main.cpp

int mx = 0;
int my = 0;

float ax=0.0;
float ay=0.0;
float speed = 0.1;

float gx = 0.0f, gy = 0.0f, gz = 0.0f, gradius = 5.0f;

Octree* ot = new Octree(Point3D(minimum,minimum,minimum),Point3D(maximum,maximum,maximum));
Octree* region;
vector<Point3D*> points, range_query;
Point3D* temp;
sphere radius;
Point3D* origin_radius = radius.origin;

void read_obj(ifstream& file, vector<Point3D*>& pts){
	float x, y, z;
	int i;
	string str, temp;
	Point3D* pt;
	
	while (getline(file, str)){
		if (str[0] == 'v') break;
	}
	while (getline(file, str)){
		if (str[1] != ' ') break;
		temp.clear();
		i = 2;
		while (str[i] != ' ') temp += str[i++];
		x = stof(temp) * ESCALAR;
		i++; temp.clear();
		while (str[i] != ' ') temp += str[i++];
		y = stof(temp) * ESCALAR;
		i++; temp.clear();
		while (i < str.size()) temp += str[i++];
		z = stof(temp) * ESCALAR;
		pt = new Point3D(x,y,z);
		pts.push_back(pt);
		ot->insert(pt);
	}
	cout << "filled object\n";
	file.close();
}

int rdtsc(){
	__asm__ __volatile__("rdtsc");
}

float random_float(float LO, float HI){
	srand(rdtsc());
	float r = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(HI-LO)));
	return r;
}

void fill_points(){
	Point3D* temp;
	for(int i = 0; i < RANDOM_POINTS; i++){
		temp = new Point3D((random_float(minimum,maximum)), random_float(minimum,maximum), random_float(minimum,maximum));
		points.push_back(temp);
		ot->insert(temp);
	}
}

void find_in_Octree(Octree* main, Octree*& region){ //procedure to follow
	//press ESP to initiate it
	float x, y, z;
	cout << "Ingrese su punto:\n";
	cin >> x >> y >> z;
	Point3D* temp = new Point3D(x,y,z);
	if (main->find(temp, region)){
		cout << "Point is in:\n";
	} else {
		cout << "Point would be in:\n";
	}
	cout << "Region:\n" << "Minimum point: " << region->volume.mini.x << ", " << region->volume.mini.y << ", " <<
				   region->volume.mini.z << endl;
	cout << "Maximum point: " << region->volume.maxi.x << ", " << region->volume.maxi.y << ", " << region->volume.maxi.z << endl;
	glColor3d(0,255,0);
	region->draw();
	glColor3d(255,20,47);
}

//dibuja un simple gizmo
void displayGizmo()
{
	glBegin(GL_LINES);
	/*glColor3d(255,0,0);
	glVertex3d(-30, 0,0);
	glVertex3d(30, 0,0);
	glColor3d(0, 255, 0);
	glVertex3d(0, -30,0);
	glVertex3d(0, 30,0);
	glColor3d(0, 0, 255);
	glVertex3d(0, 0,-30);
	glVertex3d(0, 0,30);*/
	glEnd();
}
bool r = false;

void OnMouseClick(int button, int state, int x, int y)
{
	Point3D* temp;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mx=x;
		my=y;
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		temp = new Point3D((random_float(minimum,maximum)), random_float(minimum,maximum), random_float(minimum,maximum));
		cout << "Point x " << temp->x << ", y " << temp->y << ", z " << temp->z << endl;
		points.push_back(temp);
		ot->insert(temp);
		r = true;
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
		r = false;
	}
	
}

void OnMouseMotion(int x, int y)
{
	int dx = mx-x;
	int dy = my-y;
	
	mx=x;
	my=y;
	
	ax += dx*speed;
	ay += dy*speed;
}

void idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

void draw_sphere(float x, float y, float z, float rad){
	glPushMatrix();
	glTranslatef(x,y,z);
	glColor3d(0,255,0);
	glutSolidSphere(rad,10,10);
	glPopMatrix();
	origin_radius->x = x;
	origin_radius->y = y;
	origin_radius->z = z;
	radius.radius = rad;
}

//funcion llamada a cada imagen
void glPaint(void) {
	
	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	gluPerspective(45.0,1.0,1.0,500.0);
	
	glTranslatef(0,0,-100.0);
	glRotatef(ax,0,1,0);
	glRotatef(ay,1,0,0);
	
	displayGizmo();
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	draw_sphere(gx,gy,gz,gradius);
	glColor3d(255,20,47);
	glutSolidCube(maximum*2);
	//dibujar quadTree (qt->draw())
	//ot->draw();
	
	range_query.clear();
	ot->range_query(range_query, radius);
	glPointSize(3);
	glBegin(GL_POINTS);
	
	glColor3d(255,0,0);
	for(int i = 0; i < points.size(); i++){
		glVertex3f(points.at(i)->x,points.at(i)->y,points.at(i)->z);
	}
	glColor3d(0,255,100);
	for (int i = 0; i < range_query.size(); i++){
		glVertex3f(range_query.at(i)->x,range_query.at(i)->y,range_query.at(i)->z);
	}
	//cout << "Points inside sphere: " << range_query.size() << endl;
	glColor3d(255,20,47);
	glEnd();
	//dibuja el gizmo
	
	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

//
//inicializacion de OpenGL
//
void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro
	glEnable(GL_DEPTH_TEST);
	//modo projeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;
	case SPACEBAR:
		/*gx += 1.0f;
		cout << "x: " << gx << endl;*/
		break;
	case 'd':
		gx += 1.0f;
		break;
	case 'a':
		gx -= 1.0f;
		break;
	case 'w':
		gy += 1.0f;
		break;
	case 's':
		gy -= 1.0f;
		break;
	case 'f':
		gz += 1.0f;
		break;
	case 'r': 
		gz -= 1.0f;
		break;
	case '+':
		gradius += 1.0f;
		break;
	case '-':
		gradius -= 1.0f;
		break;
	default:
		break;
	}
	
}
//
//el programa principal
//
int main(int argc, char** argv) {
	
	//Inicializacion de la GLUT
	
//	ifstream file("Objects/bunny.obj");
//	ifstream file("Objects/f16.obj");
//	ifstream file("Objects/tyra.obj");
	
//	read_obj(file, points);
	
	fill_points();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("Octree"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL
	
	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
	
	
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}
