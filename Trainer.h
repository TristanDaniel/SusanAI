#pragma once

#include "LemonDrop.h"

namespace ControllerTrainer {

    const int generations = 50;  // number of generations
    const int genSize = 30;  // size of each generation
    const int bestToKeep = 5;
    const int newAgentsPerGen = genSize - bestToKeep;  // number of fresh controllers to generate for each generation
    const int genLength = 1200;  // number of turns per generation


    void train();

}