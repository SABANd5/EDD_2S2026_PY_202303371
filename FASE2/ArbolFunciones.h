#ifndef ARBOLFUNCIONES_H
#define ARBOLFUNCIONES_H
#include <string>
#include <vector>

// Estructura para almacenar los datos de la función
struct Funcion {
    std::string codigo_funcion;    // Clave de indexación del AVL
    std::string codigo_pelicula;   // Para vincular con el BST de cartelera
    std::string horario;
    std::string sala;
    int filas;
    int columnas;
    std::string archivo_asientos;  // Se generará automáticamente, ej: "F001_funcion.json"[cite: 1]
    bool ya_realizada;             // Ayudará a pintar el nodo verde o rojo en Graphviz[cite: 1]
};

// Nodo del Árbol AVL
struct NodoAVL {
    Funcion dato;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura; // Atributo vital para calcular el factor de balanceo

    // Constructor del nodo
    NodoAVL(Funcion f) : dato(f), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

// Clase principal del Árbol AVL
class ArbolFunciones {
public:
    ArbolFunciones();

    // Métodos principales del administrador[cite: 1]
    void insertar(Funcion f);
    void eliminar(std::string codigo_funcion);
    void editar(std::string codigo_funcion, Funcion fModificada);
    Funcion* buscarFuncion(std::string codigo_funcion);

    // Listados solicitados en Fase 2[cite: 1]
    std::vector<Funcion> obtenerFuncionesPreorden();
    std::vector<Funcion> obtenerFuncionesInorden();
    std::vector<Funcion> obtenerFuncionesPostorden();

    // Generación del reporte
    void generarReporteGraphviz();


private:
    NodoAVL* raiz;

    // Métodos de utilidad exclusivos del AVL
    int obtenerAltura(NodoAVL* nodo);
    int obtenerFactorBalance(NodoAVL* nodo);
    void actualizarAltura(NodoAVL* nodo);

    // Mecanismos de balanceo (Rotaciones)
    NodoAVL* rotacionDerecha(NodoAVL* y);
    NodoAVL* rotacionIzquierda(NodoAVL* x);

    // Métodos recursivos auxiliares
    NodoAVL* insertarRecursivo(NodoAVL* nodo, Funcion f);
    NodoAVL* eliminarRecursivo(NodoAVL* nodo, std::string codigo_funcion);
    NodoAVL* encontrarMinimo(NodoAVL* nodo);

    // Recorridos auxiliares
    void recorridoPreorden(NodoAVL* nodo, std::vector<Funcion>& lista);
    void recorridoInorden(NodoAVL* nodo, std::vector<Funcion>& lista);
    void recorridoPostorden(NodoAVL* nodo, std::vector<Funcion>& lista);

    // Auxiliar de reporte
    void generarCuerpoDot(NodoAVL* nodo, std::string& dotContent);
    NodoAVL* obtenerMinimo(NodoAVL* nodo);
};

#endif // ARBOLFUNCIONES_H