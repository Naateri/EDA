#include <iostream>
#include "RTree.h"

RTree::RTree(int m){
	this->m = m;
	this->root = new RNode;
}

void RTree::choose_leaf(RNode*& N, B_Box* elem){
	if (N->pointers.size() == 0) return; //N is a leaf
	cout << "Going to a leaf\n";
	B_Box* cur_bbox = new B_Box;
	int pos;
	double cur_area, enlargement, min_enlargement = 999999;
	for(int i = 0; i < N->objects.size(); i++){
		*cur_bbox = N->covering_rectangle;
		N->objects.push_back(elem);
		N->adjust_rectangle();
		
		enlargement = cur_bbox->area() - N->covering_rectangle.area();
		if (enlargement < 0) enlargement *= -1;
		if (enlargement == min_enlargement){
			if (cur_bbox->area() < cur_area){
				pos = i;
				cur_area = cur_bbox->area();
			}
		}
		else if (enlargement < min_enlargement){
			pos = i;
			cur_area = cur_bbox->area();
			min_enlargement = enlargement;
		}
		N->objects.erase(N->objects.end() - 1);
		
		N->adjust_rectangle();
	}
	
	delete cur_bbox;
	N = N->pointers.at(pos);
	choose_leaf(N, elem);
}

pair<int, int> RTree::PickSeeds(RNode*& cur_node, B_Box*& max_J){ //BIEN
	//returns index of selected pair
	double d, max_d;
	max_d = -9999999;
	//max_d = 9999999;
	pair<int, int> result;
	B_Box* J = new B_Box;
	//B_Box J; //ver cual top left es mas top left, mismo con bottom right
	for (int i = 0; i < cur_node->objects.size(); i++){
		for (int j = i+1; j < cur_node->objects.size(); j++){
			//i,j current pair
			//top left
			if (cur_node->objects.at(i)->top_left.x < cur_node->objects.at(j)->top_left.x)
				J->top_left.x = cur_node->objects.at(i)->top_left.x;
			else 
				J->top_left.x = cur_node->objects.at(j)->top_left.x;
			if (cur_node->objects.at(i)->top_left.y > cur_node->objects.at(j)->top_left.y)
				J->top_left.y = cur_node->objects.at(i)->top_left.y;
			else 
				J->top_left.y = cur_node->objects.at(j)->top_left.y;
			//bottom right
			if (cur_node->objects.at(i)->bottom_right.x > cur_node->objects.at(j)->bottom_right.x)
				J->bottom_right.x = cur_node->objects.at(i)->bottom_right.x;
			else
				J->bottom_right.x = cur_node->objects.at(j)->bottom_right.x;
			if (cur_node->objects.at(i)->bottom_right.y < cur_node->objects.at(j)->bottom_right.y)
				J->bottom_right.y = cur_node->objects.at(i)->bottom_right.y;
			else
				J->bottom_right.y = cur_node->objects.at(j)->bottom_right.y;
			//J->grow(5);
			d = J->area() - cur_node->objects.at(i)->area() - cur_node->objects.at(j)->area();
			if (d > max_d){
				max_d = d;
				result.first = i;
				result.second = j;
				cout << "i: " << i << ", j: " << j << endl;
				cout << "d: " << d << endl;
				max_J = J;
			}
		}
	}
	return result;
}

int RTree::PickNext(RNode*& cur_node, RNode* child1, RNode* child2, bool& group){ //returns index of next node
	double d1, d2, dif_d, max_dif;
	bool max;
	max_dif = -1;
	int res;
	B_Box J;
	for(int i = 0; i < cur_node->objects.size(); i++){
		d1 = child1->covering_rectangle.area();
		child1->objects.push_back(cur_node->objects.at(i));
		child1->adjust_rectangle();
		d1 = d1 - child1->covering_rectangle.area();
		child1->objects.erase(child1->objects.end() - 1); //erasing object inserted
		child1->adjust_rectangle();
		
		d2 = child2->covering_rectangle.area();
		child2->objects.push_back(cur_node->objects.at(i));
		child2->adjust_rectangle();
		d2 = d2 - child2->covering_rectangle.area();
		child2->objects.erase(child2->objects.end() - 1); //erasing object inserted
		child2->adjust_rectangle();
		
		cout << "picknext d1 " << d1 << ", d2 " << d2 << endl;
		
		if (d2 > d1){
			dif_d = d2-d1;
			max = 1;
		}
		else{
			dif_d = d1-d2;
			max = 0;
		}
		
		if (dif_d > max_dif){
			max_dif = dif_d;
			res = i;
			group = max; //0 if d1 is bigger, 1 if d2 is bigger
			//so, if 0 put on child2 if 1 put on child1
		}
	}
	return res;
}

void RTree::QuadraticSplit(RNode*& cur_node){
	int i, j, pick_next;
	bool group;
	RNode* child1, *child2;
	B_Box* J;// = new B_Box;
	pair<int, int> pick_seeds = PickSeeds(cur_node, J);
	i = pick_seeds.first; j = pick_seeds.second;
	B_Box* j_node = cur_node->objects.at(j);
	
	cout << "PickSeeds Done\n";
	
	child1 = new RNode;
	child2 = new RNode;
	
	child1->objects.push_back(cur_node->objects.at(i)); //pushing to first child
	child2->objects.push_back(cur_node->objects.at(j)); //pushing to second child
	cur_node->objects.erase(cur_node->objects.begin() + i); //erasing from original node
	j = 0;
	while(true){
		if (cur_node->objects.at(j) == j_node) break;
		j++;
	}
	cout << "erasing new j: " << j << endl;
	cur_node->objects.erase(cur_node->objects.begin() + j); //erasing from original node	
	
	child1->adjust_rectangle();
	child2->adjust_rectangle();
	
	int cur_size = cur_node->objects.size(); //to go through rest of nodes
	cout << "Created two childs\n"; 
	for(i = 0; i < cur_size; i++){
		if (child1->objects.size() +  cur_size - i - 1 == this->m>>1){ 
			//if all the other objects need to be pushed to child1 so that
			//it has m/2 entries, push them all
			for(vector<B_Box*>::iterator it = cur_node->objects.begin();
				it != cur_node->objects.end(); it++){
				child1->objects.push_back(*it);
			}
			break;
		}
		
		if (child2->objects.size() +  cur_size - i - 1 == this->m>>1){
			//if all the other objects need to be pushed to child2 so that
			//it has m/2 entries, push them all
			for(vector<B_Box*>::iterator it = cur_node->objects.begin();
				it != cur_node->objects.end(); it++){
				child2->objects.push_back(*it);
			}
			break;
		}
		
		pick_next = PickNext(cur_node, child1, child2, group);
		if (group){
			child1->objects.push_back(cur_node->objects.at(pick_next));
			//cur_node->objects.erase(cur_node->objects.begin() + pick_next);
		} else {
			child2->objects.push_back(cur_node->objects.at(pick_next));
			//cur_node->objects.erase(cur_node->objects.begin() + pick_next);
		}
		cur_node->objects.erase(cur_node->objects.begin() + pick_next);
	}
	
	cur_node->objects.clear();
	
	cout << "cur_node size " << cur_node->objects.size() << endl;
	cout << "child 1 size " << child1->objects.size() << endl;
	cout << "child 2 size " << child2->objects.size() << endl;
	
	child1->print_points();
	child2->print_points();
	
	child1->adjust_rectangle();
	child2->adjust_rectangle();
	
	cur_node->pointers.push_back(child1);
	cur_node->pointers.push_back(child2);
	
	/*if (child1->covering_rectangle.top_left.x < child2->covering_rectangle.top_left.x)
		J->top_left.x = child1->covering_rectangle.top_left.x;
	else J->top_left.x = child2->covering_rectangle.top_left.x;
	
	if (child1->covering_rectangle.top_left.y > child2->covering_rectangle.top_left.y)
		J->top_left.y = child1->covering_rectangle.top_left.y;
	else J->top_left.y = child2->covering_rectangle.top_left.y;

	if (child1->covering_rectangle.bottom_right.x > child2->covering_rectangle.bottom_right.x)
		J->bottom_right.x = child1->covering_rectangle.bottom_right.x;
	else J->bottom_right.x = child2->covering_rectangle.bottom_right.x;
	
	if (child1->covering_rectangle.bottom_right.y < child2->covering_rectangle.bottom_right.y)
		J->bottom_right.y = child1->covering_rectangle.bottom_right.y;
	else J->bottom_right.y = child2->covering_rectangle.bottom_right.y; */
	
	J->top_left.x = child1->covering_rectangle.top_left.x - 10;
	J->top_left.y = child1->covering_rectangle.top_left.y + 10;
	J->bottom_right.x = child1->covering_rectangle.bottom_right.x + 10;
	J->bottom_right.y = child1->covering_rectangle.bottom_right.y - 10;
	
	cur_node->objects.push_back(J);
	//cur_node->objects.push_back(&child1->covering_rectangle);
	//cur_node->objects.push_back(&child2->covering_rectangle);
	J = new B_Box;
	J->top_left.x = child2->covering_rectangle.top_left.x - 5;
	J->top_left.y = child2->covering_rectangle.top_left.y + 5;
	J->bottom_right.x = child2->covering_rectangle.bottom_right.x + 5;
	J->bottom_right.y = child2->covering_rectangle.bottom_right.y - 5;
	
	cur_node->objects.push_back(J);
	
	/*cout << "Father's top left: " << cur_node->objects.at(0)->top_left.x << ' ' << cur_node->objects.at(0)->top_left.y << endl;
	cout << "Father's bottom right: " << cur_node->objects.at(0)->bottom_right.x << ' ' << cur_node->objects.at(0)->bottom_right.y << endl;*/
	
	if (cur_node == root){
		cout << "Root updated\n";
		/*cout << "Root's top left: " << root->objects.at(0)->top_left.x << ' ' << root->objects.at(0)->top_left.y << endl;
		cout << "Root's bottom right: " << root->objects.at(0)->bottom_right.x << ' ' << root->objects.at(0)->bottom_right.y << endl;*/
	}
	cout << "QuadraticSplit over\n";
}

bool RTree::insert(B_Box* elem){
	RNode* temp;
	temp = this->root;
	choose_leaf(temp, elem);
	cout << "Element inserted\n";
	
	temp->objects.push_back(elem);
	temp->adjust_rectangle();
	if (temp->objects.size() <= m) {
		cout << "Size in current leaf: " << temp->objects.size() << endl;		
		return 1;
	}
	
	cout << "Splitting\n";
	cout << "Points before the split: ";
	temp->print_points();
	//int n; cin >> n;
	QuadraticSplit(temp);
	return 1;
}

void RTree::draw_visits(RNode* cur_node, int alt){
	if (cur_node == 0) return;
	/*if (root->pointers.size() >= 2){
		if (cur_node == root->pointers.at(0)) cout << "child1\n";
		else if (cur_node == root->pointers.at(1)) cout << "child2\n";
	}*/
	if (alt == 0) {
		glColor3d(255,0,0);
	} else {
		glColor3d(0,255,0);
	}
	for(int i = 0; i < cur_node->objects.size(); i++){		
		glRectf( (float)cur_node->objects.at(i)->top_left.x, (float)cur_node->objects.at(i)->top_left.y,
			(float)cur_node->objects.at(i)->bottom_right.x, (float)cur_node->objects.at(i)->bottom_right.y );		
	}
	alt++;
	alt = alt%2;
	for(int i = 0; i < cur_node->pointers.size(); i++){
		draw_visits(cur_node->pointers.at(i), alt);
	}
}

void RTree::draw(){
	RNode* temp = this->root;
	int alt = 0;
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	draw_visits(temp, alt);
}
