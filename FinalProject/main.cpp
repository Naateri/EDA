#include <iostream>
#include "va_file.h"
#include "QuadTree.h"
#include <GL/glut.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <ctime>
using namespace std;

#define KEY_ESC 27

QuadTree* qt;
VA_File* va_file;
VA_File* qt_file;
vector<Point2D*> points;
vector<PointND*> full_data_pts;

vector<int> k_s = {5, 10, 100, 500, 1000, 5000};

bool quadtree_search = 0; //change to 1 if you want to search
//nearest neighbours in a quadtree leaf
//0 for the whole vector

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

void idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

//funcion llamada a cada imagen
void glPaint(void) {
	
	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(-90.0f, 90.0f, -180.0f, 180.0f, -1.0f, 1.0f);
	
	glPointSize(3);
	glBegin(GL_POINTS);
	for(int i = 0; i < points.size(); i++)
		glVertex2f((float)points.at(i)->x,(float)points.at(i)->y);
	
	glColor3d(255, 0, 0);
	
	if (quadtree_search){
		Point2D* temp = new Point2D;
		for(int i = 0; i < qt_file->qtree.size(); i++){
			temp->x = qt_file->qtree.at(i)->point[0];
			temp->y = qt_file->qtree.at(i)->point[1];
			glVertex2f((float)temp->x,(float)temp->y );
		}
	}
	else {
		for(int i = 0; i < va_file->knn_index.size(); i++){ //knn points
			glVertex2f((float)points.at(va_file->knn_index[i])->x,(float)points.at(va_file->knn_index[i])->y );
		}
	}
	
	glColor3d(0, 255, 0);
	
	glVertex2f((float)points.at(6)->x,(float)points.at(6)->y); //point to query
	
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
						//cout << "x: " << geo->x << endl;
					} else if (cur_dim == 1){
						geo->y = atof(real_num.c_str());
						//cout << "y: " << geo->y << endl;
					}
					//filling on the data point
					data->point[cur_dim] = atof(real_num.c_str());
					cur_dim++;
					real_num.clear();
					continue;
				}
				real_num = real_num + num[i];
			}
			//last iter
			if (cur_dim == 1){
				geo->y = atof(real_num.c_str());
				//cout << "y: " << geo->y << endl;
			}
			//filling on the data point
			data->point[cur_dim] = atof(real_num.c_str());
			points.push_back(geo);
			qt->insert(geo, data);
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
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("Geographical Data"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL
	
	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutIdleFunc(&idle);
	
	int dimensions = 10;
	
	qt = new QuadTree();
	va_file = new VA_File(dimensions);
	
	//insert_points("dataset_d10pts100.csv", 10);
	//insert_points("dataset_d2pts100.csv", 2);
	clock_t begin = clock();
	insert_points("dataset_d10pts100000.csv", dimensions);
	clock_t end = clock();
	
	double elapsed_secs = double(end-begin) / CLOCKS_PER_SEC;
	
	cout << "dataset built\n";
	cout << "Built in " << elapsed_secs << " seconds\n";
	
	vector<PointND*> temp_points_nd;
	
	if (quadtree_search){
		cout << "Quadtree search\n";
		QuadTree* region;
		temp_points_nd.clear();
		qt->find(points.at(6), region);
		for(int i = 0; i < region->n_points.size(); i++){
			temp_points_nd.push_back(region->n_points.at(i));
		}
		qt_file = new VA_File(dimensions);
		qt_file->build(temp_points_nd);
		qt_file->simple_search(full_data_pts.at(6), 25);
	} else { //searching at all the canvas
		cout << "Using all the canvas\n";
		for(int i = 0; i < k_s.size(); i++){
			clock_t begin2 = clock();
			va_file->simple_search(full_data_pts.at(6), k_s.at(i)); //test search
			clock_t end2 = clock();
			elapsed_secs = double(end2-begin2) / CLOCKS_PER_SEC;
			cout << "knn found in " << elapsed_secs << " seconds\n";
		}
		//va_file->simple_search(full_data_pts.at(6), 25); //test search
	}
	
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}

