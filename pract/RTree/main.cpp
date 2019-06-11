#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include "RTree.h"
#include <math.h>
#include <vector>
#include<GL/glut.h>

#define KEY_ESC 27
#define KEYS 3

using namespace std;

std::vector<Point2D*> points;
std::vector<B_Box*> rectangles;

//dibuja un simple gizmo
void displayGizmo(){
	glBegin(GL_LINES);
	glEnable(GL_PROGRAM_POINT_SIZE);
	/*glColor3d(255,20,47);
	glVertex2d(-300, 0);
	glVertex2d(300, 0);
	glColor3d(0, 0, 255);
	glVertex2d(0, -300);
	glVertex2d(0, 300);*/
	glEnd();
}
bool r = false;
int click = 0;
B_Box *rect;
RTree* rt = new RTree(KEYS);

void draw_point(int x, int y);

void OnMouseClick(int button, int state, int x, int y){
	Point2D* pt;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		//convertir x,y
		//insertar un nuevo punto en el quadtree
		std::cout << x-300 << " " << 300-y << std::endl;
		draw_point(x,y);	
		pt = new Point2D(x-300,300-y);
		click++;
		if (click == 2){
			click = 0;
			rect = new B_Box(*(points.at(points.size() - 1)), *pt);
			rectangles.push_back(rect);
			//cout << "Rectangle created\n";
			rt->insert(rect);
		}
		//qt->insert(pt);
		points.push_back(pt);
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		r = true;
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
		r = false;
	}
}

void OnMouseMotion(int x, int y){
	if(r)
		std::cout << x-300 << " " << 300-y << std::endl;
	//opcional
	//hacer algo x,z cuando se mueve el mouse
}

void idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

//funcion llamada a cada imagen
void glPaint(void) {
	
	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(-300.0f,  300.0f,-300.0f, 300.0f, -1.0f, 1.0f);
	
	//dibujar quadTree (qt->draw())
	glPointSize(3);
	glBegin(GL_POINTS);
	/*for(int i = 0; i < points.size(); i++)
		glVertex2f((float)points.at(i)->x,(float)points.at(i)->y);*/
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();	
	//glDrawArrays(GL_POINTS,0,12);
	//glVertex2f(100,100);
	glEnd();
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	/*for (int i = 0; i < rectangles.size(); i++){
		glRectf((float)rectangles.at(i)->top_left.x, (float)rectangles.at(i)->top_left.y,
			(float)rectangles.at(i)->bottom_right.x, (float)rectangles.at(i)->bottom_right.y);
	}*/
	rt->draw();
	//dibuja el gizmo
	displayGizmo();
	
	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

void draw_point(int x, int y){
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(x-300,300-y);
	glEnd();
	displayGizmo();
	
	glutSwapBuffers();
}

//
//inicializacion de OpenGL
//
void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0, 0, 0, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro
	
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
		
	default:
		break;
	}
	
}
//
//el programa principal
//
int main(int argc, char** argv) {
	
	//Inicializacion de la GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("RTree"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL
	
	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
	
	//qt = new quadTree();
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}
