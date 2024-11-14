
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
using namespace std;

double h = 0.002;
double T = 250;
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
        double k2 = f(t + h/2, y + h * k1/2);
        double k3 = f(t+ h/2, y+h * k2/2);
        double k4 = f(t+h, y+ h*k3);

        
        y = y + (h / 6) * (k1 + 2*k2 + 2*k3 + k4);
        
        t += h;
        
        l.push_back(y);
        
        cout << "t = " << t << " y = " << fixed << setprecision(5) << y << endl;
    }
    
    return 0;
}

