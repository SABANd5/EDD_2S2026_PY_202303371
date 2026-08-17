/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnAgregarPelicula;
    QLineEdit *txtCodigo;
    QLineEdit *txtTitulo;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *txtGenero;
    QSpinBox *spinDuracion;
    QLineEdit *txtClasificacion;
    QLineEdit *txtIdioma;
    QLineEdit *txtEstreno;
    QLineEdit *txtFin;
    QLabel *GENERO;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QPushButton *btnCargaMasiva;
    QPushButton *btnReportes;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnAgregarPelicula = new QPushButton(centralwidget);
        btnAgregarPelicula->setObjectName("btnAgregarPelicula");
        btnAgregarPelicula->setGeometry(QRect(250, 220, 231, 71));
        txtCodigo = new QLineEdit(centralwidget);
        txtCodigo->setObjectName("txtCodigo");
        txtCodigo->setGeometry(QRect(250, 190, 113, 24));
        txtTitulo = new QLineEdit(centralwidget);
        txtTitulo->setObjectName("txtTitulo");
        txtTitulo->setGeometry(QRect(370, 190, 113, 24));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(280, 160, 61, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(400, 160, 49, 16));
        txtGenero = new QLineEdit(centralwidget);
        txtGenero->setObjectName("txtGenero");
        txtGenero->setGeometry(QRect(90, 400, 113, 24));
        spinDuracion = new QSpinBox(centralwidget);
        spinDuracion->setObjectName("spinDuracion");
        spinDuracion->setGeometry(QRect(490, 220, 101, 51));
        txtClasificacion = new QLineEdit(centralwidget);
        txtClasificacion->setObjectName("txtClasificacion");
        txtClasificacion->setGeometry(QRect(210, 400, 113, 24));
        txtIdioma = new QLineEdit(centralwidget);
        txtIdioma->setObjectName("txtIdioma");
        txtIdioma->setGeometry(QRect(450, 400, 113, 24));
        txtEstreno = new QLineEdit(centralwidget);
        txtEstreno->setObjectName("txtEstreno");
        txtEstreno->setGeometry(QRect(330, 400, 113, 24));
        txtFin = new QLineEdit(centralwidget);
        txtFin->setObjectName("txtFin");
        txtFin->setGeometry(QRect(570, 400, 113, 24));
        GENERO = new QLabel(centralwidget);
        GENERO->setObjectName("GENERO");
        GENERO->setGeometry(QRect(110, 360, 49, 16));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(220, 360, 91, 16));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(350, 360, 61, 16));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(470, 360, 49, 16));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(580, 370, 61, 16));
        btnCargaMasiva = new QPushButton(centralwidget);
        btnCargaMasiva->setObjectName("btnCargaMasiva");
        btnCargaMasiva->setGeometry(QRect(70, 130, 101, 24));
        btnReportes = new QPushButton(centralwidget);
        btnReportes->setObjectName("btnReportes");
        btnReportes->setGeometry(QRect(70, 170, 80, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnAgregarPelicula->setText(QCoreApplication::translate("MainWindow", "Agregar Pelicula", nullptr));
        txtCodigo->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "CODIGO", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "TITULO", nullptr));
        txtGenero->setText(QString());
        GENERO->setText(QCoreApplication::translate("MainWindow", "GENERO", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "CLASIFICACION", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "ESTRENO", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "IDIOMA", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "FIN FECHA", nullptr));
        btnCargaMasiva->setText(QCoreApplication::translate("MainWindow", "CARGA MASIVA", nullptr));
        btnReportes->setText(QCoreApplication::translate("MainWindow", "REPORTES", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
