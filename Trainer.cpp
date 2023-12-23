
#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Trainer.h"

using namespace std;

void ControllerTrainer::train() {
    cout << "Train start" << endl;

    LemonDrop::Controller controllers[genSize];
    std::string baseName = "controller";

    for (int i = 0; i < genSize; i++) {
        string  name = baseName + "_g0_a" + to_string(i);
        controllers[i] = LemonDrop::Controller(name, true, true, false);
    }

    for (int i = 0; i < generations; i++) {
        cout << "!!!!!!!!!!!!!!!!!!! Starting Generation " + to_string(i) + " !!!!!!!!!!!!!!!!!!!";

        if (i != 0) {
            float minHighestFitness = -10000;
            LemonDrop::Controller minBestController;
            float highestFitness = -10000;
            LemonDrop::Controller bestController;

            vector<LemonDrop::Controller> bestControllers;
            int queueSize = genSize - newAgentsPerGen;

            for (LemonDrop::Controller controller : controllers) {
                float contFitness = controller.getFitness();

                if (bestControllers.size() < queueSize) {
                    bestControllers.push_back(controller);
                    if (contFitness < minHighestFitness) {
                        minHighestFitness = contFitness;
                        minBestController = controller;
                    }
                } else {
                    if (contFitness > minHighestFitness) {
                        bestControllers.push_back(controller);
                        minHighestFitness = contFitness;

                        bestControllers.erase(remove_if(bestControllers.begin(), bestControllers.end(), [&minBestController](LemonDrop::Controller c) { return c.getName() == minBestController.getName(); }), bestControllers.end());

                        minBestController = controller;
                        for (auto c : bestControllers) {
                            if (c.getFitness() < minHighestFitness) {
                                minBestController = c;
                                minHighestFitness = c.getFitness();
                            }
                        }
                    }
                }

                if (contFitness > highestFitness) {
                    highestFitness = contFitness;
                    bestController = controller;
                }
            }

            bestController.totalSave("best_g" + to_string(i-1) + "_" + to_string(bestController.getFitness()) + "_" + bestController.getName());

            int idx = 0;
            for (auto c : bestControllers) {
                controllers[idx++] = c;
            }

            for (int j = genSize - newAgentsPerGen; j < genSize; ++j) {
                string  name = baseName + "_g" + to_string(i) + "_a" + to_string(j);

                controllers[j] = LemonDrop::Controller(name, true, true, false);
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
    bestController.totalSave("best_final_" + to_string(bestController.getFitness()) + "_" + bestControllerName);
}