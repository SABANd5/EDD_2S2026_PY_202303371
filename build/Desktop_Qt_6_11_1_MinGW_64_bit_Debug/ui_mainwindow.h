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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *B;
    QWidget *page1Login;
    QPushButton *btnIniciarSesion;
    QLineEdit *txtCorreo;
    QLineEdit *txtPassword;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QWidget *page3Usuario;
    QLabel *label_7;
    QLineEdit *txtNombreCliente;
    QSpinBox *spinFila;
    QSpinBox *spinColumna;
    QPushButton *btnReservar;
    QPushButton *btnCancelarReserva;
    QPushButton *btnCerrarSesionUsuario;
    QLineEdit *txtSolNombre;
    QLineEdit *txtSolTelefono;
    QComboBox *cmbSolTipo;
    QTextEdit *txtSolDescripcion;
    QPushButton *btnEnviarSolicitud;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QWidget *page2Admin;
    QLineEdit *txtCodigo;
    QPushButton *btnCargaMasiva;
    QPushButton *btnAgregarPelicula;
    QLineEdit *txtClasificacion;
    QLineEdit *txtFin;
    QLabel *label_3;
    QLineEdit *txtEstreno;
    QLabel *label_6;
    QLabel *GENERO;
    QLineEdit *txtGenero;
    QLineEdit *txtTitulo;
    QLabel *label;
    QPushButton *btnReportes;
    QLabel *label_5;
    QSpinBox *spinDuracion;
    QLineEdit *txtIdioma;
    QLabel *label_2;
    QLabel *label_4;
    QPushButton *btnCerrarSesionAdmin;
    QTableWidget *tablaCarteleraAdmin;
    QPushButton *btnActualizarCartelera;
    QLabel *label_11;
    QPushButton *btnCargarSolicitud;
    QLabel *lblSolId;
    QLabel *lblSolCliente;
    QLabel *lblSolTipo;
    QLabel *lblSolDesc;
    QPushButton *btnAprobarSolicitud;
    QPushButton *btnRechazarSolicitud;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(797, 621);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        B = new QStackedWidget(centralwidget);
        B->setObjectName("B");
        B->setGeometry(QRect(20, 10, 791, 571));
        B->setAutoFillBackground(false);
        page1Login = new QWidget();
        page1Login->setObjectName("page1Login");
        btnIniciarSesion = new QPushButton(page1Login);
        btnIniciarSesion->setObjectName("btnIniciarSesion");
        btnIniciarSesion->setGeometry(QRect(370, 450, 121, 41));
        txtCorreo = new QLineEdit(page1Login);
        txtCorreo->setObjectName("txtCorreo");
        txtCorreo->setGeometry(QRect(170, 280, 113, 24));
        txtPassword = new QLineEdit(page1Login);
        txtPassword->setObjectName("txtPassword");
        txtPassword->setGeometry(QRect(180, 330, 113, 24));
        txtPassword->setEchoMode(QLineEdit::EchoMode::Password);
        label_8 = new QLabel(page1Login);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(350, 300, 49, 16));
        label_9 = new QLabel(page1Login);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(330, 340, 49, 16));
        label_10 = new QLabel(page1Login);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(140, 40, 49, 16));
        B->addWidget(page1Login);
        page3Usuario = new QWidget();
        page3Usuario->setObjectName("page3Usuario");
        label_7 = new QLabel(page3Usuario);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(210, 40, 191, 16));
        txtNombreCliente = new QLineEdit(page3Usuario);
        txtNombreCliente->setObjectName("txtNombreCliente");
        txtNombreCliente->setGeometry(QRect(120, 230, 113, 24));
        spinFila = new QSpinBox(page3Usuario);
        spinFila->setObjectName("spinFila");
        spinFila->setGeometry(QRect(230, 120, 71, 31));
        spinColumna = new QSpinBox(page3Usuario);
        spinColumna->setObjectName("spinColumna");
        spinColumna->setGeometry(QRect(130, 120, 71, 31));
        btnReservar = new QPushButton(page3Usuario);
        btnReservar->setObjectName("btnReservar");
        btnReservar->setGeometry(QRect(140, 190, 80, 24));
        btnCancelarReserva = new QPushButton(page3Usuario);
        btnCancelarReserva->setObjectName("btnCancelarReserva");
        btnCancelarReserva->setGeometry(QRect(130, 160, 121, 24));
        btnCerrarSesionUsuario = new QPushButton(page3Usuario);
        btnCerrarSesionUsuario->setObjectName("btnCerrarSesionUsuario");
        btnCerrarSesionUsuario->setGeometry(QRect(640, 10, 111, 71));
        txtSolNombre = new QLineEdit(page3Usuario);
        txtSolNombre->setObjectName("txtSolNombre");
        txtSolNombre->setGeometry(QRect(120, 370, 113, 24));
        txtSolTelefono = new QLineEdit(page3Usuario);
        txtSolTelefono->setObjectName("txtSolTelefono");
        txtSolTelefono->setGeometry(QRect(240, 370, 113, 24));
        cmbSolTipo = new QComboBox(page3Usuario);
        cmbSolTipo->addItem(QString());
        cmbSolTipo->addItem(QString());
        cmbSolTipo->addItem(QString());
        cmbSolTipo->setObjectName("cmbSolTipo");
        cmbSolTipo->setGeometry(QRect(380, 370, 141, 31));
        txtSolDescripcion = new QTextEdit(page3Usuario);
        txtSolDescripcion->setObjectName("txtSolDescripcion");
        txtSolDescripcion->setGeometry(QRect(120, 410, 401, 121));
        btnEnviarSolicitud = new QPushButton(page3Usuario);
        btnEnviarSolicitud->setObjectName("btnEnviarSolicitud");
        btnEnviarSolicitud->setGeometry(QRect(540, 430, 91, 24));
        label_12 = new QLabel(page3Usuario);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(130, 320, 101, 16));
        label_13 = new QLabel(page3Usuario);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(130, 100, 61, 16));
        label_14 = new QLabel(page3Usuario);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(230, 100, 49, 16));
        B->addWidget(page3Usuario);
        page2Admin = new QWidget();
        page2Admin->setObjectName("page2Admin");
        txtCodigo = new QLineEdit(page2Admin);
        txtCodigo->setObjectName("txtCodigo");
        txtCodigo->setGeometry(QRect(100, 70, 113, 24));
        btnCargaMasiva = new QPushButton(page2Admin);
        btnCargaMasiva->setObjectName("btnCargaMasiva");
        btnCargaMasiva->setGeometry(QRect(110, 140, 101, 24));
        btnAgregarPelicula = new QPushButton(page2Admin);
        btnAgregarPelicula->setObjectName("btnAgregarPelicula");
        btnAgregarPelicula->setGeometry(QRect(110, 210, 121, 20));
        txtClasificacion = new QLineEdit(page2Admin);
        txtClasificacion->setObjectName("txtClasificacion");
        txtClasificacion->setGeometry(QRect(200, 300, 113, 24));
        txtFin = new QLineEdit(page2Admin);
        txtFin->setObjectName("txtFin");
        txtFin->setGeometry(QRect(560, 300, 113, 24));
        label_3 = new QLabel(page2Admin);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(210, 260, 91, 16));
        txtEstreno = new QLineEdit(page2Admin);
        txtEstreno->setObjectName("txtEstreno");
        txtEstreno->setGeometry(QRect(320, 300, 113, 24));
        label_6 = new QLabel(page2Admin);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(570, 270, 61, 16));
        GENERO = new QLabel(page2Admin);
        GENERO->setObjectName("GENERO");
        GENERO->setGeometry(QRect(100, 260, 49, 16));
        txtGenero = new QLineEdit(page2Admin);
        txtGenero->setObjectName("txtGenero");
        txtGenero->setGeometry(QRect(80, 300, 113, 24));
        txtTitulo = new QLineEdit(page2Admin);
        txtTitulo->setObjectName("txtTitulo");
        txtTitulo->setGeometry(QRect(220, 70, 113, 24));
        label = new QLabel(page2Admin);
        label->setObjectName("label");
        label->setGeometry(QRect(130, 40, 61, 21));
        btnReportes = new QPushButton(page2Admin);
        btnReportes->setObjectName("btnReportes");
        btnReportes->setGeometry(QRect(110, 180, 71, 21));
        label_5 = new QLabel(page2Admin);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(460, 260, 49, 16));
        spinDuracion = new QSpinBox(page2Admin);
        spinDuracion->setObjectName("spinDuracion");
        spinDuracion->setGeometry(QRect(360, 90, 101, 51));
        txtIdioma = new QLineEdit(page2Admin);
        txtIdioma->setObjectName("txtIdioma");
        txtIdioma->setGeometry(QRect(440, 300, 113, 24));
        label_2 = new QLabel(page2Admin);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(250, 40, 49, 16));
        label_4 = new QLabel(page2Admin);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(340, 260, 61, 16));
        btnCerrarSesionAdmin = new QPushButton(page2Admin);
        btnCerrarSesionAdmin->setObjectName("btnCerrarSesionAdmin");
        btnCerrarSesionAdmin->setGeometry(QRect(640, 20, 101, 31));
        tablaCarteleraAdmin = new QTableWidget(page2Admin);
        tablaCarteleraAdmin->setObjectName("tablaCarteleraAdmin");
        tablaCarteleraAdmin->setGeometry(QRect(50, 340, 461, 81));
        btnActualizarCartelera = new QPushButton(page2Admin);
        btnActualizarCartelera->setObjectName("btnActualizarCartelera");
        btnActualizarCartelera->setGeometry(QRect(530, 340, 80, 24));
        label_11 = new QLabel(page2Admin);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(360, 60, 71, 16));
        btnCargarSolicitud = new QPushButton(page2Admin);
        btnCargarSolicitud->setObjectName("btnCargarSolicitud");
        btnCargarSolicitud->setGeometry(QRect(50, 460, 151, 24));
        lblSolId = new QLabel(page2Admin);
        lblSolId->setObjectName("lblSolId");
        lblSolId->setGeometry(QRect(250, 460, 49, 16));
        lblSolCliente = new QLabel(page2Admin);
        lblSolCliente->setObjectName("lblSolCliente");
        lblSolCliente->setGeometry(QRect(230, 500, 49, 16));
        lblSolTipo = new QLabel(page2Admin);
        lblSolTipo->setObjectName("lblSolTipo");
        lblSolTipo->setGeometry(QRect(240, 530, 49, 16));
        lblSolDesc = new QLabel(page2Admin);
        lblSolDesc->setObjectName("lblSolDesc");
        lblSolDesc->setGeometry(QRect(230, 440, 71, 16));
        btnAprobarSolicitud = new QPushButton(page2Admin);
        btnAprobarSolicitud->setObjectName("btnAprobarSolicitud");
        btnAprobarSolicitud->setGeometry(QRect(580, 460, 131, 24));
        btnRechazarSolicitud = new QPushButton(page2Admin);
        btnRechazarSolicitud->setObjectName("btnRechazarSolicitud");
        btnRechazarSolicitud->setGeometry(QRect(600, 500, 80, 24));
        B->addWidget(page2Admin);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 797, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        B->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnIniciarSesion->setText(QCoreApplication::translate("MainWindow", "INICIAR SESION", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "correo", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "password", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "login", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "VISTA CLIENTE", nullptr));
        btnReservar->setText(QCoreApplication::translate("MainWindow", "RESERVAR", nullptr));
        btnCancelarReserva->setText(QCoreApplication::translate("MainWindow", "CANCELAR RESERVA", nullptr));
        btnCerrarSesionUsuario->setText(QCoreApplication::translate("MainWindow", "CERRAR SESION", nullptr));
        cmbSolTipo->setItemText(0, QCoreApplication::translate("MainWindow", "Cumplea\303\261os", nullptr));
        cmbSolTipo->setItemText(1, QCoreApplication::translate("MainWindow", "Aniversario", nullptr));
        cmbSolTipo->setItemText(2, QCoreApplication::translate("MainWindow", "Requerimiento Especial", nullptr));

        btnEnviarSolicitud->setText(QCoreApplication::translate("MainWindow", "ENVIAR SOLI", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "HACER SOLICITUD", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "COLUMNA", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "FILA", nullptr));
        txtCodigo->setText(QString());
        btnCargaMasiva->setText(QCoreApplication::translate("MainWindow", "CARGA MASIVA", nullptr));
        btnAgregarPelicula->setText(QCoreApplication::translate("MainWindow", "AGREGAR PELICULA", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "CLASIFICACION", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "FIN FECHA", nullptr));
        GENERO->setText(QCoreApplication::translate("MainWindow", "GENERO", nullptr));
        txtGenero->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "CODIGO", nullptr));
        btnReportes->setText(QCoreApplication::translate("MainWindow", "REPORTES", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "IDIOMA", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "TITULO", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "ESTRENO", nullptr));
        btnCerrarSesionAdmin->setText(QCoreApplication::translate("MainWindow", "CERRAR SESION", nullptr));
        btnActualizarCartelera->setText(QCoreApplication::translate("MainWindow", "ACTUALIZAR", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "DURACI\303\223N", nullptr));
        btnCargarSolicitud->setText(QCoreApplication::translate("MainWindow", "CARGAR UNA SOLICITUD", nullptr));
        lblSolId->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        lblSolCliente->setText(QCoreApplication::translate("MainWindow", "CLIENTE", nullptr));
        lblSolTipo->setText(QCoreApplication::translate("MainWindow", "TIPO", nullptr));
        lblSolDesc->setText(QCoreApplication::translate("MainWindow", "DESCUENTO", nullptr));
        btnAprobarSolicitud->setText(QCoreApplication::translate("MainWindow", "APROBAR SOLICITUD", nullptr));
        btnRechazarSolicitud->setText(QCoreApplication::translate("MainWindow", "RECHAZAR", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
