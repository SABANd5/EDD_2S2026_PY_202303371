#include "arbolpeliculas.h"
#include <iostream>
#include <fstream> // Necesario para generar el archivo .dot
#include <sstream>
#include <QDate>
// Constructor
ArbolPeliculas::ArbolPeliculas() {
    raiz = nullptr;
}

// Método público de inserción
void ArbolPeliculas::insertar(Pelicula p) {
    raiz = insertarRecursivo(raiz, p);
}

// Método recursivo privado para insertar manteniendo el orden del BST
NodoBST* ArbolPeliculas::insertarRecursivo(NodoBST* nodo, Pelicula p) {
    // Si llegamos a un espacio vacío, creamos el nuevo nodo
    if (nodo == nullptr) {
        return new NodoBST(p);
    }

    // Comparamos los códigos alfanuméricamente para ordenar el árbol
    if (p.codigo < nodo->dato.codigo) {
        nodo->izquierdo = insertarRecursivo(nodo->izquierdo, p);
    } else if (p.codigo > nodo->dato.codigo) {
        nodo->derecho = insertarRecursivo(nodo->derecho, p);
    } else {
        // El código de la película ya existe (podrías manejar un update aquí en el futuro)
        std::cout << "Aviso: La película con código " << p.codigo << " ya se encuentra registrada." << std::endl;
    }

    return nodo;
}

// Método público para disparar la generación del reporte
void ArbolPeliculas::generarReporteGraphviz() {
    std::ofstream archivo("reporte_cartelera.dot");

    if (archivo.is_open()) {
        archivo << "digraph G {\n";
        archivo << "  rankdir=TB;\n"; // Top to Bottom

        // Retiramos el "fillcolor" global para que el color individual de cada nodo tenga efecto
        archivo << "  node [shape=record, style=filled, fontname=\"Arial\"];\n";

        if (raiz != nullptr) {
            std::string dotContent = "";
            generarCuerpoDot(raiz, dotContent);
            archivo << dotContent;
        } else {
            // Manejo de árbol vacío para que Graphviz no falle
            archivo << "  \"Vacio\" [label=\"Cartelera Vacía\", shape=box, fillcolor=white];\n";
        }

        archivo << "}\n";
        archivo.close();

        // Ejecutamos el comando de consola para convertir el .dot a .png
        int resultado = system("dot -Tpng reporte_cartelera.dot -o reporte_cartelera.png");

        if (resultado == 0) {
            std::cout << "Reporte generado con éxito: reporte_cartelera.png" << std::endl;
        } else {
            std::cout << "Error al compilar el archivo DOT. ¿Está Graphviz en el PATH?" << std::endl;
        }
    } else {
        std::cout << "Error al abrir/crear el archivo .dot" << std::endl;
    }
}

// Método recursivo para armar las conexiones y el diseño de los nodos en DOT
void ArbolPeliculas::generarCuerpoDot(NodoBST* nodo, std::string& dotContent) {
    if (nodo != nullptr) {

        // 1. Lógica para determinar el color (Verde o Amarillo)
        std::string colorHex = "\"#81C784\""; // Verde por defecto (En cartelera)

        // Convertimos el string de la fecha a un objeto QDate (Asumiendo formato yyyy-MM-dd)
        QDate fechaFin = QDate::fromString(QString::fromStdString(nodo->dato.fechaFin), "yyyy-MM-dd");

        if (fechaFin.isValid()) {
            // Calculamos cuántos días faltan para que la película salga de cartelera
            qint64 diasRestantes = QDate::currentDate().daysTo(fechaFin);

            // Si faltan 7 días o menos, se considera "próximo a retirar"
            if (diasRestantes <= 7) {
                colorHex = "\"#FFF59D\""; // Amarillo pastel
            }
        }

        // 2. Armamos la estructura del nodo aplicando el color calculado
        dotContent += "  \"" + nodo->dato.codigo + "\" [fillcolor=" + colorHex + ", label=\"{Código: " + nodo->dato.codigo +
                      " | Título: " + nodo->dato.titulo +
                      " | Duración: " + std::to_string(nodo->dato.duracion) + " min" +
                      " | Clasificación: " + nodo->dato.clasificacion + "}\"];\n";

        // Conexión con el hijo izquierdo
        if (nodo->izquierdo != nullptr) {
            dotContent += "  \"" + nodo->dato.codigo + "\" -> \"" + nodo->izquierdo->dato.codigo + "\";\n";
            generarCuerpoDot(nodo->izquierdo, dotContent);
        }

        // Conexión con el hijo derecho
        if (nodo->derecho != nullptr) {
            dotContent += "  \"" + nodo->dato.codigo + "\" -> \"" + nodo->derecho->dato.codigo + "\";\n";
            generarCuerpoDot(nodo->derecho, dotContent);
        }
    }
}

// Método para lectura del CSV
void ArbolPeliculas::cargarDesdeCSV(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    std::string linea;

    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo CSV en la ruta: " << rutaArchivo << std::endl;
        return;
    }

    // Leemos (y descartamos) la primera línea porque son los encabezados
    std::getline(archivo, linea);

    int peliculasCargadas = 0;

    // Leemos línea por línea hasta el final
    while (std::getline(archivo, linea)) {
        std::stringstream stream(linea);
        std::string item;
        Pelicula p;

        // Extraemos cada valor separado por coma
        std::getline(stream, p.codigo, ',');
        std::getline(stream, p.titulo, ',');
        std::getline(stream, p.genero, ',');

        // La duración es un int, extraemos como string y convertimos
        std::getline(stream, item, ',');
        try {
            p.duracion = std::stoi(item);
        } catch (...) {
            p.duracion = 0; // Por si hay algún error en el formato del número
        }

        std::getline(stream, p.clasificacion, ',');
        std::getline(stream, p.idioma, ',');
        std::getline(stream, p.fechaEstreno, ',');
        std::getline(stream, p.fechaFin, ',');

        // Insertamos en el BST
        insertar(p);
        peliculasCargadas++;
    }

    archivo.close();
    std::cout << "Carga masiva exitosa. Se cargaron " << peliculasCargadas << " peliculas." << std::endl;
}
// Método público
void ArbolPeliculas::eliminar(std::string codigo) {
    raiz = eliminarRecursivo(raiz, codigo);
    std::cout << "Intento de eliminacion de " << codigo << " finalizado." << std::endl;
}

// Método recursivo para eliminar
NodoBST* ArbolPeliculas::eliminarRecursivo(NodoBST* nodo, std::string codigo) {
    // Caso base: llegamos al final y no lo encontramos
    if (nodo == nullptr) {
        return nodo;
    }

    // Buscamos el nodo recorriendo el árbol
    if (codigo < nodo->dato.codigo) {
        nodo->izquierdo = eliminarRecursivo(nodo->izquierdo, codigo);
    } else if (codigo > nodo->dato.codigo) {
        nodo->derecho = eliminarRecursivo(nodo->derecho, codigo);
    } else {
        // ¡Lo encontramos! Ahora evaluamos los 3 casos:

        // Caso 1 y 2: Un solo hijo o ningún hijo (hoja)
        if (nodo->izquierdo == nullptr) {
            NodoBST* temp = nodo->derecho;
            delete nodo;
            return temp;
        } else if (nodo->derecho == nullptr) {
            NodoBST* temp = nodo->izquierdo;
            delete nodo;
            return temp;
        }

        // Caso 3: Tiene dos hijos
        // Buscamos el sucesor inorden (el nodo más pequeño del subárbol derecho)
        NodoBST* temp = encontrarMinimo(nodo->derecho);

        // Copiamos los datos del sucesor al nodo actual
        nodo->dato = temp->dato;

        // Eliminamos el sucesor en el subárbol derecho
        nodo->derecho = eliminarRecursivo(nodo->derecho, temp->dato.codigo);
    }
    return nodo;
}

// Método auxiliar para encontrar el nodo más a la izquierda (el más pequeño)
NodoBST* ArbolPeliculas::encontrarMinimo(NodoBST* nodo) {
    NodoBST* actual = nodo;
    while (actual && actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }
    return actual;
}

// Método público que devuelve el vector con las películas
std::vector<Pelicula> ArbolPeliculas::obtenerCarteleraInorden() {
    std::vector<Pelicula> listaPeliculas;
    recorridoInorden(raiz, listaPeliculas);
    return listaPeliculas;
}

// Método recursivo Inorden
void ArbolPeliculas::recorridoInorden(NodoBST* nodo, std::vector<Pelicula>& lista) {
    if (nodo != nullptr) {
        recorridoInorden(nodo->izquierdo, lista); // Visita hijo izquierdo
        lista.push_back(nodo->dato);              // Almacena la raíz
        recorridoInorden(nodo->derecho, lista);   // Visita hijo derecho
    }
}
// En arbolpeliculas.cpp:
Pelicula* ArbolPeliculas::buscarPelicula(std::string codigo) {
    NodoBST* actual = raiz;
    while (actual != nullptr) {
        if (codigo < actual->dato.codigo) {
            actual = actual->izquierdo;
        } else if (codigo > actual->dato.codigo) {
            actual = actual->derecho;
        } else {
            return &(actual->dato); // ¡Encontrada!
        }
    }
    return nullptr; // No existe
}

// Método público
bool ArbolPeliculas::existePeliculaPorTitulo(std::string titulo) {
    return buscarPorTituloRecursivo(raiz, titulo);
}

// Método recursivo
bool ArbolPeliculas::buscarPorTituloRecursivo(NodoBST* nodo, std::string titulo) {
    // Si llegamos a una hoja y no está, retornamos falso
    if (nodo == nullptr) {
        return false;
    }

    // Si la encontramos, retornamos verdadero inmediatamente
    if (nodo->dato.titulo == titulo) {
        return true;
    }

    // Buscamos en el lado izquierdo y luego en el derecho
    return buscarPorTituloRecursivo(nodo->izquierdo, titulo) ||
           buscarPorTituloRecursivo(nodo->derecho, titulo);
}

// ==========================================
// RECORRIDOS DEL ÁRBOL DE PELÍCULAS (BST)
// ==========================================

// PREORDEN: Raíz -> Izquierda -> Derecha
std::vector<Pelicula> ArbolPeliculas::obtenerCarteleraPreorden() {
    std::vector<Pelicula> lista;
    recorridoPreorden(raiz, lista);
    return lista;
}

void ArbolPeliculas::recorridoPreorden(NodoBST* nodo, std::vector<Pelicula>& lista) {
    if (nodo != nullptr) {
        lista.push_back(nodo->dato);
        recorridoPreorden(nodo->izquierdo, lista);
        recorridoPreorden(nodo->derecho, lista);
    }
}

// POSTORDEN: Izquierda -> Derecha -> Raíz
std::vector<Pelicula> ArbolPeliculas::obtenerCarteleraPostorden() {
    std::vector<Pelicula> lista;
    recorridoPostorden(raiz, lista);
    return lista;
}

void ArbolPeliculas::recorridoPostorden(NodoBST* nodo, std::vector<Pelicula>& lista) {
    if (nodo != nullptr) {
        recorridoPostorden(nodo->izquierdo, lista);
        recorridoPostorden(nodo->derecho, lista);
        lista.push_back(nodo->dato);
    }
}