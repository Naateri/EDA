#include <iostream>
#include "RTree.h"

#define is_not !=

RTree::RTree(int m){
	this->m = m;
	this->root = new RNode;
}

void RTree::choose_leaf(RNode*& N, B_Box* elem){
	//CL2
	if (N->pointers.size() == 0){ //N is a leaf
		return;
	}
	cout << "Going to a leaf\n";
	//while (N->pointers.size() > 0){
	//CL3
	
	B_Box* cur_bbox = new B_Box;
	int pos;
	double cur_area, enlargement, min_enlargement = 999999;
	for(int i = 0; i < N->objects.size(); i++){
		N->adjust_rectangle();
		*cur_bbox = N->covering_rectangle;
		N->objects.push_back(elem);
		N->adjust_rectangle();
		
		//enlargement = cur_bbox->area() - N->covering_rectangle.area();
		enlargement = abs(cur_bbox->area() - N->covering_rectangle.area());
		if (enlargement < 0) enlargement *= -1;
		if (enlargement == min_enlargement){
			if (cur_bbox->area() < cur_area){ //resolve ties
				pos = i; //choosing entry with smaller area
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
	
	//CL4
	
	N = N->pointers.at(pos);
	cout << "leaf chosen so far: " << pos << endl;
	//}
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

void RTree::adjust_tree(RNode*& L, RNode* LL){
	RNode* N = L, *NN = LL; //AT1
	//AT2
	while (N != this->root){ //if n is the root, stop
		//AT3
		RNode* P = N->parent, *temp;
		N->adjust_rectangle();
		//AT4
		RNode* PP = 0;
		if (NN is_not NULL){
			pair<RNode*, RNode*> split_result;
			P->pointers.push_back(NN);
			P->objects.push_back(&NN->covering_rectangle);
			if (P->objects.size() > m){
				split_result = QuadraticSplit(P);
				P = split_result.first;
				PP = split_result.second;
			}
		}
		//AT 5
		N = P; 
		NN = PP;
	}
	
}

pair<RNode*, RNode*> RTree::QuadraticSplit(RNode*& cur_node){
	int i, j, pick_next;
	bool group;
	RNode* child1, *child2;
	pair<RNode*, RNode*> res;
	B_Box* J;// = new B_Box;
	pair<int, int> pick_seeds = PickSeeds(cur_node, J);
	i = pick_seeds.first; j = pick_seeds.second;
	B_Box* j_node = cur_node->objects.at(j);
	
	cout << "PickSeeds Done\n";
	
	child1 = new RNode;
	child2 = new RNode;
	
	RNode* cur_node_p = cur_node->parent;	
	//setting parent
	//child1->parent = child2->parent = cur_node;	
	
	child1->objects.push_back(cur_node->objects.at(i)); //pushing to first child
	child2->objects.push_back(cur_node->objects.at(j)); //pushing to second child
	if (cur_node->pointers.size() > 0){
		child1->pointers.push_back(cur_node->pointers.at(i));
		child2->pointers.push_back(cur_node->pointers.at(j));
		
		cur_node->pointers.erase(cur_node->pointers.begin() + i); //erasing ptr from original node
	}
	cur_node->objects.erase(cur_node->objects.begin() + i); //erasing from original node
	j = 0;
	while(true){
		if (cur_node->objects.at(j) == j_node) break;
		j++;
	}
	cout << "erasing new j: " << j << endl;
	cur_node->objects.erase(cur_node->objects.begin() + j); //erasing from original node	
	
	if (cur_node->pointers.size() > 0){
		cur_node->pointers.erase(cur_node->pointers.begin() + j); //erasing ptr from original node
	}
	
	child1->adjust_rectangle();
	child2->adjust_rectangle();
	
	int cur_size = cur_node->objects.size(); //to go through rest of nodes
	
	for(i = 0; i < cur_size; i++){ //for each node in the original node
		if (child1->objects.size() + cur_size - i - 1 == this->m>>1){ 
			//if all the other objects need to be pushed to child1 so that
			//it has m/2 entries, push them all
			for(vector<B_Box*>::iterator it = cur_node->objects.begin();
				it != cur_node->objects.end(); it++){
				child1->objects.push_back(*it);
			}
			for(vector<RNode*>::iterator it = cur_node->pointers.begin();
				it != cur_node->pointers.end(); it++){
				child1->pointers.push_back(*it);
			}
			break;
		}
		
		if (child2->objects.size() + cur_size - i - 1 == this->m>>1){
			//if all the other objects need to be pushed to child2 so that
			//it has m/2 entries, push them all
			for(vector<B_Box*>::iterator it = cur_node->objects.begin();
				it != cur_node->objects.end(); it++){
				child2->objects.push_back(*it);
			}
			for(vector<RNode*>::iterator it = cur_node->pointers.begin();
				it != cur_node->pointers.end(); it++){
					child2->pointers.push_back(*it);
			}
			break;
		}
		
		pick_next = PickNext(cur_node, child1, child2, group);
		if (group){
			child1->objects.push_back(cur_node->objects.at(pick_next));
			
			if (cur_node->pointers.size() > 0) child1->pointers.push_back(cur_node->pointers.at(pick_next));
			
		} else {
			child2->objects.push_back(cur_node->objects.at(pick_next));
			
			if (cur_node->pointers.size() > 0) child2->pointers.push_back(cur_node->pointers.at(pick_next));
		}
		cur_node->objects.erase(cur_node->objects.begin() + pick_next);
		if (cur_node->pointers.size() > 0) cur_node->pointers.erase(cur_node->pointers.begin() + pick_next);
	}
	
	cur_node->objects.clear();
	cur_node->pointers.clear();
	
	if (cur_node == root){
		cout << "Root updated\n";
		root_split = 1;
		
		child1->parent = root;
		child2->parent = root;
		
		child1->adjust_rectangle();
		child2->adjust_rectangle();
		
		root->pointers.push_back(child1);
		root->pointers.push_back(child2);
		
		J = new B_Box;
		
		J->top_left.x = child1->covering_rectangle.top_left.x;// - 5;
		J->top_left.y = child1->covering_rectangle.top_left.y;// + 5;
		J->bottom_right.x = child1->covering_rectangle.bottom_right.x;// + 5;
		J->bottom_right.y = child1->covering_rectangle.bottom_right.y;// - 5;
		
		//cur_node->objects.push_back(J); //child1 info
		root->objects.push_back(&child1->covering_rectangle);
		root->objects.push_back(&child2->covering_rectangle);
		
		J = new B_Box;
		J->top_left.x = child2->covering_rectangle.top_left.x;// - 5;
		J->top_left.y = child2->covering_rectangle.top_left.y;// + 5;
		J->bottom_right.x = child2->covering_rectangle.bottom_right.x;// + 5;
		J->bottom_right.y = child2->covering_rectangle.bottom_right.y;// - 5;
		
		//cur_node->objects.push_back(J); //child2 info
		root->adjust_rectangle();
	}
	
	if (cur_node != root){
		cout << "just one extra node\n";
		//child1 = cur_node;
		cur_node = child1;
		child1->parent = cur_node_p;
		child2->parent = cur_node_p;
		//Comment below if stuff is not working
		
		child1->adjust_rectangle();
		child2->adjust_rectangle(); 
		
		cur_node_p->pointers.push_back(child1);
		cur_node_p->pointers.push_back(child2);
		
		J = new B_Box;
		
		J->top_left.x = child1->covering_rectangle.top_left.x;// - 5;
		J->top_left.y = child1->covering_rectangle.top_left.y;// + 5;
		J->bottom_right.x = child1->covering_rectangle.bottom_right.x;// + 5;
		J->bottom_right.y = child1->covering_rectangle.bottom_right.y;// - 5;
		
		//cur_node_p->objects.push_back(J); //child1 info
		cur_node_p->objects.push_back(&child1->covering_rectangle);
		cur_node_p->objects.push_back(&child2->covering_rectangle);
		
		J = new B_Box;
		J->top_left.x = child2->covering_rectangle.top_left.x; //-5;
		J->top_left.y = child2->covering_rectangle.top_left.y; //+5;
		J->bottom_right.x = child2->covering_rectangle.bottom_right.x; //+5;
		J->bottom_right.y = child2->covering_rectangle.bottom_right.y; //-5;
		
		//cur_node_p->objects.push_back(J); 
		
		cur_node_p->adjust_rectangle();
		
		cout << "parent size: " << cur_node_p->objects.size() << endl;
		
		if (cur_node_p->objects.size() > m){
			QuadraticSplit(cur_node_p);
			//adjust_tree(child1, child2);
		}
	}
	
	res.first = child1;
	res.second = child2;
	cout << "QuadraticSplit over\n";
	return res;
}

bool RTree::insert(B_Box* elem){
	RNode* temp;
	temp = this->root;
	root_split = 0;
	choose_leaf(temp, elem);
	cout << "Element inserted\n";
	
	temp->objects.push_back(elem);
	//temp->adjust_rectangle();
	if (temp->objects.size() <= m) {
		cout << "Size in current leaf: " << temp->objects.size() << endl;
		//adjust_tree(temp, 0);
		temp->adjust_rectangle();
		return 1;
	}
	
	cout << "Splitting\n";
	cout << "Points before the split: ";
	temp->print_points();
	pair<RNode*, RNode*> split_result;
	//int n; cin >> n;
	split_result = QuadraticSplit(temp);
	if (root_split) return 1;
	//adjust_tree(split_result.first, split_result.second); 
	return 1;
}

void RTree::draw_visits(RNode* cur_node, int alt){
	if (cur_node == 0) return;
	if (alt == 0) {
		glColor3d(255,0,0);
	} else if (alt == 1) {
		glColor3d(0,255,0);
	} else {
		glColor3d(0,0,255);
	}
	if (cur_node->pointers.size() == 0) {
		glColor3d(0,0,255);
		/*for(int i = 0; i < cur_node->objects.size(); i++){		
			glRectf( (float)cur_node->objects.at(i)->top_left.x, (float)cur_node->objects.at(i)->top_left.y,
					(float)cur_node->objects.at(i)->bottom_right.x, (float)cur_node->objects.at(i)->bottom_right.y );	
		}*/
	}
	cur_node->adjust_rectangle();
	//glColor3d(255,0,0);
	for(int i = 0; i < cur_node->objects.size(); i++){		
		glRectf( (float)cur_node->objects.at(i)->top_left.x, (float)cur_node->objects.at(i)->top_left.y,
			(float)cur_node->objects.at(i)->bottom_right.x, (float)cur_node->objects.at(i)->bottom_right.y );		
	}
	//glRectf( (float)cur_node->covering_rectangle.top_left.x, (float)cur_node->covering_rectangle.top_left.y,
	//		(float)cur_node->covering_rectangle.bottom_right.x, (float)cur_node->covering_rectangle.bottom_right.y);
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
