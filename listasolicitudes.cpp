#include "listasolicitudes.h"
#include <iostream>

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