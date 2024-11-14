
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
using namespace std;

double h = 0.002;
double T = 400;
vector<double> l;  
double y = 1;  

double t = 0;  


double f(double t, double x) {
    double k = 1.0; 
    return k * x;   
}

int main() {
        
    l.push_back(y);
    
    while (t <= T) {
        double k1 = f(t, y);
        double k2 = f(t + h, y + h * k1);
        
        y = y + (h / 2) * (k1 + k2);
        
        t += h;
        
        l.push_back(y);
        
        cout << "t = " << t << " y = " << fixed << setprecision(5) << y << endl;
    }
    
    return 0;
}

