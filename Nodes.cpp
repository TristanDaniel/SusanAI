
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>
#include <unordered_map>

//#include "Nodes.h" // redundant include, keeping to be explicit
#include "Synapses.h"
#include "Utils.h"


using namespace std;
using Nodes::Node;
using Nodes::NotInputNode;
using Nodes::Input;
using Nodes::RandomInput;
using Nodes::Output;
using Flags::NodeFlag;


Node::Node(unsigned int i) : Structures::Part(i) {}
Input::Input(unsigned int i) : Node(i) {}

float NotInputNode::getValue() {

    // If the map isn't empty, it means we reached this point in a cycle
    if (!synCheckStatus.empty()) {
        unordered_map<int, char> nm;
        nm = synCheckStatus;
        float val = getValueInLoop(nm);
        synCheckStatus = nm;
        lastValue = val;
        return val;
    }

    // at this point we know we are reaching the node for the first time,
    // so we can set the value to 0 and populate the map.
    // this initiates a clean value check,
    // meaning all synapses are assumed to be unchecked.
    for (auto syn : synapses) {
        synCheckStatus[syn->getID()] = 'u';
    }

    // TODO: add if statement to check injection, don't reset val if true
    value = 0;

	for (auto& syn : synapses) {
        synCheckStatus[syn->getID()] = 'v';
        value += syn->getData();
        synCheckStatus[syn->getID()] = 'c';
	}

	lastValue = value;

    synCheckStatus.clear();

	return value;

}

float NotInputNode::getValueInLoop(unordered_map<int, char> statChecks) {
    switch (cycleFlag) {
        case NodeFlag::IGNORE_ON_CYCLE:
            return 0;
        case NodeFlag::PARTIAL_ON_CYCLE:
            lastValue = value;
            return value;
        case NodeFlag::NONE_FLAG:
        default: //default cycle handler is to ignore value of visiting nodes and finish calc
            float tempVal = 0;
            for (auto& syn : synapses) {
                if (synCheckStatus[syn->getID()] == 'u') {
                    synCheckStatus[syn->getID()] = 'v';
                    tempVal += syn->getData();
                    synCheckStatus[syn->getID()] = 'c';
                }
            }

            lastValue = value + tempVal;
            return lastValue;
    }
}

float Node::getLastValue() const { return lastValue; }

float Input::getValue() { return value; }

float RandomInput::getValue() {
	switch (mode)
	{
	case 0:
		return genDefault();
	case 1:
		return genNegDefault();
	case 2:
		return genExpanded();
	case 3:
		return genUpperbound(maximum);
	case 4:
		return genBounded(minimum, maximum);
	case 5:
		return genBoundedInts(minimum, maximum);
	default:
		return genDefault();
	}
}

float RandomInput::getValueInLoop(std::unordered_map<int, char> statChecks) { return 0; }

void Node::setValue(float v) { value = v; }

void Node::setFlags(std::vector<Flags::NodeFlag> f) {

    for (NodeFlag flag : f) {
        addFlag(flag);
    }
}
void Node::addFlag(Flags::NodeFlag f) {
    switch (f) {
        case NodeFlag::IGNORE_ON_CYCLE:
        case NodeFlag::PARTIAL_ON_CYCLE:
            cycleFlag = f;
            break;
        default:
            break;
    }
}
void Node::removeFlag(Flags::NodeFlag f) {
    switch (f) {
        case NodeFlag::IGNORE_ON_CYCLE:
        case NodeFlag::PARTIAL_ON_CYCLE:
            cycleFlag = Flags::NodeFlag::NONE_FLAG;
            break;
        default:
            break;
    }



    //flags.erase(remove(flags.begin(), flags.end(), f), flags.end());
}
//bool Node::hasFlag(Flags::NodeFlag f) { return find(flags.begin(), flags.end(), f) != flags.end(); }


void Node::addSynapse(Synapses::Synapse* syn) { synapses.push_back(syn); }
void Node::removeSynapse(Synapses::Synapse* syn) {
	//const Synapses::Synapse s = syn;

	synapses.erase(remove(synapses.begin(), synapses.end(), syn), synapses.end());
}

float RandomInput::randFloat() { return (float)(rand()) / (float)(RAND_MAX); }
float RandomInput::randFloat(float min, float max) { return min + (randFloat() * (float)(max - min)); }

int RandomInput::randInt() { return rand(); }
int RandomInput::randInt(int min, int max) { return min + (randInt() % (max - min)); }


float RandomInput::genDefault() { return randFloat(); }
float RandomInput::genNegDefault() { return -1 * genDefault(); }
float RandomInput::genExpanded() { return randFloat(-1, 1); }
float RandomInput::genUpperbound(float max) { return randFloat(0, max); }
float RandomInput::genBounded(float min, float max) { return randFloat(min, max); }
float RandomInput::genBoundedInts(int min, int max) { return randInt(min, max); }


RandomInput::RandomInput(unsigned int i, int m) : Input(i), mode(m), minimum(0), maximum(1) { srand(time(0)); }
RandomInput::RandomInput(unsigned int i, int m, float min, float max) : Input(i), mode(m) {
	srand(time(0));
	
	if (min < max) {
		minimum = min;
		maximum = max;
	}
	else {
		minimum = max;
		maximum = min;
	}
}

void Output::getOutput() {
	value = getValue();
	cout << value << endl;
}


