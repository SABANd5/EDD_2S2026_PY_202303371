#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 1. Incluimos nuestros headers del backend (Fase 1)
#include "arbolpeliculas.h"
#include "listasolicitudes.h"
#include "matrizdispersa.h"
#include "promociones.h"

// 2. Incluimos los NUEVOS headers del backend (Fase 2)
#include "ArbolFunciones.h"
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots de botones heredados de la Fase 1
    void on_btnAgregarPelicula_clicked();
    void on_btnReportes_clicked();
    void on_btnCargaMasiva_clicked();
    void on_btnReservar_clicked();
    void on_btnCancelarReserva_clicked();
    void on_btnIniciarSesion_clicked();
    void on_btnCerrarSesionAdmin_clicked();
    void on_btnCerrarSesionUsuario_clicked();
    void on_btnActualizarCartelera_clicked();
    void on_btnEnviarSolicitud_clicked();
    void on_btnCargarSolicitud_clicked();
    void on_btnAprobarSolicitud_clicked();
    void on_btnRechazarSolicitud_clicked();
    void on_btnCrearPromo_clicked();
    void on_btnAnadirBeneficio_clicked();
    void on_btnCrearFuncion_clicked();
    void on_btnRefrescarSala_clicked();
    void on_btnVerRepPromos_clicked();
    void on_btnVerRepCartelera_clicked();
    void on_btnVerRepSolicitudes_clicked();
    void on_btnVerRepSala_clicked();
    void on_btnConsultarEstado_clicked();
    void on_btnEliminarFuncion_clicked();
    void on_btnBuscarPelicula_clicked();
    void on_btnVerPromociones_clicked();
    void on_btnVerCarteleraUsuario_clicked();
    void actualizarCombosPeliculas();

    // Sugerencia: Puedes agregar un slot nuevo para el botón de carga de clientes


    void on_btnReporteAVL_clicked();

    void on_btnReporteArbolB_clicked();

    void on_btnReporteHash_clicked();

    void on_btnCargaMasivaClientes_clicked();

private:
    Ui::MainWindow *ui;

    // 3. Estructuras heredadas de la Fase 1
    ArbolPeliculas cartelera;
    ListaSolicitudes solicitudes;
    MatrizDispersa sala;
    GestorPromociones promociones;

    // 4. Estructuras NUEVAS requeridas para la Fase 2
    ArbolFunciones avlFunciones;
    ArbolBClientes arbolClientes;
    TablaHashReservas tablaReservas;

    // 5. Declaración de los métodos de parseo JSON
    void cargarMasivaPeliculas(const QString& rutaArchivo);
    void cargarMasivaClientes(const QString& rutaArchivo);
    void actualizarTablaPeliculas();
    void actualizarTablaClientes();
};

#endif // MAINWINDOW_H