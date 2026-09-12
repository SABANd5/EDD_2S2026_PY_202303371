#include "ArbolBClientes.h"
#include <fstream>
#include <iostream>

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

std::vector<Cliente> ArbolBClientes::obtenerClientes() {
    std::vector<Cliente> lista;
    if (raiz != nullptr && raiz->numClaves > 0) {
        obtenerClientesRecursivo(raiz, lista);
    }
    return lista;
}

void ArbolBClientes::obtenerClientesRecursivo(NodoB* nodo, std::vector<Cliente>& lista) {
    if (nodo == nullptr) return;
    int i;
    for (i = 0; i < nodo->numClaves; i++) {
        // Visitar hijo izquierdo antes de la clave
        if (!nodo->hoja) obtenerClientesRecursivo(nodo->hijos[i], lista);
        // Guardar la clave actual
        lista.push_back(nodo->claves[i]);
    }
    // Visitar el último hijo
    if (!nodo->hoja) obtenerClientesRecursivo(nodo->hijos[i], lista);
}

// INORDEN: Izquierda -> Raíz -> Derecha (Orden natural)
std::vector<Cliente> ArbolBClientes::obtenerClientesInorden() {
    std::vector<Cliente> lista;
    if (raiz != nullptr) recorridoInordenRecursivo(raiz, lista);
    return lista;
}

void ArbolBClientes::recorridoInordenRecursivo(NodoB* nodo, std::vector<Cliente>& lista) {
    if (nodo == nullptr) return;
    int i;
    for (i = 0; i < nodo->numClaves; i++) {
        if (!nodo->hoja) recorridoInordenRecursivo(nodo->hijos[i], lista);
        lista.push_back(nodo->claves[i]);
    }
    if (!nodo->hoja) recorridoInordenRecursivo(nodo->hijos[i], lista);
}

// PREORDEN: Raíz -> Izquierda -> Derecha
std::vector<Cliente> ArbolBClientes::obtenerClientesPreorden() {
    std::vector<Cliente> lista;
    if (raiz != nullptr) recorridoPreordenRecursivo(raiz, lista);
    return lista;
}

void ArbolBClientes::recorridoPreordenRecursivo(NodoB* nodo, std::vector<Cliente>& lista) {
    if (nodo == nullptr) return;
    // Primero guardamos todas las claves del nodo actual (Raíz)
    for (int i = 0; i < nodo->numClaves; i++) {
        lista.push_back(nodo->claves[i]);
    }
    // Luego recorremos los hijos de izquierda a derecha
    if (!nodo->hoja) {
        for (int i = 0; i <= nodo->numClaves; i++) {
            recorridoPreordenRecursivo(nodo->hijos[i], lista);
        }
    }
}

// POSTORDEN: Izquierda -> Derecha -> Raíz
std::vector<Cliente> ArbolBClientes::obtenerClientesPostorden() {
    std::vector<Cliente> lista;
    if (raiz != nullptr) recorridoPostordenRecursivo(raiz, lista);
    return lista;
}

void ArbolBClientes::recorridoPostordenRecursivo(NodoB* nodo, std::vector<Cliente>& lista) {
    if (nodo == nullptr) return;
    // Primero recorremos todos los hijos hasta el fondo
    if (!nodo->hoja) {
        for (int i = 0; i <= nodo->numClaves; i++) {
            recorridoPostordenRecursivo(nodo->hijos[i], lista);
        }
    }
    // Finalmente guardamos las claves del nodo
    for (int i = 0; i < nodo->numClaves; i++) {
        lista.push_back(nodo->claves[i]);
    }
}

// ==========================================
// ELIMINACIÓN DEL ÁRBOL B (ORDEN 4)
// ==========================================
void ArbolBClientes::eliminar(std::string id) {
    if (raiz == nullptr) return;

    eliminarDeNodo(raiz, id);

    // Si la raíz se queda vacía tras fusionar, reducimos la altura del árbol
    if (raiz->numClaves == 0) {
        NodoB* temp = raiz;
        if (raiz->hoja) {
            raiz = nullptr;
        } else {
            raiz = raiz->hijos[0];
        }
        delete temp;
    }
}

int ArbolBClientes::buscarClave(NodoB* nodo, std::string id) {
    int idx = 0;
    while (idx < nodo->numClaves && nodo->claves[idx].id < id) {
        ++idx;
    }
    return idx;
}

void ArbolBClientes::eliminarDeNodo(NodoB* nodo, std::string id) {
    int idx = buscarClave(nodo, id);

    // Caso 1: La clave está en el nodo actual
    if (idx < nodo->numClaves && nodo->claves[idx].id == id) {
        if (nodo->hoja) removerDeHoja(nodo, idx);
        else removerDeNoHoja(nodo, idx);
    }
    // Caso 2: La clave NO está en el nodo actual
    else {
        if (nodo->hoja) return; // La clave no existe en el árbol

        bool esUltimo = (idx == nodo->numClaves);

        // Balancear si el nodo hijo tiene el mínimo de claves (1)
        if (nodo->hijos[idx]->numClaves < 2) {
            llenar(nodo, idx);
        }

        if (esUltimo && idx > nodo->numClaves) {
            eliminarDeNodo(nodo->hijos[idx - 1], id);
        } else {
            eliminarDeNodo(nodo->hijos[idx], id);
        }
    }
}

void ArbolBClientes::removerDeHoja(NodoB* nodo, int idx) {
    for (int i = idx + 1; i < nodo->numClaves; ++i) {
        nodo->claves[i - 1] = nodo->claves[i];
    }
    nodo->numClaves--;
}

void ArbolBClientes::removerDeNoHoja(NodoB* nodo, int idx) {
    std::string id = nodo->claves[idx].id;

    if (nodo->hijos[idx]->numClaves >= 2) {
        Cliente pred = obtenerPredecesor(nodo, idx);
        nodo->claves[idx] = pred;
        eliminarDeNodo(nodo->hijos[idx], pred.id);
    } else if (nodo->hijos[idx + 1]->numClaves >= 2) {
        Cliente suc = obtenerSucesor(nodo, idx);
        nodo->claves[idx] = suc;
        eliminarDeNodo(nodo->hijos[idx + 1], suc.id);
    } else {
        fusionar(nodo, idx);
        eliminarDeNodo(nodo->hijos[idx], id);
    }
}

Cliente ArbolBClientes::obtenerPredecesor(NodoB* nodo, int idx) {
    NodoB* actual = nodo->hijos[idx];
    while (!actual->hoja) {
        actual = actual->hijos[actual->numClaves];
    }
    return actual->claves[actual->numClaves - 1];
}

Cliente ArbolBClientes::obtenerSucesor(NodoB* nodo, int idx) {
    NodoB* actual = nodo->hijos[idx + 1];
    while (!actual->hoja) {
        actual = actual->hijos[0];
    }
    return actual->claves[0];
}

void ArbolBClientes::llenar(NodoB* nodo, int idx) {
    if (idx != 0 && nodo->hijos[idx - 1]->numClaves >= 2) {
        prestarDeAnterior(nodo, idx);
    } else if (idx != nodo->numClaves && nodo->hijos[idx + 1]->numClaves >= 2) {
        prestarDeSiguiente(nodo, idx);
    } else {
        if (idx != nodo->numClaves) fusionar(nodo, idx);
        else fusionar(nodo, idx - 1);
    }
}

void ArbolBClientes::prestarDeAnterior(NodoB* nodo, int idx) {
    NodoB* hijo = nodo->hijos[idx];
    NodoB* hermano = nodo->hijos[idx - 1];

    for (int i = hijo->numClaves - 1; i >= 0; --i) hijo->claves[i + 1] = hijo->claves[i];
    if (!hijo->hoja) {
        for (int i = hijo->numClaves; i >= 0; --i) hijo->hijos[i + 1] = hijo->hijos[i];
    }

    hijo->claves[0] = nodo->claves[idx - 1];
    if (!hijo->hoja) hijo->hijos[0] = hermano->hijos[hermano->numClaves];

    nodo->claves[idx - 1] = hermano->claves[hermano->numClaves - 1];
    hijo->numClaves++;
    hermano->numClaves--;
}

void ArbolBClientes::prestarDeSiguiente(NodoB* nodo, int idx) {
    NodoB* hijo = nodo->hijos[idx];
    NodoB* hermano = nodo->hijos[idx + 1];

    hijo->claves[hijo->numClaves] = nodo->claves[idx];
    if (!hijo->hoja) hijo->hijos[hijo->numClaves + 1] = hermano->hijos[0];

    nodo->claves[idx] = hermano->claves[0];

    for (int i = 1; i < hermano->numClaves; ++i) hermano->claves[i - 1] = hermano->claves[i];
    if (!hermano->hoja) {
        for (int i = 1; i <= hermano->numClaves; ++i) hermano->hijos[i - 1] = hermano->hijos[i];
    }

    hijo->numClaves++;
    hermano->numClaves--;
}

void ArbolBClientes::fusionar(NodoB* nodo, int idx) {
    NodoB* hijo = nodo->hijos[idx];
    NodoB* hermano = nodo->hijos[idx + 1];

    hijo->claves[hijo->numClaves] = nodo->claves[idx];

    for (int i = 0; i < hermano->numClaves; ++i)
        hijo->claves[i + hijo->numClaves + 1] = hermano->claves[i];

    if (!hijo->hoja) {
        for (int i = 0; i <= hermano->numClaves; ++i)
            hijo->hijos[i + hijo->numClaves + 1] = hermano->hijos[i];
    }

    for (int i = idx + 1; i < nodo->numClaves; ++i)
        nodo->claves[i - 1] = nodo->claves[i];

    for (int i = idx + 2; i <= nodo->numClaves; ++i)
        nodo->hijos[i - 1] = nodo->hijos[i];

    hijo->numClaves += hermano->numClaves + 1;
    nodo->numClaves--;

    delete hermano;
}