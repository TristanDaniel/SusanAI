
#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <filesystem>

#include "Trainer.h"

using namespace std;

void ControllerTrainer::train() {
    cout << "Train start" << endl;

    LemonDrop::Controller* controllers[genSize];
    std::string baseName = "controller";

    cout << "Making first generation" << endl;
    for (int i = 0; i < genSize; i++) {
        string  name = baseName + "_g0_a" + to_string(i);
        controllers[i] = new LemonDrop::Controller(name, true, true, false);
        controllers[i]->resetFitness();
    }

    for (int i = 0; i < generations; i++) {
        cout << "!!!!!!!!!!!!!!!!!!! Starting Generation " + to_string(i) + " !!!!!!!!!!!!!!!!!!!\n";

        if (i != 0) {
            cout << "Processing gen " << to_string(i) << endl;
            float minHighestFitness = 1000000;
            LemonDrop::Controller* minBestController;
            //float highestFitness = -10000;
            //LemonDrop::Controller* bestController;

            vector<LemonDrop::Controller*> bestControllers;
            bestControllers.clear();
            int queueSize = genSize - newAgentsPerGen;

            for (auto controller : controllers) {
                float contFitness = controller->getSavedFitness();

                if (bestControllers.size() < queueSize) {
                    bestControllers.push_back(controller);
                    if (contFitness < minHighestFitness) {
                        minHighestFitness = contFitness;
                        minBestController = &*controller;
                    }
//                    if (contFitness > highestFitness) {
//                        highestFitness = contFitness;
//                        bestController = &*controller;
//                    }
                } else {
                    if (contFitness > minHighestFitness) {
                        bestControllers.push_back(controller);
                        minHighestFitness = contFitness;

                        bestControllers.erase(remove_if(bestControllers.begin(), bestControllers.end(),
                                                        [&minBestController](LemonDrop::Controller *c)
                                                        { return c->getName() == minBestController->getName(); }),bestControllers.end());
                        remove(("..\\" + minBestController->getName() + ".lsv").c_str());

                        minBestController = &*controller;
                        for (auto c : bestControllers) {
                            if (c->getSavedFitness() < minHighestFitness) {
                                minBestController = &*c;
                                minHighestFitness = c->getSavedFitness();
                            }
                        }
                    } else {
                        remove(("..\\" + controller->getName() + ".lsv").c_str());
                    }
                }

//                if (contFitness > highestFitness) {
//                    highestFitness = contFitness;
//                    bestController = &*controller;
//                }
            }


            if (i == (generations - 1)) continue;

            int idx = 0;
            for (auto cont : bestControllers) {
                string saveName = "best_g" + to_string(i-1) + "_" + to_string(cont->getSavedFitness()) + "_" + cont->getName();
                //cont->totalSave(saveName);
                filesystem::copy("..\\" + cont->getName() + ".lsv", "..\\" + saveName + ".lsv");
                remove(("..\\" + cont->getName() + ".lsv").c_str());

                controllers[idx] = new LemonDrop::Controller(cont->getName(), saveName, true, false);
                //controllers[idx]->resetFitness();
                controllers[idx++]->loadSavedData();
                //cont->totalSave(cont->getName() + "_g" + to_string(i-1));
            }

            //bestController->totalSave("best_g" + to_string(i-1) + "_" + to_string(bestController->getFitness()) + "_" + bestController->getName());

            for (int j = genSize - newAgentsPerGen; j < genSize; ++j) {
                string  name = baseName + "_g" + to_string(i) + "_a" + to_string(j);

                controllers[j] = new LemonDrop::Controller(name, true, true, false);
                controllers[j]->resetFitness();
            }
        }

        thread runThreads[genSize];
        for (int j = 0; j < genSize; j++) runThreads[j] = thread(*controllers[j], genLength);
        for (auto & runThread : runThreads) runThread.join();

    }

    float highestFitness = -10000;
    LemonDrop::Controller* bestController;

    for (auto controller : controllers) {
        float contFitness = controller->getSavedFitness();

        if (contFitness > highestFitness) {
            highestFitness = contFitness;
            bestController = &*controller;
        }
    }

    string  bestControllerName = bestController->getName();
    filesystem::copy("..\\" + bestControllerName + ".lsv", "..\\best_final_" + to_string(bestController->getSavedFitness()) + "_" + bestControllerName + ".lsv");
    //bestController->totalSave("best_final_" + to_string(bestController->getSavedFitness()) + "_" + bestControllerName);
}