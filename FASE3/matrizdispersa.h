#ifndef MATRIZDISPERSA_H
#define MATRIZDISPERSA_H

#include <string>

struct NodoMatriz {
    int fila;
    int columna;
    std::string cliente; // Almacenará "LIBRE" o el codigo_reserva

    NodoMatriz* arriba;
    NodoMatriz* abajo;
    NodoMatriz* izquierda;
    NodoMatriz* derecha;

    // Usaremos fila = -1 y columna = -1 para los nodos de cabecera y la raíz
    NodoMatriz(int f, int c, std::string cli = "LIBRE")
        : fila(f), columna(c), cliente(cli),
        arriba(nullptr), abajo(nullptr), izquierda(nullptr), derecha(nullptr) {}
};

class MatrizDispersa {
public:
    MatrizDispersa();

    // Genera la cuadrícula base de la sala
    void inicializarSala(int totalFilas, int totalColumnas, std::string pelicula, std::string horario, std::string numSala);

    // Se cambia nombreCliente por codigo_reserva
    bool reservarAsiento(int fila, int columna, std::string codigo_reserva);
    bool cancelarReserva(int fila, int columna);

    // Se eliminó cancelarPorCliente()

    void generarReporteGraphviz();
    std::string obtenerEstadoAsiento(int fila, int columna);

    // Nuevos métodos de la Fase 2
    void vaciarMatriz();
    void cargarDesdeArchivo(std::string nombreArchivo, int totalFilas, int totalColumnas, std::string pelicula, std::string horario, std::string numSala);
    void guardarEnArchivo(std::string nombreArchivo, std::string codigoFuncion);

private:
    NodoMatriz* raiz;

    // Métodos para gestionar las cabeceras
    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearFila(int fila);
    NodoMatriz* crearColumna(int columna);

    std::string peliculaActual;
    std::string horarioActual;
    std::string salaActual;

    // Método interno para cruzar punteros
    void insertarAsientoLibre(int fila, int columna);
};

#endif // MATRIZDISPERSA_H