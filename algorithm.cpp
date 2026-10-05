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

void Algorithm::buildGraph()
{
    const std::vector<Point> points = dataset->getPoints();
    int N_v = points.size(); //pocet vrholov grafu (dim matice susednosti)

    //nastavime velkost matice na N_v*N_v a vyplneme ju nulami zatial:
    diffusionCoefs.assign(N_v, std::vector<double>(N_v, 0.0)); //v assign: 1.arg nastavi #riadkov, 2.arg. co tie riadky obsahuju (tu: stlpec nul s dlzkou N_v)

    for (int i = 0; i < N_v; i++) {
        for (int j = 0; j < N_v; j++) {
            if (i == j) continue; //vrchol nema hranu sam zo sebou

            diffusionCoefs[i][j] = calculateDiffusionCoef(points[i], points[j]);
        }
    }
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

