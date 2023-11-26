
#include <windows.h>
#include <unistd.h>
#include <iostream>

#include "LemonDrop.h"
#include "Utils.h"

using LemonDrop::Controller;

bool Controller::newNode(unsigned int type, ParamPackages::NodeParams params) {

    unsigned int id;
    Nodes::Node* n;

    switch (type) {
        case 0:
            //basic node
            id = nodes.getNextID();

            n = new Nodes::NotInputNode(id);

            nodes.addNode(n);

            return true;

        case 1:
        {
            //Random input
            id = nodes.getNextID();

            int mode = params.randInputParams.mode;
            float min = params.randInputParams.min;
            float max = params.randInputParams.max;

            n = new Nodes::RandomInput(id, mode, min, max);

            nodes.addNode(n);

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
            //basic output
            id = nodes.getNextID();

            n = new Nodes::Output(id);

            outputs.addNode(n);
            nodes.addNode(n);

            return true;

        default:
            return false;
    }
}

bool Controller::newSynapse(unsigned int type, ParamPackages::SynapseParams params) {
    unsigned int id;
    Synapses::Synapse* s;

    switch (type) {
        case 0:
            //passthrough
            id = synapses.getNextID();

            s = new Synapses::PassthroughSynapse(id);

            synapses.addSynapse(s);

            return true;

        case 1:
        {
            //weighted
            id = synapses.getNextID();
            float weight = params.weightedSynapseParams.weight;

            s = new Synapses::WeightedSynapse(id, weight);

            synapses.addSynapse(s);

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

void Controller::mainLoop() {
    while (true) {
        std::cout << std::to_string((int)Flags::NodeFlag::PARTIAL_ON_CYCLE);

        getAllOutputs();
        sleep(1);
    }
}