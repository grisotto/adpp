#include <vector>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "gurobi_c++.h"

using namespace std;
vector<int> computeNP(int W, vector<int> w, vector<int> b){

	vector<int> normalPatterns; // Return vector
	if (W<=0) return normalPatterns; // Early return

	//+ Vars
	int m = w.size(), last = 0;
    vector<int> x(W+1);

	for (int i=0; i<m; i++) b[i] = min(b[i], W/w[i]); // Improving b[]
	x[0] = 1; // Considering the first point

    //+ Generating points
    for (int i=0; i<m; i++) if (W-w[i]>=0) for (int _=0;_<b[i];_++) for (int j=min(W-w[i],last); j>=0; j--) if (x[j]==1){
		x[j+w[i]] = 1;
		if (j==last) last = j+w[i];
    }

	//+ Saving points
    for (int i=0; i<=W; i++) if (x[i]) normalPatterns.push_back(i);
    return normalPatterns;
}

bool knapsack2d(int W, int H, vector<int> w, vector<int> h,  vector<int> d, vector<int> b,vector<int> value){


    int m = w.size();
    //+ Setting the points for packing
    vector<int> wSet = computeNP(W, w, b);
    vector<int> hSet = computeNP(H, h, b);

        cout << wSet.size() << endl;
        cout << hSet.size() << endl;
	//+ Setting Gurobi Model/Environment
	GRBEnv env = GRBEnv();
	GRBModel model = GRBModel(env);
    model.set(GRB_DoubleParam_TimeLimit, 1800.0);
	model.getEnv().set(GRB_IntParam_OutputFlag, 1);
    try{


	//+ Setting Model Variables
	vector<vector<vector<GRBVar>>> x(m);
	for (int i = 0; i < m; i++) {

		x[i] = vector<vector<GRBVar>>();
		for (auto p: hSet)
            if (h[i]+p<=H){

			x[i].push_back(vector<GRBVar>());
			for (auto q : wSet)
                if(w[i]+q<=W)
                    x[i].back().push_back(model.addVar(0.0, 1.0, value[i], GRB_BINARY));


		}
	}

	model.update();
    model.set(GRB_IntAttr_ModelSense, -1);

	//+ Setting Model Constraints
	//++ Overlapping Constraints
	for (auto s: hSet) for (auto t: wSet){

		GRBLinExpr expr = 0.0;
		for (int i = 0; i < m; i++)
			for (int p = 0; p < x[i].size(); p++) 
                if (hSet[p] <= s && s < hSet[p] + h[i])
				    for (int q = 0; q < x[i][p].size(); q++)
                        if (wSet[q] <= t && t < wSet[q] + w[i])
                            expr += 1.0 * x[i][p][q];

		model.addConstr(expr <= 1.0);
	}


	//++ Item copies Constraints
	for (int i = 0; i < m; i++){
		GRBLinExpr expr = 0.0;
		for (auto p: x[i]) for (auto q: p) expr += q;
		model.addConstr(expr <= b[i]);
	}

	//+ Solving Model
	model.optimize();
    cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
    }
    catch(GRBException e){
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }
    catch(...){
        cout << "Exception during optmization" << endl;
    }

}


int main (int argc, char* argv[]){

    int N;
    int W;
    int H;
    vector<int> w;
    vector<int> h ;
    vector<int> d; // minimum number of pieces of that type 
    vector<int> b; // maximum number of pieces of that type 
    vector<int> value;
	
    string filename = argv[1];
    int temp;
    ifstream in(filename);
    //read a instance
        //--format
        //N
        //W H 
        //--for i..N
            //w h  b d value
    if(in){
        in >> N;
        in >> W;
        in >> H;
        for(int i =0; i< N; i++){
            in >> temp; 
            w.push_back(temp);
            in >> temp; 
            h.push_back(temp);
            in >> temp; 
            d.push_back(temp);
            in >> temp; 
            b.push_back(temp);
            in >> temp; 
            value.push_back(temp);
        }

    }else{
        cerr << "NO such file:" << filename << endl; 
        throw(1);
    }
    //cout << "W H" << endl;
    //cout << W << " " << H << endl;
        //cout << "w h d b value" << endl; 
    //for(auto i= 0; i<value.size(); i++){
        //cout << w[i] << " " << h[i] << " " << d[i] << " " << b[i] << " " << value[i] << " " << endl;
    //}

    knapsack2d(W,H,w,h,d,b,value);
}

