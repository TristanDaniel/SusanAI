#pragma once

#include "LemonDrop.h"

namespace ControllerTrainer {

    const int generations = 5;  // number of generations
    const int genSize = 20;  // size of each generation
    const int bestToKeep = 5;
    const int newAgentsPerGen = genSize - bestToKeep;  // number of fresh controllers to generate for each generation
    const int genLength = 750;  // number of turns per generation


    void train();

}