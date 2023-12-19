#pragma once

#include "LemonDrop.h"

namespace ControllerTrainer {

    const int generations = 10;  // number of generations
    const int genSize = 10;  // size of each generation
    const int genLength = 100;  // number of turns per generation
    const int topToMove = 1;  // number of controllers from the previous generation to use when making the next generation
    const int newAgentsPerGen = 2;  // number of fresh controllers to generate for each generation


}