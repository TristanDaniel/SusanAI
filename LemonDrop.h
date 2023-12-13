#pragma once

#include <string>

#include "Handlers.h"
#include "Utils.h"

namespace LemonDrop {

    class Controller {

    protected:

        Handlers::NodeHandler nodes;
        Handlers::NodeHandler outputs;
        Handlers::NodeHandler fireables;
        Handlers::NodeHandler unusedNodes;

        Handlers::SynapseHandler synapses;
        Handlers::SynapseHandler unusedSynapse;

        Nodes::Input* unusedNodesInput,
                    * unusedSynsInput,
                    * networkSizeInput,
                    * fitnessInput,
                    * fitnessDeltaInput,
                    * fitnessAvgInput,
                    * turnsSinceFitnessDecInput,
                    * outputCalcTimeInput;

        int loopwait = 1000;

        float fitness, prevFitness;
        int fitDecTurns;
        long long int calcTime{};
        int fitAvgTurns = 10;
        UtilClasses::RunningAverage<float> fitnessAvg;
        int calcAvgTurns = 10;
        UtilClasses::RunningAverage<long long int> calcAvg;



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

        void actionNodeAddNodeFunction(Nodes::ActionNode* actionNode);
        void actionNodeAddSynapseFunction(Nodes::ActionNode* actionNode);
        void actionNodeMakeConnectionFunction(Nodes::ActionNode* actionNode);
        void actionNodeSetFlagForNodeFunction(Nodes::ActionNode* actionNode);

//        void actionNodeNodeToSynFunction(Nodes::MakeConnectionNode* node);
//        void actionNodeSynToNodeFunction(Nodes::MakeConnectionNode* node);
//        void actionNodeNodeToNodeFunction(Nodes::MakeConnectionNode* node);
//        void actionNodeSynToSynFunction(Nodes::MakeConnectionNode* node);

        void generateInitialController();


    public:
        Controller();
    };

}