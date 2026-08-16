#ifndef LISTASOLICITUDES_H
#define LISTASOLICITUDES_H

#include <string>

// Estructura de la Solicitud
struct Solicitud {
    int id; // Autogenerado
    std::string cliente;
    std::string telefono;
    std::string tipo; // cumpleaños, aniversario, etc.
    std::string descripcion;
    std::string fecha;
    std::string estado; // Pendiente, En proceso, Atendida, Rechazada
};

// Nodo para la Lista Circular Doblemente Enlazada
struct NodoSolicitud {
    Solicitud dato;
    NodoSolicitud* siguiente;
    NodoSolicitud* anterior;

    NodoSolicitud(Solicitud s) : dato(s), siguiente(nullptr), anterior(nullptr) {}
};

class ListaSolicitudes {
public:
    ListaSolicitudes();
    void aprobarPrimeraSolicitud();
    void rechazarPrimeraSolicitud();
    void generarReporteGraphviz();
    void registrarSolicitud(std::string cliente, std::string telefono, std::string tipo, std::string descripcion, std::string fecha);
    void procesarPrimeraSolicitud(); // Muestra la primera en la cola
    // ... luego agregaremos los métodos de aprobar/rechazar

private:
    NodoSolicitud* cabeza;
    int contadorIds; // Para autogenerar el número de solicitud
};

#endif // LISTASOLICITUDES_H