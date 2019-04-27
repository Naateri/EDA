#include "kdtree.h"

bool comp_x(Point3D* i, Point3D* j){
	if (i->values[0] == j->values[0]){
		if (i->values[1] == j->values[1]){
			return i->values[2] < j->values[2];
		} else return i->values[1] < j->values[1];
	}
	return i->values[0] < j->values[0];
}

bool comp_y(Point3D* i, Point3D* j){
	if (i->values[1] == j->values[1]){
		if (i->values[0] == j->values[0]){
			return i->values[2] < j->values[2];
		} else return i->values[0] < j->values[0];
	}
	return i->values[1] < j->values[1];
}

bool comp_z(Point3D* i, Point3D* j){
	if (i->values[2] == j->values[2]){
		if (i->values[0] == j->values[0]){
			return i->values[1] < j->values[1];
		} else return i->values[0] < j->values[0];
	}
	return i->values[2] < j->values[2];
}

KDTree::KDTree(){
	root = 0;
}

KDTree::KDTree(vector<Point3D*> points, bbox_3d lim, int maxp){
	bbox_3d low_bound, up_bound;
	this->points = points;
	this->limits = lim;
	this->max_points = maxp;
	two_five = points.size() * 0.25;
	fifty = points.size() * 0.5;
	seven_five = points.size() * 0.75;
	b_two_five = b_fifty = b_seven_five = 0;
	
	float median_x;
	Node* temp;
	
	up_bound = low_bound = lim;
	
	sort(this->points.begin(), this->points.end(), comp_x);
	this->root = new Node(this->points[this->points.size()>>1], this->limits);
	this->root->region = points;
	
	//after sorting, this->points[this->points.size()>>1] is the median
	clock_t begin = clock();
	clock_t* ptr_begin = &begin;
	median_x = this->points[this->points.size()>>1]->values[0];
	low_bound.maxi.values[0] = median_x;
	up_bound.mini.values[0] = median_x;
	
	root->left = new Node(0, low_bound);
	root->right = new Node(0, up_bound);
	//will be updated soon
	root->left->region = root->region;
	root->right->region = root->region;
	
	int current_points = 2;
		
	create(root->left, 1, ptr_begin, current_points);
	create(root->right, 1, ptr_begin, current_points);
	
	clock_t end = clock();
	times[3] = double(end - *ptr_begin)/CLOCKS_PER_SEC + times[2];
	//cout << "Total of points: " << current_points << endl;
	
	//for(int i = 0; i < 4; i++) times[i] *= 10;
	
	cout << "Tree created.\n";
}

bool KDTree::find(Point3D* pt, Node**& p, short cur_dim){
	p = &root;
	while ((*p)->value && (*p)->value != pt){
		if (pt->values[cur_dim] < ((*p)->value)->values[cur_dim]){
			p = &((*p)->left);
		}
		else{
			p = &((*p)->right);
		}
		cur_dim = (cur_dim + 1) % 3;
	}
	if (!(*p)->value) return 0;
	for (int i = 0; i < (*p)->region.size(); i++){
		if ((*p)->region.at(i) == pt) return 1;
		return 0;
	}
	//return !!((*p)->value);
}

bool KDTree::insert(Point3D* pt){
	/*Node** p;
	if (find(pt, p,0)) return 0;*/
	;
}


void KDTree::create(Node* current, short cur_dim, clock_t* begin, int& current_points){
	vector<Point3D*> pts;
	bbox_3d low_bound, up_bound;
	float median;
	Point3D* median_pt;
	low_bound = up_bound = current->limitor;
	for(int i = 0; i < current->region.size(); i++){ //to find median of points only inside bbox
		//current->limitor.push_if_in_bbox(pts, points.at(i));
		//other choice: store points as a node's attribute (maybe in a vector)
		current->limitor.push_if_in_bbox(pts, current->region.at(i));
	}
	current->region.clear();
	current->region = pts;
	if (pts.size() <= max_points){
		current_points += pts.size();
		return; //max points at region, end of creation
	}
	
	if (cur_dim == 0){ //comparing x's
		sort(pts.begin(), pts.end(), comp_x);
	}
	else if (cur_dim == 1){ //comparing y's
		sort(pts.begin(), pts.end(), comp_y);
	}
	else { //comparing z's
		sort(pts.begin(), pts.end(), comp_z);
	}
	
	median = pts.at(pts.size()>>1)->values[cur_dim];
	low_bound.maxi.values[cur_dim] = median;
	up_bound.mini.values[cur_dim] = median;
	
	median_pt = pts.at(pts.size()>>1);
	current->value = median_pt;
	
	current->left = new Node(0, low_bound);
	current->right = new Node(0, up_bound);
	//will be updated
	current->left->region = current->region;
	current->right->region = current->region;
	current->region.clear(); //no need to keep the vector, so let's save some space
	
	cur_dim = (cur_dim+1) % 3;
	
	current_points+=2;
	if (current_points >= (int)two_five && !b_two_five){
		clock_t end = clock();
		times[0] = double(end - *begin)/CLOCKS_PER_SEC;
		b_two_five = 1;
		clock_t new_begin = clock();
		begin = &new_begin;
		//cout << "25%\n";
	} else if (current_points >= (int)fifty && !b_fifty){
		clock_t end = clock();
		times[1] = double(end - *begin)/CLOCKS_PER_SEC + times[0];
		b_fifty = 1;
		clock_t new_begin = clock();
		begin = &new_begin;
		//cout << "50%\n";
	}else if (current_points >= (int)seven_five && !b_seven_five){
		clock_t end = clock();
		times[2] = double(end - *begin)/CLOCKS_PER_SEC + times[1];
		b_seven_five = 1;
		clock_t new_begin = clock();
		begin = &new_begin;
		//cout << "75%\n";
	}
	//cout << "current point: " << cur_points_inserted << endl;
	create(current->left, cur_dim, begin, current_points);
	create(current->right, cur_dim, begin, current_points);
}

void KDTree::preorder(Node* current){
	if (!current->value) return;
	current->value->print();
	if (current->left->value) {
		cout << "Left: ";
		preorder(current->left);
	}
	if (current->right->value) {
		cout << "Right: ";
		preorder(current->right);
	}
}

void KDTree::inorder(Node* current){
	if (!current->value) return;
	inorder(current->left);
	current->value->print();
	inorder(current->right);
}

void KDTree::print_tree(Node* current, short cur_dim){
	if (!current->value) return;
	cout << "Current dimension: " << cur_dim << endl;
	cur_dim = (cur_dim+1)%3;
	cout << "Value: "; current->value->print();
	if (current->left->value) cout << "Left child: "; current->left->value->print();
	if (current->right->value) cout << "Right child: "; current->right->value->print();
	print_tree(current->left, cur_dim);
	print_tree(current->right, cur_dim);
}

void KDTree::draw_kd(Node* current, short current_dimension){
	if (!current->value) return;
	float line;
	line = current->value->values[current_dimension];
	glBegin(GL_LINES);
	if (current_dimension == 0){
		glColor3d(255,0,0);
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.mini.values[2]);
		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.mini.values[2]);
		
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.mini.values[2]);
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.maxi.values[2]);
		
		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.maxi.values[2]);
		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.mini.values[2]);
		
		glVertex3d(line,current->limitor.maxi.values[1],current->limitor.maxi.values[2]);
		glVertex3d(line,current->limitor.mini.values[1],current->limitor.maxi.values[2]);
		
		
	} else if (current_dimension == 1){
		glColor3d(0,255,0);
		
		glVertex3d(current->limitor.mini.values[0],line,current->limitor.mini.values[2]);
		glVertex3d(current->limitor.maxi.values[0],line,current->limitor.mini.values[2]);
		
		glVertex3d(current->limitor.mini.values[0], line, current->limitor.mini.values[2]);
		glVertex3d(current->limitor.mini.values[0], line, current->limitor.maxi.values[2]);
		
		glVertex3d(current->limitor.maxi.values[0], line, current->limitor.maxi.values[2]);
		glVertex3d(current->limitor.maxi.values[0],line,current->limitor.mini.values[2]);
		
		glVertex3d(current->limitor.maxi.values[0],line,current->limitor.maxi.values[2]);
		glVertex3d(current->limitor.mini.values[0],line,current->limitor.maxi.values[2]);
		
		
	} else {
		glColor3d(0,0,255);
		
		glVertex3d(current->limitor.mini.values[0],current->limitor.mini.values[1],line);
		glVertex3d(current->limitor.maxi.values[0],current->limitor.mini.values[1],line);
		
		glVertex3d(current->limitor.mini.values[0],current->limitor.mini.values[1], line);
		glVertex3d(current->limitor.mini.values[0], current->limitor.maxi.values[1],line);
		
		glVertex3d(current->limitor.maxi.values[0], current->limitor.maxi.values[1],line);
		glVertex3d(current->limitor.maxi.values[0],current->limitor.mini.values[1],line);
		
		glVertex3d(current->limitor.maxi.values[0],current->limitor.maxi.values[1],line);
		glVertex3d(current->limitor.mini.values[0],current->limitor.maxi.values[1],line);
		
	}
	glColor3d(255,20,47);
	glEnd();
	current_dimension = (current_dimension+1)%3;
	draw_kd(current->left, current_dimension);
	draw_kd(current->right, current_dimension);
}

Point3D* KDTree::get_random_point(){
	srand(rand());
	return points.at(rand()%points.size());
}
