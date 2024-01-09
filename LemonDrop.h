#pragma once

#include <string>

#include "Handlers.h"
#include "Utils.h"

namespace LemonDrop {

    class Controller {

    protected:

        std::string name;

        bool withoutSaveMode = false;
        bool verboseActionsMode = true;

        Handlers::NodeHandler nodes;
        Handlers::NodeHandler outputs;
        Handlers::NodeHandler fireables;
        Handlers::NodeHandler valueInputs;
        Handlers::NodeHandler unusedNodes;

        std::vector<UtilClasses::ActionGroup*> actionGroups;
        UtilClasses::ActionGroup baseAG1, baseAG2, extraAG;

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
                    * lastActionTypeInput,
                    * actionTypeAgvInput,
                    * turnsSinceStructureChangeInput;

        int loopwait = 500;

        int turn;

        float fitness, prevFitness, fitnessDelta;
        int fitDecTurns, fitIncrTurns;
        long long int calcTime{};
        int fitAvgTurns = 10;
        UtilClasses::RunningAverage<float> fitnessAvg;
        int calcAvgTurns = 10;
        UtilClasses::RunningAverage<long long int> calcAvg;
        int lastActionType, lastActionType1, lastActionType2, lastActionType3,
            lastActionType4, lastActionType5, lastActionType6;
        int actionTypeAvgTurns = 10;
        UtilClasses::RunningAverage<int> actionTypeAvg;
        int turnsSinceStructureChange;
        int ag1repeatTurns, ag2repeatTurns;
        int timesDoneNothing;

        bool brokeActionNode;



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
        bool actionNodeMakeConnectionFunction(Nodes::ActionNode* actionNode);
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
        float getTurnAndStructureFitnessImpact();
        float calcFitness();

        void setMetricInputs();

        void makeStandardLayer(int layerStartID, int nodesInLayer);
        void connectTwoLayers(int prevLayerStartID, int nodesInPrevLayer, int nextLayerStartID, int nodesInNextLayer);
        void createAndConnectUniformRepeatedLayers(int firstLayerStartID, int nodesPerLayer, int layers);

        void loop();

    public:
        Controller();
        Controller(const std::string& contName, bool generateNew, bool withoutSaveMode, bool verboseActionsMode);
        Controller(const std::string& contName, const std::string& fileToLoadFrom, bool withoutSaveMode, bool verboseActionsMode);

        [[noreturn]] void mainLoop();
        void mainLoop(int turnLimit);

        void operator()(int turnLimit);

        [[nodiscard]] float getFitness();

        [[nodiscard]] std::string getName() const;

        void totalSave();
        void totalSave(const std::string& fileName);

        void setWithoutSaveMode(bool mode);
        void setVerboseActionsMode(bool mode);

        void resetFitness();
        float getSavedFitness();

        void loadSavedData();
    };

}