#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

vector<vector<double>> chargerDonnees(const string& nomFichier) {
    ifstream fichier(nomFichier);
    string ligne;
    vector<vector<double>> donnees;

    getline(fichier, ligne); // Passer la ligne d'en-tête

    while (getline(fichier, ligne)) {
        stringstream ss(ligne);
        string valeur;
        vector<double> ligneDonnees;

        while (getline(ss, valeur, ',')) {
            ligneDonnees.push_back(stod(valeur));  // Convertir chaque valeur en double
        }

        donnees.push_back(ligneDonnees);
    }

    return donnees;
}

sf::VertexArray genererFondEtoile(int largeur, int hauteur, int nbEtoiles) {
    sf::VertexArray etoiles(sf::Points, nbEtoiles);
    for (int i = 0; i < nbEtoiles; ++i) {
        etoiles[i].position = sf::Vector2f(rand() % largeur, rand() % hauteur);
        etoiles[i].color = sf::Color::White;
    }
    return etoiles;
}

void afficherSimulation() {
    string nomFichier = "simulation_data.csv";
    vector<vector<double>> donnees = chargerDonnees(nomFichier);

    if (donnees.empty()) {
        cout << "Erreur : les données du fichier CSV n'ont pas été chargées correctement.\n";
        return;
    }

    sf::RenderWindow fenetre(sf::VideoMode(1280, 720), "Simulation Particule Trou Noir");

    double facteurEchelle = 1e-7;
    double rayonMax = 1.5e10;

    sf::VertexArray trajectoire(sf::LineStrip);
    sf::VertexArray etoiles = genererFondEtoile(1280, 720, 1000);

    sf::View vue = fenetre.getDefaultView();
    float facteurZoom = 1.0f;
    bool estEnPause = false;

    size_t index = 0;
    while (fenetre.isOpen()) {
        sf::Event evenement;
        while (fenetre.pollEvent(evenement)) {
            if (evenement.type == sf::Event::Closed)
                fenetre.close();
            else if (evenement.type == sf::Event::MouseWheelScrolled) {
                if (evenement.mouseWheelScroll.delta > 0)
                    facteurZoom *= 0.9f;
                else
                    facteurZoom *= 1.1f;
                vue.setSize(fenetre.getDefaultView().getSize() * facteurZoom);
            }
            else if (evenement.type == sf::Event::KeyPressed) {
                if (evenement.key.code == sf::Keyboard::Up)
                    vue.move(0, -10);
                else if (evenement.key.code == sf::Keyboard::Down)
                    vue.move(0, 10);
                else if (evenement.key.code == sf::Keyboard::Left)
                    vue.move(-10, 0);
                else if (evenement.key.code == sf::Keyboard::Right)
                    vue.move(10, 0);
                else if (evenement.key.code == sf::Keyboard::Space)
                    estEnPause = !estEnPause;
            }
        }

        if (!estEnPause) {
            if (index < donnees.size()) {
                double r = donnees[index][1];
                double theta = donnees[index][3];

                r = std::min(r, rayonMax);

                double x = r * cos(theta) * facteurEchelle;
                double y = r * sin(theta) * facteurEchelle;

                cout << "Index: " << index << " - r: " << r << ", theta: " << theta << ", x: " << x << ", y: " << y << endl;

                sf::Color couleur = sf::Color::Red;
                if (r < rayonMax / 3) {
                    couleur = sf::Color::Green;
                }
                else if (r < 2 * rayonMax / 3) {
                    couleur = sf::Color::Yellow;
                }

                if (abs(x) < 640 && abs(y) < 360) {
                    trajectoire.append(sf::Vertex(sf::Vector2f(640 + x, 360 + y), couleur));
                }
                else {
                    cout << "Point hors limites : x = " << x << ", y = " << y << endl;
                }

                ++index;
            }
        }

        fenetre.clear();
        fenetre.setView(vue);
        fenetre.draw(etoiles);
        fenetre.draw(trajectoire);
        fenetre.display();

        sf::sleep(sf::milliseconds(100));
    }
}

int main() {
    cout << "Simulation de la trajectoire d'une particule autour d'un trou noir\n";
    cout << "Lancement de l'affichage de la trajectoire.\n";

    afficherSimulation();

    return 0;
}

