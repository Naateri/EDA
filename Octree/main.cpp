#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "octree.h"

#define MAX_POINTS 100
#define KEY_ESC 27

using namespace std;

/*struct Point3D{
	float x, y, z;
	Point3D(){
		this->x = this->y = this->z = (float)0;
	}
	Point3D(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct bbox_3d{
	Point3D mini, maxi;
	bbox_3d(){
		mini = maxi = Point3D(0.0f,0.0f,0.0f);
	}
	bbox_3d(Point3D top_l, Point3D bot_r) : mini(top_l), maxi(bot_r) {;};
};

///Octree.h

class Octree{
private:
	int max_elements = 4;
	bool filled = 0;
	Octree* zero, *one, *two, *three, *four, *five, *six, *seven;
	vector<Point3D*> points;
	//bbox_3d volume;
	bool in_bbox(Point3D* pt);
public:
	bbox_3d volume;
	Octree();
	Octree(Point3D mini, Point3D maxi);
	bool insert(Point3D* pt);
	void subdivide();
	void draw();
	void set_maxpts(int);
	bool find(Point3D* pt, Octree*& region);
};

///Octree.cpp

Octree::Octree(){
	this->zero = this->one = this->two = this->three
		= this->four = this->five = this->six = this->seven = 0;
	bbox_3d temp(Point3D(0.0f, 0.0f, 0.0f), Point3D(0.0f, 0.0f, 0.0f));
	this->volume = temp;
}

Octree::Octree(Point3D mini, Point3D maxi){
	bbox_3d temp(mini, maxi);
	this->volume = temp;
	this->zero = this->one = this->two = this->three
		= this->four = this->five = this->six = this->seven = 0;
}

bool Octree::in_bbox(Point3D* pt){
	return pt->x >= volume.mini.x && pt->x <= volume.maxi.x &&
		pt->y >= volume.mini.y && pt->y <= volume.maxi.y
		&& pt->z >= volume.mini.z && pt->z <= volume.maxi.z;
	
}

bool Octree::insert(Point3D* pt){
	//checking if it's in the correct cube, return 0;
	if (!in_bbox(pt)){
		//cout << "Out\n";
		return 0;
	}
	if (points.size() < this->max_elements && !zero){
		points.push_back(pt);
		//cout << "Points.size(): " << points.size() << endl;
		return 1;
	}
	//if (!zero) cout << "current points: " << points.size() << endl;
	if (!zero){
		subdivide();
		//cout << "Partitioning\n";
	} 
	
	if (zero->insert(pt)) return 1;
	if (one->insert(pt)) return 1;
	if (two->insert(pt)) return 1;
	if (three->insert(pt)) return 1;
	if (four->insert(pt)) return 1;
	if (five->insert(pt)) return 1;
	if (six->insert(pt)) return 1;
	if (seven->insert(pt)) return 1;
	
	//cout << "some sort of error\n";
	
	return 0;
}

void Octree::subdivide(){
	Point3D mini, maxi;
	float x2 = ((volume.maxi.x + volume.mini.x) / 2);
	float y2 = ((volume.maxi.y + volume.mini.y) / 2);
	float z2 = (( volume.maxi.z + volume.mini.z ) / 2);
	mini = volume.mini;
	maxi.x = x2;
	maxi.y = y2;
	maxi.z = z2;
	zero = new Octree(mini, maxi);
	//cout << "zero->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.x = x2;
	mini.y = volume.mini.y;
	mini.z = volume.mini.z;
	maxi.x = volume.maxi.x;
	maxi.y = y2;
	maxi.z = z2;
	one = new Octree(mini, maxi);
	//cout << "one->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.x = volume.mini.x;
	mini.y = y2;
	mini.z = volume.mini.z;
	maxi.x = x2;
	maxi.y = volume.maxi.y;
	maxi.z = z2;
	two = new Octree(mini, maxi);
	
	mini.x = x2;
	mini.y = y2;
	mini.z = volume.mini.z;
	maxi = volume.maxi;
	maxi.z = z2;
	three = new Octree(mini, maxi);
	
	///Bottom part: z/2 (or so I thought but it's not just that ggwp)
	
	//cout << "bottom half\n";
	
	mini = volume.mini;
	mini.z = z2;
	maxi.x = x2;
	maxi.y = y2;
	maxi.z = volume.maxi.z;
	four = new Octree(mini, maxi);
	//cout << "zero->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.x = x2;
	mini.y = volume.mini.y;
	mini.z = z2;
	maxi.x = volume.maxi.x;
	maxi.y = y2;
	maxi.z = volume.maxi.z;	
	five = new Octree(mini, maxi);
	//cout << "one->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.x = volume.mini.x;
	mini.y = y2;
	mini.z = z2;
	maxi.x = x2;
	maxi.y = volume.maxi.y;
	maxi.z = volume.maxi.z;
	six = new Octree(mini, maxi);
	//cout << "two->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.x = x2;
	mini.y = y2;
	mini.z = z2;
	maxi = volume.maxi;
	maxi.z = volume.maxi.z;
	
	seven = new Octree(mini, maxi);
	
	zero->set_maxpts(this->max_elements);
	one->set_maxpts(this->max_elements);
	two->set_maxpts(this->max_elements);
	three->set_maxpts(this->max_elements);
	four->set_maxpts(this->max_elements);
	five->set_maxpts(this->max_elements);
	six->set_maxpts(this->max_elements);
	seven->set_maxpts(this->max_elements);
	
	//cout << "num of points: " << points.size() << endl;
	for(int i = 0; i < this->points.size(); i++){
		zero->insert(points.at(i));
		one->insert(points.at(i));
		two->insert(points.at(i));
		three->insert(points.at(i));
		four->insert(points.at(i));
		five->insert(points.at(i));
		six->insert(points.at(i));
		seven->insert(points.at(i));
	}
}

bool Octree::find(Point3D* pt, Octree*& region){
	if (!in_bbox(pt)) return 0;
	
	if (!zero){ //can't go any further down
		region = this; //point, whether it exists or not, should be here
		for (int i = 0; i < this->points.size(); i++){
			if (points.at(i)->x == pt->x && points.at(i)->y == pt->y
					&& points.at(i)->z == pt->z){
				return 1;
			}
		}
		return 0;
	}
	zero->find(pt, region);
	one->find(pt, region);
	two->find(pt, region);
	three->find(pt, region);
	four->find(pt, region);
	five->find(pt, region);
	six->find(pt, region);
	seven->find(pt, region);
	return 0;
}

void Octree::draw(){
	
	//if (!zero && !one && !two && !three
	//	&& !four && !five && !six && !seven) return;
	if (!zero) return;
	glBegin(GL_LINES);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	glColor3d(255,20,47);
	glVertex3d(zero->volume.mini.x, zero->volume.maxi.y, 0);
	glVertex3d(three->volume.maxi.x, three->volume.mini.y, 0);
	//glColor3d(0, 0, 255);
	glVertex3d(zero->volume.maxi.x, zero->volume.mini.y, 0);
	glVertex3d(three->volume.mini.x, three->volume.maxi.y, 0);
	
	//glColor3d(255,20,47);
	glVertex3d(zero->volume.mini.x, zero->volume.maxi.y, volume.maxi.z);
	glVertex3d(three->volume.maxi.x, three->volume.mini.y, volume.maxi.z);
	//glColor3d(0, 0, 255);
	glVertex3d(zero->volume.maxi.x, zero->volume.mini.y, volume.maxi.z);
	glVertex3d(three->volume.mini.x, three->volume.maxi.y, volume.maxi.z);
	
	//glColor3d(255,20,47);
	glVertex3d(zero->volume.mini.x, zero->volume.maxi.y, volume.mini.z);
	glVertex3d(three->volume.maxi.x, three->volume.mini.y, volume.mini.z);
	//glColor3d(0, 0, 255);
	glVertex3d(zero->volume.maxi.x, zero->volume.mini.y, volume.mini.z);
	glVertex3d(three->volume.mini.x, three->volume.maxi.y, volume.mini.z);
	
	/////////x////////////////////
	
	glVertex3d(0, zero->volume.maxi.y, zero->volume.mini.z); //repeat changing x
	glVertex3d(0, seven->volume.mini.y, seven->volume.maxi.z);
	
	glVertex3d(volume.mini.x, zero->volume.maxi.y, zero->volume.mini.z); //repeat changing x
	glVertex3d(volume.mini.x, seven->volume.mini.y, seven->volume.maxi.z);
	
	glVertex3d(volume.mini.x, zero->volume.mini.y, zero->volume.maxi.z);
	glVertex3d(volume.mini.x, seven->volume.maxi.y, seven->volume.mini.z);
	glVertex3d(volume.maxi.x, zero->volume.maxi.y, zero->volume.mini.z); //repeat changing x
	glVertex3d(volume.maxi.x, seven->volume.mini.y, seven->volume.maxi.z);
	
	glVertex3d(volume.maxi.x, zero->volume.mini.y, zero->volume.maxi.z);
	glVertex3d(volume.maxi.x, seven->volume.maxi.y, seven->volume.mini.z);
	
	glColor3d(255,20,47);
	
	/////////////y//////////
	
	//glColor3d(255, 0, 0);
	glVertex3d(seven->volume.maxi.x, volume.mini.y, zero->volume.maxi.z); 
	glVertex3d(zero->volume.mini.x, volume.mini.y, seven->volume.mini.z);
	
	glVertex3d(seven->volume.mini.x, volume.mini.y, zero->volume.mini.z);
	glVertex3d(zero->volume.maxi.x, volume.mini.y, seven->volume.maxi.z);
	
	glColor3d(255,20,47);
	glVertex3d(seven->volume.maxi.x, volume.maxi.y, zero->volume.maxi.z); 
	glVertex3d(zero->volume.mini.x, volume.maxi.y, seven->volume.mini.z);
	
	glVertex3d(seven->volume.mini.x, volume.maxi.y, zero->volume.mini.z);
	glVertex3d(zero->volume.maxi.x, volume.maxi.y, seven->volume.maxi.z);
	
	
	if (zero) zero->draw();
	if (one) one->draw();
	if (two) two->draw();
	if (three) three->draw();
	if (four) four->draw();
	if (five) five->draw();
	if (six) six->draw();
	if (seven) seven->draw();
	glEnd();
}

void Octree::set_maxpts(int pts){
	this->max_elements = pts;
}
*/
///main.cpp

int mx = 0;
int my = 0;

float ax=0.0;
float ay=0.0;
float speed = 0.1;

//Octree creation (global so it can be drawn)
Octree* ot = new Octree(Point3D(-10.0f,-10.0f,-10.0f),Point3D(10.0f,10.0f,10.0f));
vector<Point3D*> points; //vector to draw the inserted points
Point3D* temp;

int rdtsc(){
	__asm__ __volatile__("rdtsc");
}

float random_float(float LO, float HI){
	srand(rdtsc());
	float r = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(HI-LO)));
	return r;
}

void fill_points(Octree* ot){
	Point3D* temp;
	for (int i = 0; i < MAX_POINTS; i++){
		temp = new Point3D((random_float(-10.0f,10.0f)), random_float(-10.0f,10.0f), random_float(-10.0f,10.0f));
		ot->insert(temp);
		points.push_back(temp);
	}
}

void find_in_Octree(Octree* main, Octree* region){ //procedure to follow
	//press ESC to initiate it
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
		temp = new Point3D((random_float(-10.0f,10.0f)), random_float(-10.0f,10.0f), random_float(-10.0f,10.0f));
		cout << "Point x " << temp->x << ", y " << temp->y << ", z " << temp->z << endl;
		//float x, y, z; cin >> x >> y >> z; temp->x = x; temp->y = y; temp->z = z;
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
	glutSolidCube(20);
	//dibujar quadTree (qt->draw())
	ot->draw();
	
	glPointSize(3);
	glBegin(GL_POINTS);
	
	for(int i = 0; i < points.size(); i++){
		glColor3d(255,0,0);
		glVertex3f(points.at(i)->x,points.at(i)->y,points.at(i)->z);
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
		//exit(0);
		Octree* region;
		find_in_Octree(ot, region);
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
	
	int pts;
	cout << "Ingrese la cantidad de puntos que desee para particionar el octree: ";
	cin >> pts;
	
	ot->set_maxpts(pts);
	fill_points(ot);
	
	cout<< MAX_POINTS << " inserted\n";
	
	//Inicializacion de la GLUT
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
	
	
	//qt = new quadTree();
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}
