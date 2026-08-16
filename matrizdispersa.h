#ifndef MATRIZDISPERSA_H
#define MATRIZDISPERSA_H

#include <string>

struct NodoMatriz {
    int fila;
    int columna;
    std::string cliente; // "LIBRE" o el nombre del cliente

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
    void inicializarSala(int totalFilas, int totalColumnas);
    bool reservarAsiento(int fila, int columna, std::string nombreCliente);
    bool cancelarReserva(int fila, int columna);
    void cancelarPorCliente(std::string nombreCliente); // Extra útil
    void generarReporteGraphviz();

private:
    NodoMatriz* raiz;

    // Métodos para gestionar las cabeceras
    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearFila(int fila);
    NodoMatriz* crearColumna(int columna);

    // Método interno para cruzar punteros
    void insertarAsientoLibre(int fila, int columna);
};

#endif // MATRIZDISPERSA_H