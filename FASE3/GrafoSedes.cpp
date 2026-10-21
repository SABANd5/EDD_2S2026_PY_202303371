#include "GrafoSedes.h"
#include <iostream>
#include <fstream>
#include <algorithm>

GrafoSedes::GrafoSedes() {
    cabeza = nullptr;
}

void GrafoSedes::insertarSede(std::string cod, std::string nom, std::string dir, int salas, std::string tel) {
    if (buscarVertice(cod) != nullptr) return; // Ya existe

    VerticeSede* nuevo = new VerticeSede(cod, nom, dir, salas, tel);
    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else {
        VerticeSede* aux = cabeza;
        while (aux->siguiente != nullptr) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
}

VerticeSede* GrafoSedes::buscarVertice(std::string codigo) {
    VerticeSede* aux = cabeza;
    while (aux != nullptr) {
        if (aux->codigo == codigo) return aux;
        aux = aux->siguiente;
    }
    return nullptr;
}

std::vector<std::string> GrafoSedes::obtenerPeliculasDeSede(std::string codigoSede, ArbolFunciones* avlFunciones) {
    std::vector<std::string> peliculasUnicas;
    // Extraemos todas las funciones y filtramos manualmente para no cambiar el AVL
    std::vector<Funcion> todasFunciones = avlFunciones->obtenerFuncionesInorden();

    for (const Funcion& f : todasFunciones) {
        if (f.sede == codigoSede) {
            if (std::find(peliculasUnicas.begin(), peliculasUnicas.end(), f.codigo_pelicula) == peliculasUnicas.end()) {
                peliculasUnicas.push_back(f.codigo_pelicula);
            }
        }
    }
    return peliculasUnicas;
}

void GrafoSedes::agregarOActualizarArista(VerticeSede* origen, std::string destino, int peso) {
    if (peso <= 0) {
        eliminarArista(origen, destino);
        return;
    }

    Arista* aux = origen->listaAdyacencia;
    while (aux != nullptr) {
        if (aux->destino == destino) {
            aux->peso = peso; // Actualizar peso si cambian las funciones compartidas
            return;
        }
        aux = aux->siguiente;
    }

    // No existe, crear nueva arista
    Arista* nueva = new Arista(destino, peso);
    if (origen->listaAdyacencia == nullptr) {
        origen->listaAdyacencia = nueva;
    } else {
        Arista* temp = origen->listaAdyacencia;
        while (temp->siguiente != nullptr) {
            temp = temp->siguiente;
        }
        temp->siguiente = nueva;
    }
}

void GrafoSedes::eliminarArista(VerticeSede* origen, std::string destino) {
    if (origen->listaAdyacencia == nullptr) return;

    if (origen->listaAdyacencia->destino == destino) {
        Arista* temp = origen->listaAdyacencia;
        origen->listaAdyacencia = origen->listaAdyacencia->siguiente;
        delete temp;
        return;
    }

    Arista* actual = origen->listaAdyacencia;
    Arista* anterior = nullptr;

    while (actual != nullptr && actual->destino != destino) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual != nullptr) {
        anterior->siguiente = actual->siguiente;
        delete actual;
    }
}

void GrafoSedes::recalcularGrafo(ArbolFunciones* avlFunciones) {
    VerticeSede* v1 = cabeza;
    while (v1 != nullptr) {
        std::vector<std::string> peliculasV1 = obtenerPeliculasDeSede(v1->codigo, avlFunciones);

        VerticeSede* v2 = cabeza;
        while (v2 != nullptr) {
            if (v1->codigo != v2->codigo) {
                std::vector<std::string> peliculasV2 = obtenerPeliculasDeSede(v2->codigo, avlFunciones);

                int peliculasEnComun = 0;
                for (const std::string& p1 : peliculasV1) {
                    if (std::find(peliculasV2.begin(), peliculasV2.end(), p1) != peliculasV2.end()) {
                        peliculasEnComun++;
                    }
                }
                // Actualiza la conexión en ambas direcciones (Grafo no dirigido)
                agregarOActualizarArista(v1, v2->codigo, peliculasEnComun);
            }
            v2 = v2->siguiente;
        }
        v1 = v1->siguiente;
    }
}

void GrafoSedes::generarReporteGrafo() {
    std::ofstream archivo("reporte_grafo.dot");
    if (archivo.is_open()) {
        archivo << "graph GrafoSedes {\n";
        archivo << "  rankdir=LR;\n";
        archivo << "  node [shape=circle, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n";

        VerticeSede* v = cabeza;
        while (v != nullptr) {
            archivo << "  \"" << v->codigo << "\" [label=\"" << v->codigo << "\\n" << v->nombre << "\"];\n";

            Arista* a = v->listaAdyacencia;
            while (a != nullptr) {
                // Para no dibujar la misma línea bidireccional dos veces en DOT:
                if (v->codigo < a->destino) {
                    archivo << "  \"" << v->codigo << "\" -- \"" << a->destino
                            << "\" [label=\"" << a->peso << "\", weight=\"" << a->peso << "\"];\n";
                }
                a = a->siguiente;
            }
            v = v->siguiente;
        }
        archivo << "}\n";
        archivo.close();
        system("dot -Tpng reporte_grafo.dot -o reporte_grafo.png");
    }
}

void GrafoSedes::generarReporteListaAdyacencia() {
    std::ofstream archivo("reporte_adyacencia.dot");
    if (archivo.is_open()) {
        archivo << "digraph ListaAdyacencia {\n";
        archivo << "  rankdir=LR;\n";
        archivo << "  node [shape=record, style=filled, fontname=\"Arial\"];\n";

        VerticeSede* v = cabeza;
        while (v != nullptr) {
            archivo << "  \"V_" << v->codigo << "\" [label=\"" << v->codigo << "\", fillcolor=\"#81D4FA\"];\n";

            Arista* a = v->listaAdyacencia;
            std::string nodoActual = "V_" + v->codigo;

            int contador = 0;
            while (a != nullptr) {
                std::string idNodoArista = "A_" + v->codigo + "_" + std::to_string(contador);
                archivo << "  \"" << idNodoArista << "\" [label=\"{Destino: " << a->destino << " | Peso: " << a->peso << "}\", fillcolor=\"#FFF59D\"];\n";
                archivo << "  \"" << nodoActual << "\" -> \"" << idNodoArista << "\";\n";

                nodoActual = idNodoArista;
                a = a->siguiente;
                contador++;
            }
            v = v->siguiente;
        }
        archivo << "}\n";
        archivo.close();
        system("dot -Tpng reporte_adyacencia.dot -o reporte_adyacencia.png");
    }
}

// <-- INICIO CÓDIGO NUEVO
bool compararPesos(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    return a.second > b.second; // Orden descendente por cantidad de películas en común
}

std::vector<std::pair<std::string, int>> GrafoSedes::obtenerSedesSimilares(std::string codigoSede) {
    std::vector<std::pair<std::string, int>> similares;
    VerticeSede* v = buscarVertice(codigoSede);

    if (v != nullptr) {
        Arista* a = v->listaAdyacencia;
        while(a != nullptr) {
            similares.push_back({a->destino, a->peso});
            a = a->siguiente;
        }
    }
    // Ordenamos para que las de mayor peso (más similares) queden al principio
    std::sort(similares.begin(), similares.end(), compararPesos);
    return similares;
}
// FIN CÓDIGO NUEVO -->