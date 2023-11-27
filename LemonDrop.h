#pragma once

#include <string>

#include "Handlers.h"
#include "Utils.h"

namespace LemonDrop {

    class Controller {

    protected:

        Handlers::NodeHandler nodes;
        Handlers::NodeHandler outputs;

        Handlers::SynapseHandler synapses;


        bool newNode(unsigned int type, ParamPackages::NodeParams params);
        bool newOutput(unsigned int type, ParamPackages::NodeParams params);

        bool newSynapse(unsigned int type, ParamPackages::SynapseParams params);


        bool addSynapseToNode(unsigned int synID, unsigned int nodeID);
        bool addNodeToSynapse(unsigned int nodeID, unsigned int synID);


        void getAllOutputs();

        [[noreturn]] void mainLoop();

        void saveActionToFile(std::string s);
        void loadFromFile();


    public:
        Controller();
    };

}