
#include <algorithm>
#include "Handlers.h"

using Handlers::Handler;
using Handlers::NodeHandler;
using Handlers::SynapseHandler;

void NodeHandler::InitHandler() {
    nextID = 0;

    nodes.clear();
}

void NodeHandler::addNode(Nodes::Node* n) {
    nodes.push_back(n);
    items++;
}

void NodeHandler::removeNodeByID(int id) {
    if (nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
                               [&id](Nodes::Node* node) { return node->getID() == id; }),nodes.end())
                               != nodes.end()) items--;
}

Nodes::Node* NodeHandler::getNodeByID(unsigned int id) {
    return *std::find_if(nodes.begin(), nodes.end(),
                         [&id](Nodes::Node* node) { return node->getID() == id;});
}

unsigned int Handler::getCurrID() const { return nextID; }
unsigned int Handler::getNextID() { return nextID++; }

std::vector<Nodes::Node*> NodeHandler::getNodes() {
    return nodes;
}

void SynapseHandler::InitHandler() {
    nextID = 0;

    synapses.clear();
}

void SynapseHandler::addSynapse(Synapses::Synapse *s) {
    synapses.push_back(s);
    items++;
}

void SynapseHandler::removeSynapseByID(int id) {
    if (synapses.erase(std::remove_if(synapses.begin(), synapses.end(),
                                  [&id](Synapses::Synapse* synapse) { return synapse->getID() == id; }),synapses.end())
                                  != synapses.end()) items--;
}

Synapses::Synapse* SynapseHandler::getSynapseByID(unsigned int id) {
    return *std::find_if(synapses.begin(), synapses.end(),
                         [&id](Synapses::Synapse* synapse) { return synapse->getID() == id;});
}

void Handler::checkID(unsigned int id) { if (id > nextID) nextID = id; }

int Handler::getNumItems() const { return items; }

