#include "brkga2dkDecoder_3.h"
#include <set>
#include "rectangle.h"
#include <cmath>
#include <iterator>
brkga2dkDecoder_3::brkga2dkDecoder_3(knapsack2d& instance) {
    //copy of items with m_instance.items.b > 0
    m_instance.W = instance.W;
    m_instance.H = instance.H;
    for(auto &item : instance.items) {
        auto item_copy = item;
        item_copy.b = 1;
        for(int i=0; i<item.b; ++i) {
            m_instance.items.push_back(item_copy);
        }
    }
    m_instance.N = m_instance.items.size();
}

brkga2dkDecoder_3::~brkga2dkDecoder_3() { }

//chromosomeSize
int brkga2dkDecoder_3::getChromosomeSize(){
    return 2 * m_instance.N;
}
double brkga2dkDecoder_3::decode(const std::vector< double >& chromosome) const {
    vector<int> A(m_instance.N);
    //for item give a position
    for(int i=0; i<m_instance.N; ++i) {
        A[i] = i;
    }

    sort(A.begin(), A.end(), [&](int a, int b) { return chromosome[a] < chromosome[b]; });
    double value = 0.0;

    bool reject_corner_limits;
    bool reject_corner_fit;
    typedef pair<int,int> pii;
    // using set because insert ordered
    set<pii> LB, BL;
    //first corner
    LB.insert(pii(0,0));
    vector<Rectangle> rects;
    for(int i=0; i<m_instance.N; ++i) {
        int idx = A[i];
        //int typ = chromosome[idx + m_instance.N] > 0.5? 1 :0;
        int typ = (int)floor(chromosome[idx + m_instance.N] * LB.size());


        auto p = *next(LB.begin(), typ);
        pii tl, br;
        reject_corner_limits = false;
        reject_corner_fit = false;
        // does my rectangle fit?
        Rectangle cur(p, m_instance.items[idx].width, m_instance.items[idx].height);
        if (cur.m_x_max > m_instance.W || cur.m_y_max > m_instance.H)
            reject_corner_limits= true;

        if(!reject_corner_limits){
            for(auto &r : rects) {
                if (cur.doesIntersect(r)){ 
                    reject_corner_fit = true;
                    break;
                }
            }

            if(!reject_corner_fit){
                // put rectangle
                rects.push_back(cur);
                value += m_instance.items[idx].value;
                //add new corner points to LB and BL
                tl = pii(cur.m_x_min, cur.m_y_max);
                br = pii(cur.m_x_max, cur.m_y_min);
                LB.insert(tl);
                LB.insert(br);

                LB.erase(p);
                break;
            }
        }
    }
    //}

return -value;
}
