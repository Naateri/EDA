#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <opencv2/opencv.hpp>

#define WIDTH 736
#define HEIGHT 1024
#define REGIONS 64

using namespace std;

typedef unsigned char uchar;
typedef vector<int> int_vec;
typedef vector<double> double_vec;
typedef ofstream txt_file;
typedef string str;
typedef stringstream ss;

str intToStr(const int& z){
	ss leString;
	leString << z;
	return leString.str();
}

int stringToInt(str a){
	int ret;
	ss convi(a);
	convi >> ret;
	return ret;
}

str num4digits(int z){
	str ret = "";
	while (z < 1000){
		ret += "0";
		z *= 10;
	}
	ret += intToStr(z);
	return ret;
}

double average(int_vec& values){
	double total = 0.0;
	for (int i = 0; i < values.size(); i++){
		total += values.at(i);
	}
	return total / values.size();
}

cv::Vec3b get_RGB(cv::Mat img, int x, int y){
	return img.at<cv::Vec3b>(cv::Point(x,y));
}

double_vec dimensions(const char* filename){
	//get points
	cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);
	int new_rows, new_cols, size, ptr;
	//int_vec red_v, green_v, blue_v;
	cv::Vec3b colors;
	new_rows = img.rows >> 3;
	new_cols = img.cols >> 3;
	size = new_rows * new_cols * 3;
	double_vec dimensions;
	size /= 3;
	int_vec red_v (size);
	int_vec green_v (size);
	int_vec blue_v (size);
	for(int i = 0; i < REGIONS>>3; i++){
		/*red_v.clear();
		//red_v.resize(size);
		green_v.clear();
		//green_v.resize(size);
		blue_v.clear();
		//blue_v.resize(size);*/
		//fill vectors with color values
		for(int j = 0; j < REGIONS>>3; j++){
			ptr = 0;
			for(int y = 0; y < new_rows; y++){
				for(int x = 0; x < new_cols; x++){
					colors = get_RGB(img, new_cols*i +x, new_rows*j + y);
					//cout << "point, x: " << new_cols*i + x << ", y: " << new_rows*j + y << endl;
					//int j; cin >> j;
					red_v.at(ptr) = int(colors[2]);
					green_v.at(ptr) = int(colors[1]);
					blue_v.at(ptr) = int(colors[0]); //opencv stores as BGR
					ptr++;
				}
			}
			dimensions.push_back(average(red_v));
			dimensions.push_back(average(green_v));
			dimensions.push_back(average(blue_v));
		}
	}
	return dimensions;
}

void vec_to_txt(double_vec& dim){
	txt_file result;
	result.open("desc_image_result.txt", fstream::app);
	for(int i = 0; i < dim.size(); i++){
		result << dim.at(i) << ',';
	}
	result << endl;
}

void build_representative(double_vec& dim, int cols, int rows, cv::Mat &result){
	int ptr = 0;
	int new_cols, new_rows;
	new_cols = result.cols >> 3;
	new_rows = result.rows >> 3;
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			cv::Vec3b color;
			color[0] = (int) dim.at(ptr+2);
			color[1] = (int) dim.at(ptr+1);
			color[2] = (int) dim.at(ptr);
			ptr += 3;
			for(int x = 0; x < result.cols >> 3; x++){
				for (int y = 0; y < result.rows >> 3; y++){
					result.at<cv::Vec3b>(cv::Point(new_cols*i + x,new_rows*j + y)) = color;
					//cout << "x: " << new_cols*i + x << ", y: " << new_rows*j + y << endl;
				}
			}
			//result.at<cv::Vec3b>(cv::Point(x,y)) = color;
		}
	}
}


void rgb_values(const char* filename, int_vec& red, int_vec& green, int_vec& blue){
	cv::Mat image;
	int size, ptr = 0;
	cv::Vec3b colors;
	image = cv::imread(filename, cv::IMREAD_COLOR);
	size = image.rows * image.cols * 3;
	red.resize(size/3);
	green.resize(size/3);
	blue.resize(size/3);
	//int_vec values;
	for(int y = 0; y < image.rows; y++){
		for(int x = 0; x < image.cols; x++){
			colors = get_RGB(image, x, y);
			/*values.at(ptr++) = int(colors[0]);
			values.at(ptr++) = int(colors[1]);
			values.at(ptr++) = int(colors[2]);*/
			red.at(ptr) = int(colors[2]);
			green.at(ptr) = int(colors[1]);
			blue.at(ptr) = int(colors[0]);
			ptr++;
			//cout << "Blue value: " << int(colors[0]) << endl;
		}
	}
	//return values;
}

str folders[] = {"accordion", "airplanes", "anchor", "ant", "BACKGROUND_GOOGLE",
	"barrel", "bass", "beaver", "binocular", "bonsai", "brain", "brontosaurus",
	"buddha", "butterfly", "camera", "cannon", "car_side", "ceiling_fan",
	"cellphone", "chair", "chandelier", "cougar_body", "cougar_face",
	"crab", "crayfish", "crocodile", "crocodile_head", "cup",
	"dalmatian", "dollar_bill", "dolphin", "dragonfly",
	"electric_guitar", "elephant", "emu", "euphonium", "ewer",
	"Faces", "Faces_easy", "ferry", "flamingo","flamingo_head",
	"garfield","gerenuk","gramophone","grand_piano", "hawksbill",
	"headphone","hedgehog","helicopter","ibis","inline_skate",
	"joshua_tree","kangaroo","ketch","lamp","laptop","Leopards",
	"llama","lobster","lotus","mandolin","mayfly","menorah",
	"metronome","minaret","Motorbikes","nautilus","octopus",
	"okapi","pagoda","panda","pigeon","pizza","platypus",
	"pyramid","revolver","rhino","rooster","saxophone","schooner",
	"scissors","scorpion","sea_horse","snoopy","soccer_ball",
	"stapler","starfish","stegosaurus","stop_sign","strawberry",
	"sunflower","tick","trilobite","umbrella","watch","water_lilly",
	"wheelchair","wild_cat","windsor_chair","wrench","ying_yang"};

int amount[] = {55,800,42,42,468,47,54,46,33,128,98,43,85,91,50,
	43,123,47,59,62,107,47,69,73,70,50,51,57,67,52,65,68,75,64,
	53,64,85,435,435,67,67,45,34,34,51,99,100,42,54,88,80,31,64,
	86,114,61,81,200,78,41,66,43,40,87,32,76,798,55,35,39,47,38,
	45,53,34,57,82,59,49,40,63,39,84,57,35,64,45,86,59,64,35,85,
	49,86,75,239,37,59,34,56,39,60};

//102 folders

int main(int argc, char *argv[]) {
	
	/*int_vec red, green, blue;
	rgb_values("reddit.png", red, green, blue);
	cout << "red.at(1203): " << red.at(1203) << endl;*/
	double_vec dims;
	char* filename;
	str folder, temp, num;
	str one_oh_one = "101_ObjectCategories/";
	str pic = "/image_0001.jpg";
	int cur_pic, max_pic;
	for(int i = 0; i < 102; i++){
		folder = folders[i];
		max_pic = amount[i];
		temp = "101_ObjectCategories/" + folder + "/image_0001.jpg"; //21 + fs + 7 (10) ? 
		filename = new char[21 + folder.size() + 15 + 1];
		strcpy(filename, temp.c_str());
		for(cur_pic = 1; cur_pic <= max_pic; cur_pic++){
			dims.clear();
			num = num4digits(cur_pic);
			/*cout << filename[21 + folder.size() + 7] << filename[21 + folder.size() + 8] <<
				filename[21 + folder.size() + 9] << filename[21 + folder.size() + 10] << endl;*/
			filename[21 + folder.size() + 7] = num[0];
			filename[21 + folder.size() + 8] = num[1];
			filename[21 + folder.size() + 9] = num[2];
			filename[21 + folder.size() + 10] = num[3];
			//cout << "filename: " << filename << endl;
			dims = dimensions(filename);
			vec_to_txt(dims);
		}
		delete [] filename;				
	}
	/*dims = dimensions("image_0020.jpg");
	vec_to_txt(dims);
	cv::Mat img = cv::imread("image_0020.jpg", cv::IMREAD_COLOR);
	build_representative(dims, 0, 0, img);
	imwrite("image_0020_r.jpg", img);*/
	
	return 0;
}

