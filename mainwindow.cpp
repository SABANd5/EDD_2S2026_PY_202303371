#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox> // Para mostrar alertas emergentes
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Evento generado al hacer clic en el botón
void MainWindow::on_btnAgregarPelicula_clicked()
{
    // 1. Extraer los datos de la interfaz gráfica y convertirlos a std::string
    std::string codigo = ui->txtCodigo->text().toStdString();
    std::string titulo = ui->txtTitulo->text().toStdString();
    std::string genero = ui->txtGenero->text().toStdString();
    int duracion = ui->spinDuracion->value(); // Si usas un QSpinBox para números
    std::string clasificacion = ui->txtClasificacion->text().toStdString();
    std::string idioma = ui->txtIdioma->text().toStdString();
    std::string fechaEstreno = ui->txtEstreno->text().toStdString();
    std::string fechaFin = ui->txtFin->text().toStdString();

    // 2. Validar que los campos clave no estén vacíos (Opcional pero recomendado)
    if(codigo.empty() || titulo.empty()) {
        QMessageBox::warning(this, "Error", "El código y título son obligatorios.");
        return;
    }

    // 3. Empaquetar en nuestro Struct y enviarlo a nuestro Árbol BST
    Pelicula nuevaPelicula = {codigo, titulo, genero, duracion, clasificacion, idioma, fechaEstreno, fechaFin};

    cartelera.insertar(nuevaPelicula);

    // 4. Mostrar feedback al usuario y limpiar los campos
    QMessageBox::information(this, "Éxito", "Película agregada a la cartelera.");

    ui->txtCodigo->clear();
    ui->txtTitulo->clear();
    // ... limpiar el resto
}
void MainWindow::on_btnReportes_clicked()
{
    // Aquí puedes disparar todos los reportes al mismo tiempo,
    // o crear un menú para que el admin elija cuál generar.
    // Por ahora, generaremos los 4 de golpe para asegurar esos 18 puntos[cite: 1].

    cartelera.generarReporteGraphviz();
    solicitudes.generarReporteGraphviz();
    sala.generarReporteGraphviz();
    promociones.generarReporteGraphviz();

    QMessageBox::information(this, "Reportes Generados", "Los archivos .png de Graphviz se han generado en la carpeta del proyecto (build).");
}



void MainWindow::on_btnCargaMasiva_clicked()
{
    // Abre el explorador de archivos y filtra para que solo muestre archivos .csv
    QString rutaArchivoQt = QFileDialog::getOpenFileName(this, "Seleccionar archivo CSV", "", "Archivos CSV (*.csv)");

    // Verificamos si el usuario seleccionó un archivo o canceló la ventana
    if (rutaArchivoQt.isEmpty()) {
        QMessageBox::information(this, "Cancelado", "No se seleccionó ningún archivo.");
        return;
    }

    // Convertimos el QString a std::string para nuestro backend
    std::string rutaArchivo = rutaArchivoQt.toStdString();

    // Llamamos a la función que construimos en el backend
    cartelera.cargarDesdeCSV(rutaArchivo);

    QMessageBox::information(this, "Éxito", "La carga masiva se ha completado. Revisa la consola para más detalles.");
}

