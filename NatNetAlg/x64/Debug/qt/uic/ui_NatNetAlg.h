/********************************************************************************
** Form generated from reading UI file 'NatNetAlg.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NATNETALG_H
#define UI_NATNETALG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NatNetAlgClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *NatNetAlgClass)
    {
        if (NatNetAlgClass->objectName().isEmpty())
            NatNetAlgClass->setObjectName("NatNetAlgClass");
        NatNetAlgClass->resize(600, 400);
        menuBar = new QMenuBar(NatNetAlgClass);
        menuBar->setObjectName("menuBar");
        NatNetAlgClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(NatNetAlgClass);
        mainToolBar->setObjectName("mainToolBar");
        NatNetAlgClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(NatNetAlgClass);
        centralWidget->setObjectName("centralWidget");
        NatNetAlgClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(NatNetAlgClass);
        statusBar->setObjectName("statusBar");
        NatNetAlgClass->setStatusBar(statusBar);

        retranslateUi(NatNetAlgClass);

        QMetaObject::connectSlotsByName(NatNetAlgClass);
    } // setupUi

    void retranslateUi(QMainWindow *NatNetAlgClass)
    {
        NatNetAlgClass->setWindowTitle(QCoreApplication::translate("NatNetAlgClass", "NatNetAlg", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NatNetAlgClass: public Ui_NatNetAlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NATNETALG_H
