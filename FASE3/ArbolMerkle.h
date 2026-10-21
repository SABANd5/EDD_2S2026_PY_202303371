#ifndef ARBOLMERKLE_H
#define ARBOLMERKLE_H

#include <string>
#include <vector>

// Estructura de Transacción para la Blockchain
struct Transaccion {
    std::string codigo_transaccion;
    std::string codigo_reserva;
    std::string id_cliente;
    std::string codigo_funcion;
    int fila;
    int columna;
    double monto;
    std::string metodo_pago;
    std::string fecha_pago;
};

// Nodo del Árbol de Merkle
struct NodoMerkle {
    std::string hash;
    NodoMerkle* izquierdo;
    NodoMerkle* derecho;
    NodoMerkle(std::string h) : hash(h), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolMerkle {
public:
    ArbolMerkle();
    ~ArbolMerkle();

    std::string construirArbol(const std::vector<Transaccion>& transacciones);
    void generarReporte(std::string filename);

private:
    NodoMerkle* raiz;

    void limpiarArbol(NodoMerkle* nodo);
    std::string calcularHashSha256(const std::string& input);
    std::string transaccionAString(const Transaccion& t);
    void generarDotRecursivo(NodoMerkle* nodo, std::string& dot, int& idCounter);
};

#endif // ARBOLMERKLE_H