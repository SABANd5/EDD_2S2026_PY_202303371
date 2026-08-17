#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 1. Incluimos nuestros headers del backend
#include "arbolpeliculas.h"
#include "listasolicitudes.h"
#include "matrizdispersa.h"
#include "promociones.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Aquí Qt Designer irá agregando automáticamente los "Slots" (eventos de botones)
private slots:
    void on_btnAgregarPelicula_clicked(); // Ejemplo que crearemos ahora

    void on_btnReportes_clicked();

    void on_btnCargaMasiva_clicked();

private:
    Ui::MainWindow *ui;

    // 2. Declaramos nuestras estructuras como miembros de la ventana
    // Así persistirán en memoria mientras la ventana esté abierta
    ArbolPeliculas cartelera;
    ListaSolicitudes solicitudes;
    MatrizDispersa sala;
    GestorPromociones promociones;
};

#endif // MAINWINDOW_H