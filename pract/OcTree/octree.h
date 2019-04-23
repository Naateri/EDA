#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
#include <math.h>
#include<GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#define MAX_PTS 4

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
	float distance(Point3D* b){
		float res = 0.0f;
		res += pow(x - b->x, 2);
		res += pow(y - b->y, 2);
		res += pow(z - b->z, 2);
		return sqrt(res);
	}
};

struct bbox_3d{
	Point3D mini, maxi;
	bbox_3d(){
		mini = maxi = Point3D(0.0f,0.0f,0.0f);
	}
	bbox_3d(Point3D top_l, Point3D bot_r) : mini(top_l), maxi(bot_r) {;};
	bool intersects(bbox_3d B){
		return mini.x < B.maxi.x && maxi.x > B.mini.x &&
			mini.y < B.maxi.y && maxi.y > B.mini.y &&
			mini.z < B.mini.z && maxi.z > B.maxi.z;
	}
	bool point_in_bbox(Point3D* pt){
		return pt->x >= mini.x && pt->x <= maxi.x &&
			pt->y >= mini.y && pt->y <= maxi.y
			&& pt->z >= mini.z && pt->z <= maxi.z;
	}
};

struct sphere{
	float radius;
	Point3D* origin;
	sphere(){
		radius = 0.0f;
		origin = new Point3D(0,0,0);
	}
	sphere(Point3D* org, float r): radius(r), origin(org){;};
	bool point_in_sphere(Point3D* pt){
		//cout << "distance " << origin->distance(pt) << ' ' << radius << endl;
		return origin->distance(pt) <= radius;
	}
	bool bbox_in_sphere(bbox_3d AABB){ //based from https://yal.cc/rectangle-circle-intersection-test/
		float deltaX, deltaY, deltaZ;
		float mid_x, mid_y, mid_z;
		mid_x = (AABB.maxi.x + AABB.mini.x)/2;
		mid_y = (AABB.maxi.y + AABB.mini.y)/2;
		mid_z = (AABB.maxi.z + AABB.mini.z)/2;
		deltaX = origin->x - max(AABB.mini.x, min(origin->x, AABB.mini.x + (AABB.maxi.x - AABB.mini.x)));
		deltaY = origin->y - max(AABB.mini.y, min(origin->y, AABB.mini.y + (AABB.maxi.y - AABB.mini.y)));;
		deltaZ = origin->z - max(AABB.mini.z, min(origin->z, AABB.mini.z + (AABB.maxi.z - AABB.mini.z)));;
		//return (pow(deltaX, 2) + pow(deltaY,2) + pow(deltaZ,2)) < pow(radius,2);
		return (deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ) < (radius*radius);
	}
};

///Octree.h

class Octree{
private:
	const int max_elements = MAX_PTS;
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
	bool find(Point3D* pt, Octree*& region);
	void range_query(vector<Point3D*>& points, sphere radius);
};




#endif
