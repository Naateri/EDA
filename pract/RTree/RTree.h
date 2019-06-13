#ifndef RTREE_H
#define RTREE_H

#include <math.h>
#include <vector>
#include <list>
#include <utility>
#include <GL/glut.h>
#include <iostream>

using namespace std;

struct Point2D{
	int x, y;
	Point2D(){
		this->x = this->y = 0;
	}
	Point2D(int x, int y){
		this-> x = x;
		this->y = y;
	}
	void print(){
		cout << "x: " << x << ", y: " << y << endl;
	}
};

struct B_Box{ //bounding box
	Point2D top_left, bottom_right; //minimum needed for a bounding box
	//bool leaf = 1;
	B_Box(){
		top_left = Point2D(0,0);
		bottom_right = Point2D(0,0);
	}
	B_Box(Point2D top_l, Point2D bot_r) : top_left(top_l), bottom_right(bot_r) {;};
	double area(){
		return ( (top_left.x - bottom_right.x) * (top_left.y - bottom_right.y) );
	}
	void print(){
		cout << "top left: "; top_left.print();
		cout << "bot right: "; bottom_right.print();
	}
	void grow(int s){
		top_left.x -= s;
		top_left.y += s;
		bottom_right.x += s;
		bottom_right.y -= s;
	}
};


struct RNode{
	B_Box covering_rectangle;
	vector<B_Box*> objects;	
	vector<RNode*> pointers;
	RNode* parent = 0;
	/*RNode(){;}
	RNode(int m){
		//objects.resize(m);
		//pointers.resize(0);
		;
	}*/
	void adjust_rectangle(){
		int min_tl_x, max_tl_y, max_br_x, min_br_y;
		min_tl_x = min_br_y = 2147483647;
		max_tl_y = max_br_x = -2147483646;
		for(int i = 0; i < objects.size(); i++){
			if (objects.at(i)->top_left.x < min_tl_x) min_tl_x = objects.at(i)->top_left.x;
			if (objects.at(i)->top_left.y > max_tl_y) max_tl_y = objects.at(i)->top_left.y;
			if (objects.at(i)->bottom_right.x > max_br_x) max_br_x = objects.at(i)->bottom_right.x;
			if (objects.at(i)->bottom_right.y < min_br_y) min_br_y = objects.at(i)->bottom_right.y;
		}
		covering_rectangle.top_left.x = min_tl_x - 10;
		covering_rectangle.top_left.y = max_tl_y + 10;
		covering_rectangle.bottom_right.x = max_br_x + 10;
		covering_rectangle.bottom_right.y = min_br_y - 10;
	}
	
	void print_points(){
		for(int i = 0; i < objects.size(); i++){
			cout << "Child" << i+1 << ": ";
			objects.at(i)->print();
		}
	}
};

class RTree{
private:
	RNode* root;
	bool root_split;
	list<RNode*> path; //idea
	int m;
	void draw_visits(RNode* cur_node, int alt);
	void choose_leaf(RNode*& N, B_Box* elem);
	void adjust_tree(RNode*& L, RNode* LL);
	pair<int, int> PickSeeds(RNode*& cur_node, B_Box*& max_J); //returns index of pair
	int PickNext(RNode*& cur_node, RNode* child1, RNode* child2, bool& group); //returns index of next node
	pair<RNode*, RNode*> QuadraticSplit(RNode*& cur_node);
public:
	RTree(int m);
	bool find(B_Box elem);
	bool insert(B_Box* elem);
	bool del(B_Box elem);
	void draw();
};

#endif
