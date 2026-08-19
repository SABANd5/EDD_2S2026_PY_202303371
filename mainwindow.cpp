#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox> // Para mostrar alertas emergentes
#include <QFileDialog>
#include <QPixmap>
#include <QFile>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->B->setCurrentWidget(ui->page1Login);
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


// --- 1. RESERVAR ASIENTO ---
void MainWindow::on_btnReservar_clicked()
{
    // Leer los datos ingresados en la interfaz
    int fila = ui->spinFila->value();
    int columna = ui->spinColumna->value();
    std::string cliente = ui->txtNombreCliente->text().toStdString();

    // Validación básica de campos
    if (cliente.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Por favor, ingresa tu nombre para realizar la reserva.");
        return;
    }

    // Intentamos reservar en la Matriz Dispersa
    bool exito = sala.reservarAsiento(fila, columna, cliente);

    if (exito) {
        QMessageBox::information(this, "Reserva Confirmada",
                                 QString("¡Asiento F%1-C%2 reservado con éxito para %3!")
                                     .arg(fila).arg(columna).arg(QString::fromStdString(cliente)));

        // Limpiamos el campo del nombre para la siguiente reserva
        ui->txtNombreCliente->clear();
    } else {
        // La matriz nos devolvió 'false', significa que está ocupado o las coordenadas no existen
        QMessageBox::critical(this, "Asiento No Disponible",
                              "Lo sentimos, ese asiento ya está ocupado o no existe en esta sala. Por favor, selecciona otro.");
    }
}

// --- 2. CANCELAR RESERVA ---
void MainWindow::on_btnCancelarReserva_clicked()
{
    // Para cancelar, el PDF indica que el cliente debe ingresar su nombre.
    // Utilizaremos el método extra que creamos para buscar por nombre en toda la matriz.
    std::string cliente = ui->txtNombreCliente->text().toStdString();

    if (cliente.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Ingresa tu nombre para buscar y cancelar tus reservas.");
        return;
    }

    // Llamamos al backend para que libere los asientos
    sala.cancelarPorCliente(cliente);

    QMessageBox::information(this, "Cancelación",
                             "Se han procesado las cancelaciones. Si tenías asientos a tu nombre, ahora están libres.");

    ui->txtNombreCliente->clear();
}

// --- LÓGICA DE INICIO DE SESIÓN ---
void MainWindow::on_btnIniciarSesion_clicked()
{
    // Extraemos el correo y contraseña ingresados
    QString correo = ui->txtCorreo->text();
    QString password = ui->txtPassword->text();

    // 1. Validar Credenciales de Administrador
    if (correo == "admin@usac.edu.gt" && password == "admin123") {
        // Cambiamos a la vista del Administrador
        ui->B->setCurrentWidget(ui->page2Admin);
        ui->txtCorreo->clear();
        ui->txtPassword->clear();
    }
    // 2. Validar Credenciales de Cliente en Taquilla[cite: 1]
    else if (correo == "cliente@gmail.com" && password == "cliente123") {
        // Cambiamos a la vista del Cliente
        ui->B->setCurrentWidget(ui->page3Usuario);
        ui->txtCorreo->clear();
        ui->txtPassword->clear();
    }
    // 3. Credenciales incorrectas
    else {
        QMessageBox::critical(this, "Error de Autenticación", "Correo o contraseña incorrectos. Por favor, intenta de nuevo.");
    }
}


// --- LÓGICA PARA CERRAR SESIÓN ---
void MainWindow::on_btnCerrarSesionAdmin_clicked()
{
    // Devolvemos al usuario a la página de Login
    ui->B->setCurrentWidget(ui->page1Login);
}

void MainWindow::on_btnCerrarSesionUsuario_clicked()
{
    // Devolvemos al usuario a la página de Login
    ui->B->setCurrentWidget(ui->page1Login);
}


// --- ACTUALIZAR TABLA DE CARTELERA CON ALERTAS (VISTA ADMIN) ---
void MainWindow::on_btnActualizarCartelera_clicked()
{
    std::vector<Pelicula> peliculas = cartelera.obtenerCarteleraInorden();

    ui->tablaCarteleraAdmin->setColumnCount(7);
    QStringList encabezados = {"Código", "Título", "Género", "Duración", "Clasificación", "Idioma", "Estreno"};
    ui->tablaCarteleraAdmin->setHorizontalHeaderLabels(encabezados);
    ui->tablaAsientos->horizontalHeader()->setVisible(true); // Asegurarnos que se vean

    ui->tablaCarteleraAdmin->setRowCount(peliculas.size());

    // Variables para nuestra alerta
    QDate fechaHoy = QDate::currentDate();
    QString alertaMensaje = "";
    bool hayAlertas = false;

    // Llenar la tabla
    for (size_t i = 0; i < peliculas.size(); ++i) {
        ui->tablaCarteleraAdmin->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(peliculas[i].codigo)));
        ui->tablaCarteleraAdmin->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(peliculas[i].titulo)));
        ui->tablaCarteleraAdmin->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(peliculas[i].genero)));
        ui->tablaCarteleraAdmin->setItem(i, 3, new QTableWidgetItem(QString::number(peliculas[i].duracion) + " min"));
        ui->tablaCarteleraAdmin->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(peliculas[i].clasificacion)));
        ui->tablaCarteleraAdmin->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(peliculas[i].idioma)));
        ui->tablaCarteleraAdmin->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(peliculas[i].fechaEstreno)));

        // --- LÓGICA DE ALERTA DE 7 DÍAS ---
        QString fechaFinStr = QString::fromStdString(peliculas[i].fechaFin);
        QDate fechaFin = QDate::fromString(fechaFinStr, "yyyy-MM-dd"); // El formato que pide el PDF[cite: 1]

        if (fechaFin.isValid()) {
            qint64 diasRestantes = fechaHoy.daysTo(fechaFin);

            // Si faltan entre 0 y 7 días para que salga de cartelera
            if (diasRestantes >= 0 && diasRestantes <= 7) {
                hayAlertas = true;
                alertaMensaje += "- " + QString::fromStdString(peliculas[i].titulo) +
                                 " (Sale en " + QString::number(diasRestantes) + " días)\n";

                // Pintamos toda la fila de un color rojito/salmón de advertencia
                for (int col = 0; col < 7; col++) {
                    ui->tablaCarteleraAdmin->item(i, col)->setBackground(QBrush(QColor(255, 150, 150)));
                }
            }
        }
    }

    ui->tablaCarteleraAdmin->resizeColumnsToContents();

    // Lanzamos el pop-up si encontramos películas a punto de caducar
    if (hayAlertas) {
        QMessageBox::warning(this, "¡Alerta de Cartelera!",
                             "Atención Admin, las siguientes películas están próximas a salir de cartelera:\n\n" + alertaMensaje);
    }
}

// --- USUARIO: ENVIAR SOLICITUD ---
void MainWindow::on_btnEnviarSolicitud_clicked()
{
    std::string nombre = ui->txtSolNombre->text().toStdString();
    std::string telefono = ui->txtSolTelefono->text().toStdString();
    std::string tipo = ui->cmbSolTipo->currentText().toStdString();
    std::string descripcion = ui->txtSolDescripcion->toPlainText().toStdString();
    std::string fecha = "2026-08-16"; // Puedes usar QDate::currentDate().toString() luego

    if (nombre.empty() || descripcion.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Por favor llena tu nombre y la descripción.");
        return;
    }

    // Insertamos en la Lista Circular Doblemente Enlazada
    solicitudes.registrarSolicitud(nombre, telefono, tipo, descripcion, fecha);

    QMessageBox::information(this, "Enviado", "Tu solicitud especial ha sido registrada y está en estado: Pendiente.");

    // Limpiar campos
    ui->txtSolNombre->clear();
    ui->txtSolTelefono->clear();
    ui->txtSolDescripcion->clear();
}


// --- ADMIN: CARGAR SOLICITUD EN PANTALLA ---
void MainWindow::on_btnCargarSolicitud_clicked()
{
    Solicitud* actual = solicitudes.obtenerPrimeraSolicitud();

    if (actual == nullptr) {
        ui->lblSolId->setText("ID: --");
        ui->lblSolCliente->setText("No hay solicitudes pendientes.");
        ui->lblSolTipo->setText("Tipo: --");
        ui->lblSolDesc->setText("Descripción: --");
        return;
    }

    // Mostramos los datos en los Labels
    ui->lblSolId->setText(QString("ID Solicitud: %1").arg(actual->id));
    ui->lblSolCliente->setText(QString::fromStdString(actual->cliente));
    ui->lblSolTipo->setText(QString::fromStdString(actual->tipo));
    ui->lblSolDesc->setText(QString::fromStdString(actual->descripcion));
}

// --- ADMIN: APROBAR ---
void MainWindow::on_btnAprobarSolicitud_clicked()
{
    if (solicitudes.obtenerPrimeraSolicitud() != nullptr) {
        solicitudes.aprobarPrimeraSolicitud(); // Cambia estado y rota la lista
        QMessageBox::information(this, "Aprobada", "La solicitud pasó a 'Atendida'.");
        on_btnCargarSolicitud_clicked(); // Recargar la pantalla con la que sigue
    }
}

// --- ADMIN: RECHAZAR ---
void MainWindow::on_btnRechazarSolicitud_clicked()
{
    if (solicitudes.obtenerPrimeraSolicitud() != nullptr) {
        solicitudes.rechazarPrimeraSolicitud(); // Elimina el nodo
        QMessageBox::information(this, "Rechazada", "La solicitud fue eliminada de la cola.");
        on_btnCargarSolicitud_clicked(); // Recargar la pantalla
    }
}


void MainWindow::on_btnCrearPromo_clicked()
{
    std::string codigo = ui->txtPromoCodigo->text().toStdString();
    std::string nombre = ui->txtPromoNombre->text().toStdString();
    std::string vigencia = ui->txtPromoVigencia->text().toStdString();
    std::string dias = ui->txtPromoDias->text().toStdString();

    if (codigo.empty() || nombre.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "El código y el nombre de la promoción son obligatorios.");
        return;
    }

    // Insertamos en la lista circular principal
    promociones.registrarPromocion(codigo, nombre, vigencia, dias);

    QMessageBox::information(this, "Éxito", "Promoción principal registrada. Ahora puedes añadirle beneficios.");

    // Limpiamos los campos
    ui->txtPromoCodigo->clear();
    ui->txtPromoNombre->clear();
    ui->txtPromoVigencia->clear();
    ui->txtPromoDias->clear();
}

// --- ADMIN: AÑADIR BENEFICIO A PROMOCIÓN EXISTENTE ---
void MainWindow::on_btnAnadirBeneficio_clicked()
{
    std::string codigoPromo = ui->txtBenCodigoPromo->text().toStdString();
    std::string tipo = ui->txtBenTipo->text().toStdString();
    std::string descripcion = ui->txtBenDesc->text().toStdString();
    std::string valor = ui->txtBenValor->text().toStdString();

    if (codigoPromo.empty() || tipo.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Debes indicar el código de la promoción y el tipo de beneficio.");
        return;
    }

    // Insertamos en la sub-lista doblemente enlazada
    promociones.anadirBeneficio(codigoPromo, tipo, descripcion, valor);

    QMessageBox::information(this, "Beneficio Añadido", "El beneficio se ha enlazado a la promoción correctamente.");

    // Limpiamos los campos (dejamos el código por si quiere seguir agregando a la misma)
    ui->txtBenTipo->clear();
    ui->txtBenDesc->clear();
    ui->txtBenValor->clear();
}

// --- ADMIN: CREAR FUNCIÓN Y GENERAR MATRIZ ---
void MainWindow::on_btnCrearFuncion_clicked()
{
    int filas = ui->spinFilasSala->value();
    int columnas = ui->spinColumnasSala->value();

    if (filas <= 0 || columnas <= 0) {
        QMessageBox::warning(this, "Error", "Las dimensiones de la sala deben ser mayores a 0.");
        return;
    }

    // Como el PDF indica que solo hay una función a la vez,
    // inicializarSala() reconstruirá la cuadrícula con asientos LIBRES.
    // (Nota: Si tu método inicializarSala no limpia la memoria anterior, asegúrate de hacerlo o re-instanciar el objeto 'sala').

    sala = MatrizDispersa(); // Reiniciamos la matriz para cumplir la regla de sobrescritura
    sala.inicializarSala(filas, columnas);

    QMessageBox::information(this, "Función Creada",
                             QString("Se ha generado el mapa de asientos con %1 filas y %2 columnas.").arg(filas).arg(columnas));
}


void MainWindow::on_btnRefrescarSala_clicked()
{
    // Leemos las dimensiones directamente de lo que el Admin configuró en sus SpinBoxes
    int totalFilas = ui->spinFilasSala->value();
    int totalColumnas = ui->spinColumnasSala->value();

    if (totalFilas == 0 || totalColumnas == 0) {
        QMessageBox::warning(this, "Aviso", "Aún no se ha creado una función con asientos válidos.");
        return;
    }

    // Preparamos la tabla visual
    ui->tablaAsientos->setRowCount(totalFilas);
    ui->tablaAsientos->setColumnCount(totalColumnas);
    ui->tablaAsientos->horizontalHeader()->setVisible(false);
    ui->tablaAsientos->verticalHeader()->setVisible(false);

    // Pintamos la tabla consultando a nuestro nuevo método
    for (int f = 1; f <= totalFilas; f++) {
        for (int c = 1; c <= totalColumnas; c++) {

            std::string estado = sala.obtenerEstadoAsiento(f, c);
            QTableWidgetItem *asientoItem = new QTableWidgetItem();

            if (estado == "LIBRE") {
                asientoItem->setBackground(QBrush(Qt::green));
                asientoItem->setText("Libre");
            } else if (estado == "ND") {
                asientoItem->setBackground(QBrush(Qt::gray)); // Por si hay algún error de punteros
                asientoItem->setText("Error");
            } else {
                asientoItem->setBackground(QBrush(Qt::red));
                asientoItem->setText(QString::fromStdString(estado)); // Mostramos el nombre del cliente
            }

            asientoItem->setTextAlignment(Qt::AlignCenter);
            ui->tablaAsientos->setItem(f - 1, c - 1, asientoItem);
        }
    }
    ui->tablaAsientos->resizeColumnsToContents();
}



// Función auxiliar para cargar la imagen (puedes crearla o poner el código en cada botón)
void cargarImagenEnVisor(QLabel* label, QString rutaImagen) {
    if (QFile::exists(rutaImagen)) {
        QPixmap pixmap(rutaImagen);
        label->setPixmap(pixmap);
    } else {
        label->setText("Error: La imagen no existe. Genera el reporte primero.");
    }
}

// --- VISORES DE REPORTES GRAPHVIZ ---

void MainWindow::on_btnVerRepCartelera_clicked()
{
    // Ejecuta la generación por si hubo cambios recientes
    cartelera.generarReporteGraphviz();
    cargarImagenEnVisor(ui->lblVisorReportes, "reporte_cartelera.png");
}

void MainWindow::on_btnVerRepSolicitudes_clicked()
{
    solicitudes.generarReporteGraphviz();
    cargarImagenEnVisor(ui->lblVisorReportes, "reporte_solicitudes.png");
}

void MainWindow::on_btnVerRepSala_clicked()
{
    sala.generarReporteGraphviz();
    cargarImagenEnVisor(ui->lblVisorReportes, "reporte_matriz.png");
}

void MainWindow::on_btnVerRepPromos_clicked()
{
    promociones.generarReporteGraphviz();
    cargarImagenEnVisor(ui->lblVisorReportes, "reporte_promociones.png");
}


void MainWindow::on_btnConsultarEstado_clicked()
{
    std::string telefono = ui->txtConsultaTelefono->text().toStdString();

    if (telefono.empty()) {
        QMessageBox::warning(this, "Aviso", "Ingresa tu número de teléfono.");
        return;
    }

    std::string resultado = solicitudes.consultarEstado(telefono);
    QMessageBox::information(this, "Estado de Solicitud", QString::fromStdString(resultado));
}

// --- ADMIN: ELIMINAR FUNCIÓN ---
void MainWindow::on_btnEliminarFuncion_clicked()
{
    sala = MatrizDispersa(); // Reiniciamos la matriz a su estado vacío

    // Si tienes la tabla visible, la limpiamos
    ui->tablaAsientos->clear();
    ui->tablaAsientos->setRowCount(0);
    ui->tablaAsientos->setColumnCount(0);

    QMessageBox::information(this, "Función Eliminada", "La función actual y su mapa de asientos han sido borrados del sistema.");
}

// --- CLIENTE: BUSCAR PELÍCULA ---
void MainWindow::on_btnBuscarPelicula_clicked()
{
    std::string codigo = ui->txtBuscarCodigoPelicula->text().toStdString();

    if (codigo.empty()) return;

    Pelicula* p = cartelera.buscarPelicula(codigo);

    if (p != nullptr) {
        QString info = QString("Título: %1\nGénero: %2\nDuración: %3 min\nClasificación: %4\nIdioma: %5")
                           .arg(QString::fromStdString(p->titulo))
                           .arg(QString::fromStdString(p->genero))
                           .arg(p->duracion)
                           .arg(QString::fromStdString(p->clasificacion))
                           .arg(QString::fromStdString(p->idioma));

        QMessageBox::information(this, "Película Encontrada", info);
    } else {
        QMessageBox::warning(this, "No Encontrada", "No existe ninguna película con ese código en cartelera.");
    }
}


// --- CLIENTE: VER PROMOCIONES ---
void MainWindow::on_btnVerPromociones_clicked()
{
    std::string textoPromos = promociones.obtenerTextoPromociones();
    QMessageBox::information(this, "Promociones Activas", QString::fromStdString(textoPromos));
}

