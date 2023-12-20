#pragma once

#include <string>

#include "Handlers.h"
#include "Utils.h"

namespace LemonDrop {

    class Controller {

    protected:

        std::string name;

        Handlers::NodeHandler nodes;
        Handlers::NodeHandler outputs;
        Handlers::NodeHandler fireables;
        Handlers::NodeHandler valueInputs;
        Handlers::NodeHandler unusedNodes;

        Handlers::SynapseHandler synapses;
        Handlers::SynapseHandler weightedSynapses;
        Handlers::SynapseHandler unusedSynapses;

        Nodes::Input* unusedNodesInput,
                    * unusedSynsInput,
                    * networkSizeInput,
                    * fitnessInput,
                    * fitnessDeltaInput,
                    * fitnessAvgInput,
                    * turnsSinceFitnessDecInput,
                    * outputCalcTimeInput,
                    * lastActionTypeInput;

        int loopwait = 500;

        float fitness, prevFitness, fitnessDelta;
        int fitDecTurns;
        long long int calcTime{};
        int fitAvgTurns = 10;
        UtilClasses::RunningAverage<float> fitnessAvg;
        int calcAvgTurns = 10;
        UtilClasses::RunningAverage<long long int> calcAvg;
        int lastActionType;



        bool newNode(unsigned int type, ParamPackages::NodeParams params);
        bool newOutput(unsigned int type, ParamPackages::NodeParams params);

        bool newSynapse(unsigned int type, ParamPackages::SynapseParams params);


        bool addSynapseToNode(unsigned int synID, unsigned int nodeID, bool loading);
        bool addSynapseToNode(unsigned int synID, bool uuSyn, unsigned int nodeID, bool uuNode, bool loading);
        bool addNodeToSynapse(unsigned int nodeID, unsigned int synID, bool loading);
        bool addNodeToSynapse(unsigned int nodeID, bool uuNode, unsigned int synID, bool uuSyn, bool loading);


        void getAllOutputs();

        void saveActionToFile(const std::string& s);
        void loadFromFile();

        void initController();

        void actionNodeAddNodeFunction(Nodes::ActionNode* actionNode);
        void actionNodeAddSynapseFunction(Nodes::ActionNode* actionNode);
        void actionNodeMakeConnectionFunction(Nodes::ActionNode* actionNode);
        void actionNodeSetFlagForNodeFunction(Nodes::ActionNode* actionNode);
        void actionNodeUpdateWeightFunction(Nodes::ActionNode* actionNode);
        void actionNodeUpdateNodeValueFunction(Nodes::ActionNode* actionNode);

//        void actionNodeNodeToSynFunction(Nodes::MakeConnectionNode* node);
//        void actionNodeSynToNodeFunction(Nodes::MakeConnectionNode* node);
//        void actionNodeNodeToNodeFunction(Nodes::MakeConnectionNode* node);
//        void actionNodeSynToSynFunction(Nodes::MakeConnectionNode* node);

        void generateInitialController();

        float getUnusedPartFitnessImpact();
        float getCalcTimeFitnessImpact();
        float getFitnessFitnessImpact();
        float calcFitness();

        void setMetricInputs();

        void makeStandardLayer(int layerStartID, int nodesInLayer);
        void connectTwoLayers(int prevLayerStartID, int nodesInPrevLayer, int nextLayerStartID, int nodesInNextLayer);
        void createAndConnectUniformRepeatedLayers(int firstLayerStartID, int nodesPerLayer, int layers);


    public:
        Controller();
        Controller(const std::string& contName, bool generateNew);
        Controller(const std::string& contName, const std::string& fileToLoadFrom);

        [[noreturn]] void mainLoop();
        void mainLoop(int turnLimit);

        void operator()(int turnLimit);

        [[nodiscard]] float getFitness();

        std::string getName() const;
    };

}