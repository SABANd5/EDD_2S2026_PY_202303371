#include "ArbolMerkle.h"
#include <QCryptographicHash>
#include <QString>
#include <fstream>
#include <cmath>

ArbolMerkle::ArbolMerkle() : raiz(nullptr) {}

ArbolMerkle::~ArbolMerkle() {
    limpiarArbol(raiz);
}

void ArbolMerkle::limpiarArbol(NodoMerkle* nodo) {
    if (nodo != nullptr) {
        limpiarArbol(nodo->izquierdo);
        limpiarArbol(nodo->derecho);
        delete nodo;
    }
}

std::string ArbolMerkle::calcularHashSha256(const std::string& input) {
    QByteArray hash = QCryptographicHash::hash(QString::fromStdString(input).toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

std::string ArbolMerkle::transaccionAString(const Transaccion& t) {
    return t.codigo_transaccion + t.codigo_reserva + t.id_cliente + t.codigo_funcion +
           std::to_string(t.fila) + std::to_string(t.columna) + std::to_string(t.monto) +
           t.metodo_pago + t.fecha_pago;
}

std::string ArbolMerkle::construirArbol(const std::vector<Transaccion>& transacciones) {
    limpiarArbol(raiz);
    raiz = nullptr;

    if (transacciones.empty()) return "0000000000000000000000000000000000000000000000000000000000000000";

    std::vector<NodoMerkle*> nodosNivelAct;

    // Nodos Hoja
    for (const Transaccion& t : transacciones) {
        std::string hashTx = calcularHashSha256(transaccionAString(t));
        nodosNivelAct.push_back(new NodoMerkle(hashTx));
    }

    // Procesar niveles hacia arriba
    while (nodosNivelAct.size() > 1) {
        std::vector<NodoMerkle*> nodosNivelSiguiente;

        // Si es impar, duplicamos el último nodo para emparejar
        if (nodosNivelAct.size() % 2 != 0) {
            nodosNivelAct.push_back(new NodoMerkle(nodosNivelAct.back()->hash));
        }

        for (size_t i = 0; i < nodosNivelAct.size(); i += 2) {
            std::string hashCombinado = calcularHashSha256(nodosNivelAct[i]->hash + nodosNivelAct[i+1]->hash);
            NodoMerkle* padre = new NodoMerkle(hashCombinado);
            padre->izquierdo = nodosNivelAct[i];
            padre->derecho = nodosNivelAct[i+1];
            nodosNivelSiguiente.push_back(padre);
        }
        nodosNivelAct = nodosNivelSiguiente;
    }

    raiz = nodosNivelAct.front();
    return raiz->hash;
}

void ArbolMerkle::generarDotRecursivo(NodoMerkle* nodo, std::string& dot, int& idCounter) {
    if (nodo != nullptr) {
        int idActual = idCounter++;

        // Protección contra strings vacíos
        std::string hashCorto = (nodo->hash.length() >= 16) ? nodo->hash.substr(0, 16) : nodo->hash;

        dot += "  Nodo" + std::to_string(idActual) + " [label=\"" + hashCorto + "...\"];\n";

        if (nodo->izquierdo != nullptr) {
            int idIzq = idCounter;
            generarDotRecursivo(nodo->izquierdo, dot, idCounter);
            dot += "  Nodo" + std::to_string(idActual) + " -> Nodo" + std::to_string(idIzq) + ";\n";
        }
        if (nodo->derecho != nullptr) {
            int idDer = idCounter;
            generarDotRecursivo(nodo->derecho, dot, idCounter);
            dot += "  Nodo" + std::to_string(idActual) + " -> Nodo" + std::to_string(idDer) + ";\n";
        }
    }
}

void ArbolMerkle::generarReporte(std::string filename) {
    if (raiz == nullptr) return;

    std::ofstream archivo(filename + ".dot");
    if (archivo.is_open()) {
        std::string dot = "digraph ArbolMerkle {\n  rankdir=TB;\n  node [shape=box, style=filled, fillcolor=\"#E1BEE7\", fontname=\"Arial\"];\n";
        int idCounter = 0;
        generarDotRecursivo(raiz, dot, idCounter);
        dot += "}\n";
        archivo.close();

        std::string comando = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
        system(comando.c_str());
    }
}