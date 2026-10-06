#include "dataset.h"
#include <random>

void Dataset::clear()
{
    points.clear();
}

void Dataset::addPoint(const Point& p)
{
    points.push_back(p);
}

const std::vector<Point>& Dataset::getPoints() {
    return points;
}

int Dataset::getNumClusters() const
{
    return N_C; 
}

void Dataset::updatePointCoordinates(int index, double newX1, double newX2)
{
    points[index].x1 = newX1;
    points[index].x2 = newX2;
}

void Dataset::generateData(int N_c)
{
    clear();
    this->N_C = N_c;
    double std_deviation = 0.05; //smerodajna odchylka bodov voci centroidu

    //generator(gen) pre nah cisla z nejakeho rozdelenia
    std::random_device rd;
    std::mt19937 gen(rd()); 

    std::uniform_real_distribution<double> unifDist(0.2, 0.8); //vytvarame rovnomerna rozdelenie v rozsah 0.2 az 0.8 (aby centroidy neboli na uplnom okraji)
    std::normal_distribution<double> normalDist(0.0, std_deviation); //vytvarame rozdelnie - predpis pre generaciu pomocou motora gen na generaciu normalne(Gaussovo) rozdelenych bodov
    //1.arg = str hodn, 2.arg = smerodajna odchylka = odmocina z rozptyla


    for (int cluster_num = 0; cluster_num < N_c; cluster_num++) {

        int pointsInCluster = 3 + (rand() % 8); //pocet bodov v klastri (nah cislo v rozsahu od 3 po 10)

        double centroid_x = unifDist(gen);
        double centroid_y = unifDist(gen);

        for (int i = 0; i < pointsInCluster; i++) {
            //okolo taziska (centroidu-stredu) "rozsypeme" body pomocou norm rozd
            double x1 = centroid_x + normalDist(gen);
            double x2 = centroid_y + normalDist(gen);

            Point pnt(x1, x2, cluster_num, false);
            points.push_back(pnt);
        }

    }
}
