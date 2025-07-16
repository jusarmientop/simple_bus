#ifndef __LOGGER_H
#define __LOGGER_H

#include <systemc.h>
#include <fstream>
#include <string>

SC_MODULE(Logger)
{
  std::ofstream file;

  SC_CTOR(Logger) {}

  // Abre el siguiente archivo numerado en la carpeta dada
  void open_next(const std::string& folder, const std::string& prefix);

  // Escribe una línea en el archivo
  void log(const std::string& message) {
    if (file.is_open()) {
      file << message << std::endl;
    }
  }

  // Cierra el archivo si está abierto
  void close() {
    if (file.is_open()) {
      file.close();
    }
  }

  ~Logger() {
    close();
  }
};

#endif
