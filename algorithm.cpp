#include "algorithm.h"
#include <algorithm> //for max function

Algorithm::Algorithm(Dataset* ds)
{
    this->dataset = ds;

    this->K1 = 3100; //z nnn_natura2000habitats_23.pdf pre LDS118 
    this->K2 = 1500; //*LDS118 - learning dataset zo 118 oznacenych segmentovanych reprezentativnych oblasti (vrcholov grafu) - vysledok finalnej optimalizacie topologie grafu siete
    this->delta = 0.003; //tiez odtialto
    this->tau = 0.1; //napr takyto casovy krok
}

double Algorithm::getEpsilon(const Point& u, const Point& v) //parameter epsilon smeru difuzie: > 0- dopredna difuzia, < 0 - spatna difuzia
{
    if (u.isNewObservation || v.isNewObservation) { //najprv akk je aspon jeden z bodov nove pozorovanie, pouzivame vzdy iba doprednu difuziu
        return 1.0; //(hodnota z nnn_directedgraph_23.pdf)
    }
    if (u.cluster_num == v.cluster_num) {
        return 1.0; //rovnake klastre->kladna konst->dopredna difuzia 
    }
    else {
        return -0.01; //rozne klastre->male zaporne eps->spatna difuzia  (tiez odtialto)
    }
}

double Algorithm::calculateDiffusionCoef(const Point& u, const Point& v)
{
    double g_e; //difuzny koef

    double eps = getEpsilon(u, v);

    double l1, l2; //rozdiely jednotlivych (x1 alebo x2) suradnic (vlastnosti)  u a v pre okamih v case, v ktorom sa algoritmus prave nachadza (predch cas krok u nas)
    l1 = u.x1 - v.x1;
    l2 = u.x2 - v.x2;

    if (u.isNewObservation || v.isNewObservation) {
        g_e = std::max(eps * (1 / (1 + K1 * l1 * l1 + K2 * l2 * l2)) - delta, 0.0);
    }
    else {
        g_e = eps * (1 / (1 + K1 * l1 * l1 + K2 * l2 * l2));
    }

    return g_e;
}

