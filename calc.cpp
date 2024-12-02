#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

using namespace std;

#ifndef M_PI // Je sais pas pourquoi mais Pi n'est pas défini chez moi. :'(
    #define M_PI 3.14159265358979323846
#endif

// Constantes physiques
const double G = 6.67430e-11;  // Constante gravitationnelle
const double c = 3.0e8;        // Vitesse de la lumière
const double M = 1.989e30;     // Masse du trou noir
const double r_s = 2 * G * M / (c * c); // Rayon de Schwarzschild

// Équations du mouvement
vector<double> equations(double t, const vector<double>& y) { // Equation du mouvement
    double r = y[0];
    double v_r = y[1];
    double theta = y[2];
    double v_theta = y[3];

    double drdt = v_r;
    double dthetadt = v_theta / r;
    double dv_rdt = -(G * M / (r * r)) + (v_r * v_r / r) - (1 - 2 * G * M / (r * c * c)) * (v_r * v_r / r);
    double dv_thetadt = 0;

    return { drdt, dv_rdt, dthetadt, dv_thetadt };
}

// Méthode de Runge-Kutta d'ordre 4
void rungeKutta(double t0, double tf, double dt, vector<double>& y, const string& filename) {
	int n = static_cast<int>((tf - t0) / dt); // static_cast<int> convertit un double en int
    double t = t0;

    ofstream file(filename);
    file << "t,r,v_r,theta,v_theta\n";

    for (int i = 0; i < n; ++i) {
        vector<double> k1 = equations(t, y);
        vector<double> k2 = equations(t + 0.5 * dt, { y[0] + 0.5 * dt * k1[0], y[1] + 0.5 * dt * k1[1], y[2] + 0.5 * dt * k1[2], y[3] + 0.5 * dt * k1[3] });
        vector<double> k3 = equations(t + 0.5 * dt, { y[0] + 0.5 * dt * k2[0], y[1] + 0.5 * dt * k2[1], y[2] + 0.5 * dt * k2[2], y[3] + 0.5 * dt * k2[3] });
        vector<double> k4 = equations(t + dt, { y[0] + dt * k3[0], y[1] + dt * k3[1], y[2] + dt * k3[2], y[3] + dt * k3[3] });

        y[0] += (dt / 6.0) * (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0]);
        y[1] += (dt / 6.0) * (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1]);
        y[2] += (dt / 6.0) * (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2]);
        y[3] += (dt / 6.0) * (k1[3] + 2 * k2[3] + 2 * k3[3] + k4[3]);

        t += dt;

        file << t << "," << y[0] << "," << y[1] << "," << y[2] << "," << y[3] << "\n";

        if (y[0] <= r_s) {
            cout << "La particule a franchi l'horizon des événements à t = " << t << " secondes.\n";
            break;
        }
    }

    file.close();
}

int main() {
    double r0 = 1.0e8;   // Distance initiale (m)
    double v_r0 = -10000; // Vitesse radiale initiale (m/s)
    double theta0 = M_PI / 2; // Angle initial (rad)
    double v_theta0 = 0.1;    // Vitesse angulaire initiale (rad/s)

    double t0 = 0.0;     // Temps initial (s)
    double tf = 1e5;     // Temps final (s)
    double dt = 0.01;    // Pas 

    vector<double> y = { r0, v_r0, theta0, v_theta0 };

    string filename = "simulation_data.csv";
    rungeKutta(t0, tf, dt, y, filename);

    cout << "Les données de simulation ont été enregistrées dans " << filename << endl;
    return 0;
}

