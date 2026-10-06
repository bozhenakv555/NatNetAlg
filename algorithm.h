#pragma once
#include "dataset.h"

class Algorithm {
private:
    Dataset* dataset;
    //parametre modelu:
    double tau; //casovy krok (delta t)
    double K1, K2; //vahove koeficienty pre jednotlive suradnice (dimenzie (tu mame 2)) FS
    double delta; //parameter velkosti difuzneho okolia (pre nove pozorovanie)

    std::vector<std::vector<double>> diffusionCoefs; //matica difuznych koedficientov na hranach medzi vsetkymi bodmi
    //- "matica susednosti" (reprezentuje nas graf, kde kazdy bod je prepojeny s kazdym:
    //ak je silny vplyv v rovnakom klastri, koeficient je velky a kladny, ak ide o odpudzovanie medzi klastrami, je zaporny
    //ak slaby, blizi sa k nule)

    std::vector<std::vector<double>> systemMatrix;   //matica sustavy pre riesenie rovnic difuzie

    double getEpsilon(const Point& u, const Point& v);

    double calculateDiffusionCoef(const Point& u, const Point& v);

public:
    Algorithm(Dataset* ds);

    void buildGraph(); //topologia grafu-priprava hran a vypocet koeficientov
    void buildSystemMatrix(); //konstrukcia prirodzeenj siete - numericka diskretizacia pomocou semi-implicitnej schemy
    void solveSystemSOR();

    void runNatNumNet(int maxTimeSteps);
};