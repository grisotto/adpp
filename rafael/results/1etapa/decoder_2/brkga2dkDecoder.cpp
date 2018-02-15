#include "brkga2dkDecoder.h"
#include <list>

brkga2dkDecoder::brkga2dkDecoder(knapsack2d& instance) {
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

void print_seq(vector<pair<int,int>> & seq) {
    for(auto &p : seq) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
}

double brkga2dkDecoder::decode(const std::vector< double >& chromosome) const {
    vector<int> A(m_instance.N);
    for(int i=0; i<m_instance.N; ++i) {
        A[i] = i;
    }

    sort(A.begin(), A.end(), [&](int a, int b) { return chromosome[a] < chromosome[b]; });
    double value = 0.0;

    typedef pair<int,int> pii;
    // vector of points of corners
    vector<pii> corners;
    //initials coorners
    corners.push_back(pii(0,m_instance.H));
    corners.push_back(pii(0,0));
    corners.push_back(pii(m_instance.W, 0));

    for(int i=0; i<m_instance.N; ++i) {
        int idx = A[i];
        int typ = chromosome[idx + m_instance.N] > 0.5? 1 :0; // 1 = BL and 0 = LB
        auto &item = m_instance.items[idx];
        pii found;
        bool found_corner = false;

        if (typ) {
            //BL
            for(int i=0; i<(int)corners.size(); ++i) {
                auto &corner = corners[i];
                // the item fit in the bin?
                if (corner.first + item.width <= m_instance.W && corner.second + item.height<= m_instance.H) {
                    found = corner;
                    found_corner = true;
                    break;
                }
            }
        } else {
            //LB
            for(int i=(int)corners.size() - 1; i>=0; --i) {
                auto &corner = corners[i];
                // the item fit in the bin?
                if (corner.first + item.width <= m_instance.W && corner.second+ item.height<= m_instance.H) {
                    found = corner;
                    found_corner = true;
                    break;
                }
            }
        }
        if (!found_corner) continue; // not using this item

        value += item.value;
        int first = -1;
        int lst = -1;
        int toplefty = found.second + item.height;
        int bottomrightx = found.first + item.width;

        //cout << "Picked (" << found.first << "," << found.second << ")  W=" << item.width << " H=" << item.height << "\n";

        //get a position of the last corner bottomright in x
        for(int i=0; i<(int)corners.size(); ++i) {
            if (corners[i].first >= bottomrightx) {
                lst = i;
                break;
            }
        }

        //get a position of the first corner topleft y
        for(int i=(int)corners.size() -1; i>=0; --i)  {
            if (corners[i].second >= toplefty) {
                first = i;
                break;
            }
        }



        //cout << "first = " << first << " lst = " << lst << endl;
        
        //remove corners from first to last and update last position
        corners.erase(corners.begin() + first + 1, corners.begin() + lst);
        lst -= lst - (first + 1);

        
        //define position of coornerleft cornerright and middle 
        //and add them to the BL/LB vector
        auto cornerleft = pii(corners[first].first, toplefty);
        auto cornerright = pii(bottomrightx, corners[lst].second);
        auto middle = pii(bottomrightx, toplefty);
        corners.insert(corners.begin() + first + 1, cornerleft);
        lst ++;

        corners.insert(corners.begin() + first + 2, middle);
        lst++;

        corners.insert(corners.begin() + lst, cornerright);
        lst++;

        
        if (cornerleft == corners[first]) {
            corners.erase(corners.begin() + first, corners.begin() + first + 2);
            lst -= 2;
        }

        if (cornerright == corners[lst]) {
            corners.erase(corners.begin() + lst - 1, corners.begin() + lst + 1);
        }
    }

    return -value;
}
