#include "va_file.h"

double distance(PointND* a, PointND* b){
	double result, temp_sum;
	result = 0;
	for(int i = 0; i < a->point.size(); i++){
		temp_sum = pow(a->point[i] - b->point[i],2);
		result += temp_sum;
	}
	return sqrt(result);
}

VA_File::VA_File(int dim){
	this->dimensions = dim;
	int b;
	bits_for_dimension.resize(dimensions);
	marks.resize(dimensions);
	b = sizeof(double) * dimensions;
	//int b = (sizeof(double) * this->dimensions) << 3;
	for (int j = 1; j <= dimensions; j++){
		this->bits_for_dimension[j-1] = floor(b/dim);
		if (j <= (b % dim)) this->bits_for_dimension[j-1]++;
		
		marks[j-1].resize( (1 << bits_for_dimension[j-1]) + 1);
		//cout << "bits in dimension " << j << ": " << bits_for_dimension[j-1] << endl;
	}
	//int q; cin >> q;
}

void VA_File::build(vector<PointND*> pts){
	//first we need the minimum and maximum values for each dimension
	vector<double> min_dims (dimensions);
	vector<double> max_dims (dimensions);
	PointND* cur_point;
	for(int i = 0; i < dimensions; i++){
		min_dims[i] = 9999999;
		max_dims[i] = -9999999;
	}
	for(int i = 0; i < pts.size(); i++){
		cur_point = pts[i];
		for(int dim = 0; dim < dimensions; dim++){
			if (cur_point->point[dim] < min_dims[dim]){
				min_dims[dim] = cur_point->point.at(dim);
			}
			if (cur_point->point[dim] > max_dims[dim]){
				max_dims[dim] = cur_point->point.at(dim);
			}
		}
	}
	//minimum and maximum values for each dimension found
	//we now put values on the divisions
	double difference, add_to, cur_value;
	for(int i = 0; i < this->dimensions; i++){
		cur_value = min_dims[i];
		difference = max_dims[i] - min_dims[i];
		add_to = difference/this->bits_for_dimension[i];
		for(int j = 0; j < this->marks[i].size(); j++){
			this->marks[i][j] = cur_value;
			cur_value += add_to;
		}
	}
	//values put on the divisions
	//we now find the approximations for each vector
	string approx, cur_dim_approx;
	this->approximations.resize(pts.size());
	int bits, cur_cell;
	for (int i = 0; i < pts.size(); i++){
		//approximation for every point
		approx.clear();
		for(int j = 0; j < this->dimensions; j++){ //going through every dimension
			cur_dim_approx.clear();
			bits = this->bits_for_dimension[j];
			cur_cell = 0;
			while (this->marks[j][cur_cell++] < pts[i]->point[j]){
				;//finding the right place
			}
			bitset<64> b(cur_cell-1);
			cur_dim_approx = b.to_string(); //bits approximation at dimension j
			approx = approx + cur_dim_approx.substr(cur_dim_approx.size() - bits, cur_dim_approx.size());
		}
		this->approximations[i] = approx;
	}
	//approximations found
	this->points = pts;
}

double VA_File::l_bound(string a_i, PointND* q){
	//based on:
	//https://stackoverflow.com/questions/5254838/calculating-distance-between-a-point-and-a-rectangular-box-nearest-point
	double cur_sum = 0, cur_val;
	int cur_bits, index, max_val, min_val;
	string temp_ai = a_i, temp;
	for(int j = 0; j < this->dimensions; j++){
		cur_bits = this->bits_for_dimension[j]; //bits on dimension j
		temp = temp_ai.substr(0, cur_bits);
		bitset<64> b(temp);
		index = b.to_ulong();
		//finding minimum and maximum values of cell at dimension j
		max_val = this->marks[j][index]; min_val = this->marks[j][index-1];
		cur_val = max(min_val - q->point[j],
					  max((double)0, q->point[j] - max_val));
		cur_sum = cur_sum + pow(cur_val, 2);
		temp_ai.erase(0, cur_bits);
	}
	return sqrt(cur_sum);
}

double VA_File::init_candidate(int k){
	for (int j = 0; j < k; j++){
		dst[j].first = DBL_MAX;
	}
	return DBL_MAX;
}

double VA_File::candidate(double d, int i, int k){
	if (d < dst[k-1].first){
		dst[k-1].first = d;
		dst[k-1].second = i;
		sort(dst.begin(), dst.end());
	}
	return dst[k-1].first;
}

void VA_File::simple_search(PointND* pt, int k){
	double l_i, d;
	dst.clear();
	dst.resize(k);
	knn_index.resize(k);
	d = init_candidate(k);
	for(int i = 0; i < approximations.size(); i++){
		l_i = l_bound(approximations[i], pt);
		if (this->points[i] == pt) continue;
		if (l_i < d) d = candidate(distance(this->points[i], pt), i, k);
	}
	
	for (int i = 0; i < k; i++){
		knn_index[i] = dst[i].second;
		qtree.push_back(points[i]);
	}
}
