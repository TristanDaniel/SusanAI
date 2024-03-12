
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
    checkID(n->getID());
    items = (int)nodes.size();
}

void NodeHandler::removeNodeByID(unsigned int id) {
    nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
                               [&id](Nodes::Node* node) { return node->getID() == id; }),nodes.end());

    items = (int)nodes.size();
}

Nodes::Node* NodeHandler::getNodeByID(unsigned int id) {
    auto node = std::find_if(nodes.begin(), nodes.end(),
                              [&id](Nodes::Node* node) { return node->getID() == id;});

    return node == nodes.end() ? getNodeByCount(id) : *node;
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
    checkID(s->getID());
    items = (int)synapses.size();
}

void SynapseHandler::removeSynapseByID(unsigned int id) {
    synapses.erase(std::remove_if(synapses.begin(), synapses.end(),
                                  [&id](Synapses::Synapse* synapse) { return synapse->getID() == id; }),synapses.end());

    items = (int)synapses.size();
}

Synapses::Synapse* SynapseHandler::getSynapseByID(unsigned int id) {
    auto syn = std::find_if(synapses.begin(), synapses.end(),
                             [&id](Synapses::Synapse* synapse) { return synapse->getID() == id;});

    return syn == synapses.end() ? getSynapseByCount(id) : *syn;
}

void Handler::checkID(unsigned int id) { if (id >= nextID) nextID = id+1; }

int Handler::getNumItems() const { return items; }

Nodes::Node* NodeHandler::getNodeByCount(unsigned int idx) {
    int i = 0;
    for (auto n : nodes) {
        if (i++ == idx) return n;
    }

    return nullptr;
}

Synapses::Synapse* SynapseHandler::getSynapseByCount(unsigned int idx) {
    int i = 0;
    for (auto syn : synapses) {
        if (i++ == idx) return syn;
    }

    return nullptr;
}

std::vector<Synapses::Synapse *> SynapseHandler::getSynapses() { return synapses; }



