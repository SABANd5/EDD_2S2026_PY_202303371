#ifndef GRAFOSEDES_H
#define GRAFOSEDES_H

#include <string>
#include <vector>
#include "ArbolFunciones.h"
// <-- INICIO CÓDIGO NUEVO
#include <queue>
#include <map>
// FIN CÓDIGO NUEVO -->

// Estructura de la Arista
struct Arista {
    std::string destino;
    int peso;
    Arista* siguiente;
    Arista(std::string d, int p) : destino(d), peso(p), siguiente(nullptr) {}
};

// Estructura del Vértice para representar la Lista de Adyacencia Manual
struct VerticeSede {
    std::string codigo;
    std::string nombre;
    std::string direccion;
    int cantidad_salas;
    std::string telefono;

    Arista* listaAdyacencia;
    VerticeSede* siguiente;

    VerticeSede(std::string cod, std::string nom, std::string dir, int salas, std::string tel)
        : codigo(cod), nombre(nom), direccion(dir), cantidad_salas(salas), telefono(tel), listaAdyacencia(nullptr), siguiente(nullptr) {}
};

class GrafoSedes {
public:
    GrafoSedes();

    void insertarSede(std::string cod, std::string nom, std::string dir, int salas, std::string tel);
    void recalcularGrafo(ArbolFunciones* avlFunciones);
    VerticeSede* buscarVertice(std::string codigo);
    // <-- INICIO CÓDIGO NUEVO (FASE 3 - RUBRICA)
    bool editarSede(std::string codigo, std::string nom, std::string dir, int salas, std::string tel);
    bool eliminarSede(std::string codigo);
    std::string buscarCaminoMinimoBFS(std::string origen, std::string destino);
    std::string obtenerRankingRelaciones();
    std::vector<std::string> obtenerPeliculasDeSede(std::string codigoSede, ArbolFunciones* avlFunciones);
    // FIN CÓDIGO NUEVO -->

    // Métodos para Graphviz requeridos por Fase 3
    void generarReporteGrafo();
    void generarReporteListaAdyacencia();
    // <-- INICIO CÓDIGO NUEVO
    std::vector<std::pair<std::string, int>> obtenerSedesSimilares(std::string codigoSede);
    // FIN CÓDIGO NUEVO -->

private:
    VerticeSede* cabeza; // Inicio de la lista de vértices

    void agregarOActualizarArista(VerticeSede* origen, std::string destino, int peso);
    void eliminarArista(VerticeSede* origen, std::string destino);
};

#endif // GRAFOSEDES_H