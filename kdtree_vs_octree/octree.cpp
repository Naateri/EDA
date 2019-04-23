#include "octree.h"

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
	return pt->values[0] >= volume.mini.values[0] && pt->values[0] <= volume.maxi.values[0] &&
		pt->values[1] >= volume.mini.values[1] && pt->values[1] <= volume.maxi.values[1]
		&& pt->values[2] >= volume.mini.values[2] && pt->values[2] <= volume.maxi.values[2];
	
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
	float x2 = ((volume.maxi.values[0] + volume.mini.values[0]) / 2);
	float y2 = ((volume.maxi.values[1] + volume.mini.values[1]) / 2);
	float z2 = (( volume.maxi.values[2] + volume.mini.values[2] ) / 2);
	mini = volume.mini;
	maxi.values[0] = x2;
	maxi.values[1] = y2;
	maxi.values[2] = z2;
	zero = new Octree(mini, maxi);
	//cout << "zero->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.values[0] = x2;
	mini.values[1] = volume.mini.values[1];
	mini.values[2] = volume.mini.values[2];
	maxi.values[0] = volume.maxi.values[0];
	maxi.values[1] = y2;
	maxi.values[2] = z2;
	one = new Octree(mini, maxi);
	//cout << "one->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.values[0] = volume.mini.values[0];
	mini.values[1] = y2;
	mini.values[2] = volume.mini.values[2];
	maxi.values[0] = x2;
	maxi.values[1] = volume.maxi.values[1];
	maxi.values[2] = z2;
	two = new Octree(mini, maxi);
	
	mini.values[0] = x2;
	mini.values[1] = y2;
	mini.values[2] = volume.mini.values[2];
	maxi = volume.maxi;
	maxi.values[2] = z2;
	three = new Octree(mini, maxi);
	
	///Bottom part: z/2 (or so I thought but it's not just that ggwp)
	
	//cout << "bottom half\n";
	
	mini = volume.mini;
	mini.values[2] = z2;
	maxi.values[0] = x2;
	maxi.values[1] = y2;
	maxi.values[2] = volume.maxi.values[2];
	four = new Octree(mini, maxi);
	//cout << "zero->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.values[0] = x2;
	mini.values[1] = volume.mini.values[1];
	mini.values[2] = z2;
	maxi.values[0] = volume.maxi.values[0];
	maxi.values[1] = y2;
	maxi.values[2] = volume.maxi.values[2];	
	five = new Octree(mini, maxi);
	//cout << "one->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.values[0] = volume.mini.values[0];
	mini.values[1] = y2;
	mini.values[2] = z2;
	maxi.values[0] = x2;
	maxi.values[1] = volume.maxi.values[1];
	maxi.values[2] = volume.maxi.values[2];
	six = new Octree(mini, maxi);
	//cout << "two->top_left: " << top_left.x << ", " << top_left.y << "; bottom_right: " << bottom_right.x << ", " << bottom_right.y << endl;
	
	mini.values[0] = x2;
	mini.values[1] = y2;
	mini.values[2] = z2;
	maxi = volume.maxi;
	maxi.values[2] = volume.maxi.values[2];
	
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
			if (points.at(i)->values[0] == pt->values[0] && points.at(i)->values[1] == pt->values[1]
					&& points.at(i)->values[2] == pt->values[2]){
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

void Octree::set_maxpts(int pts){
	this->max_elements = pts;
}

Point3D* Octree::get_rand_point(){
	srand(rand());
	return points.at(rand()%points.size());
}

void Octree::print_points(){
	cout << "Points in this region are:\n";
	for(int i = 0; i < this->points.size(); i++){
		points.at(i)->print();
	}
}
