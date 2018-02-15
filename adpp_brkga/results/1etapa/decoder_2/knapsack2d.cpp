#include "knapsack2d.h"


void knapsack2d::readInstance(const char* filename){
    ifstream in(filename);
    //read a instance
        //--format
        //N
        //W H 
        //--for i..N
            //w h  b d value
    if(in){
        in >> this->N;
        in >> this->W;
        in >> this->H;
        for(int i =0; i< N; i++){
            Item it;        
            in >> it.width; 
            in >> it.height; 
            in >> it.demand; 
            in >> it.b; 
            in >> it.value; 
            items.push_back(it);
        }

    }else{
        cerr << "NO such file:" << filename << endl; 
        throw(1);
    }
    cout << "W H" << endl;
    cout << W << " " << H << endl;
}
knapsack2d::knapsack2d(const char* filename){
    readInstance(filename);
}
knapsack2d::knapsack2d(){
}

