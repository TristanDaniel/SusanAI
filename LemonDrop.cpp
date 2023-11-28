
#include <windows.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "LemonDrop.h"
#include "Utils.h"

using namespace std;

using LemonDrop::Controller;

Controller::Controller() {
    nodes.InitHandler();
    outputs.InitHandler();
    synapses.InitHandler();
}

bool Controller::newNode(unsigned int type, ParamPackages::NodeParams params) {

    unsigned int id;
    Nodes::Node* n;

    switch (type) {
        case 0:
        {
            //basic node
            id = nodes.getNextID();

            n = new Nodes::NotInputNode(id);

            n->addFlag(params.basicNodeParams.cycleFlag);

            nodes.addNode(n);

            saveActionToFile(n->saveNode());

            return true;
        }

        case 1:
        {
            //basic input
            id = nodes.getNextID();

            n = new Nodes::Input(id);

            n->setValue(params.basicNodeParams.value);

            nodes.addNode(n);

            saveActionToFile(n->saveNode());

            return true;
        }

        case 2:
        {
            //Random input
            id = nodes.getNextID();

            int mode = params.randInputParams.mode;
            float min = params.randInputParams.min;
            float max = params.randInputParams.max;

            n = new Nodes::RandomInput(id, mode, min, max);

            n->addFlag(params.randInputParams.cycleFlag);

            nodes.addNode(n);

            saveActionToFile(n->saveNode());

            return true;
        }

        default:
            return false;
    }
}

bool Controller::newOutput(unsigned int type, ParamPackages::NodeParams params) {
    unsigned int id;
    Nodes::Output* n;

    switch (type) {
        case 0:
        {
            //basic output
            id = nodes.getNextID();

            n = new Nodes::Output(id);

            n->addFlag(params.basicNodeParams.cycleFlag);

            outputs.addNode(n);
            nodes.addNode(n);

            saveActionToFile(n->saveNode());

            return true;
        }

        default:
            return false;
    }
}

bool Controller::newSynapse(unsigned int type, ParamPackages::SynapseParams params) {
    unsigned int id;
    Synapses::Synapse* s;

    switch (type) {
        case 0:
        {
            //passthrough
            id = synapses.getNextID();

            s = new Synapses::PassthroughSynapse(id);

            synapses.addSynapse(s);

            saveActionToFile(s->saveSynapse());

            return true;
        }


        case 1:
        {
            //weighted
            id = synapses.getNextID();
            float weight = params.weightedSynapseParams.weight;

            s = new Synapses::WeightedSynapse(id, weight);

            synapses.addSynapse(s);

            saveActionToFile(s->saveSynapse());

            return true;
        }

        default:
            return false;


    }
}

bool Controller::addSynapseToNode(unsigned int synID, unsigned int nodeID) {
    Synapses::Synapse* synapse = synapses.getSynapseByID(synID);
    Nodes::Node* node = nodes.getNodeByID(nodeID);

    node->addSynapse(synapse);

    saveActionToFile(">sn,");

    return true;
}

bool Controller::addNodeToSynapse(unsigned int nodeID, unsigned int synID) {
    Synapses::Synapse* synapse = synapses.getSynapseByID(synID);
    Nodes::Node* node = nodes.getNodeByID(nodeID);

    synapse->setInput(node);



    return true;
}

void Controller::getAllOutputs() {
    for (auto* output : outputs.getNodes()) {
        ((Nodes::Output*)output)->getOutput();
    }
}

[[noreturn]] void Controller::mainLoop() {
    while (true) {
        std::cout << std::to_string((int)Flags::NodeFlag::PARTIAL_ON_CYCLE);

        getAllOutputs();
        sleep(1);
    }
}

void Controller::saveActionToFile(const std::string& s) {
    std::ofstream saveFile("controller.lds", std::ios::app);

    if (saveFile.is_open()) {
        saveFile << s;
        saveFile.close();
    } else std::cout << "WARN: save file not found, network won't be saved properly!";
}

void Controller::loadFromFile() {
    std::ifstream loadFile ("controller.lds");


}











