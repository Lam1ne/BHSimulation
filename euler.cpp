#include<iostream>
#include<cmath>
#include<vector>
#include<iomanip>
#include<cmath>
using namespace std;

int main () {
    

double T = 1000;
double h = 0.001;
double y0 = 1;
double x = 0;
vector<double> l;
l.push_back(y0);
cout << fixed << setprecision(12);

for(double i = 1, x = 0; i <= T && x <= 4; i += 1, x += h) {

    l.push_back((h+1)*l[i-1]);
    cout << exp(x) - l[i] << endl;

}

return 0;
}
