#include <iostream>
#include <vector>
//#include <ctime>
#include <cstdlib>
#include <cmath>

struct dimension_obj;

typedef std::vector<float> float_vec;
typedef std::vector<dimension_obj> dim_vec;

int rdtsc(){ //seed
	__asm__ __volatile__("rdtsc");
}

struct dimension_obj{
	///stores each dimension in a vector, qty limits size
	float_vec objects;
	int qty;
	dimension_obj(){
		this->qty = 0;
	}
	dimension_obj(int qty){ //creates an object with qty dimensions
		this->qty = qty;
		//srand (time (NULL) );
		srand(rdtsc());
		for (int i = 0; i < qty; i++){
			//objects.at(i) = ((float) rand() / (RAND_MAX));//fill vector
			objects.push_back((float) rand() / (RAND_MAX));
		}
	}
	void print(){ //testing purposes
		std::cout << "Object X\n";
		for (int i = 0; i < this->qty; i++){
			std::cout << "point " << i+1 << ": " << objects.at(i) << '\n';
		}
		std::cout << '\n';
	}
};

dim_vec create_points(int dim){ //creates 1000 points of dim dimensions
	dim_vec result(1000);
	for (int i = 0; i < 1000; i++){
		dimension_obj temp(dim);
		result.at(i) = temp; 
	}
	return result;
}

float distance(dimension_obj& A, dimension_obj& B){ //euclidean distance
	float result, temp_sum;
	result = 0;
	for(int i = 0; i < A.qty; i++){ //A.qty: amount of dimensions of A
		temp_sum = pow(A.objects.at(i) - B.objects.at(i),2);
		result += temp_sum;
	}
	return sqrt(result);
}

void distances(float& avg, float& max, float& min, dim_vec& points){ //average, min and max distances
	avg = max = 0; //on a certain set of 1000 points
	min = 999999999;
	float cur_dist;
	int total = 0; //for average calculation
	for(int i = 0; i < 1000; i++){
		for(int j = i+1; j < 1000; j++){
			cur_dist = distance(points.at(i), points.at(j)); //current distance found
			avg += cur_dist;
			total++;
			if (cur_dist > max) max = cur_dist; //finding a new max (if there is one)
			if (cur_dist < min) min = cur_dist; //finding a new min (if there is one)
		}
	}
	avg = avg / total;
}

void get_distances(int dimensions){ //calculates min, max, average distances for dimensions dimensions
	std::cout << dimensions << " dimensions\n"; //and outputs the result
	dim_vec two_d = create_points(dimensions);
	/*for (int i = 0; i < two_d.size(); i++){
		two_d.at(i).print();
	}*/
	float avg, max, min;
	distances(avg, max, min, two_d);
	std::cout << "Max distance: " << max << ", min distance: " << min << ", average: " << avg << ".\n";
}

int main(){
	int test_dimensions[] = {2,3,10,100,200,500,1000,2000,3000,4000,5000}; //dimensions to test with
	for (int i = 0; i < 11; i++){
		get_distances(test_dimensions[i]);
	}
	return 0;
}
