#include "mainwindow.h"
#include <QApplication>
#include "arbolpeliculas.h"

int main(int argc, char *argv[]) {
    // --- ZONA DE PRUEBAS DEL BACKEND ---
    ArbolPeliculas miCartelera;

    // Insertamos desordenado para probar que el BST haga su magia
    miCartelera.insertar({"P010", "Pelicula_10", "Acción", 120, "B15", "Español", "2026-08-16", "2026-09-16"});
    miCartelera.insertar({"P005", "Pelicula_5", "Comedia", 105, "A", "Subtitulada", "2026-08-16", "2026-09-16"});
    miCartelera.insertar({"P015", "Pelicula_15", "Terror", 135, "C", "Doblada", "2026-08-16", "2026-09-16"});

    // Generamos el PNG para validar
    miCartelera.generarReporteGraphviz();
    // -----------------------------------

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}