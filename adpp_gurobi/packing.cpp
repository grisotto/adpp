#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include "gurobi_c++.h"
#include "exprtk.hpp"

using namespace std;

    template <typename T>
vector<double> evaluate_function(string function, vector<int> points)
{
    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T>     expression_t;
    typedef exprtk::parser<T>             parser_t;
    bool validate = false;
    vector<double> function_evaluate; 

    string expression_string = function;
    T x;
    symbol_table_t symbol_table;
    symbol_table.add_variable("x",x);
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string,expression);
    //cout << "expression.value: "<< '\n';
    function_evaluate.push_back(1.0);
    for (vector<int>::iterator it = points.begin()+1; it != points.end(); ++it){
        x = *it;
        //cout << " " << expression.value(); 
        double result = expression.value();
        function_evaluate.push_back(result);
    }
    //cout << '\n';
    for (vector<double>::iterator it = function_evaluate.begin(); it != function_evaluate.end(); ++it){
        //cout << ' ' << *it;
    }
    //cout << '\n';

    if(validate)
        for (x = T(1); x <= T(+1); x += T(0.1))
        {
            T y = expression.value();
            printf("%19.15f\t%19.15f\n",x,y);
        }
    return function_evaluate;
}

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

bool knapsack2d(int W, int H, vector<int> w, vector<int> h,  vector<int> d, vector<int> b,vector<double> value, string flines, string ftimes){


    int m = w.size();
    //+ Setting the points for packing
    vector<int> wSet = computeNP(W, w, b);
    vector<int> hSet = computeNP(H, h, b);
    //for (int i=0; i<W; i++) wSet.push_back(i);
    //for (int i=0; i<H; i++) hSet.push_back(i);
    vector<double> function_lines = evaluate_function<double>(flines, hSet);
    vector<double> function_times = evaluate_function<double>(ftimes, wSet);
    //cout << wSet.size() << endl;
    //cout << hSet.size() << endl;
    //+ Setting Gurobi Model/Environment
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    //model.set(GRB_DoubleParam_TimeLimit, 100.0);
    model.getEnv().set(GRB_IntParam_OutputFlag, 1);
    try{


        //+ Setting Model Variables
        vector<vector<vector<GRBVar>>> x(m);
        for (int i = 0; i < m; i++) {

            x[i] = vector<vector<GRBVar>>();
            for (auto p: hSet){
                if (h[i]+p<=H){

                    x[i].push_back(vector<GRBVar>());
                    for (auto q : wSet){
                        if(w[i]+q<=W){
                            //cout << "value: " << value[i] << " lines: "<< function_lines[p] << " p: " << p << " times: " << function_times[q]<< " q: "<< q << endl;
                            //cout << "new value: " << value[i] << endl;
                            x[i].back().push_back(model.addVar(0.0, 1.0, value[i] * (function_lines[p] * function_times[q]), GRB_BINARY));
                        }
                    }
                }
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


        ////++ Item copies Constraints
        //for (int i = 0; i < m; i++){
            //GRBLinExpr expr = 0.0;
            //for (auto p: x[i]) for (auto q: p) expr += q;
            //model.addConstr(expr <= b[i]);
        //}

        ////++ Item copies Constraints
        //for (int i = 0; i < m; i++){
            //GRBLinExpr expr = 0.0;
            //for (auto p: x[i]) for (auto q: p) expr += q;
            //model.addConstr(expr >= d[i]);
        //}

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
    if( argc > 0){
        int N;
        int W;
        int H;
        vector<int> w;
        vector<int> h ;
        vector<int> d; // minimum number of pieces of that type 
        vector<int> b; // maximum number of pieces of that type 
        vector<double> value;
        string flines, ftimes;
        int temp;
        double temp2;
        //Var to read files
        ifstream in(argv[1]);
        ifstream filelines(argv[2]);
        ifstream filetimes(argv[3]);

        //read a instance
        //--format
        //N
        //W H 
        //--for i..N
        //w h value
        if(in){
            in >> N;
            in >> W;
            in >> H;
            for(int i =0; i< N; i++){
                in >> temp; 
                w.push_back(temp);
                in >> temp; 
                h.push_back(temp);
                //in >> temp; 
                //d.push_back(temp);
                //in >> temp; 
                b.push_back(1);
                in >> temp2; 
                value.push_back(temp2);
            }

        }else{
            cerr << "No instance file" << endl; 
            throw(1);
        }
        //read two functions
        //first: function for lines (y)
        //second: function for exposition time (x)
        if(filelines && filetimes){
            getline(filelines, flines);
            getline(filetimes, ftimes);
        }else{
            cerr << "No functions files" << endl; 
            throw(1);
        }


        //cout << "W H" << endl;
        //cout << W << " " << H << endl;
        //cout << "w h d b value" << endl; 
        //for(auto i= 0; i<value.size(); i++){
        //cout << w[i] << " " << h[i] << " " << d[i] << " " << b[i] << " " << value[i] << " " << endl;
        //}

        knapsack2d(W,H,w,h,d,b,value, flines, ftimes);
    }else{
        cout << "Usage: <instance_file> <lines_function> <time_function>" << endl;


    }
}

