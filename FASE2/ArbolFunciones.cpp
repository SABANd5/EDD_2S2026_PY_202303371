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
        dotContent += "  node [shape=circle, fontname=\"Arial\"];\n"; // Nodos circulares

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

// ==========================================
// BÚSQUEDA DE FUNCIÓN
// ==========================================
Funcion* ArbolFunciones::buscarFuncion(std::string codigo_funcion) {
    NodoAVL* actual = raiz;

    // Búsqueda binaria estándar aprovechando la estructura del AVL
    while (actual != nullptr) {
        if (codigo_funcion == actual->dato.codigo_funcion) {
            return &(actual->dato); // Retorna el puntero directo a los datos
        } else if (codigo_funcion < actual->dato.codigo_funcion) {
            actual = actual->izquierdo;
        } else {
            actual = actual->derecho;
        }
    }

    return nullptr; // Retorna nulo si la función no existe
}

// ==========================================
// EDICIÓN DE FUNCIÓN
// ==========================================
void ArbolFunciones::editar(std::string codigo_funcion, Funcion fModificada) {
    Funcion* f = buscarFuncion(codigo_funcion);
    if (f != nullptr) {
        *f = fModificada; // Actualiza los datos manteniendo el mismo nodo en memoria
    }
}

// ==========================================
// RECORRIDOS PARA LISTADOS
// ==========================================
void ArbolFunciones::recorridoPreorden(NodoAVL* nodo, std::vector<Funcion>& lista) {
    if (nodo != nullptr) {
        lista.push_back(nodo->dato);
        recorridoPreorden(nodo->izquierdo, lista);
        recorridoPreorden(nodo->derecho, lista);
    }
}

void ArbolFunciones::recorridoInorden(NodoAVL* nodo, std::vector<Funcion>& lista) {
    if (nodo != nullptr) {
        recorridoInorden(nodo->izquierdo, lista);
        lista.push_back(nodo->dato);
        recorridoInorden(nodo->derecho, lista);
    }
}

void ArbolFunciones::recorridoPostorden(NodoAVL* nodo, std::vector<Funcion>& lista) {
    if (nodo != nullptr) {
        recorridoPostorden(nodo->izquierdo, lista);
        recorridoPostorden(nodo->derecho, lista);
        lista.push_back(nodo->dato);
    }
}

std::vector<Funcion> ArbolFunciones::obtenerFuncionesPreorden() {
    std::vector<Funcion> lista;
    recorridoPreorden(raiz, lista);
    return lista;
}

std::vector<Funcion> ArbolFunciones::obtenerFuncionesInorden() {
    std::vector<Funcion> lista;
    recorridoInorden(raiz, lista);
    return lista;
}

std::vector<Funcion> ArbolFunciones::obtenerFuncionesPostorden() {
    std::vector<Funcion> lista;
    recorridoPostorden(raiz, lista);
    return lista;
}

// ==========================================
// NUEVO: ELIMINACIÓN Y BALANCEO AVL
// ==========================================
void ArbolFunciones::eliminar(std::string codigo_funcion) {
    raiz = eliminarRecursivo(raiz, codigo_funcion);
}

NodoAVL* ArbolFunciones::obtenerMinimo(NodoAVL* nodo) {
    NodoAVL* actual = nodo;
    while (actual->izquierdo != nullptr) actual = actual->izquierdo;
    return actual;
}

NodoAVL* ArbolFunciones::eliminarRecursivo(NodoAVL* nodo, std::string codigo_funcion) {
    if (nodo == nullptr) return nodo;

    // 1. Búsqueda binaria
    if (codigo_funcion < nodo->dato.codigo_funcion) {
        nodo->izquierdo = eliminarRecursivo(nodo->izquierdo, codigo_funcion);
    } else if (codigo_funcion > nodo->dato.codigo_funcion) {
        nodo->derecho = eliminarRecursivo(nodo->derecho, codigo_funcion);
    } else {
        // 2. Nodo encontrado: Casos de eliminación
        if ((nodo->izquierdo == nullptr) || (nodo->derecho == nullptr)) {
            NodoAVL* temp = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;
            if (temp == nullptr) {
                temp = nodo;
                nodo = nullptr;
            } else {
                *nodo = *temp; // Copia el contenido
            }
            delete temp;
        } else {
            // Nodo con dos hijos: obtener el sucesor inorden
            NodoAVL* temp = obtenerMinimo(nodo->derecho);
            nodo->dato = temp->dato;
            nodo->derecho = eliminarRecursivo(nodo->derecho, temp->dato.codigo_funcion);
        }
    }

    if (nodo == nullptr) return nodo;

    // 3. Actualizar altura y balancear utilizando las funciones existentes
    actualizarAltura(nodo);
    int balance = obtenerFactorBalance(nodo);

    // Casos de rotación
    if (balance > 1 && obtenerFactorBalance(nodo->izquierdo) >= 0)
        return rotacionDerecha(nodo);

    if (balance > 1 && obtenerFactorBalance(nodo->izquierdo) < 0) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }

    if (balance < -1 && obtenerFactorBalance(nodo->derecho) <= 0)
        return rotacionIzquierda(nodo);

    if (balance < -1 && obtenerFactorBalance(nodo->derecho) > 0) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}