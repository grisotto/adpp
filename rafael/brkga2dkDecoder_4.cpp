#include "brkga2dkDecoder_4.h"
#include <list>
brkga2dkDecoder_4::brkga2dkDecoder_4(knapsack2d& instance) {
    m_instance.W = instance.W;
    m_instance.H = instance.H;
    for(auto &item : instance.items) {
        auto item_copy = item;
        item_copy.b = 1;
        for(int i=0; i<item.b; ++i) {
            m_instance.items.push_back(item_copy);
        }
    }
    m_instance.N = (int)m_instance.items.size();
}

brkga2dkDecoder_4::~brkga2dkDecoder_4() { }

int brkga2dkDecoder_4::getChromosomeSize(){
    return 1 + m_instance.N;
}

double brkga2dkDecoder_4::decode(const std::vector< double >& chromosome) const {
    vector<int> A(m_instance.N);
    rectangletype r[m_instance.N], *pr[m_instance.N];
    int area;
    twodbinpacktype p;



    for(int i=0; i<m_instance.N; ++i) {
        A[i] = i;
    }

    sort(A.begin(), A.end(), [&](int a, int b) { return chromosome[a] < chromosome[b]; });
    double value = 0.0;

    for(int i=0; i<m_instance.N; ++i){
        int idx = A[i];
        r[i].x = m_instance.items[idx].width;
        r[i].y = m_instance.items[idx].height;
        r[i].np = -1;
        r[i].ind = i;
        r[i].value =m_instance.items[idx].value; 
        area += r[idx].x * r[idx].y;
        //cout << "x, y " << r[idx].x << ", " << r[idx].y << endl;
    }
    auto alpha = chromosome[m_instance.N+1];
    init2dbinpack(&p, m_instance.W,m_instance.H, m_instance.N, r,alpha );

    if(empacotaumaplaca(&p, 0, &area)){
        //cout << "empacotou" << endl;
        value = p.value; 
        //cout << "Valor: " << value << endl;
        //desenhaempacotamento(&p);
     
    }

    return -value;
}
