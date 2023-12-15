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
        Handlers::SynapseHandler unusedSynapses;

        Nodes::Input* unusedNodesInput = new Nodes::Input(0),
                    * unusedSynsInput = new Nodes::Input(0),
                    * networkSizeInput = new Nodes::Input(0),
                    * fitnessInput = new Nodes::Input(0),
                    * fitnessDeltaInput = new Nodes::Input(0),
                    * fitnessAvgInput = new Nodes::Input(0),
                    * turnsSinceFitnessDecInput = new Nodes::Input(0),
                    * outputCalcTimeInput = new Nodes::Input(0);

        int loopwait = 1000;

        float fitness, prevFitness, fitnessDelta;
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

        float getUnusedPartFitnessImpact();
        float getCalcTimeFitnessImpact();
        float getFitnessFitnessImpact();
        float calcFitness();
        [[nodiscard]] float getFitness() const;

        void setMetricInputs();


    public:
        Controller();
    };

}