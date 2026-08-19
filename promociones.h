#ifndef PROMOCIONES_H
#define PROMOCIONES_H

#include <string>

// --- SUB-LISTA: BENEFICIOS (Doblemente Enlazada) ---
struct Beneficio {
    std::string tipo;
    std::string descripcion;
    std::string valor;
};

struct NodoBeneficio {
    Beneficio dato;
    NodoBeneficio* siguiente;
    NodoBeneficio* anterior;

    NodoBeneficio(Beneficio b) : dato(b), siguiente(nullptr), anterior(nullptr) {}
};

// --- LISTA PRINCIPAL: PROMOCIONES (Circular Simple) ---
struct Promocion {
    std::string codigo;
    std::string nombre;
    std::string vigencia;
    std::string dias;
};

struct NodoPromocion {
    Promocion dato;
    NodoBeneficio* cabezaBeneficios; // Puntero hacia la sub-lista
    NodoPromocion* siguiente;        // Puntero circular

    NodoPromocion(Promocion p) : dato(p), cabezaBeneficios(nullptr), siguiente(nullptr) {}
};

// --- CLASE GESTORA ---
class GestorPromociones {
public:
    GestorPromociones();

    // Métodos principales del administrador
    void registrarPromocion(std::string codigo, std::string nombre, std::string vigencia, std::string dias);
    void anadirBeneficio(std::string codigoPromo, std::string tipo, std::string descripcion, std::string valor);

    // Reporte visual
    void generarReporteGraphviz();
    // Añade esto a public:
    std::string obtenerTextoPromociones();

private:
    NodoPromocion* cabezaPromociones;

    // Método auxiliar
    NodoPromocion* buscarPromocion(std::string codigo);
};

#endif // PROMOCIONES_H