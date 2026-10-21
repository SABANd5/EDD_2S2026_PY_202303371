#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "ArbolMerkle.h"
#include "TablaHashReservas.h"
#include <string>
#include <vector>

struct Bloque {
    int index;
    std::string timestamp;
    int nonce;
    std::vector<Transaccion> data;
    std::string rootMerkle;
    std::string previousHash;
    std::string hash;
    Bloque* siguiente;
    bool esValido;
};

class Blockchain {
public:
    Blockchain();
    void inicializar(TablaHashReservas* tabla);
    void agregarTransaccion(Transaccion t);
    void minarBloque();
    void generarReporteBlockchain();
    bool generarReporteMerkleBloque(int index);
    int pendientesSize();

private:
    Bloque* cabeza;
    std::vector<Transaccion> transaccionesPendientes;
    int indexCounter;
    int transaccionCounter;

    std::string calcularHash(int index, std::string timestamp, int nonce, std::string rootMerkle, std::string previousHash);
    void guardarBloqueDisco(Bloque* b);
    void leerBloquesDesdeDisco(TablaHashReservas* tabla);
};

#endif // BLOCKCHAIN_H