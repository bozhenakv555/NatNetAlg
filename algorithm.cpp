#include "algorithm.h"
#include <algorithm> //for max function

Algorithm::Algorithm(Dataset* ds)
{
    this->dataset = ds;

    this->K1 = 3100; //z nnn_natura2000habitats_23.pdf pre LDS118 
    this->K2 = 1500; //*LDS118 - learning dataset zo 118 oznacenych segmentovanych reprezentativnych oblasti (vrcholov grafu) - vysledok finalnej optimalizacie topologie grafu siete
    this->delta = 0.003; //tiez odtialto
    this->tau = 0.5; //napr takyto casovy krok
}

double Algorithm::getEpsilon(const Point& u, const Point& v) //parameter epsilon smeru difuzie: > 0 - dopredna difuzia, < 0 - spatna difuzia
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

void Algorithm::buildSystemMatrix()
{
    //moj prvy navrh:
    int N_v = dataset->getPoints().size();
    systemMatrix.assign(N_v, std::vector<double>(N_v, 0.0)); //pripravime a vyplneme nulami maticu sustavy s rozmerom N_v*N_v

    for (int i = 0; i < N_v; i++) { //prechadzame vsetky riadky
        double sum_g_e = 0.0; //suma difuznych koeficientov pre diagonalu
        for (int j = 0; j < N_v; j++) { //stlpce
            double g_e = diffusionCoefs[i][j]; //dif koef z matice susednosti na i riadku a j stkpci
            if (i != j) { //ak pocitame prvok mimo doagonaly
                systemMatrix[i][j] = -tau * g_e;
            }
            else { 
                continue; //diagonalne prvky riesime potom, ked nazbierame sucet dif koef s kazdeho stlpca tohto riadku
            }
            sum_g_e += g_e;
        }
        systemMatrix[i][i] = 1.0 + tau * sum_g_e; //presli sme kazdy stlpec v danom riaku, a pred prechodom na dalsi riadok, vypocitame diag prvok tohto
    }



    ////eigen alternativa:
    //int N_v = dataset->getPoints().size();
    //systemMatrixEigen = Eigen::MatrixXd::Zero(N_v, N_v); //matica N_v x N_v vyplnena nulami

    //for (int i = 0; i < N_v; i++) {
    //    double sum_g_e = 0.0;
    //    for (int j = 0; j < N_v; j++) {
    //        double g_e = diffusionCoefs[i][j];
    //        if (i != j) {
    //            systemMatrixEigen(i, j) = -tau * g_e; 
    //        }
    //        else {
    //            continue; //diagonalne prvky riesime potom, ked nazbierame sucet dif koef s kazdeho stlpca tohto riadku
    //        }
    //        sum_g_e += g_e;
    //    }
    //    systemMatrixEigen(i, i) = 1.0 + tau * sum_g_e;  //diagonalny prvok
    //}
}

void Algorithm::solveSystemSOR() //sustava: systemMatrix * x1/2new = ps_x1/x2prev
{
   //moj prvy navrh:
   double omega = 1.3; //parameter relaxacie
   int N_v = dataset->getPoints().size();

   //priprava vektorov pre pociatocne odhady a nasledovne "prave pocitane" hodnoty
   std::vector<double> x1new(N_v);
   std::vector<double> x2new(N_v);
   //priprava vektorov pre ps z predch cas kroku
   std::vector<double> ps_x1prev(N_v);
   std::vector<double> ps_x2prev(N_v);

   const std::vector<Point>& points = dataset->getPoints(); //aktualne suradnice bodov 
   for (int i = 0; i < N_v; i++) {
       //p.s sustavy x_i^(n-1)(vertex_index) - hodnoty konkretnej vlastnosti (x1/x2) v znamom predchadzajucom kroku v konkretnom vrchole
       ps_x1prev[i] = points[i].x1; 
       ps_x2prev[i] = points[i].x2;

       //pociatocny odhad pre iteracie (zaciname tento krok tam, kde sme skoncili v minulom kroku):
       x1new[i] = points[i].x1;
       x2new[i] = points[i].x2;
   }

   int iterations = 0;
   //MOZNOST OPTIMALIZACIE: pridat kontrolu konvergencie cez max odchylku (maxShift) noveho bodu od predchadzajuceho
   //->na konci kazkej iteracie while zistime najvacsi posun bodu: ak je mensi ako limit (stopThreshold), cyklus hned ukoncime, lebo riesenie sa uz stabilizovalo a nemeni sa velmi strmo
   while (iterations<100) {
       //Vzorec SOR (Successive Over-Relaxation (w>1)) metody:
       //x_i^(k+1) = (1 - omega) * x_i^(k) + (omega / a_ii) * [ b_i - sum_{j=1}^{i-1} a_ij * x_j^(k+1) - sum_{j=i+1}^{n} a_ij * x_j^(k) ]
        //- x_i^(k+1) = x1/2new[i] (po sor): nova prave pocitana suradnica v iteracii (k+1)
        //- omega: relaxacny parameter pre zrychlenie konvergencie
        //- x_i^(k) = x1/2new[i] (pred sor): stara hodnota suradnice z predch. iteracie (k)
        //- a_ii = systemMatrix[i][j]: diagonalny prvok matice sustavy
        //- b_i = ps_x1/2prev[i]: prava strana (pozicia z casoveho kroku n-1)
        //- 1. suma (1 do i-1) = for po j od 0 po N_v-1: pouziva nove hodnoty (k+1)
        //- 2. suma (i+1 do n) = ten isty for: pouziva stare hodnoty (k) (pred sor)

       for (int i = 0; i < N_v; i++) { //prechadzame vsetky riadky
           double sum1 = 0.0; //pre x1
           double sum2 = 0.0; //pre x2
           for (int j = 0; j < N_v; j++) { //prechadzame vsetky stlpce (ako tie sumy vo vzorci)
               if (i == j) continue; //diag prvok (a_ii) vynechavame, to je v menovateli
               //x1_new[j] automaticky berie (k+1) pre uz spocitane a (k) pre nespocitane:
               sum1 += systemMatrix[i][j] * x1new[j];
               sum2 += systemMatrix[i][j] * x2new[j];
               //namiesto tych dvoch sun nam vznikne jedna {0}^{N_v-1} resp {1}^{N_v}
           }

           //aplikujeme vzorec: (1 - omega) * x_i^(k) + (omega / a_ii) * [ b_i - suma ]
           double x1_sor = (1.0 - omega) * x1new[i] + (omega / systemMatrix[i][i]) * (ps_x1prev[i] - sum1);
           double x2_sor = (1.0 - omega) * x2new[i] + (omega / systemMatrix[i][i]) * (ps_x2prev[i] - sum2);
            
           //a hned zapisujeme prave vypocitanu hodnotu (k+1) do toho vektora pre nasledujuce body
           x1new[i] = x1_sor;
           x2new[i] = x2_sor;
       }
       iterations++;
   }
   //prepiseme pozicie bodov v datasete na nove:
   for (int i = 0; i < N_v; i++) {
       dataset->updatePointCoordinates(i, x1new[i], x2new[i]);
   }



   ////pre eigen maticu:
   // double omega = 1.3; 
   // int N_v = dataset->getPoints().size();

   // std::vector<double> x1new(N_v);
   // std::vector<double> x2new(N_v);
   // std::vector<double> ps_x1prev(N_v);
   // std::vector<double> ps_x2prev(N_v);

   // const std::vector<Point>& points = dataset->getPoints();
   // for (int i = 0; i < N_v; i++) {
   //     ps_x1prev[i] = points[i].x1;
   //     ps_x2prev[i] = points[i].x2;

   //     x1new[i] = points[i].x1;
   //     x2new[i] = points[i].x2;
   // }

   // int iterations = 0;
   // while (iterations < 100) {
   //     for (int i = 0; i < N_v; i++) {
   //         double sum1 = 0.0;
   //         double sum2 = 0.0;
   //         for (int j = 0; j < N_v; j++) {
   //             if (i == j) continue;

   //             //zmena iba tu
   //             sum1 += systemMatrixEigen(i, j) * x1new[j];
   //             sum2 += systemMatrixEigen(i, j) * x2new[j];
   //         }

   //         //aj tu
   //         double x1_sor = (1.0 - omega) * x1new[i] + (omega / systemMatrixEigen(i, i)) * (ps_x1prev[i] - sum1);
   //         double x2_sor = (1.0 - omega) * x2new[i] + (omega / systemMatrixEigen(i, i)) * (ps_x2prev[i] - sum2);

   //         x1new[i] = x1_sor;
   //         x2new[i] = x2_sor;
   //     }
   //     iterations++;
   // }

   // for (int i = 0; i < N_v; i++) {
   //     dataset->updatePointCoordinates(i, x1new[i], x2new[i]);
   // }



   ////eigen navrh s LU dekompoziciou:
   //int N_v = dataset->getPoints().size();
   ////vektory pre prave strany (predchadzajuce pozicie)
   //Eigen::VectorXd ps_x1prev(N_v);
   //Eigen::VectorXd ps_x2prev(N_v);
   //const std::vector<Point>& points = dataset->getPoints();
   //for (int i = 0; i < N_v; i++) {
   //    ps_x1prev(i) = points[i].x1;
   //    ps_x2prev(i) = points[i].x2;
   //}
   ////priame riesenie sustavy cez lu dekompoziciu v eigen: matica sa rozlozi na trojuholnikove casti - hornu a dolnu (raz, kedze je rovnaka pre x1/2), a vysledok pre x1 a x2 sa vypocita presne bez iteracii v jednom kroku
   //Eigen::VectorXd x1new = systemMatrixEigen.partialPivLu().solve(ps_x1prev);
   //Eigen::VectorXd x2new = systemMatrixEigen.partialPivLu().solve(ps_x2prev);
   ////prepis do datasetu
   //for (int i = 0; i < N_v; i++) {
   //    dataset->updatePointCoordinates(i, x1new(i), x2new(i));
   //}
}

void Algorithm::runNatNumNet(int maxTimeSteps)
{
    for (int step = 1; step <= maxTimeSteps; step++) {
        //1. krok: vytvorime/aktualizujeme topologiu grafu a spocitame difuzne koeficienty pre hrany
        buildGraph();

        //2. krok: zostavime maticu sustavy pre tento casovy krok
        buildSystemMatrix();

        //3. krok: vyriesime sustavu rovnic pomocou SOR a spustime difuziu (pohyb bodov)
        solveSystemSOR();
    }
}