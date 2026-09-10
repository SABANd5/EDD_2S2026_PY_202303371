#include "ArbolBClientes.h"
#include <fstream>

ArbolBClientes::ArbolBClientes() {
    raiz = new NodoB(true);
    t = 2; // Grado mínimo t=2 para un Árbol B de Orden 4 (máx 3 claves, 4 hijos)
}

// ==========================================
// BÚSQUEDA
// ==========================================
Cliente* ArbolBClientes::buscar(std::string id) {
    return buscarRecursivo(raiz, id);
}

Cliente* ArbolBClientes::buscarRecursivo(NodoB* nodo, std::string id) {
    int i = 0;
    while (i < nodo->numClaves && id > nodo->claves[i].id) {
        i++;
    }
    if (i < nodo->numClaves && id == nodo->claves[i].id) {
        return &(nodo->claves[i]);
    }
    if (nodo->hoja) {
        return nullptr;
    }
    return buscarRecursivo(nodo->hijos[i], id);
}

// ==========================================
// INSERCIÓN Y DIVISIÓN (ORDEN 4)
// ==========================================
void ArbolBClientes::insertar(Cliente c) {
    NodoB* r = raiz;
    // Si la raíz está llena (3 claves para orden 4)
    if (r->numClaves == 3) {
        NodoB* s = new NodoB(false);
        raiz = s;
        s->hijos[0] = r;
        dividirHijo(s, 0, r);
        insertarNoLleno(s, c);
    } else {
        insertarNoLleno(r, c);
    }
}

void ArbolBClientes::dividirHijo(NodoB* padre, int i, NodoB* hijoLleno) {
    NodoB* nuevoHijo = new NodoB(hijoLleno->hoja);
    nuevoHijo->numClaves = 1; // Se quedará con la clave de la derecha

    // Transferir la clave índice 2 al nuevo hijo
    nuevoHijo->claves[0] = hijoLleno->claves[2];

    // Transferir los hijos si no es hoja
    if (!hijoLleno->hoja) {
        nuevoHijo->hijos[0] = hijoLleno->hijos[2];
        nuevoHijo->hijos[1] = hijoLleno->hijos[3];
    }

    hijoLleno->numClaves = 1; // El hijo original se queda con 1 clave (índice 0)

    // Hacer espacio en el padre para el nuevo hijo
    for (int j = padre->numClaves; j >= i + 1; j--) {
        padre->hijos[j + 1] = padre->hijos[j];
    }
    padre->hijos[i + 1] = nuevoHijo;

    // Hacer espacio en el padre para la clave central que sube
    for (int j = padre->numClaves - 1; j >= i; j--) {
        padre->claves[j + 1] = padre->claves[j];
    }
    padre->claves[i] = hijoLleno->claves[1];
    padre->numClaves++;
}

void ArbolBClientes::insertarNoLleno(NodoB* nodo, Cliente c) {
    int i = nodo->numClaves - 1;

    if (nodo->hoja) {
        // Desplazar claves mayores hacia la derecha para hacer espacio
        while (i >= 0 && c.id < nodo->claves[i].id) {
            nodo->claves[i + 1] = nodo->claves[i];
            i--;
        }
        nodo->claves[i + 1] = c;
        nodo->numClaves++;
    } else {
        // Encontrar el hijo correspondiente
        while (i >= 0 && c.id < nodo->claves[i].id) {
            i--;
        }
        i++;
        // Si el hijo está lleno, dividirlo antes de bajar
        if (nodo->hijos[i]->numClaves == 3) {
            dividirHijo(nodo, i, nodo->hijos[i]);
            if (c.id > nodo->claves[i].id) {
                i++;
            }
        }
        insertarNoLleno(nodo->hijos[i], c);
    }
}

// ==========================================
// REPORTE GRAPHVIZ
// ==========================================
void ArbolBClientes::generarNodosDot(NodoB* nodo, std::string& dotContent, int& contador) {
    if (nodo == nullptr) return;

    int idActual = contador++;

    // Formato 'record' para simular compartimentos visuales
    dotContent += "  node" + std::to_string(idActual) + " [label=\"";

    for(int i = 0; i < nodo->numClaves; i++) {
        dotContent += "<f" + std::to_string(i) + "> | " + nodo->claves[i].id + "\\n" + nodo->claves[i].nombre + " | ";
    }
    dotContent += "<f" + std::to_string(nodo->numClaves) + ">\"];\n";

    if (!nodo->hoja) {
        for (int i = 0; i <= nodo->numClaves; i++) {
            generarNodosDot(nodo->hijos[i], dotContent, contador);
        }
    }
}

void ArbolBClientes::generarEnlacesDot(NodoB* nodo, std::string& dotContent, int idActual, int& contador) {
    if (nodo == nullptr || nodo->hoja) return;

    for (int i = 0; i <= nodo->numClaves; i++) {
        int idHijo = contador++;
        // Enlace desde el compartimento f(i) hacia el nodo hijo
        dotContent += "  \"node" + std::to_string(idActual) + "\":f" + std::to_string(i) + " -> \"node" + std::to_string(idHijo) + "\";\n";
        generarEnlacesDot(nodo->hijos[i], dotContent, idHijo, contador);
    }
}

void ArbolBClientes::generarReporteGraphviz() {
    std::ofstream archivo("reporte_arbolb.dot");
    if (archivo.is_open()) {
        std::string dotContent = "digraph ArbolBClientes {\n";
        dotContent += "  node [shape=record, style=filled, fillcolor=lightblue];\n";

        int contadorNodos = 0;
        int contadorEnlaces = 0;

        if (raiz->numClaves > 0) {
            generarNodosDot(raiz, dotContent, contadorNodos);
            generarEnlacesDot(raiz, dotContent, 0, contadorEnlaces);
        }

        dotContent += "}\n";
        archivo << dotContent;
        archivo.close();

        system("dot -Tpng reporte_arbolb.dot -o reporte_arbolb.png");
    }
}