#include "ArbolFunciones.h"
#include <iostream>
#include <fstream>
#include <algorithm> // Para std::max

ArbolFunciones::ArbolFunciones() : raiz(nullptr) {}

// ==========================================
// UTILIDADES DE BALANCEO (ALTURA Y FACTOR)
// ==========================================
int ArbolFunciones::obtenerAltura(NodoAVL* nodo) {
    if (nodo == nullptr) return 0;
    return nodo->altura;
}

int ArbolFunciones::obtenerFactorBalance(NodoAVL* nodo) {
    if (nodo == nullptr) return 0;
    return obtenerAltura(nodo->izquierdo) - obtenerAltura(nodo->derecho);
}

void ArbolFunciones::actualizarAltura(NodoAVL* nodo) {
    if (nodo != nullptr) {
        nodo->altura = 1 + std::max(obtenerAltura(nodo->izquierdo), obtenerAltura(nodo->derecho));
    }
}

// ==========================================
// ROTACIONES AVL
// ==========================================
NodoAVL* ArbolFunciones::rotacionDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierdo;
    NodoAVL* T2 = x->derecho;

    // Realizar rotación
    x->derecho = y;
    y->izquierdo = T2;

    // Actualizar alturas
    actualizarAltura(y);
    actualizarAltura(x);

    return x; // Nueva raíz
}

NodoAVL* ArbolFunciones::rotacionIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecho;
    NodoAVL* T2 = y->izquierdo;

    // Realizar rotación
    y->izquierdo = x;
    x->derecho = T2;

    // Actualizar alturas
    actualizarAltura(x);
    actualizarAltura(y);

    return y; // Nueva raíz
}

// ==========================================
// INSERCIÓN Y CREACIÓN DE ARCHIVO JSON
// ==========================================
NodoAVL* ArbolFunciones::insertarRecursivo(NodoAVL* nodo, Funcion f) {
    // 1. Inserción normal de BST
    if (nodo == nullptr) return new NodoAVL(f);

    if (f.codigo_funcion < nodo->dato.codigo_funcion)
        nodo->izquierdo = insertarRecursivo(nodo->izquierdo, f);
    else if (f.codigo_funcion > nodo->dato.codigo_funcion)
        nodo->derecho = insertarRecursivo(nodo->derecho, f);
    else
        return nodo; // No se permiten códigos duplicados

    // 2. Actualizar altura de este nodo ancestro
    actualizarAltura(nodo);

    // 3. Obtener el factor de balanceo para verificar si se desequilibró
    int balance = obtenerFactorBalance(nodo);

    // Casos de desequilibrio:
    // Caso Izquierda Izquierda
    if (balance > 1 && f.codigo_funcion < nodo->izquierdo->dato.codigo_funcion)
        return rotacionDerecha(nodo);

    // Caso Derecha Derecha
    if (balance < -1 && f.codigo_funcion > nodo->derecho->dato.codigo_funcion)
        return rotacionIzquierda(nodo);

    // Caso Izquierda Derecha
    if (balance > 1 && f.codigo_funcion > nodo->izquierdo->dato.codigo_funcion) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }

    // Caso Derecha Izquierda
    if (balance < -1 && f.codigo_funcion < nodo->derecho->dato.codigo_funcion) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

void ArbolFunciones::insertar(Funcion f) {
    // Si el nombre del archivo viene vacío, lo generamos con el formato exigido
    if (f.archivo_asientos.empty()) {
        f.archivo_asientos = f.codigo_funcion + "_funcion.json";
    }

    raiz = insertarRecursivo(raiz, f);

    // Crear el archivo JSON físico inicial con los asientos ocupados vacíos
    std::ofstream archivo(f.archivo_asientos);
    if (archivo.is_open()) {
        archivo << "{\n";
        archivo << "  \"codigo_funcion\": \"" << f.codigo_funcion << "\",\n";
        archivo << "  \"asientos_ocupados\": []\n";
        archivo << "}\n";
        archivo.close();
    }
}

// ==========================================
// REPORTE GRAPHVIZ
// ==========================================
void ArbolFunciones::generarCuerpoDot(NodoAVL* nodo, std::string& dotContent) {
    if (nodo != nullptr) {
        // Color verde si está programada, rojo si ya se realizó
        std::string color = nodo->dato.ya_realizada ? "red" : "green";

        dotContent += "  \"" + nodo->dato.codigo_funcion + "\" [label=\"" + nodo->dato.codigo_funcion + "\\n" + nodo->dato.horario + "\\n" + nodo->dato.sala + "\", style=filled, fillcolor=" + color + "];\n";

        if (nodo->izquierdo != nullptr) {
            dotContent += "  \"" + nodo->dato.codigo_funcion + "\" -> \"" + nodo->izquierdo->dato.codigo_funcion + "\";\n";
            generarCuerpoDot(nodo->izquierdo, dotContent);
        }
        if (nodo->derecho != nullptr) {
            dotContent += "  \"" + nodo->dato.codigo_funcion + "\" -> \"" + nodo->derecho->dato.codigo_funcion + "\";\n";
            generarCuerpoDot(nodo->derecho, dotContent);
        }
    }
}

void ArbolFunciones::generarReporteGraphviz() {
    std::ofstream archivo("reporte_avl.dot");
    if (archivo.is_open()) {
        std::string dotContent = "digraph AVLFunciones {\n";
        dotContent += "  node [shape=circle, fontname=\"Arial\"];\n"; // Nodos circulares[cite: 1]

        if (raiz != nullptr) {
            generarCuerpoDot(raiz, dotContent);
        } else {
            dotContent += "  \"Vacio\";\n";
        }

        dotContent += "}\n";
        archivo << dotContent;
        archivo.close();

        // Ejecutar el comando del sistema para generar la imagen
        system("dot -Tpng reporte_avl.dot -o reporte_avl.png");
    }
}