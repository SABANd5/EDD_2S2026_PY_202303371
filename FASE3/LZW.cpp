#include "LZW.h"
#include <map>
#include <vector>
#include <fstream>
#include <cstdint>

void CompresorLZW::comprimirYGuardar(const std::string& entrada, const std::string& rutaArchivo) {
    std::map<std::string, int> diccionario;
    for (int i = 0; i < 256; i++) {
        diccionario[std::string(1, char(i))] = i;
    }

    std::string actual = "";
    std::vector<int> salida;
    int dictSize = 256;

    for (char c : entrada) {
        std::string combinado = actual + c;
        if (diccionario.find(combinado) != diccionario.end()) {
            actual = combinado;
        } else {
            salida.push_back(diccionario[actual]);
            if (dictSize < 65536) { // Límite seguro para 16 bits
                diccionario[combinado] = dictSize++;
            }
            actual = std::string(1, c);
        }
    }
    if (!actual.empty()) {
        salida.push_back(diccionario[actual]);
    }

    // Escribimos a archivo en formato binario asegurando código de 16 bits
    std::ofstream archivo(rutaArchivo, std::ios::binary);
    for (int codigo : salida) {
        uint16_t cod16 = static_cast<uint16_t>(codigo);
        archivo.write(reinterpret_cast<const char*>(&cod16), sizeof(cod16));
    }
    archivo.close();
}

std::string CompresorLZW::leerYDescomprimir(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo, std::ios::binary);
    if (!archivo.is_open()) return "";

    std::vector<int> comprimido;
    uint16_t cod16;
    while (archivo.read(reinterpret_cast<char*>(&cod16), sizeof(cod16))) {
        comprimido.push_back(static_cast<int>(cod16));
    }
    archivo.close();

    if (comprimido.empty()) return "";

    std::map<int, std::string> diccionario;
    for (int i = 0; i < 256; i++) {
        diccionario[i] = std::string(1, char(i));
    }

    int dictSize = 256;
    std::string actual = std::string(1, char(comprimido[0]));
    std::string resultado = actual;
    std::string entrada = "";

    for (size_t i = 1; i < comprimido.size(); i++) {
        int codigo = comprimido[i];
        if (diccionario.find(codigo) != diccionario.end()) {
            entrada = diccionario[codigo];
        } else if (codigo == dictSize) {
            entrada = actual + actual[0];
        } else {
            return ""; // Datos corruptos
        }

        resultado += entrada;
        if (dictSize < 65536) {
            diccionario[dictSize++] = actual + entrada[0];
        }
        actual = entrada;
    }
    return resultado;
}