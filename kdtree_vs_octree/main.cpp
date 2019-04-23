#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>
#include <math.h>
#include <GL/glut.h>
#include <string>

//#include "octree.h"
#include "kdtree.h"

#define GEN_POINTS 10
#define LOW -20.0f
#define HIGH 20.0f
#define PTS_SEARCH 15
#define ESCALAR 7
#define KEY_ESC 27

using namespace std;

int test_amount[] = {100, 1000, 5000, 10000, 50000, 100000, 250000, 500000,1000000};
Point3D low(LOW,LOW,LOW);
Point3D high(HIGH, HIGH, HIGH);
bbox_3d volume(low, high);

///OpenGL template based on the one by Marc LeGuen

vector<Point3D*> points;
KDTree* kd;

int mx = 0;
int my = 0;

float ax=0.0;
float ay=0.0;
float speed = 0.1;

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
		//ot->insert(pt);
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

void create_points(vector<Point3D*>& pts){
	Point3D* temp;
	for (int i = 0; i < GEN_POINTS; i++){
		temp = new Point3D(random_float(LOW, HIGH), random_float(LOW, HIGH), random_float(LOW, HIGH));
		pts.push_back(temp);
	}
}

void create_find(int size, ofstream& file){
	vector<Point3D*> points;
	Point3D* temp;
	double elapsed_time;
	
	for(int i = 0; i < size; i++){
		temp = new Point3D(random_float(LOW, HIGH), random_float(LOW, HIGH), random_float(LOW, HIGH));
		points.push_back(temp);
	}
	
	//kdtree creation
	clock_t begin = clock();
	KDTree *test = new KDTree(points, volume);
	clock_t end = clock();
	elapsed_time = double(end-begin)/CLOCKS_PER_SEC;
	cout << "Insertion KD-Tree with " << size << " points: " << elapsed_time << " seconds.\n";
	file << "Insertion KD-Tree with " << size << " points: " << elapsed_time << " seconds.\n";
	
	/*
	//Octree creation
	Octree *ot = new Octree(low, high);
	clock_t begin1 = clock();
	for(int i = 0; i < points.size(); i++){
		ot->insert(points.at(i));
	}
	clock_t end1 = clock();
	elapsed_time = double(end1-begin1)/CLOCKS_PER_SEC;	
	cout << "Insertion Octree with " << size << " points: " << elapsed_time << " seconds.\n";
	file << "Insertion Octree with " << size << " points: " << elapsed_time << " seconds.\n"; */
	
	//kdtree find
	Node** p;
	elapsed_time = 0.0f;
	for(int i = 0; i < 5; i++){
		temp = test->get_random_point();
		clock_t begin2 = clock();
		test->find(temp, p, 0);
		clock_t end2 = clock();
		//(*p)->value->print(); //prints point
		elapsed_time += double(end2-begin2)/CLOCKS_PER_SEC;
	}
	elapsed_time /= 5;
	cout << "Search in a KD-Tree with " << size << " points: " << elapsed_time << " seconds.\n";
	file << "Search in a KD-Tree with " << size << " points: " << elapsed_time << " seconds.\n";
	
	/*
	//octree find
	
	elapsed_time = 0.0f;
	Octree* region;
	for(int i = 0; i < 5; i++){
		temp = ot->get_rand_point();
		clock_t begin3 = clock();
		ot->find(temp, region);
		clock_t end3 = clock();
		//region->print_points(); //prints points in region found
		elapsed_time += double(end3-begin3)/CLOCKS_PER_SEC;
	}
	elapsed_time /= 5;
	cout << "Search in an Octree with " << size << " points: " << elapsed_time << " seconds.\n";
	file << "Search in an Octree with " << size << " points: " << elapsed_time << " seconds.\n"; */
		
	delete test;
	//delete ot;
	points.clear();
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
	kd->draw_kd(kd->root, 0);
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


int main (int argc, char *argv[]) {
	vector<Point3D*> test_points; //kd-tree
	create_points(test_points);
	int n;
	
	Point3D* pt;//= new Point3D(3,2,3);
	Node** p;
	
	//create_points(points);
	ifstream file("Objects/bunny.obj");
	//ifstream file("Objects/f16.obj");
	//ifstream file("Objects/tyra.obj");
	
	read_obj(file, points);
	
	
	kd = new KDTree(points,volume);
	
	cout<< GEN_POINTS << " inserted\n";
	
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
		
	/*for(int i = 0; i < PTS_SEARCH; i++){
		pt = kd->get_random_point();
		if (kd->find(pt, p, 0)) (*p)->value->print(); //prints value 
	*/
	
	/*vector <Point3D*> test_points; //octree
	create_points(test_points);
	Point3D* temp;
	Octree* ot = new Octree(low, high);
	Octree* region;
	for(int i = 0; i < test_points.size(); i++){
		ot->insert(test_points.at(i));
	}
	temp = ot->get_rand_point();
	if (ot->find(temp, region)){
		temp->print();
		region->print_points();
	}
	*/
	
	/*ofstream result;
	result.open("times.txt", fstream::app);
	
	for (int i = 0; i < 10; i++){
		create_find(test_amount[i], result);
	}	
	result.close();*/
	
	//glutMainLoop(); //bucle de rendering
	
	return 0;
}

