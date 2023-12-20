
#include <string>
#include <thread>
#include <iostream>

#include "Trainer.h"

using namespace std;

void ControllerTrainer::train() {
    cout << "Train start" << endl;

    LemonDrop::Controller controllers[genSize];
    std::string baseName = "controller";

    for (int i = 0; i < genSize; i++) {
        string  name = baseName + "_g0_a" + to_string(i);
        controllers[i] = LemonDrop::Controller(name, true);
    }

    for (int i = 0; i < generations; i++) {
        cout << "!!!!!!!!!!!!!!!!!!! Starting Generation " + to_string(i) + " !!!!!!!!!!!!!!!!!!!";

        if (i != 0) {
            float highestFitness = -10000;
            LemonDrop::Controller bestController;

            for (LemonDrop::Controller controller : controllers) {
                float contFitness = controller.getFitness();

                if (contFitness > highestFitness) {
                    highestFitness = contFitness;
                    bestController = controller;
                }
            }

            string  bestControllerName = bestController.getName();
            LemonDrop::Controller best("best_g" + to_string(i), bestControllerName);
            for (int j = 0; j < genSize - newAgentsPerGen; ++j) {
                string  name = baseName + "_g" + to_string(i) + "_a" + to_string(j);

                controllers[j] = LemonDrop::Controller(name, bestControllerName);
            }

            for (int j = genSize - newAgentsPerGen; j < genSize; ++j) {
                string  name = baseName + "_g" + to_string(i) + "_a" + to_string(j);

                controllers[j] = LemonDrop::Controller(name, true);
            }
        }

        thread runThreads[genSize];
        for (int j = 0; j < genSize; j++) runThreads[j] = thread(controllers[j], genLength);
        for (auto & runThread : runThreads) runThread.join();
    }

    float highestFitness = -10000;
    LemonDrop::Controller bestController;

    for (LemonDrop::Controller controller : controllers) {
        float contFitness = controller.getFitness();

        if (contFitness > highestFitness) {
            highestFitness = contFitness;
            bestController = controller;
        }
    }

    string  bestControllerName = bestController.getName();
    LemonDrop::Controller("best_final", bestControllerName);
}