#include "ArbolFunciones.h"
#include <iostream>
#include <fstream>
#include <algorithm> // Para std::max
#include "ArbolPeliculas.h"
#include <QDate>
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

    // <-- INICIO CORRECCIÓN: Evitar sobreescribir si el archivo ya existe
    std::ifstream checkFile(f.archivo_asientos);
    if (!checkFile.is_open()) {
        std::ofstream archivo(f.archivo_asientos);
        if (archivo.is_open()) {
            archivo << "{\n";
            archivo << "  \"codigo_funcion\": \"" << f.codigo_funcion << "\",\n";
            archivo << "  \"asientos_ocupados\": []\n";
            archivo << "}\n";
            archivo.close();
        }
    } else {
        checkFile.close();
    }
    // FIN CORRECCIÓN -->
}

// ==========================================
// REPORTE GRAPHVIZ
// ==========================================
void ArbolFunciones::generarCuerpoDot(NodoAVL* nodo, std::string& dotContent, ArbolPeliculas* cartelera) {
    if (nodo != nullptr) {
        bool peliculaVencida = false;

        // 1. Buscamos la película en la cartelera usando el código que tiene la función
        if (cartelera != nullptr) {
            Pelicula* p = cartelera->buscarPelicula(nodo->dato.codigo_pelicula);

            if (p != nullptr) {
                // Convertimos el string a fecha real
                QDate fechaFin = QDate::fromString(QString::fromStdString(p->fechaFin), "yyyy-MM-dd");

                // Comparamos: si la fecha fin ya pasó, marcamos la película como vencida
                if (fechaFin.isValid() && fechaFin < QDate::currentDate()) {
                    peliculaVencida = true;
                }
            }
        }

        // 2. Aplicamos la regla del auxiliar: Rojo pastel si venció, Verde pastel si está vigente
        std::string colorHex = peliculaVencida ? "\"#EF5350\"" : "\"#81C784\"";

        // Armamos el nodo circular
        dotContent += "  \"" + nodo->dato.codigo_funcion + "\" [fillcolor=" + colorHex +
                      ", label=\"" + nodo->dato.codigo_funcion + "\\n" +
                      nodo->dato.horario + "\\nSala " + nodo->dato.sala + "\"];\n";

        // Conexión con el hijo izquierdo
        if (nodo->izquierdo != nullptr) {
            dotContent += "  \"" + nodo->dato.codigo_funcion + "\" -> \"" + nodo->izquierdo->dato.codigo_funcion + "\";\n";
            generarCuerpoDot(nodo->izquierdo, dotContent, cartelera);
        }

        // Conexión con el hijo derecho
        if (nodo->derecho != nullptr) {
            dotContent += "  \"" + nodo->dato.codigo_funcion + "\" -> \"" + nodo->derecho->dato.codigo_funcion + "\";\n";
            generarCuerpoDot(nodo->derecho, dotContent, cartelera);
        }
    }
}

void ArbolFunciones::generarReporteGraphviz(ArbolPeliculas* cartelera) {
    std::ofstream archivo("reporte_avl.dot");

    if (archivo.is_open()) {
        std::string dotContent = "digraph AVLFunciones {\n";
        dotContent += "  rankdir=TB;\n";
        dotContent += "  node [shape=circle, style=filled, fontname=\"Arial\"];\n";

        if (raiz != nullptr) {
            generarCuerpoDot(raiz, dotContent, cartelera);
            archivo << dotContent;
        } else {
            archivo << "  \"Vacio\" [label=\"Sin Funciones\", shape=box, fillcolor=white];\n";
        }

        archivo << "}\n";
        archivo.close();

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