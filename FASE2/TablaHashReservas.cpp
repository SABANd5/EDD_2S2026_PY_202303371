#include "TablaHashReservas.h"
#include <fstream>
#include <iostream>
#include <string>
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



// ==========================================
// REPORTE GRAPHVIZ (TABLA HASH)
// ==========================================
void TablaHashReservas::generarReporteGraphviz() {
    std::ofstream archivo("reporte_hash.dot");
    if (!archivo.is_open()) return;

    int total_reservas = 0;
    int buckets_ocupados = 0;
    int colisiones = 0;

    std::string dotContent = "digraph TablaHash {\n";
    dotContent += "  rankdir=LR;\n"; // De izquierda a derecha
    dotContent += "  node [shape=record, fontname=\"Arial\"];\n";

    // 1. Dibujar el arreglo principal (Buckets) usando un nodo HTML
    dotContent += "  HashArray [shape=none, margin=0, label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"6\">\n";

    // NOTA: Ajusta "tamano" al nombre de la variable de capacidad de tu tabla
    for (int i = 0; i < capacidad; i++) {
        if (tabla[i] == nullptr) {
            // Bucket Vacío (Color Grisáceo)
            dotContent += "    <tr><td port=\"p" + std::to_string(i) + "\" bgcolor=\"#E0E0E0\">" + std::to_string(i) + "</td></tr>\n";
        } else {
            // Bucket Ocupado (Color Verde Claro)
            dotContent += "    <tr><td port=\"p" + std::to_string(i) + "\" bgcolor=\"#C8E6C9\">" + std::to_string(i) + "</td></tr>\n";
        }
    }
    dotContent += "  </table>>];\n\n";

    // 2. Dibujar las listas enlazadas (Reservas y Colisiones)
    for (int i = 0; i < capacidad; i++) {
        NodoHash* actual = tabla[i];
        if (actual != nullptr) {
            buckets_ocupados++;
            int count_nodo = 0;
            std::string prevNode = "HashArray:p" + std::to_string(i); // Iniciamos apuntando desde el bucket

            while (actual != nullptr) {
                total_reservas++;
                count_nodo++;
                if (count_nodo > 1) colisiones++; // Si hay más de 1 en el mismo bucket, es colisión

                // Crear nodo para la reserva (Color Azul Claro)
                std::string nodeId = "node_" + std::to_string(i) + "_" + std::to_string(count_nodo);
                std::string label = "Clave (Cód): " + actual->dato.codigo_reserva +
                                    "\\nCliente: " + actual->dato.id_cliente +
                                    "\\nFunción: " + actual->dato.codigo_funcion;

                dotContent += "  " + nodeId + " [label=\"" + label + "\", style=filled, fillcolor=\"#BBDEFB\"];\n";

                // Conectar con el nodo anterior
                dotContent += "  " + prevNode + " -> " + nodeId + ";\n";

                prevNode = nodeId; // El nodo actual será el "anterior" para la siguiente iteración
                actual = actual->siguiente;
            }
        }
    }

    // 3. Crear el Cuadro de Estadísticas Generales (Color Amarillo Claro)
    std::string statsLabel = "ESTADÍSTICAS DEL SISTEMA\\n"
                             "-------------------------------\\n"
                             "Tamaño de la tabla: " + std::to_string(capacidad) + "\\n"
                                                        "Reservas almacenadas: " + std::to_string(total_reservas) + "\\n"
                                                                "Buckets ocupados: " + std::to_string(buckets_ocupados) + "\\n"
                                                                  "Total de Colisiones: " + std::to_string(colisiones);

    dotContent += "\n  Stats [shape=note, label=\"" + statsLabel + "\", style=filled, fillcolor=\"#FFF9C4\"];\n";

    dotContent += "}\n";
    archivo << dotContent;
    archivo.close();

    // Ejecutar Graphviz para generar la imagen PNG
    system("dot -Tpng reporte_hash.dot -o reporte_hash.png");
}