#ifndef ARBOLBCLIENTES_H
#define ARBOLBCLIENTES_H
#include <string>
#include <vector>
#include <iostream>

// Estructura del Cliente
struct Cliente {
    std::string id;            // Clave principal del Árbol B
    std::string nombre;
    std::string correo;
    std::string telefono;
    std::string password;
    std::string tipo;          // "cliente" o "admin"

    // Solo almacena referencias a sus reservas para evitar duplicidad[cite: 1]
    std::vector<std::string> codigos_reservas;
};

// Nodo del Árbol B (Orden 4)
struct NodoB {
    bool hoja;
    int numClaves;
    Cliente claves[3];         // Orden 4 - 1 = Máximo 3 claves por nodo
    NodoB* hijos[4];           // Máximo 4 hijos por nodo[cite: 1]

    NodoB(bool esHoja) {
        hoja = esHoja;
        numClaves = 0;
        for (int i = 0; i < 4; i++) {
            hijos[i] = nullptr;
        }
    }
};

// Clase principal Árbol B
class ArbolBClientes {
public:
    ArbolBClientes();
    std::vector<Cliente> obtenerClientes();
    void insertar(Cliente c);
    Cliente* buscar(std::string id);
    void listarClientes(); // Recorrido inorden para listados[cite: 1]
    void generarReporteGraphviz();
    void eliminar(std::string id);

    // Nuevos métodos para los recorridos de la tabla
    std::vector<Cliente> obtenerClientesInorden();
    std::vector<Cliente> obtenerClientesPreorden();
    std::vector<Cliente> obtenerClientesPostorden();

private:
    NodoB* raiz;
    int t; // Grado mínimo (t=2 para Orden 4)
    void obtenerClientesRecursivo(NodoB* nodo, std::vector<Cliente>& lista);
    // Métodos internos del Árbol B
    void dividirHijo(NodoB* padre, int i, NodoB* hijo);
    void insertarNoLleno(NodoB* nodo, Cliente c);
    Cliente* buscarRecursivo(NodoB* nodo, std::string id);
    void listarRecursivo(NodoB* nodo);

    // Auxiliares para Graphviz
    void generarNodosDot(NodoB* nodo, std::string& dotContent, int& contador);
    void generarEnlacesDot(NodoB* nodo, std::string& dotContent, int idActual, int& contador);
    void recorridoInordenRecursivo(NodoB* nodo, std::vector<Cliente>& lista);
    void recorridoPreordenRecursivo(NodoB* nodo, std::vector<Cliente>& lista);
    void recorridoPostordenRecursivo(NodoB* nodo, std::vector<Cliente>& lista);
    int buscarClave(NodoB* nodo, std::string id);
    void eliminarDeNodo(NodoB* nodo, std::string id);
    void removerDeHoja(NodoB* nodo, int idx);
    void removerDeNoHoja(NodoB* nodo, int idx);
    Cliente obtenerPredecesor(NodoB* nodo, int idx);
    Cliente obtenerSucesor(NodoB* nodo, int idx);
    void llenar(NodoB* nodo, int idx);
    void prestarDeAnterior(NodoB* nodo, int idx);
    void prestarDeSiguiente(NodoB* nodo, int idx);
    void fusionar(NodoB* nodo, int idx);
};

#endif // ARBOLBCLIENTES_H