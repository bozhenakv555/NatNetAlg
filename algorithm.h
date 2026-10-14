#pragma once
#include "dataset.h"
#include <Eigen/Dense> //pridany eigen pre pracu s maticami

class Algorithm {
private:
    Dataset* dataset;
    //parametre modelu:
    double K1, K2; //vahove koeficienty pre jednotlive suradnice (dimenzie (tu mame 2)) FS
    double delta; //parameter velkosti difuzneho okolia (pre nove pozorovanie)
    double tau; //casovy krok 

    std::vector<std::vector<double>> diffusionCoefs; //matica difuznych koedficientov na hranach medzi vsetkymi bodmi
    //- "matica susednosti" (reprezentuje nas graf, kde kazdy bod je prepojeny s kazdym:
    //ak je silny vplyv v rovnakom klastri, koeficient je velky a kladny, ak ide o odpudzovanie medzi klastrami, je zaporny
    //ak slaby, blizi sa k nule)

    std::vector<std::vector<double>> systemMatrix;  //matica sustavy pre riesenie rovnic difuzie
    //Eigen::MatrixXd systemMatrixEigen; //alternativna matica sustavy pre riesenie rovnic difuzie s pouzitim eigen

    double getEpsilon(const Point& v, const Point& u);

    double calculateDiffusionCoef(const Point& v, const Point& u);

public:
    //Algorithm(Dataset* ds, double K1 = 3100.0, double K2 = 1500.0, double delta = 0.0, double tau = 0.5) : dataset(ds), K1(K1), K2(K2), delta(delta), tau(tau) {};
    Algorithm(Dataset* ds);
    Algorithm(Dataset* ds, double K1, double K2);
    Algorithm(Dataset* ds, double K1, double K2, double delta, double tau);

    bool buildGraph(double diffusionGap = 0.01); //topologia grafu-priprava hran a vypocet koeficientov (a rovno zistujeme ci uz mozeme zastavit simulaciu - vrati true ak sa uz vsetky body v klastroch dostatocne zhlukli (koeficient sa blizi k 1))
    void buildSystemMatrix(); //konstrukcia prirodzenej siete - numericka diskretizacia pomocou semi-implicitnej schemy
    void solveSystemSOR(); //vyriesenie sustavy rovnic difuzie v tej scheme pomocou iteracnej metody SOR

    int runNatNumNet(int maxTimeSteps);

    int saveSimulationToFile(std::string filename, int maxTimeSteps);
};