#include "NatNetAlg.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "dataset.h"
#include "algorithm.h"
#include <cmath> //kvoli sqrt
#include <fstream> //pre subory

//int main() {
//    Dataset dataset;
//
//    //vygenerujeme 5 klastrov 
//    dataset.generateData(5);
//
//    dataset.setNewcomerStatus(0, true); //nastavime 0-ty bod ako newcomer
//
//    std::cout << "Pociatocne pozicie bodov:" << std::endl;
//    const auto& points = dataset.getPoints();
//    for (size_t i = 0; i < points.size(); i++) {
//        std::cout << "bod " << i << " (klaster " << points[i].cluster_num << ")";
//        if (points[i].isNewObservation) std::cout << " [NEWCOMER]";
//        std::cout << ": x1 = " << points[i].x1 << ", x2 = " << points[i].x2 << std::endl;
//    }
//
//    Algorithm alg(&dataset);
//    //alg.runNatNumNet(500); //zatial necham 500 krokov, neskor pridame zastavovacie kriterium
//    int timeSteps = alg.saveSimulationToFile("simulaciaNatNetdata.txt", 500);
//
//    std::cout << "\nPozicie bodov po ukonceni simulacie (realny pocet krokov: " << timeSteps << "):" << std::endl;
//    const auto& updated_points = dataset.getPoints();
//    for (size_t i = 0; i < updated_points.size(); i++) {
//        std::cout << "bod " << i << " (klaster " << updated_points[i].cluster_num << ")";
//        if (updated_points[i].isNewObservation) std::cout << " [NEWCOMER]";
//        std::cout << ": x1 = " << updated_points[i].x1 << ", x2 = " << updated_points[i].x2 << std::endl;
//    }
//
//    dataset.setNewcomerStatus(0, false); //vratime bod v povodny status
//    dataset.setNewcomerStatus(10, false);
//    return 0;
//}

int main() {
    Dataset dataset;
    //vygenerujeme zakladny dataset: 5 klastrov po 5 bodov = 25 bodov (vdaka seedu 99 bude vzdy rovnaky)
    dataset.generateData(5);

    //ulozime si pociatocne pozicie, aby sme ich mohli pred kazdym testom obnovit do povodneho stavu
    std::vector<Point> original_points = dataset.getPoints();
    int N_v = original_points.size();

    //vytvorime jednoduchy textovy subor, kde budeme ukladat vysledky do stlpcov (K1, K2, Uspesnost)
    std::ofstream resultsFile("vysledky_grid_search.txt");
    //grid search - systematicke testovanie vsetkych kombinacii parametrov K1 a K2 (ako na mriezke), aby sme zistili, pri akych hodnotach sa newcomer zariadi do klastra s najvyssou uspesnostou
    resultsFile << "K1\tK2\tSpravne\tUspesnost\n"; //hlavicka tabulky

    std::cout << "K1, K2 od 0 do 5000 z krokom 500\n" << std::endl;

    for (double k1 = 0; k1 <= 5000; k1 += 500) {

        for (double k2 = 0; k2 <= 5000; k2 += 500) {

            int correctly_classified = 0; //counter uspesnych pripadov pre tuto konkretnu dvojicu K1, K2

            //cylkus pre testovanie kazdeho jedneho bodu ako newcomera (Leave-One-Out)
            for (int i = 0; i < N_v; i++) {

                //nastavenie datasetu do povodneho stavu - body posunie spat na pociatocne suradnice a zmaze predch newcomera (zmaze posuny z predchadzajuceho testu)
                for (int j = 0; j < N_v; j++) {
                    dataset.updatePointCoordinates(j, original_points[j].x1, original_points[j].x2);
                    dataset.setNewcomerStatus(j, false);
                }

                //nastavime i-ty bod ako newcomera
                dataset.setNewcomerStatus(i, true);
                int correctCluster_num = original_points[i].cluster_num; //sem by sa mal spravne zaradit

                //vytvorime a spustime algoritmus (delta = 0.0 zatial, tau dame napr 0.1)
                Algorithm alg(&dataset, k1, k2, 0.0, 0.1);

                //pre vizualizaciu: exportujeme iba ked testujeme prveho newcomera (i == 0), inak by pocitac robil 3025 textovych suborov a zblaznil by sa, aj tak dufam, ze nezomrie:)
                if (i == 0) {
                    std::string visFile0new = "simulacia0new_K1_" + std::to_string((int)k1) + "_K2_" + std::to_string((int)k2) + ".txt";
                    alg.saveSimulationToFile(visFile0new, 500);
                }
                else {
                    alg.runNatNumNet(500); //vsetkych ostatnych 24 newcomerov zbehne rychlo iba v pamati RAM a iba pre ten resultsFile
                }

                //vyhodnotenie: ku ktoremu klastru je newcomer po zastaveni simulacie najblizsie?
                const std::vector<Point>& current_points = dataset.getPoints();
                double min_distance = 999999; //na zaciatok dame obrovske cislo
                int nearestClust_num = -1;

                for (int j = 0; j < N_v; j++) { //prejdeme kazdy bod z aktualneho datasetu a urcime vzdial od newcomera
                    if (i == j) continue; //newcomera sameho so sebou neporovnavame

                    double dx = current_points[i].x1 - current_points[j].x1;
                    double dy = current_points[i].x2 - current_points[j].x2;
                    double distance = std::sqrt(dx * dx + dy * dy);

                    if (distance < min_distance) {
                        min_distance = distance;
                        nearestClust_num = current_points[j].cluster_num;
                    }
                }

                //ak je newcomer po stabilizacii stale prilis daleko (viac ako 0.1) od vsetkych bodov, znamena to, ze je to outlierma nasilu ho nezaradime nikam
                if (min_distance > 0.1) {
                    nearestClust_num = -1; //zamerne neexistujuci klaster, aby zlyhala kontrola uspesnosti vyssie
                }  

                //skontrolujeme ci sa trafil
                if (nearestClust_num == correctCluster_num) {
                    correctly_classified++;
                }
            }

            //vypiseme a ulozime vysledok pre danu kombinaciu K-ciek
            double accuracy = ((double)correctly_classified / N_v) * 100.0;
            std::cout << "K1: " << k1 << "\tK2: " << k2 << "\t-> Uspesnost: " << accuracy << "% (" << correctly_classified << "/25)\n";

            //zapis do nasej tabulky
            resultsFile << k1 << "\t" << k2 << "\t" << correctly_classified << "\t" << accuracy << "\n";
        }
    }

    resultsFile.close();
    std::cout << "\nGrid Search uspesne ukonceny. Tabulka ulozena v: vysledky_grid_search.txt" << std::endl;

    return 0;
}