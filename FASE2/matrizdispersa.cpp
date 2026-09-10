#include "matrizdispersa.h"
#include <iostream>
#include <fstream>
#include <sstream>

MatrizDispersa::MatrizDispersa() {
    // La raíz es el ancla (0,0) de nuestra matriz
    raiz = new NodoMatriz(-1, -1, "RAIZ");
}

// Busca la cabecera de la fila, retorna nullptr si no existe
NodoMatriz* MatrizDispersa::buscarFila(int fila) {
    NodoMatriz* actual = raiz->abajo;
    while (actual != nullptr) {
        if (actual->fila == fila) return actual;
        actual = actual->abajo;
    }
    return nullptr;
}

// Busca la cabecera de la columna, retorna nullptr si no existe
NodoMatriz* MatrizDispersa::buscarColumna(int columna) {
    NodoMatriz* actual = raiz->derecha;
    while (actual != nullptr) {
        if (actual->columna == columna) return actual;
        actual = actual->derecha;
    }
    return nullptr;
}

// Crea e inserta una cabecera de fila en orden
NodoMatriz* MatrizDispersa::crearFila(int fila) {
    NodoMatriz* nuevaFila = new NodoMatriz(fila, -1, "CABECERA_FILA");
    NodoMatriz* actual = raiz;

    while (actual->abajo != nullptr && actual->abajo->fila < fila) {
        actual = actual->abajo;
    }

    nuevaFila->abajo = actual->abajo;
    if (actual->abajo != nullptr) {
        actual->abajo->arriba = nuevaFila;
    }
    nuevaFila->arriba = actual;
    actual->abajo = nuevaFila;

    return nuevaFila;
}

// Crea e inserta una cabecera de columna en orden
NodoMatriz* MatrizDispersa::crearColumna(int columna) {
    NodoMatriz* nuevaCol = new NodoMatriz(-1, columna, "CABECERA_COL");
    NodoMatriz* actual = raiz;

    while (actual->derecha != nullptr && actual->derecha->columna < columna) {
        actual = actual->derecha;
    }

    nuevaCol->derecha = actual->derecha;
    if (actual->derecha != nullptr) {
        actual->derecha->izquierda = nuevaCol;
    }
    nuevaCol->izquierda = actual;
    actual->derecha = nuevaCol;

    return nuevaCol;
}

// Método interno para insertar el nodo en el cruce exacto de fila y columna
void MatrizDispersa::insertarAsientoLibre(int fila, int columna) {
    // 1. Buscamos o creamos las cabeceras
    NodoMatriz* nodoFila = buscarFila(fila);
    if (nodoFila == nullptr) {
        nodoFila = crearFila(fila);
    }

    NodoMatriz* nodoColumna = buscarColumna(columna);
    if (nodoColumna == nullptr) {
        nodoColumna = crearColumna(columna);
    }

    // 2. Creamos el nuevo asiento con estado LIBRE
    NodoMatriz* nuevoAsiento = new NodoMatriz(fila, columna, "LIBRE");

    // 3. Inserción HORIZONTAL (en la fila)
    NodoMatriz* actualF = nodoFila;
    while (actualF->derecha != nullptr && actualF->derecha->columna < columna) {
        actualF = actualF->derecha;
    }
    nuevoAsiento->derecha = actualF->derecha;
    if (actualF->derecha != nullptr) {
        actualF->derecha->izquierda = nuevoAsiento;
    }
    nuevoAsiento->izquierda = actualF;
    actualF->derecha = nuevoAsiento;

    // 4. Inserción VERTICAL (en la columna)
    NodoMatriz* actualC = nodoColumna;
    while (actualC->abajo != nullptr && actualC->abajo->fila < fila) {
        actualC = actualC->abajo;
    }
    nuevoAsiento->abajo = actualC->abajo;
    if (actualC->abajo != nullptr) {
        actualC->abajo->arriba = nuevoAsiento;
    }
    nuevoAsiento->arriba = actualC;
    actualC->abajo = nuevoAsiento;
}


void MatrizDispersa::generarReporteGraphviz() {
    std::ofstream archivo("reporte_matriz.dot");
    if (!archivo.is_open()) {
        std::cout << "Error al crear el archivo DOT de la matriz." << std::endl;
        return;
    }

    int totalLibres = 0;
    int totalOcupados = 0;

    archivo << "digraph MatrizDispersa {\n";
    archivo << "  node [shape=box, fontname=\"Arial\"];\n";
    archivo << "  nodesep=0.5;\n";
    archivo << "  ranksep=0.5;\n";

    // Reemplaza la línea antigua del Nodo Raíz por esta:
    archivo << "  Raiz [label=\"Función:\\n" << peliculaActual << " - " << horarioActual << "\\nSala: " << salaActual << "\", style=filled, fillcolor=lightgray];\n";

    // --- 1. Generar Nodos de Cabecera de Columnas ---
    NodoMatriz* actualC = raiz->derecha;
    archivo << "  { rank=same; Raiz; ";
    while (actualC != nullptr) {
        archivo << "\"C" << actualC->columna << "\"; ";
        actualC = actualC->derecha;
    }
    archivo << "}\n";

    actualC = raiz->derecha;
    while (actualC != nullptr) {
        archivo << "  \"C" << actualC->columna << "\" [label=\"Col " << actualC->columna << "\", style=filled, fillcolor=lightblue];\n";
        if (actualC->derecha != nullptr) {
            archivo << "  \"C" << actualC->columna << "\" -> \"C" << actualC->derecha->columna << "\" [dir=both];\n";
        }
        actualC = actualC->derecha;
    }
    if (raiz->derecha != nullptr) {
        archivo << "  Raiz -> \"C" << raiz->derecha->columna << "\" [dir=both];\n";
    }

    // --- 2. Generar Nodos de Cabecera de Filas y Asientos ---
    NodoMatriz* actualF = raiz->abajo;
    while (actualF != nullptr) {
        // Cabecera de fila
        archivo << "  \"F" << actualF->fila << "\" [label=\"Fila " << actualF->fila << "\", style=filled, fillcolor=lightblue];\n";

        if (actualF->abajo != nullptr) {
            archivo << "  \"F" << actualF->fila << "\" -> \"F" << actualF->abajo->fila << "\" [dir=both];\n";
        } else if (actualF == raiz->abajo) {
            archivo << "  Raiz -> \"F" << actualF->fila << "\" [dir=both];\n"; // Conexión de raíz a primera fila
        }

        // Agrupar la fila actual para que salga horizontal
        archivo << "  { rank=same; \"F" << actualF->fila << "\"; ";

        NodoMatriz* asiento = actualF->derecha;
        while (asiento != nullptr) {
            std::string idNodo = "N_F" + std::to_string(asiento->fila) + "_C" + std::to_string(asiento->columna);
            archivo << "\"" << idNodo << "\"; ";

            // Determinar color y contar estadísticas
            std::string color = "lightgreen";
            if (asiento->cliente == "LIBRE") {
                totalLibres++;
            } else {
                color = "salmon";
                totalOcupados++;
            }

            // Crear el nodo del asiento
            archivo << "  \"" << idNodo << "\" [label=\"" << asiento->cliente << "\", style=filled, fillcolor=" << color << "];\n";

            // Enlaces horizontales
            if (asiento->izquierda == actualF) {
                archivo << "  \"F" << actualF->fila << "\" -> \"" << idNodo << "\" [dir=both];\n";
            } else {
                std::string idAnt = "N_F" + std::to_string(asiento->izquierda->fila) + "_C" + std::to_string(asiento->izquierda->columna);
                archivo << "  \"" << idAnt << "\" -> \"" << idNodo << "\" [dir=both];\n";
            }

            // Enlaces verticales
            if (asiento->arriba->fila == -1) { // Apunta a la cabecera de columna
                archivo << "  \"C" << asiento->columna << "\" -> \"" << idNodo << "\" [dir=both];\n";
            } else {
                std::string idArr = "N_F" + std::to_string(asiento->arriba->fila) + "_C" + std::to_string(asiento->arriba->columna);
                archivo << "  \"" << idArr << "\" -> \"" << idNodo << "\" [dir=both];\n";
            }

            asiento = asiento->derecha;
        }
        archivo << "}\n";
        actualF = actualF->abajo;
    }
    // Conexión inicial de raíz si hay filas
    if (raiz->abajo != nullptr) {
        archivo << "  Raiz -> \"F" << raiz->abajo->fila << "\" [dir=both];\n";
    }

    // --- 3. Estadísticas de la función ---
    int totalAsientos = totalLibres + totalOcupados;
    archivo << "  Stats [shape=note, label=\"ESTADÍSTICAS\\nTotal Asientos: " << totalAsientos
            << "\\nLibres: " << totalLibres
            << "\\nOcupados: " << totalOcupados << "\", style=filled, fillcolor=yellow];\n";

    archivo << "}\n";
    archivo.close();

    // Compilar a PNG
    int resultado = system("dot -Tpng reporte_matriz.dot -o reporte_matriz.png");
    if (resultado == 0) {
        std::cout << "Reporte de la matriz generado: reporte_matriz.png" << std::endl;
    }
}

// ==========================================
// ADAPTACIONES FASE 2: GESTIÓN DE MEMORIA Y RESERVAS
// ==========================================
void MatrizDispersa::vaciarMatriz() {
    // 1. Recorrer y eliminar todos los nodos internos y las cabeceras de fila
    NodoMatriz* filaActual = raiz->abajo;

    while (filaActual != nullptr) {
        NodoMatriz* actualValor = filaActual->derecha;

        // Borrar los asientos de la fila
        while (actualValor != nullptr) {
            NodoMatriz* aBorrar = actualValor;
            actualValor = actualValor->derecha;
            delete aBorrar;
        }

        // Borrar la cabecera de la fila
        NodoMatriz* filaBorrar = filaActual;
        filaActual = filaActual->abajo;
        delete filaBorrar;
    }

    // 2. Eliminar cabeceras de columnas
    NodoMatriz* colActual = raiz->derecha;
    while (colActual != nullptr) {
        NodoMatriz* colBorrar = colActual;
        colActual = colActual->derecha;
        delete colBorrar;
    }

    // 3. Reiniciar los punteros del nodo raíz para dejar la matriz limpia
    raiz->abajo = nullptr;
    raiz->derecha = nullptr;
}

void MatrizDispersa::inicializarSala(int totalFilas, int totalColumnas, std::string pelicula, std::string horario, std::string numSala) {
    vaciarMatriz(); // Limpieza obligatoria antes de cargar una nueva función

    peliculaActual = pelicula;
    horarioActual = horario;
    salaActual = numSala;

    for (int f = 1; f <= totalFilas; f++) {
        for (int c = 1; c <= totalColumnas; c++) {
            insertarAsientoLibre(f, c);
        }
    }
}

// Ahora recibe codigo_reserva en lugar del nombre del cliente
bool MatrizDispersa::reservarAsiento(int fila, int columna, std::string codigo_reserva) {
    NodoMatriz* nodoFila = buscarFila(fila);
    if (nodoFila == nullptr) return false;

    NodoMatriz* actual = nodoFila->derecha;
    while (actual != nullptr && actual->columna <= columna) {
        if (actual->columna == columna) {
            if (actual->cliente == "LIBRE") {
                actual->cliente = codigo_reserva; // Se almacena exclusivamente el código
                return true;
            }
            return false;
        }
        actual = actual->derecha;
    }
    return false;
}

bool MatrizDispersa::cancelarReserva(int fila, int columna) {
    NodoMatriz* nodoFila = buscarFila(fila);
    if (nodoFila == nullptr) return false;

    NodoMatriz* actual = nodoFila->derecha;
    while (actual != nullptr && actual->columna <= columna) {
        if (actual->columna == columna) {
            if (actual->cliente != "LIBRE") {
                actual->cliente = "LIBRE";
                return true;
            }
            return false;
        }
        actual = actual->derecha;
    }
    return false;
}

std::string MatrizDispersa::obtenerEstadoAsiento(int fila, int columna) {
    NodoMatriz* nodoFila = buscarFila(fila);
    if (nodoFila == nullptr) return "ND";

    NodoMatriz* actual = nodoFila->derecha;
    while (actual != nullptr && actual->columna <= columna) {
        if (actual->columna == columna) return actual->cliente; // Retorna "LIBRE" o el codigo_reserva
        actual = actual->derecha;
    }
    return "ND";
}

// ==========================================
// ADAPTACIONES FASE 2: PERSISTENCIA JSON
// ==========================================
void MatrizDispersa::guardarEnArchivo(std::string nombreArchivo, std::string codigoFuncion) {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    archivo << "{\n";
    archivo << "  \"codigo_funcion\": \"" << codigoFuncion << "\",\n";
    archivo << "  \"asientos_ocupados\": [\n";

    bool primerElemento = true;
    NodoMatriz* actualF = raiz->abajo;

    // Solo se guardan en el JSON las celdas que contienen reservas[cite: 1]
    while (actualF != nullptr) {
        NodoMatriz* actualC = actualF->derecha;
        while (actualC != nullptr) {
            if (actualC->cliente != "LIBRE") {
                if (!primerElemento) archivo << ",\n";
                archivo << "    {\n";
                archivo << "      \"fila\": " << actualC->fila << ",\n";
                archivo << "      \"columna\": " << actualC->columna << ",\n";
                archivo << "      \"codigo_reserva\": \"" << actualC->cliente << "\"\n";
                archivo << "    }";
                primerElemento = false;
            }
            actualC = actualC->derecha;
        }
        actualF = actualF->abajo;
    }

    archivo << "\n  ]\n}\n";
    archivo.close();
}

void MatrizDispersa::cargarDesdeArchivo(std::string nombreArchivo, int totalFilas, int totalColumnas, std::string pelicula, std::string horario, std::string numSala) {
    // 1. Pre-llenar la matriz con asientos libres para mantener la estructura visual requerida[cite: 1]
    inicializarSala(totalFilas, totalColumnas, pelicula, horario, numSala);

    // 2. Leer el JSON y actualizar únicamente los asientos ocupados[cite: 1]
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    std::string linea;
    int f = -1, c = -1;
    std::string codReserva = "";

    while (std::getline(archivo, linea)) {
        if (linea.find("\"fila\"") != std::string::npos) {
            f = std::stoi(linea.substr(linea.find(":") + 1));
        }
        else if (linea.find("\"columna\"") != std::string::npos) {
            c = std::stoi(linea.substr(linea.find(":") + 1));
        }
        else if (linea.find("\"codigo_reserva\"") != std::string::npos) {
            int inicio = linea.find("\"", linea.find(":") + 1) + 1;
            int fin = linea.find("\"", inicio);
            codReserva = linea.substr(inicio, fin - inicio);

            if (f != -1 && c != -1 && !codReserva.empty()) {
                reservarAsiento(f, c, codReserva);
                f = -1; c = -1; codReserva = "";
            }
        }
    }
    archivo.close();
}