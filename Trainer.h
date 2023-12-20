#pragma once

#include "LemonDrop.h"

namespace ControllerTrainer {

    const int generations = 100;  // number of generations
    const int genSize = 50;  // size of each generation
    const int genLength = 10;  // number of turns per generation
    const int newAgentsPerGen = 45;  // number of fresh controllers to generate for each generation

    void train();

}