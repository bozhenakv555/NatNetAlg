#pragma once
#include "dataset.h"

class Algorithm {
private:
    Dataset* dataset;
    //parametre modelu:
    double tau; //casovy krok (delta t)
    double K1, K2; //vahove koeficienty pre jednotlive suradnice (dimenzie (tu mame 2)) FS
    double delta; //parameter velkosti difuzneho okolia (pre nove pozorovanie)

    double getEpsilon(const Point& u, const Point& v);

    double calculateDiffusionCoef(const Point& u, const Point& v);

public:
    Algorithm(Dataset* ds);
   
    void runNatNumNet(int maxTimeSteps);
}


