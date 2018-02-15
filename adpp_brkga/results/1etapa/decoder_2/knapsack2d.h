#ifndef KNAPSACK2D
#define KNAPSACK2D

#include <vector>
#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include "item.h"



using namespace std;

class knapsack2d{

    public:
        int N; // number of items
        int W; //weight of bin
        int H; //height of bin
        vector<Item> items;

        knapsack2d(const char* filename);
        knapsack2d();

	private:
        void readInstance(const char* filename);

};
#endif
