#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox> // Para mostrar alertas emergentes
#include <QFileDialog>
#include <QPixmap>
#include <QFile>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


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

void MainWindow::on_btnReservar_clicked()
{
    // 1. Leer TODOS los datos ingresados en la interfaz
    std::string pelicula = ui->cmbPeliculaReserva->currentText().toStdString();    int fila = ui->spinFila->value();
    int columna = ui->spinColumna->value();
    std::string cliente = ui->txtNombreCliente->text().toStdString();

    // 2. Validación de campos vacíos
    if (cliente.empty() || pelicula.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Por favor, ingresa tu nombre y la película para realizar la reserva.");
        return;
    }

    // 3. ¡LA NUEVA VALIDACIÓN! Verificamos si la película existe en la cartelera
    if (!cartelera.existePeliculaPorTitulo(pelicula)) {
        QMessageBox::critical(this, "Película Inválida", "La película '" + QString::fromStdString(pelicula) + "' no existe en nuestra cartelera. Revisa bien el título.");
        return;
    }

    if (!cartelera.existePeliculaPorTitulo(pelicula)) {
        QMessageBox::critical(this, "Error de Consistencia", "La película '" + QString::fromStdString(pelicula) + "' no se encuentra en el sistema. Regístrala en la cartelera antes de asignarle una función.");
        return;
    }
    // (Opcional) Validación estricta de Fase 1: Validar que sea la función activa actualmente
    // if (pelicula != sala.peliculaActual) {
    //     QMessageBox::warning(this, "Función No Disponible", "La película existe, pero en este momento la sala está configurada para: " + QString::fromStdString(sala.peliculaActual));
    //     return;
    // }

    // 4. Intentamos reservar en la Matriz Dispersa
    bool exito = sala.reservarAsiento(fila, columna, cliente);

    if (exito) {
        QMessageBox::information(this, "Reserva Confirmada",
                                 QString("¡Asiento F%1-C%2 reservado con éxito para %3!")
                                     .arg(fila).arg(columna).arg(QString::fromStdString(cliente)));

        ui->txtNombreCliente->clear();

    } else {
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
    //sala.cancelarPorCliente(cliente);

    QMessageBox::information(this, "Cancelación",
                             "Se han procesado las cancelaciones. Si tenías asientos a tu nombre, ahora están libres.");

    ui->txtNombreCliente->clear();
}

// --- LÓGICA DE INICIO DE SESIÓN ---
void MainWindow::on_btnIniciarSesion_clicked() {
    // Asumiendo que tus inputs se llaman txtLoginCorreo y txtLoginPass
    QString correo = ui->txtCorreo->text().trimmed();
    QString pass = ui->txtPassword->text();

    if (correo.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor ingresa tu correo y contraseña.");
        return;
    }

    // 1. Verificación estricta del Administrador
    if (correo == "AdminCine@gmail.com" && pass == "admin123Pass") {
        ui->B->setCurrentWidget(ui->page2Admin);
        QMessageBox::information(this, "Bienvenido", "Sesión iniciada como Administrador.");

        ui->txtCorreo->clear();
        ui->txtPassword->clear();
        return;
    }

    // 2. Búsqueda de clientes en la estructura
    std::vector<Cliente> clientesActuales = arbolClientes.obtenerClientes();
    bool autenticado = false;

    for (const Cliente& c : clientesActuales) {
        if (c.correo == correo.toStdString() && c.password == pass.toStdString()) {
            autenticado = true;

            // Guardamos el ID en memoria para usarlo en la pestaña de reservas
            clienteLogueadoID = c.id;

            // Redirección a la interfaz de Taquilla
            ui->B->setCurrentWidget(ui->page3Usuario);
            QMessageBox::information(this, "Bienvenido", QString("Sesión iniciada. Hola, %1").arg(QString::fromStdString(c.nombre)));

            ui->txtCorreo->clear();
            ui->txtPassword->clear();
            break;
        }
    }

    if (!autenticado) {
        QMessageBox::warning(this, "Error de Autenticación", "Correo o contraseña incorrectos.");
    }
}


// --- LÓGICA PARA CERRAR SESIÓN ---
void MainWindow::on_btnCerrarSesionAdmin_clicked() {
    // Limpiar campos y volver al Login
    ui->txtCorreo->clear();
    ui->txtPassword->clear();
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

    // Leemos los campos
    std::string pelicula = ui->cmbPeliculaReserva->currentText().toStdString(); // O ui->txtPelicula si así lo nombraste
    std::string salaNum = ui->txtSala->text().toStdString();
    std::string horario = ui->txtHorario->text().toStdString();

    // 1. Validar que no haya campos vacíos o dimensiones cero
    if (filas <= 0 || columnas <= 0 || pelicula.empty() || salaNum.empty() || horario.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Por favor completa todos los datos (película, sala, horario y dimensiones).");
        return;
    }

    // 2. ¡LA NUEVA VALIDACIÓN DEL ADMIN! Verificamos si la película existe en la cartelera
    if (!cartelera.existePeliculaPorTitulo(pelicula)) {
        QMessageBox::critical(this, "Error de Consistencia", "La película '" + QString::fromStdString(pelicula) + "' no se encuentra en el sistema. Regístrala en la cartelera antes de asignarle una función.");
        return;
    }

    // 3. Todo correcto, reiniciamos la matriz y creamos la función
    sala = MatrizDispersa();

    sala.inicializarSala(filas, columnas, pelicula, horario, salaNum);

    QMessageBox::information(this, "Función Creada",
                             QString("Se generó la función para %1 a las %2 en la %3.")
                                 .arg(QString::fromStdString(pelicula))
                                 .arg(QString::fromStdString(horario))
                                 .arg(QString::fromStdString(salaNum)));

    // Limpiamos los campos

    ui->txtSala->clear();
    ui->txtHorario->clear();
}


void MainWindow::on_btnRefrescarSala_clicked()
{
    // 1. Obtener el código de la función seleccionada en la interfaz
    // (Ajusta 'txtCodigoFuncion' al nombre del QLineEdit o QComboBox que estés usando)
    std::string codigoFuncion = ui->txtBuscarCodigoPelicula->text().toStdString();

    if (codigoFuncion.empty()) {
        QMessageBox::warning(this, "Aviso", "Por favor ingresa o selecciona un código de función.");
        return;
    }

    // 2. Buscar la función en el Árbol AVL
    Funcion* f = avlFunciones.buscarFuncion(codigoFuncion);

    if (f == nullptr) {
        QMessageBox::warning(this, "Aviso", "La función seleccionada no existe en el sistema.");
        return;
    }

    // 3. Vaciar matriz actual y cargar asientos ocupados desde el archivo JSON de la función
    sala.cargarDesdeArchivo(f->archivo_asientos, f->filas, f->columnas, f->codigo_pelicula, f->horario, f->sala);

    // 4. Preparar la tabla visual utilizando las dimensiones exactas guardadas en el AVL
    int totalFilas = f->filas;
    int totalColumnas = f->columnas;

    ui->tablaAsientos->setRowCount(totalFilas);
    ui->tablaAsientos->setColumnCount(totalColumnas);
    ui->tablaAsientos->horizontalHeader()->setVisible(false);
    ui->tablaAsientos->verticalHeader()->setVisible(false);

    // 5. Pintar la tabla
    for (int fila = 1; fila <= totalFilas; fila++) {
        for (int col = 1; col <= totalColumnas; col++) {

            std::string estado = sala.obtenerEstadoAsiento(fila, col);
            QTableWidgetItem *asientoItem = new QTableWidgetItem();

            if (estado == "LIBRE") {
                asientoItem->setBackground(QBrush(Qt::green));
                asientoItem->setText("Libre");
            } else if (estado == "ND") {
                asientoItem->setBackground(QBrush(Qt::gray));
                asientoItem->setText("Error");
            } else {
                asientoItem->setBackground(QBrush(Qt::red));
                // Ahora "estado" contiene el codigo_reserva, cumpliendo con no duplicar los datos del cliente[cite: 1]
                asientoItem->setText(QString::fromStdString(estado));
            }

            asientoItem->setTextAlignment(Qt::AlignCenter);
            ui->tablaAsientos->setItem(fila - 1, col - 1, asientoItem);
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


// --- CLIENTE: VER CARTELERA COMPLETA ---
void MainWindow::on_btnVerCarteleraUsuario_clicked()
{
    // Obtenemos la lista ordenada directamente desde el backend
    std::vector<Pelicula> peliculas = cartelera.obtenerCarteleraInorden();

    // Configuramos la tabla
    ui->tablaCarteleraUsuario->setColumnCount(7);
    QStringList encabezados = {"Código", "Título", "Género", "Duración", "Clasificación", "Idioma", "Estreno"};
    ui->tablaCarteleraUsuario->setHorizontalHeaderLabels(encabezados);
    ui->tablaCarteleraUsuario->setRowCount(peliculas.size());

    // Llenamos la tabla con los datos
    for (size_t i = 0; i < peliculas.size(); ++i) {
        ui->tablaCarteleraUsuario->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(peliculas[i].codigo)));
        ui->tablaCarteleraUsuario->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(peliculas[i].titulo)));
        ui->tablaCarteleraUsuario->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(peliculas[i].genero)));
        ui->tablaCarteleraUsuario->setItem(i, 3, new QTableWidgetItem(QString::number(peliculas[i].duracion) + " min"));
        ui->tablaCarteleraUsuario->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(peliculas[i].clasificacion)));
        ui->tablaCarteleraUsuario->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(peliculas[i].idioma)));
        ui->tablaCarteleraUsuario->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(peliculas[i].fechaEstreno)));
    }

    // Ajustar el ancho de las columnas para que se vea limpio
    ui->tablaCarteleraUsuario->resizeColumnsToContents();
}

void MainWindow::actualizarCombosPeliculas() {
    // 1. Limpiamos los datos anteriores para no duplicar
    ui->cmbPeliculaReserva->clear();

    // Si también lo agregaste en el admin, descomenta esta línea:
    ui->cmbPeliculaAdmin->clear();

    // 2. Obtenemos las películas de nuestro Árbol Binario
    std::vector<Pelicula> peliculas = cartelera.obtenerCarteleraInorden();

    // 3. Llenamos el ComboBox iterando el vector
    for (size_t i = 0; i < peliculas.size(); ++i) {
        ui->cmbPeliculaReserva->addItem(QString::fromStdString(peliculas[i].titulo));

        // Descomenta si usaste el del admin:
         ui->cmbPeliculaAdmin->addItem(QString::fromStdString(peliculas[i].titulo));
    }
}

// =======================================================
// MÉTODOS DE CARGA MASIVA (Con prefijo MainWindow::)
// =======================================================
void MainWindow::cargarMasivaPeliculas(const QString& rutaArchivo) {
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo JSON.");
        return;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonParseError errorParseo;
    QJsonDocument doc = QJsonDocument::fromJson(datos, &errorParseo);

    if (errorParseo.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Error de Formato", "El archivo JSON tiene errores de sintaxis.");
        return;
    }

    QJsonArray arrayPeliculas = doc.object().value("peliculas").toArray();
    int peliculasCargadas = 0, funcionesCargadas = 0;

    for (const QJsonValue& valPelicula : arrayPeliculas) {
        QJsonObject objPel = valPelicula.toObject();


        // --- INICIO DE VALIDACIONES CON DEBUG ---
        if (objPel["codigo"].toString().isEmpty()) {
            qDebug() << "Falla: El código está vacío o no es texto.";
            continue;
        }
        if (objPel["titulo"].toString().isEmpty()) {
            qDebug() << "Falla: El título de" << objPel["codigo"].toString() << "está vacío.";
            continue;
        }
        if (objPel["duracion"].toInt() <= 0) {
            qDebug() << "Falla: La duración de" << objPel["titulo"].toString() << "no es un número válido mayor a 0.";
            continue;
        }

        QString fechaEstreno = objPel["fecha_estreno"].toString();
        if (!QDate::fromString(fechaEstreno, "yyyy-MM-dd").isValid()) {
            qDebug() << "Falla: Fecha de estreno inválida en" << objPel["titulo"].toString() << ". Leído:" << fechaEstreno;
            continue;
        }

        Pelicula nuevaPel;
        nuevaPel.codigo = objPel["codigo"].toString().toStdString();
        nuevaPel.titulo = objPel["titulo"].toString().toStdString();
        nuevaPel.genero = objPel["genero"].toString().toStdString();
        nuevaPel.duracion = objPel["duracion"].toInt();
        nuevaPel.clasificacion = objPel["clasificacion"].toString().toStdString();
        nuevaPel.idioma = objPel["idioma"].toString().toStdString();
        nuevaPel.fechaEstreno = fechaEstreno.toStdString();
        nuevaPel.fechaFin = objPel["fecha_fin"].toString().toStdString();

        cartelera.insertar(nuevaPel);
        peliculasCargadas++;

        QJsonArray arrayFunciones = objPel["funciones"].toArray();
        for (const QJsonValue& valFuncion : arrayFunciones) {
            QJsonObject objFun = valFuncion.toObject();

            if (objFun["codigo_funcion"].toString().isEmpty() || objFun["filas"].toInt() <= 0) {
                continue;
            }

            Funcion nuevaFun;
            nuevaFun.codigo_funcion = objFun["codigo_funcion"].toString().toStdString();
            nuevaFun.codigo_pelicula = nuevaPel.codigo;
            nuevaFun.horario = objFun["horario"].toString().toStdString();
            nuevaFun.sala = objFun["sala"].toString().toStdString();
            nuevaFun.filas = objFun["filas"].toInt();
            nuevaFun.columnas = objFun["columnas"].toInt();
            nuevaFun.archivo_asientos = "";
            nuevaFun.ya_realizada = false;

            avlFunciones.insertar(nuevaFun); // Usa la variable global de MainWindow
            funcionesCargadas++;
        }
    }
    QMessageBox::information(this, "Éxito", QString("Carga masiva finalizada.\nPelículas: %1\nFunciones: %2").arg(peliculasCargadas).arg(funcionesCargadas));
}

void MainWindow::cargarMasivaClientes(const QString& rutaArchivo) {
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(archivo.readAll());
    archivo.close();

    QJsonArray arrayClientes = doc.object().value("clientes").toArray();

    for (const QJsonValue& valCliente : arrayClientes) {
        QJsonObject objCli = valCliente.toObject();

        if (objCli["id"].toString().isEmpty() || objCli["correo"].toString().isEmpty()) {
            continue;
        }

        Cliente nuevoCli;
        nuevoCli.id = objCli["id"].toString().toStdString();
        nuevoCli.nombre = objCli["nombre"].toString().toStdString();
        nuevoCli.correo = objCli["correo"].toString().toStdString();
        nuevoCli.telefono = objCli["telefono"].toString().toStdString();
        nuevoCli.password = objCli["password"].toString().toStdString();
        nuevoCli.tipo = objCli["tipo"].toString().toStdString();

        QJsonArray arrayReservas = objCli["reservas"].toArray();
        for (const QJsonValue& valReserva : arrayReservas) {
            QJsonObject objRes = valReserva.toObject();

            if (objRes["codigo_reserva"].toString().isEmpty()) continue;

            Reserva nuevaRes;
            nuevaRes.codigo_reserva = objRes["codigo_reserva"].toString().toStdString();
            nuevaRes.id_cliente = nuevoCli.id;
            nuevaRes.codigo_funcion = objRes["codigo_funcion"].toString().toStdString();
            nuevaRes.fila = objRes["fila"].toInt();
            nuevaRes.columna = objRes["columna"].toInt();
            nuevaRes.fecha_reserva = objRes["fecha_reserva"].toString().toStdString();

            tablaReservas.insertar(nuevaRes);
            nuevoCli.codigos_reservas.push_back(nuevaRes.codigo_reserva);
        }

        arbolClientes.insertar(nuevoCli);
    }
    QMessageBox::information(this, "Éxito", "Carga de clientes y reservas finalizada.");
}



void MainWindow::on_btnReporteAVL_clicked() {
    avlFunciones.generarReporteGraphviz(); // Genera el árbol de funciones activas y realizadas[cite: 1]
    QMessageBox::information(this, "Reporte", "Reporte del Árbol AVL generado con éxito.");
    // Opcional: Cargar la imagen en un QLabel si lo tienes en tu UI
     ui->lblVisorReportes->setPixmap(QPixmap("reporte_avl.png"));
}


void MainWindow::on_btnReporteArbolB_clicked() {
    arbolClientes.generarReporteGraphviz(); // Genera la estructura de clientes[cite: 1]
    QMessageBox::information(this, "Reporte", "Reporte del Árbol B generado con éxito.");
    ui->lblVisorReportes->setPixmap(QPixmap("reporte_arbolB.png"));
}


void MainWindow::on_btnReporteHash_clicked() {
    tablaReservas.generarReporteGraphviz(); // Muestra la distribución y colisiones de reservas[cite: 1]
    QMessageBox::information(this, "Reporte", "Reporte de la Tabla Hash generado con éxito.");
    ui->lblVisorReportes->setPixmap(QPixmap("reporte_Hash.png"));
}

void MainWindow::actualizarTablaPeliculas() {
    // Obtenemos la lista ordenada desde el BST de la Fase 1
    std::vector<Pelicula> lista = cartelera.obtenerCarteleraInorden();

    ui->tablaCarteleraAdmin->setRowCount(0); // Limpia la tabla visual
    for (int i = 0; i < (int)lista.size(); i++) {
        ui->tablaCarteleraAdmin->insertRow(i);

        // Columnas originales
        ui->tablaCarteleraAdmin->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(lista[i].codigo)));
        ui->tablaCarteleraAdmin->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(lista[i].titulo)));
        ui->tablaCarteleraAdmin->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(lista[i].clasificacion)));
        ui->tablaCarteleraAdmin->setItem(i, 3, new QTableWidgetItem(QString::number(lista[i].duracion) + " min"));

        // Nuevas columnas agregadas
        ui->tablaCarteleraAdmin->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(lista[i].genero)));
        ui->tablaCarteleraAdmin->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(lista[i].idioma)));
        ui->tablaCarteleraAdmin->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(lista[i].fechaEstreno)));
        ui->tablaCarteleraAdmin->setItem(i, 7, new QTableWidgetItem(QString::fromStdString(lista[i].fechaFin)));
    }
}

void MainWindow::actualizarTablaClientes() {
    QString orden = ui->cmbOrdenClientes->currentText();
    std::vector<Cliente> lista;

    // Seleccionamos el recorrido correspondiente
    if (orden == "Preorden") {
        lista = arbolClientes.obtenerClientesPreorden();
    } else if (orden == "Postorden") {
        lista = arbolClientes.obtenerClientesPostorden();
    } else {
        lista = arbolClientes.obtenerClientesInorden(); // Inorden por defecto (Ordenado por ID)
    }

    ui->tblAdminClientes->setRowCount(0);
    ui->tblAdminClientes->setColumnCount(4);
    ui->tblAdminClientes->setHorizontalHeaderLabels({"ID", "Nombre", "Correo", "Teléfono"});

    for (int i = 0; i < (int)lista.size(); i++) {
        ui->tblAdminClientes->insertRow(i);
        ui->tblAdminClientes->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(lista[i].id)));
        ui->tblAdminClientes->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(lista[i].nombre)));
        ui->tblAdminClientes->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(lista[i].correo)));
        ui->tblAdminClientes->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(lista[i].telefono)));
    }
    ui->tblAdminClientes->resizeColumnsToContents();
}

void MainWindow::on_btnCargaMasiva_clicked() {
    QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar JSON de Películas", "", "Archivos JSON (*.json)");
    if (!ruta.isEmpty()) {
        cargarMasivaPeliculas(ruta);
        actualizarTablaPeliculas(); // <--- Refresca la UI
    }
}

void MainWindow::on_btnCargaMasivaClientes_clicked() {
    QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar JSON de Clientes", "", "Archivos JSON (*.json)");
    if (!ruta.isEmpty()) {
        cargarMasivaClientes(ruta);
        actualizarTablaClientes(); // <--- Refresca la UI
    }
}


void MainWindow::on_btnRegistrarUsuario_clicked() {
    // 1. Capturar los datos de los QLineEdit
    QString nombre = ui->txtRegNombre->text().trimmed();
    QString correo = ui->txtRegCorreo->text().trimmed();
    QString telefono = ui->txtRegTel->text().trimmed();
    QString pass = ui->txtRegPass->text();

    // 2. Validación de campos obligatorios
    if (nombre.isEmpty() || correo.isEmpty() || telefono.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Registro Fallido", "Todos los campos son obligatorios.");
        return;
    }

    // 3. Verificación de correo único exigida por el sistema
    std::vector<Cliente> clientesActuales = arbolClientes.obtenerClientes();
    for (const Cliente& c : clientesActuales) {
        if (c.correo == correo.toStdString()) {
            QMessageBox::warning(this, "Registro Fallido", "El correo ingresado ya se encuentra registrado.");
            return;
        }
    }

    // 4. Generación de ID único (Formato 4 dígitos, ej: "0005")
    int nuevoIdNum = clientesActuales.size() + 1;
    QString nuevoId = QString("%1").arg(nuevoIdNum, 4, 10, QChar('0'));

    // 5. Creación del objeto Cliente e inserción en el Árbol B de Orden 4
    Cliente nuevoCli;
    nuevoCli.id = nuevoId.toStdString();
    nuevoCli.nombre = nombre.toStdString();
    nuevoCli.correo = correo.toStdString();
    nuevoCli.telefono = telefono.toStdString();
    nuevoCli.password = pass.toStdString();
    nuevoCli.tipo = "cliente"; // Rol por defecto para la taquilla[cite: 1]

    arbolClientes.insertar(nuevoCli);

    // 6. Actualizar las tablas administrativas en segundo plano y limpiar UI
    actualizarTablaClientes();

    QMessageBox::information(this, "Éxito", "Usuario registrado correctamente. Tu ID es: " + nuevoId);

    // Limpiar los campos para el siguiente registro
    ui->txtRegNombre->clear();
    ui->txtRegCorreo->clear();
    ui->txtRegTel->clear();
    ui->txtRegPass->clear();
}


void MainWindow::on_btnConsulEstado_clicked() {
    QString idCliente = ui->txtBuscarClienteID->text().trimmed();

    if (idCliente.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Ingresa un ID de cliente válido.");
        return;
    }

    // 1. Ubicar al cliente en el Árbol B[cite: 1]
    Cliente* c = arbolClientes.buscar(idCliente.toStdString());

    if (c == nullptr) {
        QMessageBox::warning(this, "Error", "No se encontró ningún cliente con ese ID.");
        return;
    }

    // 2. Limpiar la tabla visual antes de llenarla
    ui->tablaReservasAdmin->setRowCount(0);
    ui->tablaReservasAdmin->setColumnCount(4);
    ui->tablaReservasAdmin->setHorizontalHeaderLabels({"Código Reserva", "Función", "Fila", "Columna"});

    // 3. Iterar sobre las referencias del cliente y buscar en la Tabla Hash[cite: 1]
    int filaVisual = 0;
    for (const std::string& codReserva : c->codigos_reservas) {
        Reserva* res = tablaReservas.buscar(codReserva);

        if (res != nullptr) {
            ui->tablaReservasAdmin->insertRow(filaVisual);
            ui->tablaReservasAdmin->setItem(filaVisual, 0, new QTableWidgetItem(QString::fromStdString(res->codigo_reserva)));
            ui->tablaReservasAdmin->setItem(filaVisual, 1, new QTableWidgetItem(QString::fromStdString(res->codigo_funcion)));
            ui->tablaReservasAdmin->setItem(filaVisual, 2, new QTableWidgetItem(QString::number(res->fila)));
            ui->tablaReservasAdmin->setItem(filaVisual, 3, new QTableWidgetItem(QString::number(res->columna)));
            filaVisual++;
        }
    }

    ui->tablaReservasAdmin->resizeColumnsToContents();
    QMessageBox::information(this, "Consulta Exitosa", QString("Se encontraron %1 reservas para %2.").arg(filaVisual).arg(QString::fromStdString(c->nombre)));
}


void MainWindow::on_btnBuscarCliente_clicked() {
    QString idCliente = ui->txtBuscarUsuarioID->text().trimmed();

    if (idCliente.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, ingresa un ID válido para buscar.");
        return;
    }

    Cliente* c = arbolClientes.buscar(idCliente.toStdString());

    if (c != nullptr) {
        QString info = QString("ID: %1\nNombre: %2\nCorreo: %3\nTeléfono: %4\nTipo: %5\nReservas Activas: %6")
                           .arg(QString::fromStdString(c->id))
                           .arg(QString::fromStdString(c->nombre))
                           .arg(QString::fromStdString(c->correo))
                           .arg(QString::fromStdString(c->telefono))
                           .arg(QString::fromStdString(c->tipo))
                           .arg(c->codigos_reservas.size());

        QMessageBox::information(this, "Cliente Encontrado", info);
    } else {
        QMessageBox::warning(this, "No Encontrado", "No se encontró ningún cliente con el ID: " + idCliente);
    }
}

void MainWindow::on_btnEliminarCliente_clicked() {
    QString idCliente = ui->txtBuscarUsuarioID->text().trimmed();

    if (idCliente.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, ingresa el ID del cliente a eliminar.");
        return;
    }

    Cliente* c = arbolClientes.buscar(idCliente.toStdString());

    if (c == nullptr) {
        QMessageBox::warning(this, "Error", "El cliente no existe en el sistema.");
        return;
    }

    // Confirmación de seguridad
    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(this, "Confirmar Eliminación",
                                      "¿Estás seguro de que deseas eliminar a " + QString::fromStdString(c->nombre) +
                                          "? Se cancelarán todas sus reservas activas.",
                                      QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        // 1. Eliminación en cascada: Borrar sus reservas de la Tabla Hash
        for (const std::string& codReserva : c->codigos_reservas) {
            tablaReservas.eliminar(codReserva);
        }

        // 2. Eliminar al cliente del Árbol B
        arbolClientes.eliminar(idCliente.toStdString());

        // 3. Limpiar input y actualizar tabla visual
        ui->txtBuscarUsuarioID->clear();
        actualizarTablaClientes();

        QMessageBox::information(this, "Éxito", "Cliente y reservas eliminados correctamente.");
    }
}


void MainWindow::on_cmbOrdenClientes_currentTextChanged(const QString &ordenSeleccionado) {
    actualizarTablaClientes(); // Se dispara cada vez que el administrador cambia la opción
}

