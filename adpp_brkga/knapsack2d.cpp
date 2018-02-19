#include "knapsack2d.h"


template <typename T>
vector<double> evaluate_function(string function, int points)
{
  typedef exprtk::symbol_table<T> symbol_table_t;
  typedef exprtk::expression<T>     expression_t;
  typedef exprtk::parser<T>             parser_t;
  bool validate = false;
  vector<double> function_evaluate; 

  string expression_string = function;
  cout << "function: "<< function<<"\n"<< endl;

  cout << "Points: "<< '\n';
  for (int it=0; it < points; it++){
    cout << " " << it ; 
  }
  cout << '\n';
  T x;
  symbol_table_t symbol_table;
  symbol_table.add_variable("x",x);
  symbol_table.add_constants();

  expression_t expression;
  expression.register_symbol_table(symbol_table);

  parser_t parser;
  parser.compile(expression_string,expression);
  cout << "expression.value: "<< '\n';
  function_evaluate.push_back(1.0);
  for (int it=1; it < points; it++){
    x = it;
    cout << " " << expression.value(); 
    double result = expression.value();
    function_evaluate.push_back(result);
  }
  cout << '\n';
  for (vector<double>::iterator it = function_evaluate.begin(); it != function_evaluate.end(); ++it){
    cout << ' ' << *it;
  }
  cout << '\n';

  if(validate)
    for (x = T(1); x <= T(+1); x += T(0.1))
    {
      T y = expression.value();
      printf("%19.15f\t%19.15f\n",x,y);
    }
  return function_evaluate;
}

void knapsack2d::readInstance(const char* filename, const char* flines, const char*ftimes){
    ifstream in(filename);
    ifstream filelines(flines);
    ifstream filetimes(ftimes);
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
    if(filelines && filetimes){
      getline(filelines, this->flines);
      getline(filetimes, this->ftimes);
      this->function_lines = evaluate_function(flines, H);
      this->function_times = evaluate_function(ftimes, W);

    }else{
        cerr << "No functions files:" << endl; 
        throw(1);
    }
    //cout << "W H" << endl;
    //cout << W << " " << H << endl;
}
knapsack2d::knapsack2d(const char* filename, const char* flines, const char* ftimes){
    readInstance(filename, flines, ftimes);
}
knapsack2d::knapsack2d(){
}

