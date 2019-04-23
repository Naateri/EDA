#include "octree.h"

float distance(Point3D* a, Point3D* b){
	return sqrt( pow(a->x - b->x, 2) + pow(a->y - b->y, 2) + pow(a->z - b->z, 2));
}

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
//	this->points.clear();
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
	if (zero->find(pt, region)) return 1;
	if (one->find(pt, region)) return 1;
	if (two->find(pt, region)) return 1;
	if (three->find(pt, region)) return 1;
	if (four->find(pt, region)) return 1;
	if (five->find(pt, region)) return 1;
	if (six->find(pt, region)) return 1;
	if (seven->find(pt, region)) return 1;
	
	return 0;
}

void Octree::range_query(vector<Point3D*>& points, sphere radius){
	//if (!(this->volume.intersects(radius))) return; //octant not within the radius
	if (!radius.bbox_in_sphere(this->volume)) return; //no intersection between sphere and current octant
	for (int i = 0; i < this->points.size(); i++){
		//if (this->volume.point_in_bbox(this->points.at(i))) 
		if (radius.point_in_sphere(this->points.at(i)))
			points.push_back(this->points.at(i));
	}
	if (!zero) return;
	zero->range_query(points, radius);
	one->range_query(points, radius);
	two->range_query(points, radius);
	three->range_query(points, radius);
	four->range_query(points, radius);
	five->range_query(points, radius);
	six->range_query(points, radius);
	seven->range_query(points, radius);
}

void Octree::draw(){
	if (!zero) return;
	glBegin(GL_LINES);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	//glColor3d(255,20,47);
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
	
	//glColor3d(255,20,47);
	
	/////////////y//////////
	
	//glColor3d(255, 0, 0);
	glVertex3d(seven->volume.maxi.x, volume.mini.y, zero->volume.maxi.z); 
	glVertex3d(zero->volume.mini.x, volume.mini.y, seven->volume.mini.z);
	
	glVertex3d(seven->volume.mini.x, volume.mini.y, zero->volume.mini.z);
	glVertex3d(zero->volume.maxi.x, volume.mini.y, seven->volume.maxi.z);
	
	//glColor3d(255,20,47);
	glVertex3d(seven->volume.maxi.x, volume.maxi.y, zero->volume.maxi.z); 
	glVertex3d(zero->volume.mini.x, volume.maxi.y, seven->volume.mini.z);
	
	glVertex3d(seven->volume.mini.x, volume.maxi.y, zero->volume.mini.z);
	glVertex3d(zero->volume.maxi.x, volume.maxi.y, seven->volume.maxi.z);
	
	glVertex3d(seven->volume.maxi.x, 0, zero->volume.maxi.z); 
	glVertex3d(zero->volume.mini.x, 0, seven->volume.mini.z);
	
	glVertex3d(seven->volume.mini.x, 0, zero->volume.mini.z);
	glVertex3d(zero->volume.maxi.x, 0, seven->volume.maxi.z);
	
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

