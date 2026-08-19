#ifndef ARBOLPELICULAS_H
#define ARBOLPELICULAS_H
#include <vector>
#include <string>

// Estructura para almacenar los datos de la película
struct Pelicula {
    std::string codigo;
    std::string titulo;
    std::string genero;
    int duracion;
    std::string clasificacion;
    std::string idioma;
    std::string fechaEstreno;
    std::string fechaFin;
};

// Nodo del Árbol Binario de Búsqueda
struct NodoBST {
    Pelicula dato;
    NodoBST* izquierdo;
    NodoBST* derecho;

    // Constructor del nodo
    NodoBST(Pelicula p) : dato(p), izquierdo(nullptr), derecho(nullptr) {}
};

// Clase principal del Árbol
class ArbolPeliculas {
public:
    ArbolPeliculas();

    // Métodos principales del administrador
    void insertar(Pelicula p);
    void eliminar(std::string codigo);
    void cargarDesdeCSV(const std::string& rutaArchivo);
    // Generación del reporte
    void generarReporteGraphviz();
    std::vector<Pelicula> obtenerCarteleraInorden();
    // Añade esto a la sección public:
    Pelicula* buscarPelicula(std::string codigo);
    bool existePeliculaPorTitulo(std::string titulo);

private:
    NodoBST* raiz;

    // Métodos recursivos auxiliares
    NodoBST* insertarRecursivo(NodoBST* nodo, Pelicula p);
    NodoBST* eliminarRecursivo(NodoBST* nodo, std::string codigo);
    NodoBST* encontrarMinimo(NodoBST* nodo);
    void generarCuerpoDot(NodoBST* nodo, std::string& dotContent);
    void recorridoInorden(NodoBST* nodo, std::vector<Pelicula>& lista);
    bool buscarPorTituloRecursivo(NodoBST* nodo, std::string titulo);
};

#endif // ARBOLPELICULAS_H