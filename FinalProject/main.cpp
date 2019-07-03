#include <iostream>
#include "va_file.h"
#include "QuadTree.h"
#include <GL/glut.h>
#include <string>
#include <fstream>
#include <stdlib.h>
using namespace std;

#define KEY_ESC 27

QuadTree* qt;
VA_File* va_file;
vector<Point2D*> points;
vector<PointND*> full_data_pts;

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
	qt->draw();
}
bool r = false;

void draw_point(int x, int y);

void OnMouseClick(int button, int state, int x, int y){
	Point2D* pt;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		//convertir x,y
		//insertar un nuevo punto en el quadtree
		std::cout << x-300 << " " << 300-y << std::endl;
		//draw_point(x,y);	
		pt = new Point2D(x-300,300-y);
		qt->insert(pt);
		points.push_back(pt);
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		r = true;
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
		r = false;
	}
}

void OnMouseMotion(int x, int y){
	if(r)
		std::cout << x-90 << " " << 180-y << std::endl;
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
	glOrtho(-90.0f,  90.0f,-180.0f, 80.0f, -1.0f, 1.0f);
	
	//dibujar quadTree (qt->draw())
	glPointSize(3);
	glBegin(GL_POINTS);
	for(int i = 0; i < points.size(); i++)
		glVertex2f((float)points.at(i)->x,(float)points.at(i)->y );
	
	glColor3d(255, 0, 0);
	
	for(int i = 0; i < va_file->knn_index.size(); i++){
		glVertex2f((float)points.at(va_file->knn_index[i])->x,(float)points.at(va_file->knn_index[i])->y );
	}
	
	glColor3d(0, 0, 255);
	
	//glDrawArrays(GL_POINTS,0,12);
	//glVertex2f(100,100);
	glEnd();
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

//en el caso que la ventana cambie de tama�o
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

void insert_points(string file, int dim){
	ifstream le_file;
	le_file.open(file.c_str());
	string num, real_num;
	double converted_num;
	int cur_dim;
	Point2D* geo;
	PointND* data;
	
	getline(le_file, num); //getting header
	if (le_file.is_open()){
		while (getline(le_file, num)){
			cur_dim = 0;
			real_num.clear();
			geo = new Point2D();
			data = new PointND(dim);
			for(int i = 0; i < num.size(); i++){
				if (num[i] == ','){
					if (cur_dim == 0){
						geo->x = atof(real_num.c_str());
					} else if (cur_dim == 1){
						geo->y = atof(real_num.c_str());
					}
					//filling on the data point
					data->point[cur_dim] = atof(real_num.c_str());
					cur_dim++;
					real_num.clear();
					continue;
				}
				real_num = real_num + num[i];
			}
			points.push_back(geo);
			qt->insert(geo);
			full_data_pts.push_back(data);
		}
	}
	cout << "Dimensions: " << cur_dim+1 << endl;
	cout << "Total points " << full_data_pts.size() << endl;
	va_file->build(full_data_pts);
	le_file.close();
}


int main (int argc, char *argv[]) {
	//PointND temp(5);
	//VA_File test(5);
	//Inicializacion de la GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tama�o de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("QuadTree"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL
	
	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
	
	
	qt = new QuadTree();
	va_file = new VA_File(10);
	
	insert_points("dataset_d10pts100.csv", 10);
	cout << "dataset built\n";
	
	va_file->simple_search(full_data_pts.at(6), 3); //test search
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}
