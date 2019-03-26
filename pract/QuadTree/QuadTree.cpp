#include "QuadTree.h"

QuadTree::QuadTree(){
	B_Box ar(Point2D(-300,300), Point2D(300,-300));
	this->area = ar;
	this->zero = this->one = this->two = this->three = 0;
	this->filled = 0;
}

QuadTree::QuadTree(Point2D top_left, Point2D bottom_right){
	B_Box ar(top_left, bottom_right);
	this->area = ar;
	cout << "Top left, x: " << area.top_left.x << ", y: " << area.top_left.y << endl;	
	this->zero = this->one = this->two = this->three = 0;
	this->filled = 0;
}

bool QuadTree::in_bbox(Point2D* pt){
	return pt->x >= area.top_left.x && pt->x <= area.bottom_right.x &&
		pt->y <= area.top_left.y && pt->y >= area.bottom_right.y;
}

bool QuadTree::insert(Point2D* pt){
	if (!in_bbox(pt)){
		cout << "Out\n";
		return 0;
	}
	if (points.size() < this->max_elements && !zero){
		points.push_back(pt);
		cout << "points.size(): " << points.size() << endl;
		return 1;
	}
	//filled = 1;
	if (!zero){
		subdivide();
	}
	
	if (zero->insert(pt)) return 1;
	if (one->insert(pt)) return 1;
	if (two->insert(pt)) return 1;
	if (three->insert(pt)) return 1;
	
	return 0;
}

void QuadTree::subdivide(){
	Point2D top_left, bottom_right;
	top_left = area.top_left;
	bottom_right.x = (area.top_left.x + area.bottom_right.x) / 2;
	bottom_right.y = (area.top_left.y + area.bottom_right.y) / 2;
	zero = new QuadTree(top_left, bottom_right);
	cout << "zero->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	top_left.x = (area.top_left.x + area.bottom_right.x)/2;
	top_left.y = area.top_left.y;
	bottom_right.x = area.bottom_right.x;
	bottom_right.y = (area.top_left.y + area.bottom_right.y)/2;
	one = new QuadTree(top_left, bottom_right);
	cout << "one->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	top_left.x = area.top_left.x;
	top_left.y = (area.top_left.y + area.bottom_right.y) / 2;
	bottom_right.x = (area.top_left.x + area.bottom_right.x) / 2;
	bottom_right.y = area.bottom_right.y;
	two = new QuadTree(top_left, bottom_right);
	cout << "two->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	top_left.x = (area.top_left.x + area.bottom_right.x)/2;
	top_left.y = (area.top_left.y + area.bottom_right.y)/2;
	bottom_right = area.bottom_right;
	three = new QuadTree(top_left, bottom_right);
	cout << "three->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	for(int i = 0; i < this->points.size(); i++){
		zero->insert(points.at(i));
		one->insert(points.at(i));
		two->insert(points.at(i));
		three->insert(points.at(i));
	}
}

void QuadTree::draw(){
	if (!zero && !one && !two && ! three) return;
	glBegin(GL_LINES);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glColor3d(255,20,47);
	glVertex2d(zero->area.top_left.x, zero->area.bottom_right.y);
	//glVertex2d(one->area.bottom_right.x, one->area.bottom_right.y);
	glVertex2d(three->area.bottom_right.x, three->area.top_left.y);
	glColor3d(0, 0, 255);
	glVertex2d(zero->area.bottom_right.x, zero->area.top_left.y);
	glVertex2d(three->area.top_left.x, three->area.bottom_right.y);
	if (zero) zero->draw();
	if (one) one->draw();
	if (two) two->draw();
	if (three) three->draw();
	glEnd();
}
