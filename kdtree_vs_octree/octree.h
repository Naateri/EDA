#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "kdtree.h"
//#include "structs.h"

using namespace std;

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
	//void draw();
	void set_maxpts(int);
	bool find(Point3D* pt, Octree*& region);
	Point3D* get_rand_point();
	void print_points();
};


#endif
