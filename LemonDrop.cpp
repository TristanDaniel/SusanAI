
#include <windows.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "LemonDrop.h"
#include "Utils.h"

using namespace std;

using LemonDrop::Controller;

Controller::Controller() {
    nodes.InitHandler();
    outputs.InitHandler();
    synapses.InitHandler();

//    std::ofstream saveFile("..\\controller.lsv");
//    saveFile.close();

    initController();
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

            saveActionToFile(s->saveSynapse());

            cout << s->saveSynapse() << endl;

            return true;
        }

        default:
            return false;


    }
}

bool Controller::addSynapseToNode(unsigned int synID, unsigned int nodeID, bool loading) {
    Synapses::Synapse* synapse = synapses.getSynapseByID(synID);
    Nodes::Node* node = nodes.getNodeByID(nodeID);

    node->addSynapse(synapse);

    if (!loading) saveActionToFile(">sn " + to_string(synID) + " " + to_string(nodeID) + " ");

    return true;
}

bool Controller::addNodeToSynapse(unsigned int nodeID, unsigned int synID, bool loading) {
    Synapses::Synapse* synapse = synapses.getSynapseByID(synID);
    Nodes::Node* node = nodes.getNodeByID(nodeID);

    synapse->setInput(node);

    if (!loading) saveActionToFile(">ns " + to_string(nodeID) + " " + to_string(synID) + " ");

    return true;
}

void Controller::getAllOutputs() {
    for (Nodes::Node* output : outputs.getNodes()) {
        if (auto* actionNode = dynamic_cast<Nodes::ActionNode *>(output)) {
            if (!actionNode->getValue()) {
                switch (actionNode->getActionType()) {
                    case Flags::ActionFlag::DO_NOTHING:
                        break;
                    case Flags::ActionFlag::ADD_NODE:
                        actionNodeAddNodeFunction(actionNode);
                        break;
                    case Flags::ActionFlag::ADD_SYNAPSE:
                        actionNodeAddSynapseFunction(actionNode);
                        break;
                    case Flags::ActionFlag::NODE_TO_SYN:
                        actionNodeNodeToSynFunction(actionNode);
                        break;
                    case Flags::ActionFlag::SYN_TO_NODE:
                        actionNodeSynToNodeFunction(actionNode);
                        break;
                    case Flags::ActionFlag::NODE_TO_NODE:
                        actionNodeNodeToNodeFunction(actionNode);
                        break;
                    case Flags::ActionFlag::SYN_TO_SYN:
                        actionNodeSynToSynFunction(actionNode);
                        break;
                    case Flags::ActionFlag::SET_FLAG_FOR_NODE:
                        actionNodeSetFlagForNodeFunction(actionNode);
                        break;
                }
            }
        } else ((Nodes::Output *) output)->getOutput(); //warning is fine, ignore
    }
}

[[noreturn]] void Controller::mainLoop() {
    while (true) {
        getAllOutputs();
        sleep(1);
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
                    nodes.checkID(id);
                } else if (infobit == "+n3") {
                    // Output
                    ss >> id;
                    ss >> cycleFlag;

                    n = new Nodes::Output(id);

                    n->addFlag(static_cast<Flags::NodeFlag>(cycleFlag));

                    nodes.addNode(n);
                    outputs.addNode(n);
                    nodes.checkID(id);
                } else if (infobit == "+n4") {
                    // fireable
                } else if (infobit == "+n5") {
                    // action
                    // action type tells which action node to make
                    // dont need separate nodetype checks,
                    // just use switch to know which one to make
                } else if (infobit == "+s0") {
                    //passthrough syn
                    ss >> id;

                    syn = new Synapses::PassthroughSynapse(id);

                    synapses.addSynapse(syn);
                    synapses.checkID(id);
                } else if (infobit == "+s1") {
                    //weighted syn
                    float weight;
                    ss >> id;
                    ss >> weight;

                    syn = new Synapses::WeightedSynapse(id, weight);

                    synapses.addSynapse(syn);
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

                    n->addSynapse(syn1);
                    syn2->setInput(n);
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
    auto* node = dynamic_cast<Nodes::AddNodeNode*>(actionNode);

    int nodeType = node->getNodeType();
    ParamPackages::NodeParams params;

    newNode(nodeType, params);
}







