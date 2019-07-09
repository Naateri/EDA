#ifndef VAFILE_H
#define VAFILE_H

#include <math.h>
#include <vector>
#include <GL/glut.h>
#include <bitset>
#include <iostream>
#include <string>
#include <algorithm>
#include <cfloat>
#include <limits>
#include <utility>

using namespace std;

struct PointND{
	vector<double> point;
	PointND(int dim){
		//point = new double[dim]();
		point.resize(dim);
	}
};

class VA_File{
private:
	int dimensions;
	vector<int> bits_for_dimension;
	vector<vector<double>> marks;
	vector<string> approximations;
	vector<PointND*> points;
	vector< pair<double, int> > dst; //first: dist, second: index
	double l_bound(string a_i, PointND* q);
	double init_candidate(int k);
	double candidate(double d, int i, int k);
public:
	vector<int> knn_index;
	vector<PointND*> qtree;
	VA_File(int dim);
	void build(vector<PointND*> pts);
	void simple_search(PointND* pt, int k); //knn
};

#endif
