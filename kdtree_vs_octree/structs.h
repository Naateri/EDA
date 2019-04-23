#include <iostream>
#include <vector>

using namespace std;

struct bbox_3d;

struct Point3D{
	float values[3];
	Point3D(){
		//		this->values = new float[3];
		this->values[0] = this->values[1] = this->values[2] = 0.0f;
	}
	Point3D(float x, float y, float z){
		//		this->values = new float[3];
		this->values[0] = x;
		this->values[1] = y;
		this->values[2] = z;
	}
	
	void print(){
		cout << "Point: (" << values[0] << ", " << values[1] << ", " << values[2] << ")\n";
	}
	
	~Point3D(){
		//		delete [] values;
	}
};

struct bbox_3d{
	Point3D mini, maxi;
	bbox_3d(){
		mini = maxi = Point3D(0.0f,0.0f,0.0f);
	}
	bbox_3d(Point3D top_l, Point3D bot_r) : mini(top_l), maxi(bot_r) {;};
	
	bool in_bbox(Point3D* pt){
		return pt->values[0] > mini.values[0] && pt->values[0] < maxi.values[0] &&
			pt->values[1] > mini.values[1] && pt->values[1] < maxi.values[1]
			&& pt->values[2] > mini.values[2] && pt->values[2] < maxi.values[2];
	}
	void push_if_in_bbox(vector<Point3D*>& pts, Point3D* pt){
		if (in_bbox(pt)) pts.push_back(pt);
	}
	
	void print(){
		cout << "Max: "; maxi.print();
		cout << "Min: "; mini.print();
	}
	
	void copy(bbox_3d other){
		for(int i = 0; i < 2; i++){
			this->mini.values[i] = other.mini.values[i];
			this->maxi.values[i] = other.maxi.values[i];
		}
	}
};

struct Node{
	Point3D* value;
	Node *left, *right;
	bbox_3d limitor;
	//vector<float> dimension_pt; //either x, y or z's values inside (below) the node
	Node(){
		this->value = 0;
		left = right = 0;
	}
	Node(Point3D* value, bbox_3d lim){
		this->value = value;
		this->limitor = lim;
		left = right = 0;
	}
};
