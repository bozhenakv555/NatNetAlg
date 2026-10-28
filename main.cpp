#include "NatNetAlg.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "dataset.h"
#include "algorithm.h"
#include <cmath> //kvoli sqrt
#include <fstream> //pre subory
#include <filesystem> //kniznica pre pracu s priecinkami a suborovym systemom

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

//pomocna struktura na zapamatanie vysledkov grid search
struct GridSearchResult {
    double delta;
    double k1;
    double k2;
    int correctly_classified;
    double accuracy;
};

int main() {
    Dataset dataset;

    double std_dev = 0.05; 

    //vygenerujeme zakladny dataset: 5 klastrov po 5 bodov = 25 bodov (vdaka seedu 99 bude vzdy rovnaky)
    dataset.generateData(5, std_dev);

    //ulozime si pociatocne pozicie, aby sme ich mohli pred kazdym testom obnovit do povodneho stavu
    std::vector<Point> original_points = dataset.getPoints();
    int N_v = original_points.size();
    int N_c = dataset.getNumClusters();

    //najdeme prveho newcomera pre kazdy klaster 
    std::vector<int> representative_newcomers(N_c, -1);
    for (int j = 0; j < N_v; j++) {
        int c = original_points[j].cluster_num;
        //ak sme pre tento klaster este neukladali ziadneho reprezentanta, zapiseme si ho
        if (representative_newcomers[c] == -1) {
            representative_newcomers[c] = j;
        }
    }

    //vytvorime jednoduchy textovy subor, kde budeme ukladat vysledky do stlpcov (K1, K2, Uspesnost)
    std::ofstream resultsFile("vysledky_grid_search3.txt");
    //grid search - systematicke testovanie vsetkych kombinacii parametrov K1 a K2 (ako na mriezke), aby sme zistili, pri akych hodnotach sa newcomer zariadi do klastra s najvyssou uspesnostou
    resultsFile << "Delta\tK1\tK2\tSpravne\tUspesnost\n"; //hlavicka tabulky

    std::cout << "Grid Search s K1, K2 od 0 do 5000 z krokom 500 a s Deltou od 0 po 0.05 s krokom 0.03\n" << std::endl;

    std::vector<GridSearchResult> all_results;
    double max_accuracy = -1.0;

    //GRID SEARCH V RAM
    for (double delta = 0.0; delta < 0.049; delta += 0.003) {
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

                    //vytvorime a spustime algoritmus (tau dame napr 0.1)
                    Algorithm alg(&dataset, k1, k2, delta, 0.1);

                    //pre vizualizaciu: exportujeme iba ked testujeme prveho newcomera (i == 0), inak by pocitac robil 3025 textovych suborov a zblaznil by sa, aj tak dufam, ze nezomrie:)
                    if (i == 0) {
                        std::string visFile0new = "simulaciaK1K2Delta_0new/simulacia0new_Delta_" + std::to_string((int)(delta * 1000)) + "_K1_" + std::to_string((int)k1) + "_K2_" + std::to_string((int)k2) + ".txt";
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
                std::cout << "Delta: " << delta << "\tK1: " << k1 << "\tK2: " << k2 << "\t\t-> Uspesnost: " << accuracy << "% (" << correctly_classified << "/25)\n";

                //zapis do nasej tabulky
                resultsFile << delta << "\t" << k1 << "\t" << k2 << "\t" << correctly_classified << "\t" << accuracy << "\n";

                //ulozime si vysledok do vektora
                GridSearchResult res{ delta, k1, k2, correctly_classified, accuracy };
                all_results.push_back(res);

                //aktualizujeme maximalnu uspesnost
                if (accuracy > max_accuracy) {
                    max_accuracy = accuracy;
                }
            }
        }
    }

    resultsFile.close();
    std::cout << "\nGrid Search uspesne ukonceny. Tabulka ulozena v: vysledky_grid_search3.txt" << std::endl;
    std::cout << "\n\nUkladame simulacne subory pre najlepsie kombinacie parametrov...\n";

    //EXPORT SIMULACNYCH SUBOROV IBA PRE NAJUSPESNEJSIE KOMBINACIE A PRE KAZDY Z 5 KLASTROV
    int exported_combinations = 0;

    for (const GridSearchResult res : all_results) {
        //vyberame iba kombinacie, ktore dosiahli maximalnu uspesnost (napr. 100% alebo najvyssiu moznu z testovanych)
        if (res.accuracy == max_accuracy) {

            //pre kazdy z 5 klastrov vygenerujeme simulaciu s jeho reprezentativnym newcomerom
            for (int c = 0; c < N_c; c++) {
                int newcomer_idx = representative_newcomers[c];

                //reset datasetu
                for (int j = 0; j < N_v; j++) {
                    dataset.updatePointCoordinates(j, original_points[j].x1, original_points[j].x2);
                    dataset.setNewcomerStatus(j, false);
                }

                dataset.setNewcomerStatus(newcomer_idx, true);

                Algorithm alg(&dataset, res.k1, res.k2, res.delta, 0.1);

                int std_dev_int = (int)std::round(std_dev * 100); //0.05 -> 5

                std::string folderName = "simulaciaK1K2Delta_std_" + std::to_string(std_dev_int) + "_klaster_" + std::to_string(c);
                std::filesystem::create_directories(folderName);

                std::string filename = folderName + "/simulacia_std_" + std::to_string(std_dev_int) +
                    "_klaster_" + std::to_string(c) +
                    "_Delta_" + std::to_string((int)std::round(res.delta * 1000)) +
                    "_K1_" + std::to_string((int)res.k1) +
                    "_K2_" + std::to_string((int)res.k2) + ".txt";

                alg.saveSimulationToFile(filename, 500);
            }
            exported_combinations++;
        }
    }

    std::cout << "Uspesne exportovane simulacie pre " << exported_combinations << " najlepsich kombinacii parametrov (pre vsetkych 5 klastrov).\n";

    return 0;
}