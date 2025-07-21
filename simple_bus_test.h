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

  simple_bus_test.h : The test bench.

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef __simple_bus_test_h
#define __simple_bus_test_h

#include <systemc.h>
#include <vector>
#include <tuple>
#include <string>

#include "simple_bus_master_blocking.h"
#include "simple_bus_master_non_blocking.h"
#include "simple_bus_fast_mem.h"
#include "simple_bus.h"
#include "simple_bus_arbiter.h"
#include "logger.h"

SC_MODULE(simple_bus_test)
{
  // clock
  sc_clock C1;

  // Módulos
  std::vector<simple_bus_master_blocking*> masters_b;
  std::vector<simple_bus_master_non_blocking*> masters_nb;
  simple_bus_fast_mem *mem_fast1;
  simple_bus_fast_mem *mem_fast2;
  simple_bus *bus;
  simple_bus_arbiter *arbiter;
  Logger* logger;

  // Constructor parametrizado
  simple_bus_test(sc_module_name name,
                  std::vector<std::tuple<int, unsigned int, bool, unsigned int>> config_b,
                  std::vector<std::tuple<int, unsigned int, bool, unsigned int>> config_nb)
    : sc_module(name), C1("C1", 10, SC_NS)
  {
    // Logger
    logger = new Logger("logger");
    logger->open_next("ejecuciones", "bus_log_");
    logger->log("#Nombre,Prioridad,direccion,lock,descanso");

    // Bus y árbitro
    bus = new simple_bus("bus", true);
    arbiter = new simple_bus_arbiter("arbiter");

    // Memorias
    mem_fast1 = new simple_bus_fast_mem("mem_fast1", 0x000, 0x3FF);  // 64 bytes
    mem_fast2 = new simple_bus_fast_mem("mem_fast2", 0x400, 0x7FF);  // otros 64 bytes

    // Conexiones de reloj
    bus->clock(C1);

    // Crear masters bloqueantes
    for (size_t i = 0; i < config_b.size(); ++i) {
      auto [prioridad, addr, lock, count] = config_b[i];
      std::string name = "master_b_" + std::to_string(i);
      auto* m = new simple_bus_master_blocking(name.c_str(), prioridad, addr, lock, count);
      m->clock(C1);
      m->bus_port(*bus);
      masters_b.push_back(m);

      logger->log(name + "," + std::to_string(prioridad)+
      "," + std::to_string(addr)+"," + std::to_string(lock)+"," + std::to_string(count));
    }

    // Crear masters no bloqueantes
    for (size_t i = 0; i < config_nb.size(); ++i) {
      auto [prioridad, addr, lock, count] = config_nb[i];
      std::string name = "master_nb_" + std::to_string(i);
      auto* m = new simple_bus_master_non_blocking(name.c_str(), prioridad, addr, lock, count);
      m->clock(C1);
      m->bus_port(*bus);
      masters_nb.push_back(m);

      logger->log(name + "," + std::to_string(prioridad)+
      "," + std::to_string(addr)+"," + std::to_string(lock)+"," + std::to_string(count));
    }

    // Conexiones
    bus->slave_port(*mem_fast1);
    bus->slave_port(*mem_fast2);
    bus->arbiter_port(*arbiter);
    bus->set_logger(logger);
  }

  // Destructor
  ~simple_bus_test()
  {
    for (auto* m : masters_b) delete m;
    for (auto* m : masters_nb) delete m;
    if (mem_fast1) delete mem_fast1;
    if (mem_fast2) delete mem_fast2;
    if (bus) delete bus;
    if (arbiter) delete arbiter;
  }
};

#endif
