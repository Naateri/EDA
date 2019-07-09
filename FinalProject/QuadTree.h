#ifndef QUADTREE_H
#define QUADTREE_H

#include <math.h>
#include <vector>
#include <GL/glut.h>
#include <iostream>

using namespace std;

struct Point2D{
	double x, y;
	Point2D(){
		this->x = this->y = 0.0;
	}
	Point2D(double x, double y){
		this-> x = x;
		this->y = y;
	}
};

struct B_Box{ //bounding box
	Point2D top_left, bottom_right; //minimum needed for a bounding box
	B_Box(){
		top_left = Point2D(0,0);
		bottom_right = Point2D(0,0);
	}
	B_Box(Point2D top_l, Point2D bot_r) : top_left(top_l), bottom_right(bot_r) {;};
};

struct PointND;

class QuadTree{
private:
	const int max_elements = 100;
	bool filled = 0;
	QuadTree* zero, *one, *two, *three;
	//00: NW, 01: NE, 10: SW, 11: SE
	vector<Point2D*> points;
	B_Box area;
	bool in_bbox(Point2D* pt);
public:
	vector<PointND*> n_points;
	QuadTree();
	QuadTree(Point2D top_left, Point2D bottom_right);
	bool insert(Point2D* pt, PointND* ptn);
	bool find(Point2D* pt, QuadTree*& region);
	void subdivide();
	void draw();
};

#endif

