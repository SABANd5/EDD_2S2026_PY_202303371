#include "Blockchain.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <fstream>

Blockchain::Blockchain() : cabeza(nullptr), indexCounter(0), transaccionCounter(1) {
    QDir().mkdir("blocks");
}

void Blockchain::inicializar(TablaHashReservas* tabla) {
    leerBloquesDesdeDisco(tabla);
}

std::string Blockchain::calcularHash(int index, std::string timestamp, int nonce, std::string rootMerkle, std::string previousHash) {
    std::string rawData = std::to_string(index) + timestamp + std::to_string(nonce) + rootMerkle + previousHash;
    QByteArray hash = QCryptographicHash::hash(QString::fromStdString(rawData).toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

void Blockchain::agregarTransaccion(Transaccion t) {
    // Generar código de transacción si viene vacío
    if (t.codigo_transaccion.empty()) {
        char buffer[10];
        sprintf(buffer, "T%03d", transaccionCounter++);
        t.codigo_transaccion = std::string(buffer);
    }
    transaccionesPendientes.push_back(t);
}

int Blockchain::pendientesSize() {
    return transaccionesPendientes.size();
}

void Blockchain::minarBloque() {
    if (transaccionesPendientes.empty()) return;

    Bloque* nuevoBloque = new Bloque();
    nuevoBloque->index = indexCounter++;
    nuevoBloque->timestamp = QDateTime::currentDateTime().toString("dd-MM-yyyy::hh:mm:ss").toStdString();
    nuevoBloque->data = transaccionesPendientes;

    ArbolMerkle arbol;
    nuevoBloque->rootMerkle = arbol.construirArbol(nuevoBloque->data);

    nuevoBloque->previousHash = (cabeza == nullptr) ? "0000" : cabeza->hash;

    // Proof of Work
    nuevoBloque->nonce = 0;
    std::string hashIntento;
    do {
        hashIntento = calcularHash(nuevoBloque->index, nuevoBloque->timestamp, nuevoBloque->nonce, nuevoBloque->rootMerkle, nuevoBloque->previousHash);
        if (hashIntento.substr(0, 4) == "0000") {
            nuevoBloque->hash = hashIntento;
            break;
        }
        nuevoBloque->nonce++;
    } while (true);

    nuevoBloque->esValido = true;
    nuevoBloque->siguiente = cabeza; // Inserción por delante (El más reciente es la cabeza)
    cabeza = nuevoBloque;

    guardarBloqueDisco(nuevoBloque);
    transaccionesPendientes.clear();
}

void Blockchain::guardarBloqueDisco(Bloque* b) {
    QJsonObject jsonObj;
    jsonObj["INDEX"] = b->index;
    jsonObj["TIMESTAMP"] = QString::fromStdString(b->timestamp);
    jsonObj["NONCE"] = b->nonce;

    QJsonArray dataArray;
    for (const Transaccion& t : b->data) {
        QJsonObject tObj;
        tObj["codigo_transaccion"] = QString::fromStdString(t.codigo_transaccion);
        tObj["codigo_reserva"] = QString::fromStdString(t.codigo_reserva);
        tObj["id_cliente"] = QString::fromStdString(t.id_cliente);
        tObj["codigo_funcion"] = QString::fromStdString(t.codigo_funcion);
        tObj["fila"] = t.fila;
        tObj["columna"] = t.columna;
        tObj["monto"] = t.monto;
        tObj["metodo_pago"] = QString::fromStdString(t.metodo_pago);
        tObj["fecha_pago"] = QString::fromStdString(t.fecha_pago);
        dataArray.append(tObj);
    }

    jsonObj["DATA"] = dataArray;
    jsonObj["ROOTMERKLE"] = QString::fromStdString(b->rootMerkle);
    jsonObj["PREVIOUSHASH"] = QString::fromStdString(b->previousHash);
    jsonObj["HASH"] = QString::fromStdString(b->hash);

    QJsonDocument doc(jsonObj);
    QString filename = QString("blocks/block%1.json").arg(b->index);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void Blockchain::leerBloquesDesdeDisco(TablaHashReservas* tabla) {
    int idx = 0;
    bool bloqueManipuladoEncontrado = false;
    std::string previousHashEsperado = "0000";
    Bloque* ultimoInsertado = nullptr;

    while (true) {
        QString filename = QString("blocks/block%1.json").arg(idx);
        QFile file(filename);
        if (!file.exists() || !file.open(QIODevice::ReadOnly)) break; // Ya no hay más bloques

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        QJsonObject obj = doc.object();

        Bloque* b = new Bloque();
        b->index = obj["INDEX"].toInt();
        b->timestamp = obj["TIMESTAMP"].toString().toStdString();
        b->nonce = obj["NONCE"].toInt();
        b->rootMerkle = obj["ROOTMERKLE"].toString().toStdString();
        b->previousHash = obj["PREVIOUSHASH"].toString().toStdString();
        b->hash = obj["HASH"].toString().toStdString();

        QJsonArray dataArray = obj["DATA"].toArray();
        for (const QJsonValue& val : dataArray) {
            QJsonObject tObj = val.toObject();
            Transaccion t;
            t.codigo_transaccion = tObj["codigo_transaccion"].toString().toStdString();
            t.codigo_reserva = tObj["codigo_reserva"].toString().toStdString();
            t.id_cliente = tObj["id_cliente"].toString().toStdString();
            t.codigo_funcion = tObj["codigo_funcion"].toString().toStdString();
            t.fila = tObj["fila"].toInt();
            t.columna = tObj["columna"].toInt();
            t.monto = tObj["monto"].toDouble();
            t.metodo_pago = tObj["metodo_pago"].toString().toStdString();
            t.fecha_pago = tObj["fecha_pago"].toString().toStdString();
            b->data.push_back(t);
        }

        // Validar integridad
        // 1. Recalcular el Árbol de Merkle con la DATA real leída para detectar alteraciones
        ArbolMerkle arbolValidacion;
        std::string rootMerkleCalculado = arbolValidacion.construirArbol(b->data);

        // 2. Recalcular el hash del bloque
        std::string hashCalculado = calcularHash(b->index, b->timestamp, b->nonce, b->rootMerkle, b->previousHash);

        // 3. Evaluar todas las reglas de inmutabilidad (incluyendo strings vacíos)
        if (hashCalculado == b->hash &&
            rootMerkleCalculado == b->rootMerkle &&
            hashCalculado.length() >= 4 && hashCalculado.substr(0, 4) == "0000" &&
            b->previousHash == previousHashEsperado &&
            !bloqueManipuladoEncontrado) {

        b->esValido = true;
            // Restaurar tabla hash (solo si es válido)
            for (const Transaccion& t : b->data) {
                Reserva r;
                r.codigo_reserva = t.codigo_reserva;
                r.codigo_funcion = t.codigo_funcion;
                r.id_cliente = t.id_cliente;
                r.fila = t.fila;
                r.columna = t.columna;
                r.fecha_reserva = t.fecha_pago; // Fecha en que se hizo
                tabla->insertar(r);
                transaccionCounter++; // Ajustar contador global
            }
            previousHashEsperado = b->hash;
            indexCounter = b->index + 1; // Ajustar siguiente índice disponible
        } else {
            b->esValido = false;
            bloqueManipuladoEncontrado = true; // Todo lo de adelante será inválido visualmente
        }

        // Inserción por delante (como pila, para que el bloque más nuevo quede de cabeza)
        b->siguiente = cabeza;
        cabeza = b;

        idx++;
    }
}

void Blockchain::generarReporteBlockchain() {
    std::ofstream archivo("reporte_blockchain.dot");
    if (archivo.is_open()) {
        archivo << "digraph Blockchain {\n";
        archivo << "  rankdir=LR;\n";
        archivo << "  node [shape=record, fontname=\"Arial\"];\n";

        Bloque* aux = cabeza;
        while (aux != nullptr) {
            std::string color = aux->esValido ? "\"#81C784\"" : "\"#EF5350\""; // Verde o Rojo

            // Protección contra strings vacíos o corruptos al usar substr
            std::string hashCorto = (aux->hash.length() >= 10) ? aux->hash.substr(0, 10) : aux->hash;
            std::string prevCorto = (aux->previousHash.length() >= 10) ? aux->previousHash.substr(0, 10) : aux->previousHash;

            archivo << "  \"Bloque" << aux->index << "\" [style=filled, fillcolor=" << color
                    << ", label=\"{INDEX: " << aux->index
                    << " | NONCE: " << aux->nonce
                    << " | HASH: " << hashCorto << "..."
                    << " | PREV: " << prevCorto << "...}\"];\n";

            if (aux->siguiente != nullptr) {
                // Como aux apunta hacia atrás en la cadena temporalmente (cabeza es el más nuevo)
                // la flecha debe apuntar de nuevo a viejo (o al revés, visualmente).
                archivo << "  \"Bloque" << aux->index << "\" -> \"Bloque" << aux->siguiente->index << "\";\n";
            }
            aux = aux->siguiente;
        }
        archivo << "}\n";
        archivo.close();
        system("dot -Tpng reporte_blockchain.dot -o reporte_blockchain.png");
    }
}

bool Blockchain::generarReporteMerkleBloque(int index) {
    Bloque* aux = cabeza;
    while (aux != nullptr) {
        if (aux->index == index) {
            ArbolMerkle arbol;
            arbol.construirArbol(aux->data);
            arbol.generarReporte("reporte_merkle_b" + std::to_string(index));
            return true; // Se encontró el bloque y se generó el reporte
        }
        aux = aux->siguiente;
    }
    return false; // No se encontró ningún bloque con ese índice
}

// <-- INICIO CÓDIGO NUEVO
bool Blockchain::buscarTransaccion(std::string codTx, Transaccion& outTx, int& outIndex, std::string& outHash) {
    Bloque* aux = cabeza;
    while (aux != nullptr) {
        if (aux->esValido) { // Solo busca en bloques que no han sido hackeados
            for (const Transaccion& t : aux->data) {
                if (t.codigo_transaccion == codTx) {
                    outTx = t;
                    outIndex = aux->index;
                    outHash = aux->hash;
                    return true;
                }
            }
        }
        aux = aux->siguiente;
    }
    return false;
}

std::vector<Transaccion> Blockchain::obtenerHistorialCliente(std::string id_cliente) {
    std::vector<Transaccion> historial;
    Bloque* aux = cabeza;
    while (aux != nullptr) {
        if (aux->esValido) {
            for (const Transaccion& t : aux->data) {
                if (t.id_cliente == id_cliente) {
                    historial.push_back(t);
                }
            }
        }
        aux = aux->siguiente;
    }
    return historial;
}

std::string Blockchain::generarReporteVentas() {
    int totalBoletos = 0;
    double totalMonto = 0.0;

    Bloque* aux = cabeza;
    while(aux != nullptr) {
        if (aux->esValido) {
            for (const Transaccion& t : aux->data) {
                totalBoletos++;
                totalMonto += t.monto;
            }
        }
        aux = aux->siguiente;
    }
    return "=== VENTAS TOTALES BLOCKCHAIN ===\n\nTotal Boletos Vendidos: " + std::to_string(totalBoletos) +
           "\nIngresos Totales Registrados: Q" + std::to_string(totalMonto);
}
// FIN CÓDIGO NUEVO -->