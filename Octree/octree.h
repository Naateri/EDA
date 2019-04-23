#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Point3D{
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




#endif
