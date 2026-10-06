#include "NatNetAlg.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "dataset.h"
#include "algorithm.h"

int main() {
    Dataset dataset;

    dataset.generateData(5);

    std::cout << "pociatocne pozicie vygenerovanych bodov:" << std::endl;
    const auto& points = dataset.getPoints();
    for (size_t i = 0; i < points.size(); i++) {
        std::cout << "bod " << i << " (klaster " << points[i].cluster_num << "): x1 = " << points[i].x1 << ", x2 = " << points[i].x2 << std::endl;
    }

    Algorithm alg(&dataset);
    alg.runNatNumNet(500);

    std::cout << "\npozicie bodov po 500 casovych krokoch:" << std::endl;
    const auto& updated_points = dataset.getPoints();
    for (size_t i = 0; i < updated_points.size(); i++) {
        std::cout << "bod " << i << " (klaster " << updated_points[i].cluster_num << "): x1 = " << updated_points[i].x1 << ", x2 = " << updated_points[i].x2 << std::endl;
    }

    return 0;
}