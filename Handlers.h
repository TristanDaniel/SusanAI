#pragma once

#include <vector>

#include "Nodes.h"
#include "Synapses.h"

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

        Nodes::Node* getNodeByID(unsigned int id);

        unsigned int getNextID();
    };

    class SynapseHandler {

    protected:
        unsigned int nextID { 0 };

        std::vector<Synapses::Synapse*> synapses;

    public:

        SynapseHandler() = default;

        void InitHandler();

        void addSynapse(Synapses::Synapse* s);
        void removeSynapseByID(int id);

        Synapses::Synapse* getSynapseByID(unsigned int id);

        unsigned int getNextID();
    };
}