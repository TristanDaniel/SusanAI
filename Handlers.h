#pragma once

#include <vector>

#include "Nodes.h"
#include "Synapses.h"

namespace Handlers{

    class Handler {

    protected:
        unsigned int nextID { 0 };
        int items { 0 };

    public:

        [[nodiscard]] unsigned int getCurrID() const;
        unsigned int getNextID();
        void checkID(unsigned int id);

        [[nodiscard]] int getNumItems() const;
    };

    class NodeHandler : public Handler {

    protected:
        unsigned int nextID { 0 };

        std::vector<Nodes::Node*> nodes;

    public:

        NodeHandler() = default;

        void InitHandler();

        void addNode(Nodes::Node* n);
        void removeNodeByID(int id);

        Nodes::Node* getNodeByID(unsigned int id);

        std::vector<Nodes::Node*> getNodes();
    };

    class SynapseHandler : public Handler {

    protected:
        unsigned int nextID { 0 };

        std::vector<Synapses::Synapse*> synapses;

    public:

        SynapseHandler() = default;

        void InitHandler();

        void addSynapse(Synapses::Synapse* s);
        void removeSynapseByID(int id);

        Synapses::Synapse* getSynapseByID(unsigned int id);
    };
}