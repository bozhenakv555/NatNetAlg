#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NatNetAlg.h"

class NatNetAlg : public QMainWindow
{
    Q_OBJECT

public:
    NatNetAlg(QWidget *parent = nullptr);
    ~NatNetAlg();

private:
    Ui::NatNetAlgClass ui;
};

