#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>
#include <math.h>
#include <GL/glut.h>
#include <string>

//#include "kdtree.h"
#include "octree.h"

#define GEN_POINTS 2022
#define LOW -50.0f
#define HIGH 50.0f
#define PTS_SEARCH 15
#define KEY_ESC 27

using namespace std;


int test_amount[] = {100, 1000, 5000, 10000, 50000, 100000, 250000, 500000,1000000};
string objects[] = { "airboat", "bunny", "cessna", "skyscraper", "trumpet"};
int num_nodes[] = {5797, 34817,3745, 2022, 11908};
float maximum[] = {2.0f, 10.0f, 20.0f, 0.05f, 0.1f, 0.15f}; //m
//float maximum_percentages[] = {0.05, 0.1, 0.15}; //m
float escalars[] = {4, 16.5, 2, 1.1, 0.08};
string folder = "Objects_Report/";
string le_file;

float ESCALAR = 0.08;

Point3D low(LOW,LOW,LOW);
Point3D high(HIGH, HIGH, HIGH);
bbox_3d volume(low, high);
ofstream file("results.txt", fstream::app);

bool kdt = 0, octr = 1;

int i = 0, j = 0;

//void test_cases(int i, ofstream& o_file);

///OpenGL template based on the one by Marc LeGuen

vector<Point3D*> points;
KDTree* kd = 0;
Octree* ot = 0;

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
	float max_y, max_z, min_y, min_z;
	max_y = max_z = min_y = min_z = 0.0f;
	while (getline(file, str)){
		if (str[0] == 'v') break;
	}
	while (getline(file, str)){
		if (str[1] != ' ' || str[0] != 'v') break;
		temp.clear();
		i = 1;
		while (str[i++] == ' ');
		i--;
		while (str[i] != ' ') temp += str[i++];
		//cout << "temp: " << temp << ", line " << line << endl;
		x = stof(temp) * ESCALAR;
		i++; temp.clear();
		while (str[i] != ' ') temp += str[i++];
		y = stof(temp) * ESCALAR;
		i++; temp.clear();
		while (i < str.size()) temp += str[i++];
		z = stof(temp) * ESCALAR;
		pt = new Point3D(x,y,z);
		pts.push_back(pt);
		if (y > max_y) max_y = y;
		if (y < min_y) min_y = y;
		if (z > max_z) max_z = z;
		if (z < min_z) min_z = z;
		//ot->insert(pt);
	}
	/*cout << "min_y: " << min_y << endl;
	cout << "max_y: " << max_y << endl;
	cout << "min_z: " << min_z << endl;
	cout << "max_z: " << max_z << endl; */
	
	cout << "filled object\n";
	file.close();
}

Point3D* get_rand_point(vector<Point3D*> points){
	srand(rand());
	return points.at(rand()%points.size());
}

void test_cases(int i, int j, ofstream& o_file){
	int m;
	double elapsed_time;
	
	if (kd) delete kd;
	if (ot) delete ot;
	
	bool draw = 0;
	Point3D* temp;
	
	points.clear();
	ESCALAR = escalars[i];	
	le_file = folder + objects[i] + ".obj";
	cout << "Opening file " << le_file << endl;
	o_file << "File: " << le_file << endl;
	ifstream file(le_file);
	//cout << "file ready to be read\n";
	read_obj(file, points);	
	
	//for (int j = 0; j < 6; j++){
		if (maximum[j] >= 1.0f) m = (int)maximum[j];
		else {
			cout << maximum[j] * points.size() << endl;
			m = (int)(maximum[j] * points.size());
			draw = 1;
		}
		cout << "m: " << m << endl;
		o_file << "m: " << m << endl;
		kd = new KDTree(points,volume,m);
		/*cout << "KDTree\n";
		cout << "25%: " << kd->times[0] << ", 50%: " << kd->times[1] << ", 75%: ";
		cout << kd->times[2] << ", 100%: " << kd->times[3] << endl;*/
		
		o_file << "KDTree\n";
		o_file << "25%: " << kd->times[0] << ", 50%: " << kd->times[1] << ", 75%: ";
		o_file << kd->times[2] << ", 100%: " << kd->times[3] << endl;
		
		Node** p;
		elapsed_time = 0.0f;
		for(int k = 0; k < 5; k++){
			temp = kd->get_random_point();
			clock_t begin2 = clock();
			kd->find(temp, p, 0);
			clock_t end2 = clock();
			//(*p)->value->print(); //prints point
			elapsed_time += double(end2-begin2)/CLOCKS_PER_SEC;
		}
		elapsed_time /= 5;
		//cout << "Search: " << elapsed_time << " seconds.\n";
		o_file << "Search: " << elapsed_time << " seconds.\n";
		o_file << "-------------------------------------\n";
		
		ot = new Octree(volume.mini, volume.maxi);
		ot->set_maxpts(m);
		ot->insert_points_vector(points);
		
		/*cout << "Octree\n";
		cout << "25%: " << ot->times[0] << ", 50%: " << ot->times[1] << ", 75%: ";
		cout << ot->times[2] << ", 100%: " << ot->times[3] << endl;*/
		
		o_file << "Octree\n";
		o_file << "25%: " << ot->times[0] << ", 50%: " << ot->times[1] << ", 75%: ";
		o_file << ot->times[2] << ", 100%: " << ot->times[3] << endl;		
		
		
		elapsed_time = 0.0f;
		Octree* region = 0;
		for(int i = 0; i < 5; i++){
			temp = get_rand_point(points);
			clock_t begin3 = clock();
			ot->find(temp, region);
			clock_t end3 = clock();
			//region->print_points(); //prints points in region found
			elapsed_time += double(end3-begin3)/CLOCKS_PER_SEC;
		}
		elapsed_time /= 5;
		//cout << "Search: " << elapsed_time << " seconds.\n";
		o_file << "Search: " << elapsed_time << " seconds.\n";
		
		//delete kd;
		//delete ot;
		o_file << "-------------------------------------\n";
		o_file << "-------------------------------------\n";
	//}
	//o_file << "-------------------------------------\n\n";
	//points.clear();
	
	//cout << "size of element: " << points.size() << endl;
	//cout<< GEN_POINTS << " inserted\n";
	
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

//dibuja un simple gizmo
void displayGizmo()
{
	glBegin(GL_LINES);
	if (kd && kdt) kd->draw_kd(kd->root, 0);
	else if (ot && oct) ot->draw();
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
	gluPerspective(120.0,1.0,1.0,500.0);
	
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
		glColor3d(255,0,120);
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
	case 'a':
		/*for(int i = 0; i < 5; i++){
			test_cases(i, file);
		}
		file.close();
		exit(0);*/
		if (i > 5 && j == 0) break;
		test_cases(i, j, file);
		j = (j + 1)%6;
		if (j == 0) i++;
		/*Octree* region;
		find_in_Octree(ot, region);*/
		break;
	case 's':
		if (octr){
			octr = 0; kdt = 1;
		} else {
			octr = 1; kdt = 0;
		}
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
	
	//ifstream file("Objects/tyra.obj");
	
	//read_obj(file, points);
	
	//Inicializacion de la GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("KDTree vs Octree"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL

	glutDisplayFunc(glPaint);
		
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
	
	glutMainLoop(); //bucle de rendering
	
	return 0;
}

