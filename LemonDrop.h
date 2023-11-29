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


        bool addSynapseToNode(unsigned int synID, unsigned int nodeID, bool loading);
        bool addNodeToSynapse(unsigned int nodeID, unsigned int synID, bool loading);


        void getAllOutputs();

        [[noreturn]] void mainLoop();

        static void saveActionToFile(const std::string& s);
        void loadFromFile();

        void initController();


    public:
        Controller();
    };

}