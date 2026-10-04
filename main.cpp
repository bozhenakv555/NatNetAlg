#include "NatNetAlg.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "dataset.h"

int main() {
    Dataset ds;

    //TEST: vygenerujeme 3 klastre
    ds.generateData(3);

    const auto& points = ds.getPoints();

    std::cout << "Celkovy pocet vygenerovanych bodov: " << points.size() << "\n";

    for (const auto& p : points) {
        std::cout << "Klaster: " << p.cluster_num
            << " | X: " << p.x
            << " | Y: " << p.y << "\n";
    }

    return 0;
}
//
//int main(int argc, char *argv[])
//{
//    QApplication app(argc, argv);
//    NatNetAlg window;
//    window.show();
//    return app.exec();
//}

