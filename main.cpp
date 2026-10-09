#include "NatNetAlg.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "dataset.h"
#include "algorithm.h"

int main() {
    Dataset dataset;

    //vygenerujeme 5 klastrov 
    dataset.generateData(5);

    dataset.setNewcomerStatus(0, true); //nastavime 0-ty bod ako newcomer
    dataset.setNewcomerStatus(10, true); //nastavime 10-ty bod ako newcomer

    std::cout << "Pociatocne pozicie bodov:" << std::endl;
    const auto& points = dataset.getPoints();
    for (size_t i = 0; i < points.size(); i++) {
        std::cout << "bod " << i << " (klaster " << points[i].cluster_num << ")";
        if (points[i].isNewObservation) std::cout << " [NEWCOMER]";
        std::cout << ": x1 = " << points[i].x1 << ", x2 = " << points[i].x2 << std::endl;
    }

    Algorithm alg(&dataset);
    alg.runNatNumNet(500); //zatial necham 500 krokov, neskor pridame zastavovacie kriterium

    std::cout << "\nPozicie bodov po 500 casovych krokoch:" << std::endl;
    const auto& updated_points = dataset.getPoints();
    for (size_t i = 0; i < updated_points.size(); i++) {
        std::cout << "bod " << i << " (klaster " << updated_points[i].cluster_num << ")";
        if (updated_points[i].isNewObservation) std::cout << " [NEWCOMER]";
        std::cout << ": x1 = " << updated_points[i].x1 << ", x2 = " << updated_points[i].x2 << std::endl;
    }

    dataset.setNewcomerStatus(0, false); //vratime bod v povodny status
    dataset.setNewcomerStatus(10, false);
    return 0;
}