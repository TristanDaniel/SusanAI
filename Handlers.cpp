
#include <algorithm>
#include "Handlers.h"

using Handlers::NodeHandler;
using Handlers::SynapseHandler;

void NodeHandler::InitHandler() {
    nextID = 0;

    nodes.clear();
}

void NodeHandler::addNode(Nodes::Node* n) {
    nodes.push_back(n);
}

void NodeHandler::removeNodeByID(int id) {
    nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
                               [&id](Nodes::Node* node) { return node->getID() == id; }),nodes.end());
}

Nodes::Node* NodeHandler::getNodeByID(unsigned int id) {
    return *std::find_if(nodes.begin(), nodes.end(),
                         [&id](Nodes::Node* node) { return node->getID() == id;});
}

unsigned int NodeHandler::getNextID() { return nextID++; }

std::vector<Nodes::Node*> NodeHandler::getNodes() {
    return nodes;
}

void SynapseHandler::InitHandler() {
    nextID = 0;

    synapses.clear();
}

void SynapseHandler::addSynapse(Synapses::Synapse *s) {
    synapses.push_back(s);
}

void SynapseHandler::removeSynapseByID(int id) {
    synapses.erase(std::remove_if(synapses.begin(), synapses.end(),
                                  [&id](Synapses::Synapse* synapse) { return synapse->getID() == id; }),synapses.end());
}

Synapses::Synapse* SynapseHandler::getSynapseByID(unsigned int id) {
    return *std::find_if(synapses.begin(), synapses.end(),
                         [&id](Synapses::Synapse* synapse) { return synapse->getID() == id;});
}

unsigned int SynapseHandler::getNextID() { return nextID++; }