#include <vector>
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
