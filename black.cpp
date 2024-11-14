#include <iostream>
#include <cmath> // pour la fonction exp
#include <vector>
using namespace std;
// Définition de la fonction f(t, y)

const double G = 6.67430e-11;  // Constante gravitationnelle (m^3 kg^−1 s^−2)
const double M = 1.989e30;     // Masse du trou noir (kg) (approximativement la masse du soleil)
const double c = 3.0e8;        // Vitesse de la lumière (m/s)

// Fonction pour la dérivée de la position
// f(t, r, v) = dérivées de la position et de la vitesse
// r : position (m)
double f_r(double r, double v) {
    return v; // La dérivée de la position est la vitesse
}

// Fonction pour la dérivée de la vitesse
// v : vitesse (m/s)
double f_v(double r, double v) {
    return -G * M / (r * r); // L'accélération est simplement la gravité de Schwarzschild
}

// Implémentation de la méthode de Runge-Kutta d'ordre 4 (RK4)
void rungeKutta(double (*f_r)(double, double), double (*f_v)(double, double),
                double r0, double v0, double t0, double tf, double h) {
    
    // Initialisation des variables
    double t = t0;
    double r = r0;
    double v = v0;
    
    // Vecteurs pour stocker les résultats (optionnel, pour afficher l'évolution)
    std::vector<double> times;
    std::vector<double> positions;
    std::vector<double> velocities;

    // Affichage des conditions initiales
    std::cout << "t = " << t << ", r = " << r << ", v = " << v << std::endl;

    // Boucle d'intégration
    while (t < tf) {
        // Calcul des quatre estimations pour la méthode RK4
        double k1_r = h * f_r(r, v);
        double k1_v = h * f_v(r, v);
        
        double k2_r = h * f_r(r + 0.5 * k1_r, v + 0.5 * k1_v);
        double k2_v = h * f_v(r + 0.5 * k1_r, v + 0.5 * k1_v);
        
        double k3_r = h * f_r(r + 0.5 * k2_r, v + 0.5 * k2_v);
        double k3_v = h * f_v(r + 0.5 * k2_r, v + 0.5 * k2_v);
        
        double k4_r = h * f_r(r + k3_r, v + k3_v);
        double k4_v = h * f_v(r + k3_r, v + k3_v);

        // Mise à jour de la position et de la vitesse
        r += (k1_r + 2 * k2_r + 2 * k3_r + k4_r) / 6.0;
        v += (k1_v + 2 * k2_v + 2 * k3_v + k4_v) / 6.0;
        
        // Mise à jour du temps
        t += h;
        
        // Sauvegarde des résultats
        times.push_back(t);
        positions.push_back(r);
        velocities.push_back(v);
        
        // Affichage des résultats à chaque étape
        std::cout << "t = " << t << ", r = " << r << ", v = " << v << std::endl;
    }
}

int main() {
    // Conditions initiales
    double r0 = 1.5e11;  // Position initiale (en m), ici à une distance proche de la Terre
    double v0 = 0.0;     // Vitesse initiale (en m/s)
    double t0 = 0.0;     // Temps initial (en s)
    double tf = 5.0e7;   // Temps final de la simulation (en s)
    double h = 1.0e4;    // Pas de temps (en s)

    // Appel de la méthode Runge-Kutta pour résoudre les équations de mouvement
    rungeKutta(f_r, f_v, r0, v0, t0, tf, h);

    return 0;
}


