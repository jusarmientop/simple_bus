/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  simple_bus_main.cpp : sc_main

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <iomanip>
#include "systemc.h"
#include "simple_bus_test.h"

namespace fs = std::filesystem;

// 1. Crea el directorio base si no existe
void create_execution_directory(const std::string& path) {
    fs::create_directories(path);
}

// 2. Encuentra el siguiente nombre disponible para log
std::string get_next_log_filename(const std::string& path) {
    int max_num = 0;
    std::regex re(R"(bus_log_(\d{6})\.csv)");

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::smatch match;
            std::string name = entry.path().filename().string();
            if (std::regex_match(name, match, re)) {
                int num = std::stoi(match[1]);
                if (num > max_num) max_num = num;
            }
        }
    }

    std::ostringstream filename;
    filename << "bus_log_" << std::setfill('0') << std::setw(6) << (max_num + 1) << ".csv";
    return filename.str();
}

// 3. Abre el archivo de log
std::ofstream open_log_file(const std::string& full_path) {
    std::ofstream file(full_path);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir archivo de log: " + full_path);
    }
    return file;
}

// 4. Muestra la ruta del archivo
void print_log_path(const std::string& full_path) {
    std::cout << "Log de ejecución: " << full_path << std::endl;
}

// Función principal
int sc_main(int, char **) {
    std::string base_path = "ejecuciones";
    create_execution_directory(base_path);

    std::string filename = get_next_log_filename(base_path);
    std::string full_path = base_path + "/" + filename;

    std::ofstream log_file;
    try {
        log_file = open_log_file(full_path);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    print_log_path(full_path);

    // Aquí pasarías log_file si tu módulo lo necesita
    simple_bus_test top("top");

    sc_start(300, SC_NS);

    log_file.close();
    return 0;
}
