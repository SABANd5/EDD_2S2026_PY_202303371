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
#include <QDateTime>
#include <QColor>
#include <QTableWidgetItem>

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

void MainWindow::on_btnReservar_clicked() {
    // 1. Validar que el usuario seleccionó una función válida
    QString codigoFuncion = ui->cbmFuncionPeli->currentData().toString();

    if (codigoFuncion.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, selecciona una función válida antes de reservar.");
        return;
    }

    // 2. Leer fila y columna de los SpinBoxes
    int fila = ui->spinFila->value();
    int columna = ui->spinColumna->value();

    // 3. Validar dimensiones de la sala usando el Árbol AVL
    Funcion* f = avlFunciones.buscarFuncion(codigoFuncion.toStdString());
    if (f == nullptr) {
        QMessageBox::critical(this, "Error", "La función seleccionada ya no existe en el sistema.");
        return;
    }

    if (fila <= 0 || fila > f->filas || columna <= 0 || columna > f->columnas) {
        QMessageBox::warning(this, "Asiento Inválido",
                             QString("Las dimensiones de la sala son: %1 filas x %2 columnas. Por favor elige un asiento dentro del rango.")
                                 .arg(f->filas).arg(f->columnas));
        return;
    }

    // 4. Validar que tengamos un usuario logueado en memoria (Árbol B)
    if (clienteLogueadoID.empty()) {
        QMessageBox::critical(this, "Error de Sesión", "No hay ningún cliente logueado.");
        return;
    }

    Cliente* clienteActual = arbolClientes.buscar(clienteLogueadoID);
    if (clienteActual == nullptr) return;

    // 5. Leer el archivo JSON de la función para verificar disponibilidad
    QString rutaArchivo = QString::fromStdString(f->archivo_asientos);
    QFile archivoIn(rutaArchivo);
    QJsonDocument doc;
    QJsonObject raizJSON;
    QJsonArray asientosOcupados;

    if (archivoIn.open(QIODevice::ReadOnly)) {
        doc = QJsonDocument::fromJson(archivoIn.readAll());
        raizJSON = doc.object();
        asientosOcupados = raizJSON["asientos_ocupados"].toArray();
        archivoIn.close();
    } else {
        QMessageBox::critical(this, "Error de Archivo", "No se pudo leer el archivo de la sala: " + rutaArchivo);
        return;
    }

    // 6. Verificar si el asiento ya está ocupado en el JSON
    for (const QJsonValue& valor : asientosOcupados) {
        QJsonObject asientoObj = valor.toObject();
        if (asientoObj["fila"].toInt() == fila && asientoObj["columna"].toInt() == columna) {
            QMessageBox::warning(this, "Asiento Ocupado",
                                 QString("El asiento en la fila %1, columna %2 ya se encuentra reservado.").arg(fila).arg(columna));
            return;
        }
    }

    // 7. Generar código único para la reserva (Ej: "R-0005-F001-F3C5")
    QString codReserva = QString("R-%1-%2-F%3C%4")
                             .arg(QString::fromStdString(clienteLogueadoID))
                             .arg(codigoFuncion)
                             .arg(fila).arg(columna);

    QString fechaActual = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    // 8. Crear la reserva e insertarla en la TABLA HASH
    Reserva nuevaReserva;
    nuevaReserva.codigo_reserva = codReserva.toStdString();
    nuevaReserva.codigo_funcion = codigoFuncion.toStdString();
    nuevaReserva.id_cliente = clienteLogueadoID;
    nuevaReserva.fila = fila;
    nuevaReserva.columna = columna;
    nuevaReserva.fecha_reserva = fechaActual.toStdString();

    tablaReservas.insertar(nuevaReserva);

    // 9. Vincular la reserva con el cliente en el ÁRBOL B
    clienteActual->codigos_reservas.push_back(nuevaReserva.codigo_reserva);

    // 10. Actualizar el archivo JSON de la función para bloquear el asiento
    QJsonObject nuevoAsiento;
    nuevoAsiento["fila"] = fila;
    nuevoAsiento["columna"] = columna;
    nuevoAsiento["id_cliente"] = QString::fromStdString(clienteLogueadoID);

    asientosOcupados.append(nuevoAsiento);
    raizJSON["asientos_ocupados"] = asientosOcupados;

    QFile archivoOut(rutaArchivo);
    if (archivoOut.open(QIODevice::WriteOnly)) {
        QJsonDocument docOut(raizJSON);
        archivoOut.write(docOut.toJson());
        archivoOut.close();
    }

    // 11. Mensaje de éxito y limpieza de SpinBoxes
    QMessageBox::information(this, "Reserva Exitosa",
                             QString("¡Tu reserva se ha procesado con éxito!\n\nCódigo de Reserva: %1\nAsiento: Fila %2, Columna %3")
                                 .arg(codReserva).arg(fila).arg(columna));

    ui->spinFila->setValue(0);
    ui->spinColumna->setValue(0);
}

// --- 2. CANCELAR RESERVA ---
void MainWindow::on_btnCancelarReserva_clicked() {
    QString codCancelar = ui->txtIdCancelar->text().trimmed();

    if (codCancelar.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, ingresa el código de la reserva a cancelar.");
        return;
    }

    if (clienteLogueadoID.empty()) return;

    // 1. Buscar en la Tabla Hash
    Reserva* res = tablaReservas.buscar(codCancelar.toStdString());

    if (res == nullptr) {
        QMessageBox::warning(this, "Error", "La reserva no existe.");
        return;
    }

    // 2. Validar que pertenezca al usuario actual
    if (res->id_cliente != clienteLogueadoID) {
        QMessageBox::critical(this, "Acceso Denegado", "Esta reserva pertenece a otro usuario.");
        return;
    }

    // 3. Validar que la reserva no esté vencida
    QDate fechaReserva = QDate::fromString(QString::fromStdString(res->fecha_reserva), "yyyy-MM-dd");
    if (fechaReserva < QDate::currentDate()) {
        QMessageBox::warning(this, "Reserva Vencida", "No puedes cancelar una reserva de una función que ya pasó.");
        return;
    }

    // Confirmación del usuario
    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(this, "Confirmar Cancelación",
                                      "¿Estás seguro de cancelar la reserva " + codCancelar + "?\nSe liberará tu asiento.",
                                      QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        // ==========================================
        // PASO A: Liberar el asiento del archivo JSON
        // ==========================================
        Funcion* f = avlFunciones.buscarFuncion(res->codigo_funcion);
        if (f != nullptr) {
            QString rutaArchivo = QString::fromStdString(f->archivo_asientos);
            QFile archivoIn(rutaArchivo);

            if (archivoIn.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(archivoIn.readAll());
                QJsonObject raizJSON = doc.object();
                archivoIn.close();

                QJsonArray asientosOcupados = raizJSON["asientos_ocupados"].toArray();
                QJsonArray asientosActualizados;

                // Copiamos todos los asientos EXCEPTO el que estamos cancelando
                for (const QJsonValue& valor : asientosOcupados) {
                    QJsonObject asientoObj = valor.toObject();
                    if (asientoObj["fila"].toInt() == res->fila && asientoObj["columna"].toInt() == res->columna) {
                        continue; // Lo saltamos (así se elimina)
                    }
                    asientosActualizados.append(asientoObj);
                }

                // Guardar los cambios físicos
                raizJSON["asientos_ocupados"] = asientosActualizados;
                QFile archivoOut(rutaArchivo);
                if (archivoOut.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                    archivoOut.write(QJsonDocument(raizJSON).toJson());
                    archivoOut.close();
                }
            }
        }

        // ==========================================
        // PASO B: Eliminar referencia en el Árbol B
        // ==========================================
        Cliente* c = arbolClientes.buscar(clienteLogueadoID);
        if (c != nullptr) {
            auto it = std::find(c->codigos_reservas.begin(), c->codigos_reservas.end(), codCancelar.toStdString());
            if (it != c->codigos_reservas.end()) {
                c->codigos_reservas.erase(it);
            }
        }

        // ==========================================
        // PASO C: Eliminar de la Tabla Hash
        // ==========================================
        tablaReservas.eliminar(codCancelar.toStdString());

        // Limpiar interfaz y actualizar la tabla visual
        ui->txtIdCancelar->clear();
        on_btnActualizarReservaUsuario_clicked();

        QMessageBox::information(this, "Éxito", "Reserva cancelada y asiento liberado correctamente.");
    }
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
            actualizarComboPeliculasUsuario();
            on_btnActualizarReservaUsuario_clicked();
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
    // 1. Leer los campos (Asegúrate de agregar txtCodigoFuncionAdmin en Qt Designer)
    std::string codigoFuncion = ui->txtCodigoFuncionAdmin->text().trimmed().toStdString();
    std::string peliculaTitulo = ui->cmbPeliculaAdmin->currentText().toStdString();
    std::string salaNum = ui->txtSala->text().trimmed().toStdString();
    std::string horario = ui->txtHorario->text().trimmed().toStdString();
    int filas = ui->spinFilasSala->value();
    int columnas = ui->spinColumnasSala->value();

    if (codigoFuncion.empty() || peliculaTitulo.empty() || salaNum.empty() || horario.empty() || filas <= 0 || columnas <= 0) {
        QMessageBox::warning(this, "Datos incompletos", "Por favor completa todos los datos, incluyendo el Código de Función.");
        return;
    }

    // 2. Buscar el código real de la película en el BST usando su título
    std::string codigoPelicula = "";
    std::vector<Pelicula> listaPelis = cartelera.obtenerCarteleraInorden();
    for (const Pelicula& p : listaPelis) {
        if (p.titulo == peliculaTitulo) {
            codigoPelicula = p.codigo;
            break;
        }
    }

    if (codigoPelicula.empty()) {
        QMessageBox::critical(this, "Error", "La película seleccionada no se encuentra en el sistema.");
        return;
    }

    // 3. Validar que el código de función no exista ya en el AVL
    if (avlFunciones.buscarFuncion(codigoFuncion) != nullptr) {
        QMessageBox::warning(this, "Código Duplicado", "Ya existe una función con el código: " + QString::fromStdString(codigoFuncion));
        return;
    }

    // 4. Crear el objeto e insertarlo en el Árbol AVL
    Funcion nuevaFun;
    nuevaFun.codigo_funcion = codigoFuncion;
    nuevaFun.codigo_pelicula = codigoPelicula;
    nuevaFun.horario = horario;
    nuevaFun.sala = salaNum;
    nuevaFun.filas = filas;
    nuevaFun.columnas = columnas;
    nuevaFun.archivo_asientos = codigoFuncion + "_asientos.json"; // Se asigna el nombre del archivo
    nuevaFun.ya_realizada = false;

    avlFunciones.insertar(nuevaFun); // Esto balancea el árbol automáticamente

    // 5. Crear físicamente el archivo JSON vacío para los asientos
    QJsonObject raizJSON;
    raizJSON["codigo_pelicula"] = QString::fromStdString(codigoPelicula);
    raizJSON["horario"] = QString::fromStdString(horario);
    raizJSON["sala"] = QString::fromStdString(salaNum);
    raizJSON["asientos"] = QJsonArray(); // Arreglo vacío, listo para recibir reservas

    QJsonDocument doc(raizJSON);
    QFile archivo(QString::fromStdString(nuevaFun.archivo_asientos));
    if (archivo.open(QIODevice::WriteOnly)) {
        archivo.write(doc.toJson());
        archivo.close();
    }

    QMessageBox::information(this, "Función Creada",
                             QString("Se generó la función %1 para la sala %2.\nArchivo base creado: %3")
                                 .arg(QString::fromStdString(codigoFuncion))
                                 .arg(QString::fromStdString(salaNum))
                                 .arg(QString::fromStdString(nuevaFun.archivo_asientos)));

    // 6. Limpiar campos
    ui->txtCodigoFuncionAdmin->clear();
    ui->txtSala->clear();
    ui->txtHorario->clear();
    ui->spinFilasSala->setValue(0);
    ui->spinColumnasSala->setValue(0);

    // Opcional: actualizarTablaFunciones(); si tienes una tabla visual para las funciones
}

void MainWindow::on_btnRefrescarSala_clicked() {
    // 1. Obtener el código de la función desde el dato oculto (userData) del ComboBox
    QString codigoFuncion = ui->cbmFuncionPeli->currentData().toString();

    if (codigoFuncion.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Por favor, selecciona una función válida.");
        return;
    }

    // 2. Buscar la función en el Árbol AVL para saber las dimensiones de la sala
    Funcion* f = avlFunciones.buscarFuncion(codigoFuncion.toStdString());
    if (f == nullptr) {
        QMessageBox::critical(this, "Error", "La función ya no se encuentra en el sistema.");
        return;
    }

    // 3. Preparar la tabla visual (QTableWidget)
    ui->tablaAsientos->setRowCount(f->filas);
    ui->tablaAsientos->setColumnCount(f->columnas);

    // Opcional: Ajustar el tamaño de celdas para que se vean cuadradas como asientos
    ui->tablaAsientos->horizontalHeader()->setDefaultSectionSize(60);
    ui->tablaAsientos->verticalHeader()->setDefaultSectionSize(60);

    // 4. Inicializar toda la matriz visual como asientos "Libres" (Verdes)
    // Nota: QTableWidget inicia en 0, pero para el usuario los asientos inician en 1
    for (int i = 0; i < f->filas; i++) {
        for (int j = 0; j < f->columnas; j++) {
            QString textoCelda = QString("Libre\nF%1 C%2").arg(i + 1).arg(j + 1);
            QTableWidgetItem* item = new QTableWidgetItem(textoCelda);

            item->setBackground(QColor(144, 238, 144)); // Color Verde Claro
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Evita que el usuario edite el texto con doble clic

            ui->tablaAsientos->setItem(i, j, item);
        }
    }

    // ==============================================================
    // 5. Extraer asientos consultando TU ESTRUCTURA EN MEMORIA (Tabla Hash)
    // ==============================================================
    std::vector<Reserva> reservas = tablaReservas.obtenerReservasPorFuncion(codigoFuncion.toStdString());

    /*
     * NOTA DE ESTRUCTURAS DE DATOS (MATRIZ DISPERSA):
     * Si generas el reporte de Graphviz de la sala aquí, inicializa la matriz:
     * sala = MatrizDispersa();
     */

    // 6. Recorrer las reservas encontradas y sobreescribir las celdas a "Ocupado" (Rojas)
    for (const Reserva& res : reservas) {
        // Convertimos a índice de interfaz (restando 1)
        int filaUI = res.fila - 1;
        int colUI = res.columna - 1;

        // Si usas la Matriz Dispersa para graficar la sala:
        // sala.insertar(res.fila, res.columna, res.id_cliente);

        // Validamos que el asiento no exceda los límites de la tabla
        if (filaUI >= 0 && filaUI < f->filas && colUI >= 0 && colUI < f->columnas) {
            QTableWidgetItem* itemOcupado = ui->tablaAsientos->item(filaUI, colUI);
            if (itemOcupado) {
                itemOcupado->setText(QString("Ocupado\nF%1 C%2").arg(res.fila).arg(res.columna));
                itemOcupado->setBackground(QColor(255, 99, 71)); // Color Rojo Tomate
            }
        }
    }
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
    // 1. Leemos qué opción está seleccionada en el ComboBox
    QString orden = ui->cmbOrdenPeliculas->currentText();
    std::vector<Pelicula> lista;

    // 2. Extraemos la lista en el orden solicitado
    if (orden == "Preorden") {
        lista = cartelera.obtenerCarteleraPreorden();
    } else if (orden == "Postorden") {
        lista = cartelera.obtenerCarteleraPostorden();
    } else {
        lista = cartelera.obtenerCarteleraInorden(); // Inorden por defecto
    }

    // 3. Preparamos la tabla visual
    ui->tablaCarteleraAdmin->setRowCount(0);
    ui->tablaCarteleraAdmin->setColumnCount(8);
    ui->tablaCarteleraAdmin->setHorizontalHeaderLabels({"Código", "Título", "Clasificación", "Duración", "Género", "Idioma", "Estreno", "Fin"});

    // 4. Llenamos los datos
    for (int i = 0; i < (int)lista.size(); i++) {
        ui->tablaCarteleraAdmin->insertRow(i);
        ui->tablaCarteleraAdmin->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(lista[i].codigo)));
        ui->tablaCarteleraAdmin->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(lista[i].titulo)));
        ui->tablaCarteleraAdmin->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(lista[i].clasificacion)));
        ui->tablaCarteleraAdmin->setItem(i, 3, new QTableWidgetItem(QString::number(lista[i].duracion) + " min"));
        ui->tablaCarteleraAdmin->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(lista[i].genero)));
        ui->tablaCarteleraAdmin->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(lista[i].idioma)));
        ui->tablaCarteleraAdmin->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(lista[i].fechaEstreno)));
        ui->tablaCarteleraAdmin->setItem(i, 7, new QTableWidgetItem(QString::fromStdString(lista[i].fechaFin)));
    }

    ui->tablaCarteleraAdmin->resizeColumnsToContents();
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
        actualizarComboPeliculasAdmin();
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


void MainWindow::on_cmbOrdenPeliculas_currentTextChanged(const QString &ordenSeleccionado) {
    actualizarTablaPeliculas(); // Se dispara automáticamente al cambiar la opción
}


// ==========================================
// 1. CARGAR Y ACTUALIZAR TABLA
// ==========================================
void MainWindow::actualizarTablaFunciones() {
    std::vector<Funcion> lista = avlFunciones.obtenerFuncionesInorden();

    ui->tblFuncionesRegistradas->setRowCount(0);
    ui->tblFuncionesRegistradas->setColumnCount(6);
    ui->tblFuncionesRegistradas->setHorizontalHeaderLabels({"Código", "Película", "Sala", "Horario", "Filas", "Columnas"});

    for (int i = 0; i < (int)lista.size(); i++) {
        ui->tblFuncionesRegistradas->insertRow(i);
        ui->tblFuncionesRegistradas->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(lista[i].codigo_funcion)));

        // Cruzamos datos con el Árbol Binario para mostrar el Título en lugar del código
        Pelicula* p = cartelera.buscarPelicula(lista[i].codigo_pelicula);
        QString titulo = p ? QString::fromStdString(p->titulo) : QString::fromStdString(lista[i].codigo_pelicula);

        ui->tblFuncionesRegistradas->setItem(i, 1, new QTableWidgetItem(titulo));
        ui->tblFuncionesRegistradas->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(lista[i].sala)));
        ui->tblFuncionesRegistradas->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(lista[i].horario)));
        ui->tblFuncionesRegistradas->setItem(i, 4, new QTableWidgetItem(QString::number(lista[i].filas)));
        ui->tblFuncionesRegistradas->setItem(i, 5, new QTableWidgetItem(QString::number(lista[i].columnas)));
    }
    ui->tblFuncionesRegistradas->resizeColumnsToContents();
}

void MainWindow::on_btnActualizarTablaFuncion_clicked() {
    actualizarTablaFunciones();
}

// ==========================================
// 2. SELECCIÓN CON CLIC EN LA TABLA
// ==========================================
void MainWindow::on_tblFuncionesRegistradas_cellClicked(int row, int column) {
    QString codigo = ui->tblFuncionesRegistradas->item(row, 0)->text();
    Funcion* f = avlFunciones.buscarFuncion(codigo.toStdString());

    if (f) {
        ui->txtEditCodigoFuncion->setText(QString::fromStdString(f->codigo_funcion));

        Pelicula* p = cartelera.buscarPelicula(f->codigo_pelicula);
        QString titulo = p ? QString::fromStdString(p->titulo) : QString::fromStdString(f->codigo_pelicula);

        ui->txtEditPelicula->setText(titulo);
        ui->txtEditSala->setText(QString::fromStdString(f->sala));
        ui->txtEditHorario->setText(QString::fromStdString(f->horario));
        ui->spinEditFila->setValue(f->filas);
        ui->spinEditCol->setValue(f->columnas);
    }
}

// ==========================================
// 3. BUSCAR POR ID
// ==========================================
void MainWindow::on_btnBuscarFuncion_clicked() {
    QString idBusqueda = ui->txtBuscarFuncionID->text().trimmed();

    if (idBusqueda.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Ingresa un ID de función válido.");
        return;
    }

    Funcion* f = avlFunciones.buscarFuncion(idBusqueda.toStdString());
    if (f) {
        ui->txtEditCodigoFuncion->setText(QString::fromStdString(f->codigo_funcion));

        Pelicula* p = cartelera.buscarPelicula(f->codigo_pelicula);
        QString titulo = p ? QString::fromStdString(p->titulo) : QString::fromStdString(f->codigo_pelicula);

        ui->txtEditPelicula->setText(titulo);
        ui->txtEditSala->setText(QString::fromStdString(f->sala));
        ui->txtEditHorario->setText(QString::fromStdString(f->horario));
        ui->spinEditFila->setValue(f->filas);
        ui->spinEditCol->setValue(f->columnas);

        QMessageBox::information(this, "Encontrada", "Datos de la función cargados para su edición.");
    } else {
        QMessageBox::warning(this, "No Encontrado", "No existe ninguna función con ese ID.");
    }
}

// ==========================================
// 4. EDITAR FUNCIÓN
// ==========================================
void MainWindow::on_btnEditFuncion_clicked() {
    QString codigo = ui->txtEditCodigoFuncion->text().trimmed();

    if (codigo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Selecciona o busca una función primero.");
        return;
    }

    Funcion* f = avlFunciones.buscarFuncion(codigo.toStdString());
    if (!f) return;

    QString nuevaSala = ui->txtEditSala->text().trimmed();
    QString nuevoHorario = ui->txtEditHorario->text().trimmed();
    QString tituloInput = ui->txtEditPelicula->text().trimmed();
    int nuevasFilas = ui->spinEditFila->value();
    int nuevasCols = ui->spinEditCol->value();

    if (nuevaSala.isEmpty() || nuevoHorario.isEmpty() || tituloInput.isEmpty() || nuevasFilas <= 0 || nuevasCols <= 0) {
        QMessageBox::warning(this, "Aviso", "Completa todos los campos correctamente.");
        return;
    }

    // Validar si el título de la película ingresado existe
    std::string nuevoCodPel = f->codigo_pelicula;
    if (tituloInput.toStdString() != cartelera.buscarPelicula(f->codigo_pelicula)->titulo) {
        bool peliEncontrada = false;
        std::vector<Pelicula> pelis = cartelera.obtenerCarteleraInorden();
        for (auto& pel : pelis) {
            if (pel.titulo == tituloInput.toStdString()) {
                nuevoCodPel = pel.codigo;
                peliEncontrada = true;
                break;
            }
        }
        if (!peliEncontrada) {
            QMessageBox::critical(this, "Error", "La película ingresada no existe en la cartelera.");
            return;
        }
    }

    // Reutilizar el método editar del Árbol AVL
    Funcion fModificada = *f;
    fModificada.codigo_pelicula = nuevoCodPel;
    fModificada.sala = nuevaSala.toStdString();
    fModificada.horario = nuevoHorario.toStdString();
    fModificada.filas = nuevasFilas;
    fModificada.columnas = nuevasCols;

    avlFunciones.editar(codigo.toStdString(), fModificada);
    actualizarTablaFunciones();

    QMessageBox::information(this, "Éxito", "Función actualizada correctamente.");
}

// ==========================================
// 5. ELIMINAR FUNCIÓN Y EN CASCADA
// ==========================================
void MainWindow::on_btnEliminarFun_clicked() {
    QString codigo = ui->txtEditCodigoFuncion->text().trimmed();

    if (codigo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Selecciona o busca una función primero.");
        return;
    }

    if (avlFunciones.buscarFuncion(codigo.toStdString()) == nullptr) {
        QMessageBox::warning(this, "Error", "La función no existe.");
        return;
    }

    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(this, "Confirmar Eliminación",
                                      "¿Estás seguro de eliminar la función " + codigo + "?\n¡Se cancelarán todas las reservas asociadas!",
                                      QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        // 1. Borrar todas las reservas huérfanas en la Tabla Hash
        tablaReservas.eliminarPorFuncion(codigo.toStdString());

        // 2. Borrar del Árbol AVL
        avlFunciones.eliminar(codigo.toStdString());

        // 3. Limpiar vista
        ui->txtEditCodigoFuncion->clear();
        ui->txtEditPelicula->clear();
        ui->txtEditSala->clear();
        ui->txtEditHorario->clear();
        ui->spinEditFila->setValue(0);
        ui->spinEditCol->setValue(0);
        ui->txtBuscarFuncionID->clear();

        actualizarTablaFunciones();
        QMessageBox::information(this, "Éxito", "Función y reservas eliminadas correctamente.");
    }
}

void MainWindow::actualizarComboPeliculasAdmin() {
    // 1. Limpiar el ComboBox por si tenía datos anteriores
    ui->cmbPeliculaAdmin->clear();

    // 2. Obtener la lista de películas desde el Árbol BST
    std::vector<Pelicula> listaPelis = cartelera.obtenerCarteleraInorden();

    // 3. Agregar una opción por defecto (opcional pero recomendado)
    ui->cmbPeliculaAdmin->addItem("Seleccione una película...");

    // 4. Llenar el ComboBox con los títulos
    for (const Pelicula& p : listaPelis) {
        ui->cmbPeliculaAdmin->addItem(QString::fromStdString(p.titulo));
    }
}

void MainWindow::on_cmbPeliculaReserva_currentTextChanged(const QString &tituloPelicula) {
    // 1. Limpiar el ComboBox de funciones
    ui->cbmFuncionPeli->clear();

    // Ignorar si es la opción por defecto o está vacío
    if (tituloPelicula.isEmpty() || tituloPelicula == "Seleccione una película...") {
        return;
    }

    // 2. Buscar el código de la película seleccionada comparando el título
    std::string codigoPelicula = "";
    std::vector<Pelicula> listaPelis = cartelera.obtenerCarteleraInorden();
    for (const Pelicula& p : listaPelis) {
        if (p.titulo == tituloPelicula.toStdString()) {
            codigoPelicula = p.codigo;
            break;
        }
    }

    // Si no se encontró el código (por algún error), salir
    if (codigoPelicula.empty()) return;

    // 3. Filtrar el Árbol AVL buscando las funciones que coincidan con ese código
    std::vector<Funcion> funciones = avlFunciones.obtenerFuncionesInorden();
    ui->cbmFuncionPeli->addItem("Seleccione una función...", QVariant("")); // Opción por defecto

    bool hayFunciones = false;
    for (const Funcion& f : funciones) {
        if (f.codigo_pelicula == codigoPelicula) {
            // Texto que verá el usuario
            QString textoVisible = QString::fromStdString("Sala: " + f.sala + " | Horario: " + f.horario);

            // addItem permite guardar datos ocultos (UserData) usando QVariant
            // Guardamos el f.codigo_funcion para usarlo más adelante en la reserva
            ui->cbmFuncionPeli->addItem(textoVisible, QVariant(QString::fromStdString(f.codigo_funcion)));
            hayFunciones = true;
        }
    }

    // 4. Mensaje visual si la película no tiene funciones programadas
    if (!hayFunciones) {
        ui->cbmFuncionPeli->setItemText(0, "No hay funciones disponibles");
    }
}

void MainWindow::actualizarComboPeliculasUsuario() {
    ui->cmbPeliculaReserva->clear();
    std::vector<Pelicula> listaPelis = cartelera.obtenerCarteleraInorden();

    ui->cmbPeliculaReserva->addItem("Seleccione una película...");
    for (const Pelicula& p : listaPelis) {
        ui->cmbPeliculaReserva->addItem(QString::fromStdString(p.titulo));
    }
}

void MainWindow::on_btnActualizarReservaUsuario_clicked()
{
    if (clienteLogueadoID.empty()) {
        QMessageBox::warning(this, "Sesión", "No hay un cliente logueado.");
        return;
    }

    Cliente* c = arbolClientes.buscar(clienteLogueadoID);

    if (c == nullptr) {
        return;
    }

    ui->tblHistorialReservas->setRowCount(0);
    ui->tblHistorialReservas->setColumnCount(6);

    ui->tblHistorialReservas->setHorizontalHeaderLabels({
        "Código",
        "Función",
        "Fecha",
        "Fila",
        "Columna",
        "Estado"
    });

    int filaVisual = 0;
    QDate fechaActual = QDate::currentDate();

    // Recorremos las reservas asociadas al cliente
    for (const std::string& codReserva : c->codigos_reservas) {

        Reserva* res = tablaReservas.buscar(codReserva);

        if (res != nullptr) {

            ui->tblHistorialReservas->insertRow(filaVisual);

            ui->tblHistorialReservas->setItem(
                filaVisual,
                0,
                new QTableWidgetItem(
                    QString::fromStdString(res->codigo_reserva)
                    )
                );

            ui->tblHistorialReservas->setItem(
                filaVisual,
                1,
                new QTableWidgetItem(
                    QString::fromStdString(res->codigo_funcion)
                    )
                );

            ui->tblHistorialReservas->setItem(
                filaVisual,
                2,
                new QTableWidgetItem(
                    QString::fromStdString(res->fecha_reserva)
                    )
                );

            ui->tblHistorialReservas->setItem(
                filaVisual,
                3,
                new QTableWidgetItem(
                    QString::number(res->fila)
                    )
                );

            ui->tblHistorialReservas->setItem(
                filaVisual,
                4,
                new QTableWidgetItem(
                    QString::number(res->columna)
                    )
                );

            // Convertir la fecha
            QDate fechaReserva = QDate::fromString(
                QString::fromStdString(res->fecha_reserva),
                "yyyy-MM-dd"
                );

            QString estado;

            if (fechaReserva < fechaActual) {
                estado = "VENCIDA";
            } else {
                estado = "VÁLIDA";
            }

            // Crear el item ANTES de modificarlo
            QTableWidgetItem* itemEstado =
                new QTableWidgetItem(estado);

            if (estado == "VENCIDA") {
                itemEstado->setForeground(QBrush(Qt::red));
            } else {
                itemEstado->setForeground(QBrush(Qt::darkGreen));
            }

            ui->tblHistorialReservas->setItem(
                filaVisual,
                5,
                itemEstado
                );

            filaVisual++;
        }
    }

    ui->tblHistorialReservas->resizeColumnsToContents();
}
