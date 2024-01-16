
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <filesystem>

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

    actionGroups.push_back(&baseAG1);
    actionGroups.push_back(&baseAG2);
    actionGroups.push_back(&extraAG);

    synapses.InitHandler();
    weightedSynapses.InitHandler();
    unusedSynapses.InitHandler();

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
    lastActionTypeInput = new Nodes::Input(nodes.getNextID()); // 8
    nodes.addNode(lastActionTypeInput);
    actionTypeAgvInput = new Nodes::Input(nodes.getNextID()); // 9
    nodes.addNode(actionTypeAgvInput);
    turnsSinceStructureChangeInput = new Nodes::Input(nodes.getNextID()); // 10
    nodes.addNode(turnsSinceStructureChangeInput);

    fitnessAvg = UtilClasses::RunningAverage<float>(fitAvgTurns);
    calcAvg = UtilClasses::RunningAverage<long long int>(calcAvgTurns);
    actionTypeAvg = UtilClasses::RunningAverage<int>(actionTypeAvgTurns);

    fitness = 100;
    prevFitness = 100;
    fitnessDelta = 0;
    calcTime = 3000000;
    fitDecTurns = 1;
    lastActionType = 0;
    turnsSinceStructureChange = 0;
    timesDoneNothing = 0;
}

Controller::Controller(const std::string& contName, const bool generateNew, const bool wsMode, const bool vaMode) : Controller() {
    name = contName;
    withoutSaveMode = wsMode;
    verboseActionsMode = vaMode;

    if (generateNew) {
        if (!withoutSaveMode) {
            std::ofstream saveFile("..\\" + contName + ".lsv");
            saveFile.close();
        }

        generateInitialController();
    } else {
        initController();
        loadSavedData();
    }

//    baseAG1.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(138)));
//    baseAG1.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(139)));
//    baseAG2.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(140)));
//    baseAG1.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(141)));
//    baseAG2.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(142)));
//    baseAG2.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(143)));

    if (!withoutSaveMode) saveActionToFile("\n");
}

Controller::Controller(const std::string &contName, const std::string &fileToLoadFrom, const bool wsMode, const bool vaMode) : Controller() {
    name = fileToLoadFrom;
    withoutSaveMode = wsMode;
    verboseActionsMode = vaMode;

    initController();
    loadSavedData();

    if (!withoutSaveMode) filesystem::copy("..\\" + fileToLoadFrom + ".lsv", "..\\" + contName + ".lsv", filesystem::copy_options::overwrite_existing);
    name = contName;

    if (!withoutSaveMode) saveActionToFile("\n");

//    baseAG1.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(138)));
//    baseAG1.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(139)));
//    baseAG1.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(140)));
//    baseAG2.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(141)));
//    baseAG2.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(142)));
//    baseAG2.addNode(dynamic_cast<Nodes::ActionNode *>(nodes.getNodeByID(143)));
}

void Controller::operator()(int turnLimit) {
    mainLoop(turnLimit);
    totalSave(name);
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

            //cout << n->saveNode() << endl;

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

            //cout << n->saveNode() << endl;

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

            //cout << n->saveNode() << endl;

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

            //cout << n->saveNode() << endl;

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

            //cout << n->saveNode() << endl;

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
                    n = new Nodes::AddNodeNode(id, threshold);

                    //n = new Nodes::ActionNode(id, threshold, 0);
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

            extraAG.addNode(dynamic_cast<Nodes::ActionNode *>(n));

            saveActionToFile(n->saveNode());

            //cout << n->saveNode() << endl;

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

            //cout << n->saveNode() << endl;

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

            //cout << s->saveSynapse() << endl;

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

            //cout << s->saveSynapse() << endl;

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
    if ((Nodes::ActionNode*)prevSynOutput) {
        brokeActionNode = true;
        return false;
    }

    node->addSynapse(synapse);

    if (prevSynOutput && prevSynOutput->isUnused()) unusedNodes.addNode(prevSynOutput);

    if (synuu && !synapse->isUnused()) unusedSynapses.removeSynapseByID(synID);
    else if (!synuu && synapse->isUnused()) unusedSynapses.addSynapse(synapse);
    if (nodeuu && !node->isUnused()) unusedNodes.removeNodeByID(nodeID);

    string saveString = ">sn " + to_string(synapse->getID()) + " " + to_string(node->getID()) + " ";

    if (!loading) {
        saveActionToFile(saveString);
        //cout << saveString << endl;
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
        //cout << saveString << endl;
    }

    return true;
}

void Controller::getAllOutputs() {
    //float highestFiringActionValue = 0;
    //Nodes::ActionNode* actionNode = nullptr;
    brokeActionNode = false;

    queue<Nodes::ActionNode*> actionQueue;
    auto start = chrono::high_resolution_clock::now();

    /*for (Nodes::Node* output : outputs.getNodes()) {
        if (auto* actionNode1 = dynamic_cast<Nodes::ActionNode *>(output)) {
            float value = actionNode1->getValue();
            if (value > highestFiringActionValue) {
                actionNode1->getOutput();

                highestFiringActionValue = value;
                actionNode = actionNode1;
            }
        } else ((Nodes::Output *) output)->getOutput();
    */

    for (auto ag : actionGroups) {
        Nodes::ActionNode* actionNode = ag->getActionNode(turn);
        if (actionNode != nullptr) actionQueue.push(actionNode);
        else actionQueue.push(new Nodes::ActionNode(0, 0, 0));
    }

    for (auto n : outputs.getNodes()) {
        ((Nodes::Output*)n)->getOutput(turn);
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    calcTime = duration.count();
    calcAvg.addValue(calcTime);
    //cout << (calcAvg.getAverage() / 1000000) << " ms, " << outputs.getNumItems() << endl;

    if (actionQueue.empty()) {
        lastActionType = 0;
        actionTypeAvg.addValue(lastActionType);

        return;
    }
    //if (highestFiringActionValue == 0) return;
    while (!actionQueue.empty()) {
        Nodes::ActionNode* actionNode = actionQueue.front();
        actionQueue.pop();

        Flags::ActionFlag actionType = actionNode->getActionType();

        switch (actionType) {
            case Flags::ActionFlag::DO_NOTHING:
                if (verboseActionsMode) cout << "Doing nothing" << endl;
                break;
            case Flags::ActionFlag::ADD_NODE:
                actionNodeAddNodeFunction(actionNode);
                turnsSinceStructureChange = 0;
                break;
            case Flags::ActionFlag::ADD_SYNAPSE:
                actionNodeAddSynapseFunction(actionNode);
                turnsSinceStructureChange = 0;
                break;
            case Flags::ActionFlag::MAKE_CONNECTION:
                if (!actionNodeMakeConnectionFunction(actionNode)) actionType = Flags::ActionFlag::DO_NOTHING;
                turnsSinceStructureChange = 0;
                break;
            case Flags::ActionFlag::SET_FLAG_FOR_NODE:
                actionNodeSetFlagForNodeFunction(actionNode);
                break;
            case Flags::ActionFlag::UPDATE_WEIGHT:
                actionNodeUpdateWeightFunction(actionNode);
                break;
            case Flags::ActionFlag::UPDATE_NODE_VALUE:
                actionNodeUpdateNodeValueFunction(actionNode);
                break;
        }

        lastActionType1 = lastActionType2;
        lastActionType2 = lastActionType3;
        lastActionType3 = lastActionType4;
        lastActionType4 = lastActionType5;
        lastActionType5 = lastActionType6;
        lastActionType6 = (int)actionType;
        actionTypeAvg.addValue(lastActionType6);
    }

    ag1repeatTurns = lastActionType1 == lastActionType4 ? ag1repeatTurns + 1 : 0;
    ag2repeatTurns = lastActionType2 == lastActionType5 ? ag2repeatTurns + 1 : 0;

}

void Controller::loop() {
    if (verboseActionsMode) cout << "loop" << endl;

    turn++;
    //cout << to_string(turn) << endl;

    setMetricInputs();
    if (verboseActionsMode) cout << "F: " << fitness << endl;

    getAllOutputs();

    turnsSinceStructureChange++;

    turn++;

    //this_thread::sleep_for(chrono::milliseconds(loopwait));
}
[[noreturn]] void Controller::mainLoop() {
    turn = 0;
    while (true) {
        loop();
    }
}
void Controller::mainLoop(const int turnLimit) {
    turn = 0;
    while (turn < turnLimit) {
        loop();
    }

    std::ofstream saveFile("..\\" + name + "_data.txt");
    saveFile << to_string(fitness) << endl;
    saveFile << to_string(fitDecTurns) << endl;
    saveFile << to_string(calcTime) << endl;
    saveFile << to_string(calcAvg.getAverage()) << endl;
    saveFile << to_string(lastActionType) << endl;
    saveFile << to_string(actionTypeAvg.getAverage()) << endl;
    saveFile << to_string(turnsSinceStructureChange) << endl;
    saveFile << to_string(timesDoneNothing) << endl;
}


void Controller::saveActionToFile(const std::string& s) {
    if (verboseActionsMode) cout << s << endl;
    if (withoutSaveMode) return;

    std::ofstream saveFile("..\\" + name + ".lsv", std::ios::app);

    if (saveFile.is_open()) {
        saveFile << s;
        saveFile.close();
    } else std::cout << "WARN: save file not found, network won't be saved properly!";
}

void Controller::loadFromFile() {
    std::ifstream loadFile ("..\\" + name + ".lsv");

    if (loadFile.is_open()) {
        string line;

        int id, cycleFlag, dropoutFlag;
        Nodes::Node* n;
        Synapses::Synapse* syn;


        while (getline(loadFile, line)) {
            stringstream  ss(line);
            string infobit; //thanks logan for the name

            while (!ss.eof()) {
                infobit = "";
                ss >> infobit;
                if (infobit == "+n0") {
                    // basic node
                    ss >> id;
                    ss >> cycleFlag;
                    ss >> dropoutFlag;

                    n = new Nodes::NotInputNode(id);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));
                    n->addFlag(static_cast<Flags::NodeFlag>(dropoutFlag));

                    nodes.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n1") {
                    // input
                    ss >> id;
                    ss >> cycleFlag;
                    ss >> dropoutFlag;

                    float val;
                    ss >> val;

                    n = new Nodes::Input(id);

                    n->setValue(val);

                    nodes.addNode(n);
                    valueInputs.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n2") {
                    // Random input
                    int mode;
                    float min, max;

                    ss >> id;
                    ss >> cycleFlag;
                    ss >> dropoutFlag;
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
                    ss >> dropoutFlag;

                    n = new Nodes::Output(id);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));
                    n->addFlag(static_cast<Flags::NodeFlag>(dropoutFlag));

                    nodes.addNode(n);
                    outputs.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n4") {
                    // fireable

                    ss >> id;
                    ss >> cycleFlag;
                    ss >> dropoutFlag;

                    float threshold;
                    ss >> threshold;

                    n = new Nodes::FireableNode(id, threshold);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));
                    n->addFlag(static_cast<Flags::NodeFlag>(dropoutFlag));

                    nodes.addNode(n);
                    unusedNodes.addNode(n);
                    nodes.checkID(id);

                } else if (infobit == "+n5") {
                    // action

                    ss >> id;
                    ss >> cycleFlag;
                    ss >> dropoutFlag;

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
                        case 6:
                            n = new Nodes::UpdateNodeValueNode(id, threshold);
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

//                    if (id2 > 140) {
//                        cout << "aaaaaaaaaa" << endl;
//                    }

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
                } else if (infobit == ">ag") {
                    // add node to action group
                    ss >> id;
                    int agNum;
                    ss >> agNum;

                    auto* an = (Nodes::ActionNode*)nodes.getNodeByID(id);
                    switch (agNum) {
                        case 0:
                            baseAG1.addNode(an);
                            break;
                        case 1:
                            baseAG2.addNode(an);
                            break;
                        case 2:
                            extraAG.addNode(an);
                            break;
                        default:
                            break;
                    }
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

bool Controller::actionNodeMakeConnectionFunction(Nodes::ActionNode *actionNode) {
    auto* node = dynamic_cast<Nodes::MakeConnectionNode*>(actionNode);

    int conType = node->getConnectionType();
    float id1 = abs(node->getID1());
    float id2 = abs(node->getID2());
    float id3 = abs(node->getID3());

    bool uu1 = node->getUU1();
    bool uu2 = node->getUU2();
    bool uu3 = node->getUU3();

    bool success = true;

    switch (conType) {
        case 0:
        {
            if (unusedNodes.getNumItems() == 0) uu1 = false;
            if (unusedSynapses.getNumItems() == 0) uu2 = false;

            unsigned int nodeIDLimVal = uu1 ? unusedNodes.getNumItems() : nodes.getCurrID();
            unsigned int synIDLimVal = uu2 ? unusedSynapses.getNumItems() : synapses.getCurrID();

            unsigned int nodeID = (unsigned int)(id1 * (float)nodeIDLimVal) % nodeIDLimVal;
            unsigned int synID = (unsigned int)(id2 * (float)synIDLimVal) % synIDLimVal;

            addNodeToSynapse(nodeID, uu1, synID, uu2, false);
            break;
        }

        case 1:
        {
            if (unusedNodes.getNumItems() == 0) uu2 = false;
            if (unusedSynapses.getNumItems() == 0) uu1 = false;

            unsigned int nodeIDLimVal = uu2 ? unusedNodes.getNumItems() : nodes.getCurrID();
            unsigned int synIDLimVal = uu1 ? unusedSynapses.getNumItems() : synapses.getCurrID();

            unsigned int nodeID = (unsigned int)(id2 * (float)nodeIDLimVal) % nodeIDLimVal;
            unsigned int synID = (unsigned int)(id1 * (float)synIDLimVal) % synIDLimVal;

            success = addSynapseToNode(synID, uu1, nodeID, uu2, false);
            break;
        }
        case 2:
        {
            if (unusedNodes.getNumItems() == 0) uu1 = false;
            if (unusedSynapses.getNumItems() == 0) uu2 = false;
            if (unusedNodes.getNumItems() == 0) uu3 = false;

            unsigned int nodeIDLimVal = uu1 ? unusedNodes.getNumItems() : nodes.getCurrID();
            unsigned int synIDLimVal = uu2 ? unusedSynapses.getNumItems() : synapses.getCurrID();
            unsigned int node2IDLimVal = uu3 ? unusedNodes.getNumItems() : nodes.getCurrID();

            unsigned int nodeID = (unsigned int)(id1 * (float)nodeIDLimVal) % nodeIDLimVal;
            unsigned int synID = (unsigned int)(id2 * (float)synIDLimVal) % synIDLimVal;
            unsigned int node2ID = (unsigned int)(id3 * (float)node2IDLimVal) % node2IDLimVal;


            addNodeToSynapse(nodeID, uu1, synID, uu2, false);
            success = addSynapseToNode(synID, uu2, node2ID, uu3, false);
            break;
        }

        case 3:
        {
            if (unusedSynapses.getNumItems() == 0) uu1 = false;
            if (unusedSynapses.getNumItems() == 0) uu2 = false;

            unsigned int synIDLimVal = uu1 ? unusedSynapses.getNumItems() : synapses.getCurrID();
            unsigned int syn2IDLimVal = uu2 ? unusedSynapses.getNumItems() : synapses.getCurrID();

            unsigned int synID = (unsigned int)(id1 * (float)synIDLimVal) % synIDLimVal;
            unsigned int syn2ID = (unsigned int)(id2 * (float)syn2IDLimVal) % syn2IDLimVal;


            auto* n = new Nodes::NotInputNode(nodes.getNextID());
            nodes.addNode(n);

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

    return success;
}

void Controller::actionNodeSetFlagForNodeFunction(Nodes::ActionNode *actionNode) {
    auto* node = dynamic_cast<Nodes::SetFlagNode*>(actionNode);

    unsigned int targetID = (unsigned int)(abs(node->getTargetID()) * nodes.getCurrID()) % nodes.getCurrID();
    auto flag = (Flags::NodeFlag)abs(node->getFlagVal());

    Nodes::Node* n = nodes.getNodeByID(targetID);
    n->addFlag(flag);

    string saveString = "=f " + to_string(targetID) + " " + to_string(abs(node->getFlagVal())) + " ";
    saveActionToFile(saveString);
    //cout << saveString << endl;
}


void Controller::generateInitialController() {
    using namespace UtilFunctions;
    //// setup ////


    //// metric inputs ////

//    fitnessInput = new Nodes::Input(nodes.getNextID()); // 0
//    nodes.addNode(fitnessInput);
//    unusedNodesInput = new Nodes::Input(nodes.getNextID()); // 1
//    nodes.addNode(unusedNodesInput);
//    unusedSynsInput = new Nodes::Input(nodes.getNextID()); // 2
//    nodes.addNode(unusedSynsInput);
//    networkSizeInput = new Nodes::Input(nodes.getNextID()); // 3
//    nodes.addNode(networkSizeInput);
//    fitnessDeltaInput = new Nodes::Input(nodes.getNextID()); // 4
//    nodes.addNode(fitnessDeltaInput);
//    fitnessAvgInput = new Nodes::Input(nodes.getNextID()); // 5
//    nodes.addNode(fitnessAvgInput);
//    turnsSinceFitnessDecInput = new Nodes::Input(nodes.getNextID()); // 6
//    nodes.addNode(turnsSinceFitnessDecInput);
//    outputCalcTimeInput = new Nodes::Input(nodes.getNextID()); // 7
//    nodes.addNode(outputCalcTimeInput);

    //// layer sizes and helpful numbers ////

    const int startID = 11;
    // base inputs
    const int baseInputs = 2 * (int)LDRandomInt(1,3); // split 50/50 between random and static
    const int baseInputHiddenLayers = (int)LDRandomInt(1,4);
    const int nodesPerInputHiddenLayer = (int)LDRandomInt(1,6);
    const int firstInputHiddenLayerStartID = startID + baseInputs;
    // main hidden layers
    const int hiddenLayers = (int)LDRandomInt(1,6);
    const int nodesPerHiddenLayer = 2 * (int)LDRandomInt(2,15);
    const int firstHiddenLayerStartID = firstInputHiddenLayerStartID
            + (nodesPerInputHiddenLayer * baseInputHiddenLayers);
    // output params inputs split layer
    const int nodesToOutputParams = (int)LDRandomInt(5,15);
    const int nodesToOutputParamsStartID = firstHiddenLayerStartID
            + (nodesPerHiddenLayer * hiddenLayers);
    const int nodesToOutputFiringThreshold = (int)LDRandomInt(2,5);
    const int nodesToOutFireThreshStartID = nodesToOutputParamsStartID + nodesToOutputParams;
    // output params layer
    const int totalOutputParams = 25;
    const int outputParamNodesStartID = nodesToOutFireThreshStartID + nodesToOutputFiringThreshold;
    const int firingThresholdNodes = (int)LDRandomInt(2,4);
    const int firingThresholdNodesStartID = outputParamNodesStartID + totalOutputParams;
    // outputs
    const int actionOutputs = 6;
    const int actionOutputsStartID = firingThresholdNodesStartID + firingThresholdNodes;
    // extras to play with
    const int extraOutputs = 5;
    const int extraOutputsStartID = actionOutputsStartID + actionOutputs;

    //// temp variables ////
    Nodes::Node* n;
    Synapses::Synapse* syn;

    //// creation ////

    // base inputs //
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

    saveActionToFile("\n");

    // base input hidden layers //
    makeStandardLayer(firstInputHiddenLayerStartID, nodesPerInputHiddenLayer);
    saveActionToFile("\n");
    connectTwoLayers(startID, baseInputs, firstInputHiddenLayerStartID, nodesPerInputHiddenLayer);
    saveActionToFile("\n");
    createAndConnectUniformRepeatedLayers(firstInputHiddenLayerStartID, nodesPerInputHiddenLayer, baseInputHiddenLayers);

    saveActionToFile("\n");

    // hidden layers and connect inputs //
    makeStandardLayer(firstHiddenLayerStartID, nodesPerHiddenLayer);
    saveActionToFile("\n");
    connectTwoLayers(0, startID, firstHiddenLayerStartID, nodesPerHiddenLayer);
    connectTwoLayers(firstHiddenLayerStartID - nodesPerInputHiddenLayer, nodesPerInputHiddenLayer, firstHiddenLayerStartID, nodesPerHiddenLayer);
    saveActionToFile("\n");
    createAndConnectUniformRepeatedLayers(firstHiddenLayerStartID, nodesPerHiddenLayer, hiddenLayers);

    saveActionToFile("\n");

    // split layer for output params and firing threshold //
    makeStandardLayer(nodesToOutputParamsStartID, nodesToOutputParams);
    saveActionToFile("\n");
    makeStandardLayer(nodesToOutFireThreshStartID, nodesToOutputFiringThreshold);
    saveActionToFile("\n");

    const int finalHiddenLayerStartID = nodesToOutputParamsStartID - nodesPerHiddenLayer;
    connectTwoLayers(finalHiddenLayerStartID, nodesPerHiddenLayer, nodesToOutputParamsStartID, nodesToOutputParams);
    connectTwoLayers(finalHiddenLayerStartID, nodesPerHiddenLayer, nodesToOutFireThreshStartID, nodesToOutputFiringThreshold);

    saveActionToFile("\n");

    // outputs params and firing threshold //
    makeStandardLayer(outputParamNodesStartID, totalOutputParams);
    saveActionToFile("\n");
    connectTwoLayers(nodesToOutputParamsStartID, nodesToOutputParams, outputParamNodesStartID, totalOutputParams);
    saveActionToFile("\n");

    makeStandardLayer(firingThresholdNodesStartID, firingThresholdNodes);
    for (int i = firingThresholdNodesStartID; i < firstHiddenLayerStartID + firingThresholdNodes; i++) {
        nodes.getNodeByID(i)->addFlag(Flags::NodeFlag::NO_DROPOUT);
    }
    saveActionToFile("\n");
    connectTwoLayers(nodesToOutFireThreshStartID, nodesToOutputFiringThreshold, firingThresholdNodesStartID, firingThresholdNodes);

    saveActionToFile("\n");

    // outputs //
    //Nodes::Node* doNothingOutput = new Nodes::ActionNode(nodes.getNextID(), LDRandomFloat(), 0);
    //saveActionToFile(doNothingOutput->saveNode());
    //nodes.addNode(doNothingOutput);
    //outputs.addNode(doNothingOutput);
    Nodes::Node* addNodeOutput = new Nodes::AddNodeNode(nodes.getNextID(), 0.5);
    saveActionToFile(addNodeOutput->saveNode());
    nodes.addNode(addNodeOutput);
    outputs.addNode(addNodeOutput);
    baseAG1.addNode((Nodes::ActionNode*)addNodeOutput);
    Nodes::Node* addSynOutput = new Nodes::AddSynapseNode(nodes.getNextID(), 0.5);
    saveActionToFile(addSynOutput->saveNode());
    nodes.addNode(addSynOutput);
    outputs.addNode(addSynOutput);
    baseAG1.addNode((Nodes::ActionNode*)addSynOutput);
    Nodes::Node* makeConOutput = new Nodes::MakeConnectionNode(nodes.getNextID(), 0.5);
    saveActionToFile(makeConOutput->saveNode());
    nodes.addNode(makeConOutput);
    outputs.addNode(makeConOutput);
    baseAG2.addNode((Nodes::ActionNode*)makeConOutput);
    Nodes::Node* setFlagOutput = new Nodes::SetFlagNode(nodes.getNextID(), 0.5);
    saveActionToFile(setFlagOutput->saveNode());
    nodes.addNode(setFlagOutput);
    outputs.addNode(setFlagOutput);
    baseAG1.addNode((Nodes::ActionNode*)setFlagOutput);
    Nodes::Node* updateWeightOutput = new Nodes::UpdateWeightNode(nodes.getNextID(), 0.5);
    saveActionToFile(updateWeightOutput->saveNode());
    nodes.addNode(updateWeightOutput);
    outputs.addNode(updateWeightOutput);
    baseAG2.addNode((Nodes::ActionNode*)updateWeightOutput);
    Nodes::Node* updateNodeValueOutput = new Nodes::UpdateNodeValueNode(nodes.getNextID(), 0.5);
    saveActionToFile(updateNodeValueOutput->saveNode());
    nodes.addNode(updateNodeValueOutput);
    outputs.addNode(updateNodeValueOutput);
    baseAG2.addNode((Nodes::ActionNode*)updateNodeValueOutput);


    saveActionToFile("\n");

    // connect outputs to params and threshold nodes //
    // params
    int currParamStartID = outputParamNodesStartID;
    int outputID = actionOutputsStartID;  // doNothing action takes no parameters
    // add node
    connectTwoLayers(currParamStartID, 8, outputID, 1);
    currParamStartID += 8;
    outputID++;
    // add synapse
    connectTwoLayers(currParamStartID, 2, outputID, 1);
    currParamStartID += 2;
    outputID++;
    // make connection
    connectTwoLayers(currParamStartID, 7, outputID, 1);
    currParamStartID += 7;
    outputID++;
    // set flag
    connectTwoLayers(currParamStartID, 2, outputID, 1);
    currParamStartID += 2;
    outputID++;
    // update weight
    connectTwoLayers(currParamStartID, 3, outputID, 1);
    currParamStartID += 3;
    outputID++;
    // update node value
    connectTwoLayers(currParamStartID, 3, outputID, 1);
    currParamStartID += 3;
    outputID++;

    saveActionToFile("\n");

    // threshold
    connectTwoLayers(firingThresholdNodesStartID, firingThresholdNodes, actionOutputsStartID, actionOutputs);

    saveActionToFile("\n");

    // extra outputs to play with //
    for (int i = extraOutputsStartID; i < extraOutputsStartID + extraOutputs; i++) {
        n = new Nodes::Output(i);

        saveActionToFile(n->saveNode());

        nodes.addNode(n);
        outputs.addNode(n);
        unusedNodes.addNode(n);
    }
}

float Controller::getUnusedPartFitnessImpact() {
    float impactValue = abs(prevFitness) / 8;

    auto uunodes = (float)unusedNodes.getNumItems();
    auto uusyns = (float)unusedSynapses.getNumItems();
    float totalUU = uunodes + uusyns;
    auto totalNetSize = (float)nodes.getNumItems() + (float)synapses.getNumItems();

    float uuuRatio = totalUU / totalNetSize;

    impactValue *= uuuRatio;

    if (uuuRatio >= 0.1) impactValue += 100000 * impactValue;

//    float scaledUURatio = 1 + (uunodes + (2 * (uusyns))) / totalNetSize;
//
//    float impactValue = (-1 * (0.1f * totalNetSize) - ((2 * uusyns) + uunodes))
//                        + ((pow(scaledUURatio, uunodes) / totalNetSize)
//                            + (pow(scaledUURatio, uusyns) / totalNetSize));

    if (verboseActionsMode) cout << "UU impact: " << impactValue << endl;

    return impactValue;
}

float Controller::getCalcTimeFitnessImpact() {
    float baseImpact = abs(fitnessAvg.getAverage()) / 16;

    float avgCalcPerOutput = calcAvg.getAverage() / (float)outputs.getNumItems();
    float currCalcPerOutput = (float)calcTime / (float)outputs.getNumItems();
    float avgToCurrCalcRatio = avgCalcPerOutput / currCalcPerOutput;
    if (avgToCurrCalcRatio > 6) avgToCurrCalcRatio = 6;

    float scaledLoopWait = 1000000.0f * (float)loopwait;
    float maxCalcPerOutput = scaledLoopWait / (float)outputs.getNumItems();

    float impactValue = currCalcPerOutput > maxCalcPerOutput ? -100 * baseImpact: baseImpact;

    impactValue += (baseImpact * avgToCurrCalcRatio) - baseImpact;

    if (verboseActionsMode) cout << "CT impact: " << impactValue << endl;

    return impactValue;
}

float Controller::getFitnessFitnessImpact() {
    float impactValue = prevFitness - (abs(prevFitness) * 0.4f);
    if (prevFitness > fitnessAvg.getAverage()) impactValue += abs(prevFitness) / 8;

    impactValue += (float)fitDecTurns;

    if (verboseActionsMode) cout << "FF impact: " << impactValue << endl;

    return impactValue;
}

float Controller::getTurnAndStructureFitnessImpact() {
    float baseImpact = abs(fitnessAvg.getAverage()) / 16;

    float impactValue = abs(prevFitness) / 32;

    if (lastActionType + 0.25 >= actionTypeAvg.getAverage()
        && lastActionType - 0.25 <= actionTypeAvg.getAverage()) impactValue += 50 * baseImpact * (float)turnsSinceStructureChange;

    impactValue += turnsSinceStructureChange > 50 ? 1000 * baseImpact : (float)turnsSinceStructureChange * baseImpact / 10;

    if (brokeActionNode) impactValue += 1000 * baseImpact;

    if (verboseActionsMode) cout << "TS impact: " << impactValue << endl;

    return impactValue;
}

float Controller::calcFitness() {
    int netSize = nodes.getNumItems() + synapses.getNumItems();
    int totalUU = unusedNodes.getNumItems() + unusedSynapses.getNumItems();

    fitness = 100 + ((float)nodes.getNumItems() / 4) + ((float)synapses.getNumItems() / 2);
    fitness -= (float)(unusedNodes.getNumItems() + unusedSynapses.getNumItems())
                * ((float)turnsSinceStructureChange + 1)
                * (((float)totalUU / (float)netSize) > 0.1 ? 10.f : 1);
    if (lastActionType4 + 0.25 >= actionTypeAvg.getAverage()
        && lastActionType4 - 0.25 <= actionTypeAvg.getAverage()) fitness -= 10 * (float)turnsSinceStructureChange;
    if (lastActionType5 + 0.25 >= actionTypeAvg.getAverage()
        && lastActionType5 - 0.25 <= actionTypeAvg.getAverage()) fitness -= 10 * (float)turnsSinceStructureChange;
    fitness += (float)fitDecTurns;
    fitness -= (float)fitIncrTurns * 5;
    if (lastActionType4 == 0) {
        timesDoneNothing++;
        fitness -= 2 * (float)timesDoneNothing;
    }
    if (lastActionType5 == 0) {
        timesDoneNothing++;
        fitness -= 2 * (float)timesDoneNothing;
    }
    fitness -= 10 * (float)ag1repeatTurns;
    fitness -= 10 * (float)ag2repeatTurns;


    fitness = max(-1000000.f, min((float)fitness, 1000000.f));

    //fitness = getFitnessFitnessImpact();
    //fitness -= getTurnAndStructureFitnessImpact();
    //fitness -= getUnusedPartFitnessImpact();
    //fitness += getCalcTimeFitnessImpact();
    //fitness += (abs(prevFitness) / 4);

    fitDecTurns = fitness >= prevFitness ? fitDecTurns + 1 : 1;
    fitIncrTurns = fitness <= prevFitness ? fitIncrTurns + 1 : 0;
    fitnessDelta = fitness - prevFitness;
    prevFitness = fitness;
    fitnessAvg.addValue(fitness);

    return fitness;
}

float Controller::getFitness() { return fitnessAvg.getAverage(); }

void Controller::setMetricInputs() {
    unusedNodesInput->setValue(UtilFunctions::sigmoid((float)unusedNodes.getNumItems()));
    unusedSynsInput->setValue(UtilFunctions::sigmoid((float)unusedSynapses.getNumItems()));
    networkSizeInput->setValue(UtilFunctions::sigmoid((float)nodes.getNumItems() + (float)synapses.getNumItems()));

    fitnessInput->setValue(UtilFunctions::sigmoid(calcFitness()));
    fitnessDeltaInput->setValue(UtilFunctions::sigmoid(fitnessDelta));
    fitnessAvgInput->setValue(UtilFunctions::sigmoid(fitnessAvg.getAverage()));
    turnsSinceFitnessDecInput->setValue(UtilFunctions::sigmoid((float)fitDecTurns));

    outputCalcTimeInput->setValue(UtilFunctions::sigmoid((float)calcTime / 1000000.0f));

    int encodedActionSeries = (lastActionType4 * 2) + (lastActionType5 * 3);
    lastActionTypeInput->setValue((float)encodedActionSeries / 26);
    //lastActionTypeInput->setValue((float)lastActionType / (float)DataBits::NUM_ACTION_TYPES);
    actionTypeAgvInput->setValue(actionTypeAvg.getAverage() / (float)DataBits::NUM_ACTION_TYPES);

    turnsSinceStructureChangeInput->setValue(UtilFunctions::sigmoid((float)turnsSinceStructureChange));
}

void Controller::actionNodeUpdateWeightFunction(Nodes::ActionNode *actionNode) {
    auto node = dynamic_cast<Nodes::UpdateWeightNode*>(actionNode);

    unsigned int targetID = (unsigned int)(node->getTargetID() * (float)weightedSynapses.getNumItems()) % weightedSynapses.getNumItems();
    float weightModifier = node->getWeightModifier();
    bool replacing = node->replacingWeight();

    auto target = (Synapses::WeightedSynapse*)weightedSynapses.getSynapseByCount(targetID);

    float newWeight = replacing ? weightModifier : target->getWeight() + weightModifier;

    target->setWeight(newWeight);

    string saveString = "=sw " + to_string(target->getID()) + " " + to_string(target->getWeight()) + " ";
    saveActionToFile(saveString);
}

void Controller::actionNodeUpdateNodeValueFunction(Nodes::ActionNode *actionNode) {
    auto node = dynamic_cast<Nodes::UpdateNodeValueNode*>(actionNode);

    unsigned int targetID = (unsigned int)(node->getTargetID() * (float)valueInputs.getNumItems()) % valueInputs.getNumItems();
    float valueModifier = node->getValueModifier();
    bool replacing = node->replacingValue();

    Nodes::Node* target = valueInputs.getNodeByCount(targetID);

    float newValue = replacing ? valueModifier : target->getValue(turn) + valueModifier;

    target->setValue(newValue);

    string saveString = "=iv " + to_string(target->getID()) + " " + to_string(target->getValue(turn)) + " ";
    saveActionToFile(saveString);
}

void Controller::makeStandardLayer(const int layerStartID, const int nodesInLayer) {
    Nodes::Node* node;

    for (int nodeID = layerStartID; nodeID < layerStartID + nodesInLayer; nodeID++) {
        node = new Nodes::NotInputNode(nodeID);
        node->addFlag(Flags::NodeFlag::DROPOUT_20);

        saveActionToFile(node->saveNode());

        nodes.addNode(node);
    }
}

void Controller::connectTwoLayers(const int prevLayerStartID, const int nodesInPrevLayer, const int nextLayerStartID, const int nodesInNextLayer) {
    Synapses::WeightedSynapse* syn;

    for (int nextLayerNodeID = nextLayerStartID; nextLayerNodeID < nextLayerStartID + nodesInNextLayer; nextLayerNodeID++) {
        for (int prevLayerNodeID = prevLayerStartID; prevLayerNodeID < prevLayerStartID + nodesInPrevLayer; prevLayerNodeID++) {
            syn = new Synapses::WeightedSynapse(synapses.getNextID(), (UtilFunctions::LDRandomFloat() * 2) - 1);

            saveActionToFile(syn->saveSynapse());

            synapses.addSynapse(syn);
            weightedSynapses.addSynapse(syn);

            addNodeToSynapse(prevLayerNodeID, syn->getID(), false);
            addSynapseToNode(syn->getID(), nextLayerNodeID, false);
        }
    }
}

void Controller::createAndConnectUniformRepeatedLayers(const int firstLayerStartID, const int nodesPerLayer, const int layers) {
    // assumes first layer is already made
    for (int i = 1; i < layers; i++) {
        const int layerStartID = firstLayerStartID + (nodesPerLayer * i);
        makeStandardLayer(layerStartID, nodesPerLayer);
        connectTwoLayers(layerStartID - nodesPerLayer, nodesPerLayer, layerStartID, nodesPerLayer);
    }
}

string Controller::getName() const { return name; }

void Controller::totalSave() {
    filesystem::copy("..\\" + name + ".lsv", "..\\" + name + "_backup_"
                        + to_string(turn) + ".lsv");

    totalSave(name);
}
void Controller::totalSave(const std::string& fileName) {
    std::ofstream saveFile("..\\" + fileName + ".lsv");
    std::ofstream graphFile("..\\" + fileName + ".graph");

    graphFile << "digraph m {\n"
                 "sep=0\n"
                 "layout=\"neato\"\n"
                 "overlap=\"scale\"\n"
                 "node[width=.25,height=.375,fontsize=9]\n";

    for (auto n : nodes.getNodes()) {
        n->graphSave(graphFile);
        if (n->getID() < 11) continue;
        n->totalSave(saveFile);
    }

    graphFile << "}";

    saveFile << endl;

    int num = 0;
    for (auto syn : synapses.getSynapses()) {
        syn->totalSave(saveFile, graphFile);
        num++;
        if (num == 200) {
            saveFile << endl;
            num = 0;
        }
    }

    saveFile << endl;

    for (auto n : baseAG1.getActionNodes()) {
        saveFile << ">ag " + to_string(n->getID()) + " 0 ";
    }
    for (auto n : baseAG2.getActionNodes()) {
        saveFile << ">ag " + to_string(n->getID()) + " 1 ";
    }
    for (auto n : extraAG.getActionNodes()) {
        saveFile << ">ag " + to_string(n->getID()) + " 2 ";
    }

    saveFile << endl;

    saveFile.close();
    graphFile.close();
}

void Controller::setWithoutSaveMode(bool mode) { withoutSaveMode = mode; }
void Controller::setVerboseActionsMode(bool mode) { verboseActionsMode = mode; }

void Controller::resetFitness() {
    fitness = 100;

    for (int i = 0; i < fitAvgTurns; i++) {
        fitnessAvg.addValue(100);
    }
}

float Controller::getSavedFitness() {
    std::ifstream loadFile("..\\" + name + "_data.txt");

    float fit;
    loadFile >> fit;

    loadFile.close();

    return fit;
}

void Controller::loadSavedData() {
    std::ifstream loadFile("..\\" + name + "_data.txt");

    loadFile >> fitness;
    prevFitness = fitness;
    for (int i = 0; i < fitAvgTurns; i++) {
        fitnessAvg.addValue(fitness);
    }

    fitnessDelta = 0;

    loadFile >> fitDecTurns;
    loadFile >> calcTime;
    float calcTimeAvg;
    loadFile >> calcTimeAvg;
    for (int i = 0; i < calcAvgTurns; i++) {
        calcAvg.addValue((long long)calcTimeAvg);
    }

    loadFile >> lastActionType;
    float actionAvg;
    loadFile >> actionAvg;
    for (int i = 0; i < actionTypeAvgTurns; i++) {
        actionTypeAvg.addValue((int)actionAvg);
    }

    loadFile >> turnsSinceStructureChange;

    loadFile.close();
}



