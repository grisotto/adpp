#include "brkga2dkDecoder_2.h"
#include <set>

brkga2dkDecoder_2::brkga2dkDecoder_2(knapsack2d& instance) {
    //comentar copia dos itens para mandar pro brkga
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

brkga2dkDecoder_2::~brkga2dkDecoder_2() { }

int brkga2dkDecoder_2::getChromosomeSize(){
    return  m_instance.N;
}
double brkga2dkDecoder_2::decode(const std::vector< double >& chromosome) const {
    vector<int> A(m_instance.N);
    for(int i=0; i<m_instance.N; ++i) {
        A[i] = i;
    }
    sort(A.begin(), A.end(), [&](int a, int b) { return chromosome[a] < chromosome[b]; });
    double value = 0.0;

    int h_used = 0;
    int w_sum = 0;
    int max_h =0; 
    for(int i=0; i<m_instance.N; ++i) {
        int idx = A[i];
        auto &item = m_instance.items[idx];
        
        if((item.height) <= m_instance.H - h_used){
             
            if((item.width + w_sum) <= m_instance.W){
                w_sum += item.width;
                value += item.value;
                if(item.height > max_h){
                    max_h = item.height;
                }                
            }else{
                h_used += max_h;
                max_h =0;
                w_sum = 0;
                --i;
            }
            
        
        }
    }

    return -value;
}
