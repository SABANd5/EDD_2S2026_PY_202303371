#ifndef LZW_H
#define LZW_H

#include <string>

class CompresorLZW {
public:
    static void comprimirYGuardar(const std::string& entrada, const std::string& rutaArchivo);
    static std::string leerYDescomprimir(const std::string& rutaArchivo);
};

#endif // LZW_H