#include "mainwindow.h"
#include <QApplication>
#include "arbolpeliculas.h"

int main(int argc, char *argv[]) {
    // --- ZONA DE PRUEBAS DEL BACKEND ---
    ArbolPeliculas miCartelera;


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}