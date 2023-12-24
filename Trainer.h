#pragma once

#include "LemonDrop.h"

namespace ControllerTrainer {

    const int generations = 50;  // number of generations
    const int genSize = 25;  // size of each generation
    const int genLength = 400;  // number of turns per generation
    const int newAgentsPerGen = 20;  // number of fresh controllers to generate for each generation


    void train();

}