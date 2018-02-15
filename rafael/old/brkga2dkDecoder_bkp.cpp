#include "brkga2dkDecoder.h"
#include <set>
#include "rectangle.h"

brkga2dkDecoder::brkga2dkDecoder(knapsack2d& instance) {
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

brkga2dkDecoder::~brkga2dkDecoder() { }

int brkga2dkDecoder::getChromosomeSize(){
    return 2 * m_instance.N;
}
double brkga2dkDecoder::decode(const std::vector< double >& chromosome) const {
    vector<int> A(m_instance.N);
    for(int i=0; i<m_instance.N; ++i) {
        A[i] = i;
    }

    sort(A.begin(), A.end(), [&](int a, int b) { return chromosome[a] < chromosome[b]; });
    double value = 0.0;

    bool reject_corner_limits;
    bool reject_corner_fit;
    typedef pair<int,int> pii;
    set<pii> LB, BL;
    LB.insert(pii(0,0));
    BL.insert(pii(0,0));
    vector<Rectangle> rects;
    for(int i=0; i<m_instance.N; ++i) {
        int idx = A[i];
        int typ = chromosome[idx + m_instance.N] > 0.5? 1 :0;
        reject_corner_limits = false;
        reject_corner_fit = false;

        set<pii> *S;
        if (typ) S = &BL;
        else S = &LB;

        for(auto p : *S) {
            pii tl, br;
            if (typ) swap(p.first, p.second);
            // does my rectangle fit?
            Rectangle cur(p, m_instance.items[idx].width, m_instance.items[idx].height);
            if (cur.m_x_max > m_instance.W || cur.m_y_max > m_instance.H)
                //goto reject_corner;
                reject_corner_limits= true;

            if(!reject_corner_limits){
                for(auto &r : rects && reject_corner_fit) {
                    if (cur.doesIntersect(r)){ 
                        //goto reject_corner;
                        reject_corner_fit = true;
                        break;
                    }
                }
                if(!reject_corner_fit){
                    // put rectangle
                    rects.push_back(cur);
                    value += m_instance.items[idx].value;
                    tl = pii(cur.m_x_min, cur.m_y_max);
                    br = pii(cur.m_x_max, cur.m_y_min);
                    LB.insert(tl);
                    LB.insert(br);

                    swap(tl.first, tl.second);
                    swap(br.first, br.second);
                    BL.insert(tl);
                    BL.insert(br);
                    S->erase(p);
                    break;
                    //reject_corner:;
                }
            }
        }
    }

    return -value;
}
