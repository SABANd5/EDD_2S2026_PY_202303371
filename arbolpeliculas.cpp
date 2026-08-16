#include "arbolpeliculas.h"
#include <iostream>
#include <fstream> // Necesario para generar el archivo .dot

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
        // Estilo base de los nodos según las características solicitadas
        archivo << "  node [shape=record, style=filled, fillcolor=lightgreen, fontname=\"Arial\"];\n";

        if (raiz != nullptr) {
            std::string dotContent = "";
            generarCuerpoDot(raiz, dotContent);
            archivo << dotContent;
        }

        archivo << "}\n";
        archivo.close();

        // Ejecutamos el comando de consola para convertir el .dot a .png
        // Asegúrate de tener Graphviz instalado y agregado al PATH de tu sistema
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
        // Armamos la estructura del nodo con los datos solicitados en el PDF
        dotContent += "  \"" + nodo->dato.codigo + "\" [label=\"{Código: " + nodo->dato.codigo +
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