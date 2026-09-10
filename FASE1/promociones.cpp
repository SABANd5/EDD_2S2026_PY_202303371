#include "promociones.h"
#include <iostream>
#include <fstream>

GestorPromociones::GestorPromociones() {
    cabezaPromociones = nullptr;
}

// 1. Insertar en la Lista Circular Simple (Promociones)
void GestorPromociones::registrarPromocion(std::string codigo, std::string nombre, std::string vigencia, std::string dias) {
    Promocion nuevaPromo = {codigo, nombre, vigencia, dias};
    NodoPromocion* nuevoNodo = new NodoPromocion(nuevaPromo);

    if (cabezaPromociones == nullptr) {
        cabezaPromociones = nuevoNodo;
        cabezaPromociones->siguiente = cabezaPromociones; // Cierra el círculo apuntando a sí mismo
    } else {
        NodoPromocion* actual = cabezaPromociones;
        // Recorremos hasta encontrar el último nodo antes de dar la vuelta
        while (actual->siguiente != cabezaPromociones) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevoNodo;
        nuevoNodo->siguiente = cabezaPromociones; // El nuevo nodo cierra el círculo
    }
    std::cout << "Promocion '" << nombre << "' registrada exitosamente." << std::endl;
}

// Método auxiliar de búsqueda
NodoPromocion* GestorPromociones::buscarPromocion(std::string codigo) {
    if (cabezaPromociones == nullptr) return nullptr;
    NodoPromocion* actual = cabezaPromociones;
    do {
        if (actual->dato.codigo == codigo) return actual;
        actual = actual->siguiente;
    } while (actual != cabezaPromociones);
    return nullptr;
}

// 2. Insertar en la Sub-Lista Doblemente Enlazada (Beneficios)
void GestorPromociones::anadirBeneficio(std::string codigoPromo, std::string tipo, std::string descripcion, std::string valor) {
    NodoPromocion* promo = buscarPromocion(codigoPromo);
    if (promo == nullptr) {
        std::cout << "Error: No se encontro una promocion con el codigo " << codigoPromo << std::endl;
        return;
    }

    Beneficio nuevoBeneficio = {tipo, descripcion, valor};
    NodoBeneficio* nuevoNodo = new NodoBeneficio(nuevoBeneficio);

    // Inserción al final de la lista doble
    if (promo->cabezaBeneficios == nullptr) {
        promo->cabezaBeneficios = nuevoNodo;
    } else {
        NodoBeneficio* actual = promo->cabezaBeneficios;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevoNodo;
        nuevoNodo->anterior = actual; // Enlace hacia atrás
    }
    std::cout << "Beneficio añadido a la promocion " << codigoPromo << "." << std::endl;
}

// 3. Generar Reporte de Lista Circular de Listas
void GestorPromociones::generarReporteGraphviz() {
    if (cabezaPromociones == nullptr) {
        std::cout << "No hay promociones para graficar." << std::endl;
        return;
    }

    std::ofstream archivo("reporte_promociones.dot");
    if (archivo.is_open()) {
        archivo << "digraph G {\n";
        archivo << "  rankdir=TB;\n";
        archivo << "  node [shape=record, fontname=\"Arial\"];\n";

        // Agrupar todas las promociones en la misma línea horizontal
        archivo << "  { rank=same; ";
        NodoPromocion* actualPromo = cabezaPromociones;
        do {
            archivo << "\"" << actualPromo->dato.codigo << "\"; ";
            actualPromo = actualPromo->siguiente;
        } while (actualPromo != cabezaPromociones);
        archivo << "}\n";

        // Crear nodos de promoción y los enlaces circulares
        actualPromo = cabezaPromociones;
        do {
            archivo << "  \"" << actualPromo->dato.codigo
                    << "\" [label=\"{" << actualPromo->dato.codigo << " | "
                    << actualPromo->dato.nombre << " | " << actualPromo->dato.vigencia
                    << "}\", style=filled, fillcolor=lightcyan];\n";

            archivo << "  \"" << actualPromo->dato.codigo << "\" -> \""
                    << actualPromo->siguiente->dato.codigo << "\";\n";
            actualPromo = actualPromo->siguiente;
        } while (actualPromo != cabezaPromociones);

        // Crear sub-listas de beneficios para cada promoción
        actualPromo = cabezaPromociones;
        do {
            if (actualPromo->cabezaBeneficios != nullptr) {
                NodoBeneficio* actualBen = actualPromo->cabezaBeneficios;
                std::string prevNode = actualPromo->dato.codigo;
                int bCount = 0;

                // Conexión inicial de la promo hacia su primer beneficio (punteada)
                std::string primerBen = prevNode + "_B0";
                archivo << "  \"" << prevNode << "\" -> \"" << primerBen << "\" [style=dashed];\n";

                while (actualBen != nullptr) {
                    std::string currNode = prevNode + "_B" + std::to_string(bCount);

                    archivo << "  \"" << currNode << "\" [label=\"{" << actualBen->dato.tipo
                            << " | " << actualBen->dato.descripcion
                            << " | Valor: " << actualBen->dato.valor
                            << "}\", style=filled, fillcolor=lightyellow];\n";

                    if (actualBen->siguiente != nullptr) {
                        std::string nextNode = prevNode + "_B" + std::to_string(bCount + 1);
                        // Doble flecha para indicar lista doblemente enlazada
                        archivo << "  \"" << currNode << "\" -> \"" << nextNode << "\" [dir=both];\n";
                    }
                    actualBen = actualBen->siguiente;
                    bCount++;
                }
            }
            actualPromo = actualPromo->siguiente;
        } while (actualPromo != cabezaPromociones);

        archivo << "}\n";
        archivo.close();

        // Ejecutar Graphviz
        system("dot -Tpng reporte_promociones.dot -o reporte_promociones.png");
        std::cout << "Reporte de Promociones generado: reporte_promociones.png" << std::endl;
    }
}
std::string GestorPromociones::obtenerTextoPromociones() {
    if (cabezaPromociones == nullptr) return "No hay promociones activas por el momento.";

    std::string res = "";
    NodoPromocion* actual = cabezaPromociones;

    do {
        res += "🔥 " + actual->dato.nombre + " (" + actual->dato.dias + ")\n";
        res += "Vigencia: " + actual->dato.vigencia + "\nBeneficios:\n";

        NodoBeneficio* ben = actual->cabezaBeneficios;
        if (ben == nullptr) res += "  - Sin beneficios asignados\n";

        while (ben != nullptr) {
            res += "  ✓ " + ben->dato.tipo + ": " + ben->dato.descripcion + " (" + ben->dato.valor + ")\n";
            ben = ben->siguiente;
        }
        res += "--------------------------------------\n";

        actual = actual->siguiente;
    } while (actual != cabezaPromociones);

    return res;
}