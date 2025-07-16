#include "logger.h"
#include <filesystem>
#include <regex>
#include <iomanip>
#include <sstream>
#include <iostream>

void Logger::open_next(const std::string& folder, const std::string& prefix) {
    namespace fs = std::filesystem;

    // Crear carpeta si no existe
    fs::create_directories(folder);

    // Buscar archivos que coincidan con el patrón prefixNNNNNN.csv
    int max_num = 0;
    std::regex re(prefix + R"((\d{6})\.csv)");

    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            std::smatch match;
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, match, re)) {
                int num = std::stoi(match[1]);
                if (num > max_num) max_num = num;
            }
        }
    }

    // Generar nombre del nuevo archivo
    std::ostringstream filename;
    filename << prefix << std::setw(6) << std::setfill('0') << (max_num + 1) << ".csv";
    std::string full_path = folder + "/" + filename.str();

    // Abrir archivo
    file.open(full_path);
    if (!file.is_open()) {
        std::cerr << "Logger: No se pudo abrir el archivo: " << full_path << std::endl;
        SC_REPORT_ERROR("Logger", "Fallo al abrir archivo CSV.");
    } else {
        std::cout << "Logger: Archivo creado: " << full_path << std::endl;
    }
}
