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

        [[nodiscard]] virtual int getNumItems() const = 0;
    };

    class NodeHandler : public Handler {

    protected:
        unsigned int nextID { 0 };

        std::vector<Nodes::Node*> nodes;

    public:

        NodeHandler() = default;

        void InitHandler();

        void addNode(Nodes::Node* n);
        void removeNodeByID(unsigned int id);

        Nodes::Node* getNodeByID(unsigned int id);
        Nodes::Node* getNodeByCount(unsigned int idx);

        std::vector<Nodes::Node*> getNodes();

        [[nodiscard]] int getNumItems() const override;
    };

    class SynapseHandler : public Handler {

    protected:
        unsigned int nextID { 0 };

        std::vector<Synapses::Synapse*> synapses;

    public:

        SynapseHandler() = default;

        void InitHandler();

        void addSynapse(Synapses::Synapse* s);
        void removeSynapseByID(unsigned int id);

        Synapses::Synapse* getSynapseByID(unsigned int id);
        Synapses::Synapse* getSynapseByCount(unsigned int idx);

        std::vector<Synapses::Synapse*> getSynapses();

        [[nodiscard]] int getNumItems() const override;
    };
}