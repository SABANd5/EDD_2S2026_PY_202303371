#include "TablaHashReservas.h"
#include <fstream>
#include <iostream>

TablaHashReservas::TablaHashReservas(int tamano) {
    capacidad = tamano;
    elementos = 0;
    tabla.resize(capacidad, nullptr);
}

// ==========================================
// FUNCIÓN HASH Y MANTENIMIENTO
// ==========================================
int TablaHashReservas::funcionHash(std::string clave) {
    int suma = 0;
    for (char c : clave) {
        suma += c; // Suma de valores ASCII
    }
    return suma % capacidad;
}

// ==========================================
// OPERACIONES PRINCIPALES
// ==========================================
void TablaHashReservas::insertar(Reserva r) {
    int indice = funcionHash(r.codigo_reserva);
    NodoHash* nuevo = new NodoHash(r);

    if (tabla[indice] == nullptr) {
        tabla[indice] = nuevo;
    } else {
        // Manejo de colisión: insertar al final de la lista enlazada
        NodoHash* actual = tabla[indice];
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
    elementos++;
}

Reserva* TablaHashReservas::buscar(std::string codigo_reserva) {
    int indice = funcionHash(codigo_reserva);
    NodoHash* actual = tabla[indice];

    while (actual != nullptr) {
        if (actual->dato.codigo_reserva == codigo_reserva) {
            return &(actual->dato);
        }
        actual = actual->siguiente;
    }
    return nullptr;
}


// ==========================================
// REPORTE GRAPHVIZ
// ==========================================
void TablaHashReservas::generarReporteGraphviz() {
    std::ofstream archivo("reporte_hash.dot");
    if (archivo.is_open()) {
        std::string dotContent = "digraph TablaHash {\n";
        dotContent += "  rankdir=LR;\n"; // De izquierda a derecha para mostrar los buckets y listas
        dotContent += "  node [shape=box];\n";

        for (int i = 0; i < capacidad; i++) {
            // Diferenciar color si el bucket está vacío o tiene elementos[cite: 1]
            std::string colorBucket = (tabla[i] == nullptr) ? "lightgrey" : "orange";
            dotContent += "  bucket" + std::to_string(i) + " [label=\"Bucket " + std::to_string(i) + "\", style=filled, fillcolor=" + colorBucket + "];\n";

            NodoHash* actual = tabla[i];
            if (actual != nullptr) {
                std::string nodoAnterior = "bucket" + std::to_string(i);
                int contadorColision = 0;

                while (actual != nullptr) {
                    std::string idNodo = "nodo_" + std::to_string(i) + "_" + std::to_string(contadorColision);
                    // Formato exigido: codigo_reserva, id_cliente -> codigo_funcion[cite: 1]
                    std::string label = actual->dato.codigo_reserva + "\\n" + actual->dato.id_cliente + " -> " + actual->dato.codigo_funcion;

                    dotContent += "  " + idNodo + " [label=\"" + label + "\"];\n";
                    dotContent += "  " + nodoAnterior + " -> " + idNodo + ";\n";

                    nodoAnterior = idNodo;
                    actual = actual->siguiente;
                    contadorColision++;
                }
            }
        }

        dotContent += "}\n";
        archivo << dotContent;
        archivo.close();

        system("dot -Tpng reporte_hash.dot -o reporte_hash.png");
    }
}

void TablaHashReservas::eliminar(std::string codigo_reserva) {
    int indice = funcionHash(codigo_reserva);
    NodoHash* actual = tabla[indice];
    NodoHash* anterior = nullptr;

    while (actual != nullptr) {
        // Cambiamos "reserva" por "dato"
        if (actual->dato.codigo_reserva == codigo_reserva) {
            if (anterior == nullptr) {
                // Es el primer elemento de la lista enlazada
                tabla[indice] = actual->siguiente;
            } else {
                // Está en medio o al final
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            return;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
}

void TablaHashReservas::eliminarPorFuncion(std::string codigo_funcion) {
    // Recorremos toda la tabla (asumiendo que tu variable de tamaño se llama 'capacidad' o 'tamano')
    for (int i = 0; i < capacidad; i++) { // Ajusta 'tamano' al nombre de tu variable
        NodoHash* actual = tabla[i];
        NodoHash* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->dato.codigo_funcion == codigo_funcion) {
                NodoHash* aBorrar = actual;
                if (anterior == nullptr) {
                    tabla[i] = actual->siguiente;
                    actual = tabla[i];
                } else {
                    anterior->siguiente = actual->siguiente;
                    actual = actual->siguiente;
                }
                delete aBorrar;
            } else {
                anterior = actual;
                actual = actual->siguiente;
            }
        }
    }
}

std::vector<Reserva> TablaHashReservas::obtenerReservasPorFuncion(std::string codigo_funcion) {
    std::vector<Reserva> encontradas;

    // NOTA: Cambia "tamano" por el nombre de la variable que usaste
    // en tu clase para definir la capacidad del arreglo (ej. capacidad, size)
    for (int i = 0; i < capacidad; i++) {
        NodoHash* actual = tabla[i];

        while (actual != nullptr) {
            if (actual->dato.codigo_funcion == codigo_funcion) {
                encontradas.push_back(actual->dato);
            }
            actual = actual->siguiente;
        }
    }

    return encontradas;
}