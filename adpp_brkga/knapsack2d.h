#ifndef KNAPSACK2D
#define KNAPSACK2D

#include <vector>
#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include "item.h"
#include "exprtk.hpp"



using namespace std;

class knapsack2d{

    public:
        int N; // number of items
        int W; //weight of bin
        int H; //height of bin
        vector<Item> items;
        string flines, ftimes;
        vector<double> function_lines;
        vector<double> function_times;

        knapsack2d(const char* filename, const char* flines, const char* ftimes);
        knapsack2d();

	private:
        void readInstance(const char* filename, const char* flines, const char* ftimes);
        vector<double> evaluate_function(string function, int b);

};
#endif
