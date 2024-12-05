#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

#define M_PI 3.14159265358979323846

using namespace std;

const double G = 6.67430e-11;   // Constante gravitationnelle
const double c = 299792458.0;   // Vitesse de la lumière

struct Etat {
    double t;                // Temps propre
    double r;                // Coordonnée radiale
    double theta;            // Coordonnée angulaire theta
    double phi;              // Coordonnée angulaire phi
    double dr_dtau;          // Vitesse radiale
    double dtheta_dtau;      // Vitesse angulaire theta
    double dphi_dtau;        // Vitesse angulaire phi
};

double rayon_schwarzschild(double masse) {
    return 2.0 * G * masse / (c * c);
}

void calculer_derivées(const Etat& etat, double rs, double masse, double E, double L, Etat& derivees) {
    double r = etat.r;
    double drdtau = etat.dr_dtau;
    double un_moins_rs_sur_r = 1.0 - rs / r;

    derivees.t = E / un_moins_rs_sur_r;
    derivees.r = drdtau;
    derivees.phi = L / (r * r);
    derivees.theta = 0.0;
    derivees.dtheta_dtau = 0.0;

    double d2r_dtau2 = -(G * masse) / (r * r) * un_moins_rs_sur_r + (L * L) / (r * r * r) * (1.0 - 3.0 * rs / r);
    derivees.dr_dtau = d2r_dtau2;
    derivees.dphi_dtau = 0.0;
}

void etape_runge_kutta(double masse, double E, double L, Etat& etat, double pas) {
    double rs = rayon_schwarzschild(masse);

    Etat k1, k2, k3, k4;

    calculer_derivées(etat, rs, masse, E, L, k1);

    Etat etat_temp = etat;
    etat_temp.t += 0.5 * pas * k1.t;
    etat_temp.r += 0.5 * pas * k1.r;
    etat_temp.phi += 0.5 * pas * k1.phi;
    etat_temp.dr_dtau += 0.5 * pas * k1.dr_dtau;

    calculer_derivées(etat_temp, rs, masse, E, L, k2);

    etat_temp = etat;
    etat_temp.t += 0.5 * pas * k2.t;
    etat_temp.r += 0.5 * pas * k2.r;
    etat_temp.phi += 0.5 * pas * k2.phi;
    etat_temp.dr_dtau += 0.5 * pas * k2.dr_dtau;

    calculer_derivées(etat_temp, rs, masse, E, L, k3);

    etat_temp = etat;
    etat_temp.t += pas * k3.t;
    etat_temp.r += pas * k3.r;
    etat_temp.phi += pas * k3.phi;
    etat_temp.dr_dtau += pas * k3.dr_dtau;

    calculer_derivées(etat_temp, rs, masse, E, L, k4);

    etat.t += (pas / 6.0) * (k1.t + 2.0 * k2.t + 2.0 * k3.t + k4.t);
    etat.r += (pas / 6.0) * (k1.r + 2.0 * k2.r + 2.0 * k3.r + k4.r);
    etat.phi += (pas / 6.0) * (k1.phi + 2.0 * k2.phi + 2.0 * k3.phi + k4.phi);
    etat.dr_dtau += (pas / 6.0) * (k1.dr_dtau + 2.0 * k2.dr_dtau + 2.0 * k3.dr_dtau + k4.dr_dtau);
}

sf::Vector2f polaires_vers_cartesiennes(double r, double phi, double echelle) {
    double r_echelle = r * echelle;
    float x = static_cast<float>(r_echelle * cos(phi));
    float y = static_cast<float>(r_echelle * sin(phi));
    return sf::Vector2f(x, y);
}

void calculer_E_L(double rs, double masse, double r0, double e, double& E, double& L) {
    if (e < 1.0) {
        if (e == 0.0) {
            E = (1.0 - rs / (2.0 * r0)) / sqrt(1.0 - 3.0 * rs / r0);
            L = sqrt(G * masse * r0 / (1.0 - 3.0 * rs / r0));
        }
        else {
            E = (1.0 - rs / (2.0 * r0)) / sqrt(1.0 - 3.0 * rs / r0) - (e * e / 2.0);
            L = sqrt(G * masse * r0 / (1.0 - 3.0 * rs / r0)) * sqrt(1.0 - e * e);
        }
    }
    else if (e == 1.0) {
        E = (1.0 - rs / (2.0 * r0)) / sqrt(1.0 - 3.0 * rs / r0);
        L = sqrt(G * masse * r0 / (1.0 - 3.0 * rs / r0));
    }
    else {
        E = (1.0 - rs / (2.0 * r0)) / sqrt(1.0 - 3.0 * rs / r0) + (e * e / 2.0);
        L = sqrt(G * masse * r0 / (1.0 - 3.0 * rs / r0)) * sqrt(e * e - 1.0);
    }
}

int main() {
    double masse = 10.0 * 1.989e30;  // 10 masses solaires
    double rs = rayon_schwarzschild(masse);

    cout << "Choisissez le type d'orbite :" << endl;
    cout << "1. Orbit Circulaire" << endl;
    cout << "2. Orbit Elliptique" << endl;
    cout << "3. Orbit Parabolique" << endl;
    cout << "4. Orbit Hyperbolique" << endl;
    int choix;
    cin >> choix;

    double r0 = 1.0e9;  // 1 000 000 km

    if (r0 <= 3 * rs) {
        cerr << "Erreur : r0 doit être supérieur à 3 fois le rayon de Schwarzschild pour une orbite stable." << endl;
        return -1;
    }

    double E, L;

    switch (choix) {
    case 1:
        E = (1.0 - rs / (2.0 * r0)) / sqrt(1.0 - 3.0 * rs / r0);
        L = sqrt(G * masse * r0 / (1.0 - 3.0 * rs / r0));
        break;
    case 2: {
        double e;
        cout << "Entrez l'excentricité de l'orbite (0 < e < 1) : ";
        cin >> e;
        if (e <= 0.0 || e >= 1.0) {
            cerr << "Excentricité invalide. Doit être entre 0 et 1 pour une orbite elliptique." << endl;
            return -1;
        }
        calculer_E_L(rs, masse, r0, e, E, L);
        break;
    }
    case 3:
        E = (1.0 - rs / (2.0 * r0)) / sqrt(1.0 - 3.0 * rs / r0);
        L = sqrt(G * masse * r0 / (1.0 - 3.0 * rs / r0));
        break;
    case 4: {
        double e;
        cout << "Entrez l'excentricité de l'orbite (e > 1) : ";
        cin >> e;
        if (e <= 1.0) {
            cerr << "Excentricité invalide. Doit être supérieure à 1 pour une orbite hyperbolique." << endl;
            return -1;
        }
        calculer_E_L(rs, masse, r0, e, E, L);
        break;
    }
    default:
        cerr << "Choix invalide." << endl;
        return -1;
    }

    double dr0_dtau = 0.0;

    Etat etat = {
        0.0,        // Temps propre initial
        r0,         // Position radiale initiale
        M_PI / 2.0, // θ = 90 degrés (plan équatorial)
        0.0,        // φ initial
        dr0_dtau,   // Vitesse radiale initiale
        0.0,        // dθ/dtau initial
        L / (r0 * r0)  // dphi/dtau initial
    };

    double tau_final = 1e6;   // Temps propre final
    double pas = 0.01;         // Pas d'intégration

    double echelle = 3.0e-7; // pixels par mètre

    sf::RenderWindow fenetre(sf::VideoMode(800, 600), "Trajectoire autour d'un trou noir");

    sf::View vue = fenetre.getDefaultView();
    vue.setCenter(0.0f, 0.0f);
    fenetre.setView(vue);

    sf::VertexArray trajectoire(sf::LinesStrip);

    sf::Font police;
    if (!police.loadFromFile("arial.ttf")) {
        cerr << "Erreur de chargement de la police.\n";
        return -1;
    }

    sf::Text texteR, textePhi, texteT;

    texteR.setFont(police);
    texteR.setCharacterSize(14);
    texteR.setFillColor(sf::Color::White);
    texteR.setPosition(10.0f, 10.0f);

    textePhi.setFont(police);
    textePhi.setCharacterSize(14);
    textePhi.setFillColor(sf::Color::White);
    textePhi.setPosition(10.0f, 30.0f);

    texteT.setFont(police);
    texteT.setCharacterSize(14);
    texteT.setFillColor(sf::Color::White);
    texteT.setPosition(10.0f, 50.0f);

    double rs_echelle = rs * echelle;
    double rs_affichage = (rs_echelle < 10.0) ? 10.0 : rs_echelle;
    sf::CircleShape trouNoir(static_cast<float>(rs_affichage));
    trouNoir.setFillColor(sf::Color(50, 50, 50));
    trouNoir.setOutlineThickness(2.0f);
    trouNoir.setOutlineColor(sf::Color::White);
    trouNoir.setOrigin(static_cast<float>(rs_affichage), static_cast<float>(rs_affichage));
    trouNoir.setPosition(0.0f, 0.0f);

    sf::CircleShape particule(5.0f);
    particule.setFillColor(sf::Color::Red);
    particule.setOrigin(5.0f, 5.0f);

    const int nombreEtoiles = 1000;
    sf::VertexArray etoiles(sf::Points, nombreEtoiles);
    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < nombreEtoiles; ++i) {
        float x = static_cast<float>((rand() % 1600) - 800);
        float y = static_cast<float>((rand() % 1200) - 600);
        etoiles[i].position = sf::Vector2f(x, y);
        etoiles[i].color = sf::Color::White;
    }

    const int etapes_par_frame = 100;

    bool enPause = false;

    sf::Clock horloge;

    while (fenetre.isOpen()) {
        sf::Event evenement;
        while (fenetre.pollEvent(evenement)) {
            if (evenement.type == sf::Event::Closed)
                fenetre.close();
            else if (evenement.type == sf::Event::KeyPressed) {
                if (evenement.key.code == sf::Keyboard::Escape)
                    fenetre.close();
                else if (evenement.key.code == sf::Keyboard::Space)
                    enPause = !enPause;
            }
            else if (evenement.type == sf::Event::MouseWheelScrolled) {
                if (evenement.mouseWheelScroll.delta != 0) {
                    if (evenement.mouseWheelScroll.delta > 0)
                        vue.zoom(0.9f);
                    else
                        vue.zoom(1.1f);
                    fenetre.setView(vue);
                }
            }
        }

        if (!enPause) {
            float deltaTime = horloge.restart().asSeconds();
            double facteur_conversion = 1000.0;
            double pas_dynamique = deltaTime * facteur_conversion;
            int etapes = static_cast<int>(pas_dynamique / pas);
            etapes = (etapes > etapes_par_frame) ? etapes_par_frame : etapes;

            for (int i = 0; i < etapes; ++i) {
                if (etat.t < tau_final && etat.r > rs) {
                    etape_runge_kutta(masse, E, L, etat, pas);
                    sf::Vector2f position = polaires_vers_cartesiennes(etat.r, etat.phi, echelle);
                    trajectoire.append(sf::Vertex(position, sf::Color::Red));

                    const int maxPoints = 10000;
                    if (trajectoire.getVertexCount() > maxPoints) {
                        trajectoire.clear();
                        trajectoire.append(sf::Vertex(position, sf::Color::Red));
                    }

                    texteR.setString("r: " + to_string(etat.r) + " m");
                    textePhi.setString("phi: " + to_string(etat.phi) + " rad");
                    texteT.setString("t: " + to_string(etat.t) + " s");

                    particule.setPosition(position);
                }
                else {
                    cout << "La particule a été absorbée par le trou noir." << endl;
                    fenetre.close();
                    break;
                }
            }
        }

        fenetre.clear(sf::Color::Black);
        fenetre.draw(etoiles);
        fenetre.draw(trouNoir);
        fenetre.draw(trajectoire);
        fenetre.draw(particule);
        fenetre.draw(texteR);
        fenetre.draw(textePhi);
        fenetre.draw(texteT);
        fenetre.display();
    }

    return 0;
}
