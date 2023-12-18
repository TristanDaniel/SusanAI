
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>

#include "LemonDrop.h"
#include "Utils.h"

using namespace std;

using LemonDrop::Controller;

Controller::Controller() {
    nodes.InitHandler();
    outputs.InitHandler();
    fireables.InitHandler();
    valueInputs.InitHandler();
    unusedNodes.InitHandler();

    synapses.InitHandler();
    weightedSynapses.InitHandler();
    unusedSynapses.InitHandler();

    fitnessAvg = UtilClasses::RunningAverage<float>(fitAvgTurns);
    calcAvg = UtilClasses::RunningAverage<long long int>(calcAvgTurns);

    fitness = 100;
    prevFitness = 100;
    fitnessDelta = 0;
    calcTime = 3000000;
    fitDecTurns = 1;

//    std::ofstream saveFile("..\\controller.lsv");
//    saveFile.close();

    initController();

    //generateInitialController();

    saveActionToFile("\n");

    mainLoop();
}

bool Controller::newNode(unsigned int type, ParamPackages::NodeParams params) {

    unsigned int id;
    Nodes::Node* n;

    switch (type) {
        case 0:
        {
            //basic node
            id = nodes.getNextID();

            n = new Nodes::NotInputNode(id);

            n->addFlag(params.basicNodeParams.cycleFlag);

            nodes.addNode(n);
            unusedNodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;
        }

        case 1:
        {
            //basic input
            id = nodes.getNextID();

            n = new Nodes::Input(id);

            n->setValue(params.basicNodeParams.value);

            nodes.addNode(n);
            valueInputs.addNode(n);
            unusedNodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;
        }

        case 2:
        {
            //Random input
            id = nodes.getNextID();

            int mode = params.randInputParams.mode;
            float min = params.randInputParams.min;
            float max = params.randInputParams.max;

            n = new Nodes::RandomInput(id, mode, min, max);

            n->addFlag(params.randInputParams.cycleFlag);

            nodes.addNode(n);
            unusedNodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;
        }

        case 3:
        {
            //basic output
            id = nodes.getNextID();

            n = new Nodes::Output(id);

            n->addFlag(params.basicNodeParams.cycleFlag);

            outputs.addNode(n);
            nodes.addNode(n);
            unusedNodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;
        }

        case 4:
        {
            //fireable node
            id = nodes.getNextID();
            float threshold = params.fireableNodeParams.threshold;

            n = new Nodes::FireableNode(id, threshold);

            n->addFlag(params.basicNodeParams.cycleFlag);

            //outputs.addNode(n);
            nodes.addNode(n);
            unusedNodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;

        }

        case 5:
        {
            //action node
            id = nodes.getNextID();
            float threshold = params.fireableNodeParams.threshold;
            int actionType = params.actionNodeParams.actionType;

            switch (actionType) {
                case 0:
                    // do nothing
                    n = new Nodes::ActionNode(id, threshold, 0);
                    break;
                case 1:
                    // add node
                    n = new Nodes::AddNodeNode(id, threshold);
                    break;
                case 2:
                    // add syn
                    n = new Nodes::AddSynapseNode(id, threshold);
                    break;
                case 3:
                    // make connection
                    n = new Nodes::MakeConnectionNode(id, threshold);
                    break;
                case 4:
                    // set flag
                    n = new Nodes::SetFlagNode(id, threshold);
                    break;
                case 5:
                    n = new Nodes::UpdateWeightNode(id, threshold);
                default:
                    // invalid input punished with do nothing action
                    n = new Nodes::ActionNode(id, threshold, 0);
                    break;
            }

            outputs.addNode(n);
            nodes.addNode(n);
            unusedNodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;
        }

        default:
            return false;
    }
}

bool Controller::newOutput(unsigned int type, ParamPackages::NodeParams params) {
    unsigned int id;
    Nodes::Output* n;

    switch (type) {
        case 0:
        {
            //basic output
            id = nodes.getNextID();

            n = new Nodes::Output(id);

            n->addFlag(params.basicNodeParams.cycleFlag);

            outputs.addNode(n);
            nodes.addNode(n);

            saveActionToFile(n->saveNode());

            cout << n->saveNode() << endl;

            return true;
        }

        default:
            return false;
    }
}

bool Controller::newSynapse(unsigned int type, ParamPackages::SynapseParams params) {
    unsigned int id;
    Synapses::Synapse* s;

    switch (type) {
        case 0:
        {
            //passthrough
            id = synapses.getNextID();

            s = new Synapses::PassthroughSynapse(id);

            synapses.addSynapse(s);
            unusedSynapses.addSynapse(s);

            saveActionToFile(s->saveSynapse());

            cout << s->saveSynapse() << endl;

            return true;
        }


        case 1:
        {
            //weighted
            id = synapses.getNextID();
            float weight = params.weightedSynapseParams.weight;

            s = new Synapses::WeightedSynapse(id, weight);

            synapses.addSynapse(s);
            weightedSynapses.addSynapse(s);
            unusedSynapses.addSynapse(s);

            saveActionToFile(s->saveSynapse());

            cout << s->saveSynapse() << endl;

            return true;
        }

        default:
            return false;


    }
}

bool Controller::addSynapseToNode(unsigned int synID, unsigned int nodeID, bool loading) {
    return addSynapseToNode(synID, false, nodeID, false, loading);
}
bool Controller::addSynapseToNode(unsigned int synID, bool uuSyn, unsigned int nodeID, bool uuNode, bool loading) {
    Synapses::Synapse* synapse = uuSyn ? unusedSynapses.getSynapseByCount(synID)
            : synapses.getSynapseByID(synID);
    Nodes::Node* node = uuNode ? unusedNodes.getNodeByCount(nodeID)
            : nodes.getNodeByID(nodeID);

    bool synuu = synapse->isUnused();
    bool nodeuu = node->isUnused();

    Nodes::Node* prevSynOutput = synapse->getOutput();

    node->addSynapse(synapse);

    if (prevSynOutput && prevSynOutput->isUnused()) unusedNodes.addNode(prevSynOutput);

    if (synuu && !synapse->isUnused()) unusedSynapses.removeSynapseByID(synID);
    else if (!synuu && synapse->isUnused()) unusedSynapses.addSynapse(synapse);
    if (nodeuu && !node->isUnused()) unusedNodes.removeNodeByID(nodeID);

    string saveString = ">sn " + to_string(synapse->getID()) + " " + to_string(node->getID()) + " ";

    if (!loading) {
        saveActionToFile(saveString);
        cout << saveString << endl;
    }

    return true;
}

bool Controller::addNodeToSynapse(unsigned int nodeID, unsigned int synID, bool loading) {
    return addNodeToSynapse(nodeID, false, synID, false, loading);
}
bool Controller::addNodeToSynapse(unsigned int nodeID, bool uuNode, unsigned int synID, bool uuSyn, bool loading) {
    Synapses::Synapse* synapse = uuSyn ? unusedSynapses.getSynapseByCount(synID)
                                       : synapses.getSynapseByID(synID);
    Nodes::Node* node = uuNode ? unusedNodes.getNodeByCount(nodeID)
                               : nodes.getNodeByID(nodeID);

    bool synuu = synapse->isUnused();
    bool nodeuu = node->isUnused();

    Nodes::Node* prevSynInput = synapse->getInput();

    synapse->setInput(node);

    if (prevSynInput && prevSynInput->isUnused()) unusedNodes.addNode(prevSynInput);

    if (synuu && !synapse->isUnused()) unusedSynapses.removeSynapseByID(synID);
    if (nodeuu && !node->isUnused()) unusedNodes.removeNodeByID(nodeID);

    string saveString = ">ns " + to_string(node->getID()) + " " + to_string(synapse->getID()) + " ";

    if (!loading) {
        saveActionToFile(saveString);
        cout << saveString << endl;
    }

    return true;
}

void Controller::getAllOutputs() {
    bool doNothing = false;
    auto start = chrono::high_resolution_clock::now();

    for (Nodes::Node* output : outputs.getNodes()) {
        if (auto* actionNode = dynamic_cast<Nodes::ActionNode *>(output)) {
            if (!doNothing && actionNode->getValue() == 0) {
                actionNode->getOutput();
                switch (actionNode->getActionType()) {
                    case Flags::ActionFlag::DO_NOTHING:
                        doNothing = true; //prevents other action nodes from firing this turn
                        break;
                    case Flags::ActionFlag::ADD_NODE:
                        actionNodeAddNodeFunction(actionNode);
                        break;
                    case Flags::ActionFlag::ADD_SYNAPSE:
                        actionNodeAddSynapseFunction(actionNode);
                        break;
                    case Flags::ActionFlag::MAKE_CONNECTION:
                        actionNodeMakeConnectionFunction(actionNode);
                        break;
                    case Flags::ActionFlag::SET_FLAG_FOR_NODE:
                        actionNodeSetFlagForNodeFunction(actionNode);
                    case Flags::ActionFlag::UPDATE_WEIGHT:
                        actionNodeUpdateWeightFunction(actionNode);
                    case Flags::ActionFlag::UPDATE_NODE_VALUE:
                        actionNodeUpdateNodeValueFunction(actionNode);
                        break;
                }
            }
        } else ((Nodes::Output *) output)->getOutput();
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    calcTime = duration.count();
    calcAvg.addValue(calcTime);
    cout << (calcAvg.getAverage() / 1000) << ", " << outputs.getNumItems() << endl;
}

[[noreturn]] void Controller::mainLoop() {
    DataBits::initTurn();
    while (true) {
        cout << "loop" << endl;

        DataBits::incrTurn();

        setMetricInputs();
        cout << "F: " << fitness << endl;

        getAllOutputs();
        this_thread::sleep_for(chrono::milliseconds(loopwait));
    }
}

void Controller::saveActionToFile(const std::string& s) {
    std::ofstream saveFile("..\\controller.lsv", std::ios::app);

    if (saveFile.is_open()) {
        saveFile << s;
        saveFile.close();
    } else std::cout << "WARN: save file not found, network won't be saved properly!";
}

void Controller::loadFromFile() {
    std::ifstream loadFile ("..\\controller.lsv");

    if (loadFile.is_open()) {
        string line;

        int id, cycleFlag;
        Nodes::Node* n;
        Synapses::Synapse* syn;


        while (getline(loadFile, line)) {
            stringstream  ss(line);
            string infobit; //thanks logan for the name

            while (!ss.eof()) {
                ss >> infobit;
                if (infobit == "+n0") {
                    // basic node
                    ss >> id;
                    ss >> cycleFlag;

                    n = new Nodes::NotInputNode(id);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));

                    nodes.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n1") {
                    // input
                    ss >> id;
                    ss >> cycleFlag;

                    float val;
                    ss >> val;

                    n = new Nodes::Input(id);

                    n->setValue(val);

                    nodes.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n2") {
                    // Random input
                    int mode;
                    float min, max;

                    ss >> id;
                    ss >> cycleFlag;
                    ss >> mode;
                    ss >> min;
                    ss >> max;

                    n = new Nodes::RandomInput(id, mode, min, max);

                    nodes.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n3") {
                    // Output
                    ss >> id;
                    ss >> cycleFlag;

                    n = new Nodes::Output(id);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));

                    nodes.addNode(n);
                    outputs.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n4") {
                    // fireable

                    ss >> id;
                    ss >> cycleFlag;

                    float threshold;
                    ss >> threshold;

                    n = new Nodes::FireableNode(id, threshold);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));

                    nodes.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);

                } else if (infobit == "+n5") {
                    // action

                    ss >> id;
                    ss >> cycleFlag;

                    float threshold;
                    ss >> threshold;

                    int actionType;
                    ss >> actionType;

                    switch (actionType) {
                        case 0:
                            // do nothing
                            n = new Nodes::ActionNode(id, threshold, 0);
                            break;
                        case 1:
                            // add node
                            n = new Nodes::AddNodeNode(id, threshold);
                            break;
                        case 2:
                            // add syn
                            n = new Nodes::AddSynapseNode(id, threshold);
                            break;
                        case 3:
                            // make connection
                            n = new Nodes::MakeConnectionNode(id, threshold);
                            break;
                        case 4:
                            // set flag
                            n = new Nodes::SetFlagNode(id, threshold);
                            break;
                        case 5:
                            // update weight
                            n = new Nodes::UpdateWeightNode(id, threshold);
                            break;
                        default:
                            // invalid input punished with do nothing action
                            n = new Nodes::ActionNode(id, threshold, 0);
                            break;
                    }

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));

                    nodes.addNode(n);
                    outputs.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);

                } else if (infobit == "+s0") {
                    //passthrough syn
                    ss >> id;

                    syn = new Synapses::PassthroughSynapse(id);

                    synapses.addSynapse(syn);
                    unusedSynapses.addSynapse(syn);
                    synapses.checkID(id);
                } else if (infobit == "+s1") {
                    //weighted syn
                    float weight;
                    ss >> id;
                    ss >> weight;

                    syn = new Synapses::WeightedSynapse(id, weight);

                    synapses.addSynapse(syn);
                    weightedSynapses.addSynapse(syn);
                    unusedSynapses.addSynapse(syn);
                    synapses.checkID(id);
                } else if (infobit == ">sn") {
                    int id1, id2;
                    ss >> id1;
                    ss >> id2;

                    addSynapseToNode(id1, id2, true);
                } else if (infobit == ">ns") {
                    int id1, id2;
                    ss >> id1;
                    ss >> id2;

                    addNodeToSynapse(id1, id2, true);
                } else if (infobit == ">nn") {
                    // convenience instr, takes n1 s n2 and makes necessary connections
                    int n1, s, n2;
                    ss >> n1;
                    ss >> s;
                    ss >> n2;

                    addNodeToSynapse(n1, s, true);
                    addSynapseToNode(s, n2, true);
                } else if (infobit == ">ss") {
                    // convenience instr, connects 2 syns with dummy node (unreferenceable)
                    int s1, s2;
                    ss >> s1;
                    ss >> s2;

                    n = new Nodes::NotInputNode(0);

                    Synapses::Synapse* syn1 = synapses.getSynapseByID(s1);
                    Synapses::Synapse* syn2 = synapses.getSynapseByID(s2);

                    bool s1uu = syn1->isUnused();
                    bool s2uu = syn2->isUnused();

                    n->addSynapse(syn1);
                    syn2->setInput(n);

                    if (s1uu && !syn1->isUnused()) unusedSynapses.removeSynapseByID(s1);
                    if (s2uu && !syn2->isUnused()) unusedSynapses.removeSynapseByID(s2);
                } else if (infobit == "=f") {
                    // set flag for node
                    ss >> id;

                    int flag;
                    ss >> flag;

                    n = nodes.getNodeByID(id);
                    n->addFlag((Flags::NodeFlag)flag);
                } else if (infobit == "=iv") {
                    // update static input value
                    // -iv id val  -- id of target and resulting value to reduce need for replace check
                    ss >> id;

                    float val;
                    ss >> val;

                    n = nodes.getNodeByID(id);
                    n->setValue(val);
                } else if (infobit == "=sw") {
                    // update synapse weight
                    // same as =iv
                    ss >> id;

                    float val;
                    ss >> val;

                    syn = synapses.getSynapseByID(id);
                    ((Synapses::WeightedSynapse*)syn)->setWeight(val);
                } else if (infobit == "eff") {
                        return;
                }
            }
        }
    }

    loadFile.close();

    //std::ofstream resetFile("..\\controller.lsv");
    //resetFile.close();
}

void Controller::initController() {
//    ParamPackages::NodeParams outP1;
//    newOutput(0, outP1);
//
//    ParamPackages::SynapseParams synP1;
//    synP1.weightedSynapseParams.weight = 0.5;
//    newSynapse(1, synP1);
//
//    ParamPackages::NodeParams rinP1;
//    rinP1.randInputParams.mode = 4;
//    rinP1.randInputParams.min = 5;
//    rinP1.randInputParams.max = 10;
//    newNode(2, rinP1);
//
//    addNodeToSynapse(1, 0);
//    addSynapseToNode(0, 0);

    loadFromFile();
}

void Controller::actionNodeAddNodeFunction(Nodes::ActionNode *actionNode) {
//    auto* node = dynamic_cast<Nodes::AddNodeNode*>(actionNode);
    auto* node = (Nodes::AddNodeNode*)actionNode;

    int nodeType = node->getNodeType();
    ParamPackages::NodeParams params = node->getParams();

    newNode(nodeType, params);
}

void Controller::actionNodeAddSynapseFunction(Nodes::ActionNode *actionNode) {
    auto* node = dynamic_cast<Nodes::AddSynapseNode*>(actionNode);

    int synType = node->getSynType();
    ParamPackages::SynapseParams params = node->getParams();

    newSynapse(synType, params);
}

void Controller::actionNodeMakeConnectionFunction(Nodes::ActionNode *actionNode) {
    auto* node = dynamic_cast<Nodes::MakeConnectionNode*>(actionNode);

    int conType = node->getConnectionType();
    float id1 = abs(node->getID1());
    float id2 = abs(node->getID2());
    float id3 = abs(node->getID3());

    bool uu1 = node->getUU1();
    bool uu2 = node->getUU2();
    bool uu3 = node->getUU3();

    switch (conType) {
        case 0:
        {
            unsigned int nodeIDLimVal = uu1 ? unusedNodes.getNumItems() : nodes.getCurrID();
            unsigned int synIDLimVal = uu2 ? unusedSynapses.getNumItems() : synapses.getCurrID();

            unsigned int nodeID = (unsigned int)(id1 * (float)nodeIDLimVal) % nodeIDLimVal;
            unsigned int synID = (unsigned int)(id2 * (float)synIDLimVal) % synIDLimVal;

            addNodeToSynapse(nodeID, uu1, synID, uu2, false);
            break;
        }

        case 1:
        {
            unsigned int nodeIDLimVal = uu2 ? unusedNodes.getNumItems() : nodes.getCurrID();
            unsigned int synIDLimVal = uu1 ? unusedSynapses.getNumItems() : synapses.getCurrID();

            unsigned int nodeID = (unsigned int)(id2 * (float)nodeIDLimVal) % nodeIDLimVal;
            unsigned int synID = (unsigned int)(id1 * (float)synIDLimVal) % synIDLimVal;

            addSynapseToNode(synID, uu1, nodeID, uu2, false);
            break;
        }
        case 2:
        {
            unsigned int nodeIDLimVal = uu1 ? unusedNodes.getNumItems() : nodes.getCurrID();
            unsigned int synIDLimVal = uu2 ? unusedSynapses.getNumItems() : synapses.getCurrID();
            unsigned int node2IDLimVal = uu3 ? unusedNodes.getNumItems() : nodes.getCurrID();

            unsigned int nodeID = (unsigned int)(id1 * (float)nodeIDLimVal) % nodeIDLimVal;
            unsigned int synID = (unsigned int)(id2 * (float)synIDLimVal) % synIDLimVal;
            unsigned int node2ID = (unsigned int)(id3 * (float)node2IDLimVal) % node2IDLimVal;


            addNodeToSynapse(nodeID, uu1, synID, uu2, false);
            addSynapseToNode(synID, uu1, node2ID, uu3, false);
            break;
        }

        case 3:
        {
            unsigned int synIDLimVal = uu1 ? unusedSynapses.getNumItems() : synapses.getCurrID();
            unsigned int syn2IDLimVal = uu2 ? unusedSynapses.getNumItems() : synapses.getCurrID();

            unsigned int synID = (unsigned int)(id1 * (float)synIDLimVal) % synIDLimVal;
            unsigned int syn2ID = (unsigned int)(id2 * (float)syn2IDLimVal) % syn2IDLimVal;


            auto* n = new Nodes::NotInputNode(0);

            Synapses::Synapse* s1 = uu1 ? unusedSynapses.getSynapseByCount(synID)
                    : synapses.getSynapseByID(synID);
            Synapses::Synapse* s2 = uu2 ? unusedSynapses.getSynapseByCount(syn2ID)
                    : synapses.getSynapseByID(syn2ID);

            bool s1uu = s1->isUnused();
            bool s2uu = s2->isUnused();

            n->addSynapse(s1);
            s2->setInput(n);

            if (s1uu && !s1->isUnused()) unusedSynapses.removeSynapseByID(synID);
            if (s2uu && !s2->isUnused()) unusedSynapses.removeSynapseByID(syn2ID);

            break;
        }
        default:
            break;
    }
}

void Controller::actionNodeSetFlagForNodeFunction(Nodes::ActionNode *actionNode) {
    auto* node = dynamic_cast<Nodes::SetFlagNode*>(actionNode);

    unsigned int targetID = (unsigned int)(abs(node->getTargetID()) * nodes.getCurrID()) % nodes.getCurrID();
    auto flag = (Flags::NodeFlag)abs(node->getFlagVal());

    Nodes::Node* n = nodes.getNodeByID(targetID);
    n->addFlag(flag);

    string saveString = "=f " + to_string(targetID) + " " + to_string(abs(node->getFlagVal())) + " ";
    saveActionToFile(saveString);
    cout << saveString << endl;
}


void Controller::generateInitialController() {
    using namespace UtilFunctions;
    //// setup ////
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-1, 1);


    //// metric inputs ////

    fitnessInput = new Nodes::Input(nodes.getNextID()); // 0
    nodes.addNode(fitnessInput);
    unusedNodesInput = new Nodes::Input(nodes.getNextID()); // 1
    nodes.addNode(unusedNodesInput);
    unusedSynsInput = new Nodes::Input(nodes.getNextID()); // 2
    nodes.addNode(unusedSynsInput);
    networkSizeInput = new Nodes::Input(nodes.getNextID()); // 3
    nodes.addNode(networkSizeInput);
    fitnessDeltaInput = new Nodes::Input(nodes.getNextID()); // 4
    nodes.addNode(fitnessDeltaInput);
    fitnessAvgInput = new Nodes::Input(nodes.getNextID()); // 5
    nodes.addNode(fitnessAvgInput);
    turnsSinceFitnessDecInput = new Nodes::Input(nodes.getNextID()); // 6
    nodes.addNode(turnsSinceFitnessDecInput);
    outputCalcTimeInput = new Nodes::Input(nodes.getNextID()); // 7
    nodes.addNode(outputCalcTimeInput);

    //// layer sizes and helpful numbers ////

    int startID = 8;
    // base inputs
    const int baseInputs = 6; // split 50/50 between random and static
    const int baseInputHiddenLayers = 2;
    const int nodesPerInputHiddenLayer = 6;
    const int firstInputHiddenLayerStartID = startID + baseInputs;
    // main hidden layers
    const int hiddenLayers = 2;
    const int nodesPerHiddenLayer = 18;
    const int firstHiddenLayerStartID = firstInputHiddenLayerStartID
            + (nodesPerInputHiddenLayer * baseInputHiddenLayers);
    // output params inputs split layer
    const int nodesToOutputParams = 10;
    const int nodesToOutputParamsStartID = firstInputHiddenLayerStartID
            + (nodesPerHiddenLayer * hiddenLayers);
    const int nodesToOutputFiringThreshold = 8;
    const int nodesToOutFireThreshStartID = nodesToOutputParamsStartID + nodesToOutputParams;
    // output params layer
    const int totalOutputParams = 25;
    const int outputParamNodesStartID = nodesToOutFireThreshStartID + nodesToOutputFiringThreshold;
    const int firingThresholdNodes = 4;
    const int firingThresholdNodesStartID = outputParamNodesStartID + totalOutputParams;
    // outputs
    const int actionOutputs = 7;
    const int actionOutputsStartID = firingThresholdNodesStartID + firingThresholdNodes;
    // extras to play with
    const int extraOutputs = 5;
    const int extraOutputsStartID = actionOutputsStartID + actionOutputs;

    //// temp variables ////
    Nodes::Node* n;
    Synapses::Synapse* syn;

    //// creation ////

    // base inputs
    for (int i = 0; i < baseInputs / 2; i++) {
        // static input
        n = new Nodes::Input(nodes.getNextID());
        n->setValue(LDRandomInt(-5, 5) * LDRandomFloat());

        saveActionToFile(n->saveNode());

        nodes.addNode(n);
        valueInputs.addNode(n);

        // random input
        n = new Nodes::RandomInput(nodes.getNextID(), 4, -1, 1);

        saveActionToFile(n->saveNode());

        nodes.addNode(n);
    }

    // base input hidden layers
    for (int i = 0; i < nodesPerInputHiddenLayer; i++) {
        n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n);

        for (int j = startID; j < startID + baseInputs; j++) {
            syn = new Synapses::WeightedSynapse(synapses.getNextID(), LDRandomFloat());

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);
            weightedSynapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i + firstInputHiddenLayerStartID, false);
        }
    }

    for (int i = 1; i < baseInputHiddenLayers; i++) {
        int layerStartID = firstInputHiddenLayerStartID + (nodesPerInputHiddenLayer * i);
        for (int newNodeID = layerStartID; newNodeID < layerStartID + nodesPerInputHiddenLayer; newNodeID++) {
            n = new Nodes::NotInputNode(newNodeID);

            saveActionToFile(n->saveNode());

            nodes.addNode(n);

            int prevLayerStartID =
        }
    }





    // rand
    for (int i = 0; i < numRand; i++) {
        Nodes::Node* n = new Nodes::RandomInput(nodes.getNextID(), 4, -1, 1);

        saveActionToFile(n->saveNode());

        nodes.addNode(n); //0, 1, 2
    }

    // fixed
    Nodes::Node* fixedN1 = new Nodes::Input(nodes.getNextID());
    fixedN1->setValue(5 * dist(mt));
    saveActionToFile(fixedN1->saveNode());
    nodes.addNode(fixedN1); // 3
    Nodes::Node* fixedN2 = new Nodes::Input(nodes.getNextID());
    fixedN2->setValue(dist(mt));
    saveActionToFile(fixedN2->saveNode());
    nodes.addNode(fixedN2); // 4
    Nodes::Node* fixedN3 = new Nodes::Input(nodes.getNextID());
    fixedN3->setValue(dist(mt));
    saveActionToFile(fixedN3->saveNode());
    nodes.addNode(fixedN3); // 5

    saveActionToFile("\n");

    //// 2 layers of 6 ////
    // 1st 6 layer
    for (int i = 0; i < 6; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 6-11
    }

    // connect base inputs to first layer of 6
    for (int i = 6; i < 12; i++) {
        for (int j = 0; j < 6; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    // 2nd 6 layer
    for (int i = 0; i < 6; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 12-17
    }

    // 1st 6 layer to 2nd 6 layer
    for (int i = 12; i < 17; i++) {
        for (int j = 6; j < 12; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    //// 2 layers of 16 ////
    // 1st 16 layer
    for (int i = 0; i < 16; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 18-33
    }

    // 2nd 6 layer to 1st 16 layer
    for (int i = 18; i < 34; i++) {
        for (int j = 12; j < 18; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    // 2nd 16 layer
    for (int i = 0; i < 16; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 34-49
    }

    // 1st 16 layer to 2nd 16 layer
    for (int i = 34; i < 50; i++) {
        for (int j = 18; j < 34; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    //// 10 and 8 ////
    // 10
    for (int i = 0; i < 10; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 50-59
    }

    // 8
    for (int i = 0; i < 8; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 60-67
    }

    saveActionToFile("\n");

    // 2nd 16 to 10
    for (int i = 50; i < 60; i++) {
        for (int j = 34; j < 50; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    // 2nd 16 to 8
    for (int i = 60; i < 68; i++) {
        for (int j = 34; j < 50; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    //// param inputs and output value inputs ////
    // 14 param inputs
    for (int i = 0; i < 14; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 68-81
    }

    // 4 output value inputs
    for (int i = 0; i < 4; i++) {
        Nodes::Node* n = new Nodes::NotInputNode(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n); // 82-85
    }

    saveActionToFile("\n");

    // 10 to params
    for (int i = 68; i < 82; i++) {
        for (int j = 50; j < 60; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    // 8 to value
    for (int i = 82; i < 86; i++) {
        for (int j = 60; j < 68; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    //// outputs ////
    // 5 outputs, 86-90
    Nodes::Node* doNothingOutput = new Nodes::ActionNode(nodes.getNextID(), 0.5, 0);
    saveActionToFile(doNothingOutput->saveNode());
    nodes.addNode(doNothingOutput);
    outputs.addNode(doNothingOutput);
    Nodes::Node* addNodeOutput = new Nodes::AddNodeNode(nodes.getNextID(), 0.5);
    saveActionToFile(addNodeOutput->saveNode());
    nodes.addNode(addNodeOutput);
    outputs.addNode(addNodeOutput);
    Nodes::Node* addSynOutput = new Nodes::AddSynapseNode(nodes.getNextID(), 0.5);
    saveActionToFile(addSynOutput->saveNode());
    nodes.addNode(addSynOutput);
    outputs.addNode(addSynOutput);
    Nodes::Node* makeConOutput = new Nodes::MakeConnectionNode(nodes.getNextID(), 0.5);
    saveActionToFile(makeConOutput->saveNode());
    nodes.addNode(makeConOutput);
    outputs.addNode(makeConOutput);
    Nodes::Node* setFlagOutput = new Nodes::SetFlagNode(nodes.getNextID(), 0.5);
    saveActionToFile(setFlagOutput->saveNode());
    nodes.addNode(setFlagOutput);
    outputs.addNode(setFlagOutput);

    saveActionToFile("\n");

    // params to outputs

    // no action - 86 - no inputs

    // add node - 87 - 6 inputs
    for (int i = 0; i < 6; i++) {
        Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

        saveActionToFile(syn->saveSynapse());

        synapses.addSynapse(syn);

        addNodeToSynapse(68+i, syn->getID(), false);
        addSynapseToNode(syn->getID(), 87, false);
    }

    // add syn - 88 - 2 inputs
    for (int i = 0; i < 2; i++) {
        Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

        saveActionToFile(syn->saveSynapse());

        synapses.addSynapse(syn);

        addNodeToSynapse(74+i, syn->getID(), false);
        addSynapseToNode(syn->getID(), 88, false);
    }

    // make con - 89 - 4 inputs
    for (int i = 0; i < 4; i++) {
        Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

        saveActionToFile(syn->saveSynapse());

        synapses.addSynapse(syn);

        addNodeToSynapse(76+i, syn->getID(), false);
        addSynapseToNode(syn->getID(), 89, false);
    }

    // set flag - 90 - 2 inputs
    for (int i = 0; i < 2; i++) {
        Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

        saveActionToFile(syn->saveSynapse());

        synapses.addSynapse(syn);

        addNodeToSynapse(78+i, syn->getID(), false);
        addSynapseToNode(syn->getID(), 90, false);
    }

    // 4 val to outputs
    for (int i = 86; i < 91; i++) {
        for (int j = 82; j < 86; j++) {
            Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);

            addNodeToSynapse(j, syn->getID(), false);
            addSynapseToNode(syn->getID(), i, false);
        }
    }

    saveActionToFile("\n");

    //// extra outputs and syns to play with ////
    for (int i = 0; i < 5; i++) {
        Nodes::Node* n = new Nodes::Output(nodes.getNextID());

        saveActionToFile(n->saveNode());

        nodes.addNode(n);
        outputs.addNode(n);

        Synapses::Synapse* syn = new Synapses::WeightedSynapse(synapses.getNextID(), dist(mt));

        saveActionToFile(syn->saveSynapse());

        synapses.addSynapse(syn);
    }

    saveActionToFile("\n");
}

float Controller::getUnusedPartFitnessImpact() {
    auto uunodes = (float)unusedNodes.getNumItems();
    auto uusyns = (float)unusedSynapses.getNumItems();
    auto totalNetSize = (float)nodes.getNumItems() + (float)synapses.getNumItems();

    float scaledUURatio = 1 + (uunodes + (2 * (uusyns))) / totalNetSize;

    float impactValue = (-1 * (0.1f * totalNetSize) - ((2 * uusyns) + uunodes))
                        + ((pow(scaledUURatio, uunodes) / totalNetSize)
                            + (pow(scaledUURatio, uusyns) / totalNetSize));

    cout << min(max(0.0f, impactValue), 100.0f) << endl;

    return min(max(0.0f, impactValue), 100.0f);
}

float Controller::getCalcTimeFitnessImpact() {
    float avgCalcPerOutput = calcAvg.getAverage() / (float)outputs.getNumItems();
    float currCalcPerOutput = (float)calcTime / (float)outputs.getNumItems();
    float avgToCurrCalcRatio = avgCalcPerOutput / currCalcPerOutput;

    // take nanosecons per second, scale to nanoseconds per cycle delay, divide by total outputs to get optimal time usage
    // subtract actual time per output to determine if suboptimal performance is reached. divide by 10mil for scaling
    float largeAvgCalcTimePunishment = (((1000000000.0f * ((float)loopwait / 1000)) / (float)outputs.getNumItems()) - (avgCalcPerOutput * ((float)loopwait / 1000))) / 10000000;

    // avgCalcPerOutput scales so that lower calcTime compared to average is good. largeAvg punishment increases percentage
    // when the avg time is lower than the time needed to make calculations take as long as the cycle delay. out of 100
    // to get a basic percentage and then a multiplyer. lower avg and bigger improvement is good
    float impactValue = (100 * avgToCurrCalcRatio + largeAvgCalcTimePunishment) / 100;

    cout << impactValue << endl;

    return impactValue;
}

float Controller::getFitnessFitnessImpact() {
    float fitnessCurrToAvgRatio = prevFitness / ((bool)fitnessAvg.getAverage() ? fitnessAvg.getAverage() : 1);
    float decTurnsScaled = (float)fitDecTurns / ((float)log(fitDecTurns + 0.272) + 1);

    float impactValue = decTurnsScaled * fitnessCurrToAvgRatio;

    cout << impactValue << endl;

    return impactValue;
}

float Controller::calcFitness() {
    fitness = 100 * getCalcTimeFitnessImpact() + getFitnessFitnessImpact() - getUnusedPartFitnessImpact();
    fitDecTurns = fitness >= prevFitness ? fitDecTurns + 1 : 1;
    fitnessDelta = fitness - prevFitness;
    prevFitness = fitness;
    fitnessAvg.addValue(fitness);

    return fitness;
}

float Controller::getFitness() const { return fitness; }

void Controller::setMetricInputs() {
    unusedNodesInput->setValue((float)unusedNodes.getNumItems());
    unusedSynsInput->setValue((float)unusedSynapses.getNumItems());
    networkSizeInput->setValue((float)nodes.getNumItems() + (float)synapses.getNumItems());

    fitnessInput->setValue(calcFitness());
    fitnessDeltaInput->setValue(fitnessDelta);
    fitnessAvgInput->setValue(fitnessAvg.getAverage());
    turnsSinceFitnessDecInput->setValue((float)fitDecTurns);

    outputCalcTimeInput->setValue((float)calcTime / 1000000.0f);
}

void Controller::actionNodeUpdateWeightFunction(Nodes::ActionNode *actionNode) {
    auto node = dynamic_cast<Nodes::UpdateWeightNode*>(actionNode);

    unsigned int targetID = (unsigned int)(node->getTargetID() * (float)weightedSynapses.getNumItems()) * weightedSynapses.getNumItems();
    float weightModifier = node->getWeightModifier();
    bool replacing = node->replacingWeight();

    auto target = (Synapses::WeightedSynapse*)weightedSynapses.getSynapseByCount(targetID);

    float newWeight = replacing ? weightModifier : target->getWeight() + weightModifier;

    target->setWeight(newWeight);

    string saveString = "=sw " + to_string(target->getID()) + " " + to_string(target->getWeight());
    saveActionToFile(saveString);
}

void Controller::actionNodeUpdateNodeValueFunction(Nodes::ActionNode *actionNode) {
    auto node = dynamic_cast<Nodes::UpdateNodeValueNode*>(actionNode);

    unsigned int targetID = (unsigned int)(node->getTargetID() * (float)valueInputs.getNumItems()) * valueInputs.getNumItems();
    float valueModifier = node->getValueModifier();
    bool replacing = node->replacingValue();

    Nodes::Node* target = valueInputs.getNodeByCount(targetID);

    float newValue = replacing ? valueModifier : target->getValue() + valueModifier;

    target->setValue(newValue);

    string saveString = "=iv " + to_string(target->getID()) + " " + to_string(target->getValue());
    saveActionToFile(saveString);
}


