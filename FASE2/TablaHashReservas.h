#ifndef TABLAHASHRESERVAS_H
#define TABLAHASHRESERVAS_H
#include <string>
#include <vector>

// Estructura de la Reserva
struct Reserva {
    std::string codigo_reserva;
    std::string id_cliente;
    std::string codigo_funcion;
    int fila;
    int columna;
    std::string fecha_reserva;
};

// Nodo para manejar colisiones mediante listas enlazadas
struct NodoHash {
    Reserva dato;
    NodoHash* siguiente;
    NodoHash(Reserva r) : dato(r), siguiente(nullptr) {}
};

// Clase principal Tabla Hash
class TablaHashReservas {
public:
    TablaHashReservas(int tamano = 37); // Tamaño inicial primo para mejor distribución

    void insertar(Reserva r);
    Reserva* buscar(std::string codigo_reserva);
    void eliminar(std::string codigo_reserva);
    void generarReporteGraphviz();
    void eliminarPorFuncion(std::string codigo_funcion);


private:
    int capacidad;
    int elementos;
    std::vector<NodoHash*> tabla;

    int funcionHash(std::string clave);
};

#endif // TABLAHASHRESERVAS_H