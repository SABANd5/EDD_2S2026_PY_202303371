#include "listasolicitudes.h"
#include <iostream>
#include <fstream>

ListaSolicitudes::ListaSolicitudes() {
    cabeza = nullptr;
    contadorIds = 1; // Las solicitudes empezarán en el ID 1
}

void ListaSolicitudes::registrarSolicitud(std::string cliente, std::string telefono, std::string tipo, std::string descripcion, std::string fecha) {
    // Armamos la estructura de la solicitud
    Solicitud nuevaSolicitud;
    nuevaSolicitud.id = contadorIds++;
    nuevaSolicitud.cliente = cliente;
    nuevaSolicitud.telefono = telefono;
    nuevaSolicitud.tipo = tipo;
    nuevaSolicitud.descripcion = descripcion;
    nuevaSolicitud.fecha = fecha;
    nuevaSolicitud.estado = "Pendiente"; // Estado por defecto según el PDF

    NodoSolicitud* nuevoNodo = new NodoSolicitud(nuevaSolicitud);

    // Lógica de inserción en Lista Circular Doblemente Enlazada
    if (cabeza == nullptr) {
        cabeza = nuevoNodo;
        cabeza->siguiente = cabeza;
        cabeza->anterior = cabeza;
    } else {
        NodoSolicitud* ultimo = cabeza->anterior; // En una lista circular, el anterior a la cabeza es el último

        // Reconectamos los punteros para insertar al final
        ultimo->siguiente = nuevoNodo;
        nuevoNodo->anterior = ultimo;
        nuevoNodo->siguiente = cabeza;
        cabeza->anterior = nuevoNodo;
    }

    std::cout << "Solicitud " << nuevaSolicitud.id << " registrada con exito." << std::endl;
}

void ListaSolicitudes::procesarPrimeraSolicitud() {
    if (cabeza == nullptr) {
        std::cout << "No hay solicitudes pendientes." << std::endl;
        return;
    }

    std::cout << "--- PRIMERA SOLICITUD PENDIENTE ---" << std::endl;
    std::cout << "ID: " << cabeza->dato.id << std::endl;
    std::cout << "Cliente: " << cabeza->dato.cliente << std::endl;
    std::cout << "Tipo: " << cabeza->dato.tipo << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

// Aprobar: Cambia el estado y pasa a la siguiente solicitud en la cola
void ListaSolicitudes::aprobarPrimeraSolicitud() {
    if (cabeza == nullptr) {
        std::cout << "No hay solicitudes pendientes para aprobar." << std::endl;
        return;
    }

    // El enunciado indica marcarla como "Atendida" o "En proceso"
    cabeza->dato.estado = "Atendida";
    std::cout << "Solicitud " << cabeza->dato.id << " aprobada y marcada como Atendida." << std::endl;

    // Rotamos la cabeza para que la siguiente solicitud sea la primera en fila
    cabeza = cabeza->siguiente;
}

// Rechazar: Elimina el nodo de la lista y reconecta el círculo
void ListaSolicitudes::rechazarPrimeraSolicitud() {
    if (cabeza == nullptr) {
        std::cout << "No hay solicitudes pendientes para rechazar." << std::endl;
        return;
    }

    NodoSolicitud* aBorrar = cabeza;

    // Caso 1: Es el único nodo en la lista
    if (cabeza->siguiente == cabeza) {
        cabeza = nullptr;
    }
    // Caso 2: Hay más de un nodo
    else {
        NodoSolicitud* ultimo = cabeza->anterior;
        cabeza = cabeza->siguiente; // Movemos la cabeza al siguiente

        // Reconectamos el círculo saltando el nodo 'aBorrar'
        ultimo->siguiente = cabeza;
        cabeza->anterior = ultimo;
    }

    delete aBorrar;
    std::cout << "Solicitud rechazada y eliminada de la lista exitosamente." << std::endl;
}

// Generar Reporte Graphviz para la Lista Circular Doblemente Enlazada
void ListaSolicitudes::generarReporteGraphviz() {
    if (cabeza == nullptr) {
        std::cout << "La lista de solicitudes está vacía. No hay reporte que generar." << std::endl;
        return;
    }

    std::ofstream archivo("reporte_solicitudes.dot");
    if (archivo.is_open()) {
        archivo << "digraph G {\n";
        archivo << "  rankdir=LR;\n"; // Left to Right
        archivo << "  node [shape=Mrecord, style=filled, fillcolor=bisque, fontname=\"Arial\"];\n";

        NodoSolicitud* actual = cabeza;

        // Primera pasada: crear los nodos
        do {
            archivo << "  \"" << actual->dato.id << "\" [label=\"{ID: " << actual->dato.id
                    << " | Cliente: " << actual->dato.cliente
                    << " | Tipo: " << actual->dato.tipo
                    << " | Estado: " << actual->dato.estado << "}\"];\n";
            actual = actual->siguiente;
        } while (actual != cabeza);

        // Segunda pasada: crear las conexiones dobles
        actual = cabeza;
        do {
            // Usamos dir=both para indicar que es doblemente enlazada en el grafo
            archivo << "  \"" << actual->dato.id << "\" -> \"" << actual->siguiente->dato.id << "\" [dir=both];\n";
            actual = actual->siguiente;
        } while (actual != cabeza);

        archivo << "}\n";
        archivo.close();

        // Compilar a PNG
        system("dot -Tpng reporte_solicitudes.dot -o reporte_solicitudes.png");
        std::cout << "Reporte generado: reporte_solicitudes.png" << std::endl;
    }
}

Solicitud* ListaSolicitudes::obtenerPrimeraSolicitud() {
    if (cabeza == nullptr) return nullptr;
    return &(cabeza->dato); // Retornamos la dirección de los datos de la cabeza
}

std::string ListaSolicitudes::consultarEstado(std::string telefono) {
    if (cabeza == nullptr) return "No hay solicitudes registradas.";

    NodoSolicitud* actual = cabeza;
    do {
        if (actual->dato.telefono == telefono) {
            return "Solicitud: " + actual->dato.tipo + " | Estado: " + actual->dato.estado;
        }
        actual = actual->siguiente;
    } while (actual != cabeza);

    return "No se encontró ninguna solicitud con ese teléfono.";
}