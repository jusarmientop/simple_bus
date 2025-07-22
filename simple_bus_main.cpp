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
#include <systemc>
#include <tuple>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include "simple_bus_test.h"

int sc_main(int argc, char* argv[])
{
  std::srand(std::time(nullptr));

  auto random_bool = []() {
    return std::rand() % 2 == 0;
  };

  // Prioridades únicas entre 1 y 4
  std::vector<int> priorities = {1, 2, 3, 4};
  std::shuffle(priorities.begin(), priorities.end(), std::default_random_engine(std::time(nullptr)));


  auto random_size = []() {
    int sizes[] = { 4, 8, 16, 32, 64, 128, 256, 512};
    return sizes[std::rand() % 4];
  };

  auto random_base_address = []() {
    return (std::rand() % 2 == 0) ? 0x000 : 0x400; // mem_fast1 o mem_fast2
  };

  // 2 masters bloqueantes
  std::vector<std::tuple<int, unsigned int, bool, unsigned int>> config_b = {
      {priorities[0], random_base_address(), random_bool(), random_size()},
      {priorities[1], random_base_address(), random_bool(), random_size()}
  };

  // 2 masters no bloqueantes
  std::vector<std::tuple<int, unsigned int, bool, unsigned int>> config_nb = {
      {priorities[2], random_base_address(), random_bool(), random_size()},
      {priorities[3], random_base_address(), random_bool(), random_size()}
  };

  simple_bus_test tb("tb", config_b, config_nb);

  sc_core::sc_start(30000, sc_core::SC_NS);

  return 0;
}


