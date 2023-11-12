#pragma once

#include <vector>

#include "Nodes.h"

namespace Handlers{

    class NodeHandler {

    protected:
        unsigned int nextID { 0 };

        std::vector<Nodes::Node*> nodes;

    public:

        NodeHandler() = default;

        void InitHandler();

        void addNode(Nodes::Node* n);
        void removeNodeByID(int id);

        unsigned int getNextID();
    };
}