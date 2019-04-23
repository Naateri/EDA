#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "kdtree.h"

#define GEN_POINTS 50
#define KEY_ESC 27
#define LOW -20.0f
#define HIGH 20.0f

using namespace std;


vector<Point3D*> points;
Point3D low(LOW,LOW,LOW);
Point3D high(HIGH, HIGH, HIGH);
bbox_3d volume(low, high);
KDTree* kd;

int mx = 0;
int my = 0;

float ax=0.0;
float ay=0.0;
float speed = 0.1;


int rdtsc(){
	__asm__ __volatile__("rdtsc");
}

float random_float(float LO, float HI){
	srand(rdtsc());
	float r = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(HI-LO)));
	return r;
}

void create_points(vector<Point3D*>& pts){
	Point3D* temp;
	for (int i = 0; i < GEN_POINTS; i++){
		temp = new Point3D(random_float(LOW, HIGH), random_float(LOW, HIGH), random_float(LOW, HIGH));
		pts.push_back(temp);
	}
}

void draw_kd(Node* current, short current_dimension){ //preorder drawing
	if (!current->value) return;
	float line;
	line = current->value->values[current_dimension];
	glBegin(GL_LINES);
	if (current_dimension == 0){
		glColor3d(255,0,0);
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.mini.values[2]);
		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.mini.values[2]);

		glVertex3d(line,current->limitor.mini.values[1],current->limitor.mini.values[2]);
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.maxi.values[2]);

		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.maxi.values[2]);
		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.mini.values[2]);

		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.maxi.values[2]);
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.maxi.values[2]);


	} else if (current_dimension == 1){
		glColor3d(0,255,0);

		glVertex3d(current->limitor.mini.values[0],line,current->limitor.mini.values[2]);
		glVertex3d(current->limitor.maxi.values[0],line,current->limitor.mini.values[2]);

		glVertex3d(current->limitor.mini.values[0], line, current->limitor.mini.values[2]);
		glVertex3d(current->limitor.mini.values[0], line, current->limitor.maxi.values[2]);

		glVertex3d(current->limitor.maxi.values[0], line, current->limitor.maxi.values[2]);
		glVertex3d(current->limitor.maxi.values[0],line,current->limitor.mini.values[2]);

		glVertex3d(current->limitor.maxi.values[0],line,current->limitor.maxi.values[2]);
		glVertex3d(current->limitor.mini.values[0],line,current->limitor.maxi.values[2]);


	} else {
		glColor3d(0,0,255);
		
		glVertex3d(current->limitor.mini.values[0],current->limitor.mini.values[1],line);
		glVertex3d(current->limitor.maxi.values[0],current->limitor.mini.values[1],line);

		glVertex3d(current->limitor.mini.values[0],current->limitor.mini.values[1], line);
		glVertex3d(current->limitor.mini.values[0], current->limitor.maxi.values[1],line);

		glVertex3d(current->limitor.maxi.values[0], current->limitor.maxi.values[1],line);
		glVertex3d(current->limitor.maxi.values[0],current->limitor.mini.values[1],line);

		glVertex3d(current->limitor.maxi.values[0],current->limitor.maxi.values[1],line);
		glVertex3d(current->limitor.mini.values[0],current->limitor.maxi.values[1],line);

	}
	glColor3d(255,20,47);
	glEnd();
	current_dimension = (current_dimension+1)%3;
	draw_kd(current->left, current_dimension);
	draw_kd(current->right, current_dimension);
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
	draw_kd(kd->root, 0);
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
		temp = new Point3D((random_float(LOW,HIGH)), random_float(LOW,HIGH), random_float(LOW,HIGH));
		//float x, y, z; cin >> x >> y >> z; temp->x = x; temp->y = y; temp->z = z;
		points.push_back(temp);
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
	//glutSolidTeapot(7);
	
	//glTranslatef(20,20,0);
	glutSolidCube(HIGH*2);
	
	glPointSize(3);
	glBegin(GL_POINTS);
	
	for(int i = 0; i < points.size(); i++){
		glColor3d(255,120,120);
		glVertex3f(points.at(i)->values[0],points.at(i)->values[1],points.at(i)->values[2]);
	}
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
	case 65:
		/*Octree* region;
		find_in_Octree(ot, region);*/
		break;
	default:
		break;
	}
	
}
//
//el programa principal
//
int main(int argc, char** argv) { //template by teacher Marc Le-Guen
	
	//asking for amount of points
	//to partition Octree
	
	create_points(points);
	kd = new KDTree(points,volume);

	cout<< GEN_POINTS << " inserted\n";

	//kd->preorder(kd->root);
	
	//Inicializacion de la GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("KDTree"); //titulo de la ventana
	
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