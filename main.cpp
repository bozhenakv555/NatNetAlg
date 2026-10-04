#include "NatNetAlg.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NatNetAlg window;
    window.show();
    return app.exec();
}
