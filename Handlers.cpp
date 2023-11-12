
#include <algorithm>
#include "Handlers.h"

using Handlers::NodeHandler;

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

unsigned int NodeHandler::getNextID() { return nextID++; }