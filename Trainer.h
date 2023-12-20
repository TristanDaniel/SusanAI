#pragma once

#include "LemonDrop.h"

namespace ControllerTrainer {

    const int generations = 100;  // number of generations
    const int genSize = 15;  // size of each generation
    const int genLength = 100;  // number of turns per generation
    const int newAgentsPerGen = 10;  // number of fresh controllers to generate for each generation

    void train();

}