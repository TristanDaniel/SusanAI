
#include <algorithm>
#include <random>
#include <iostream>
#include <unordered_map>

//#include "Nodes.h" // redundant include, keeping to be explicit
#include "Synapses.h"
#include "Utils.h"


using namespace std;
using namespace Nodes;
//using Nodes::Node;
//using Nodes::NotInputNode;
//using Nodes::Input;
//using Nodes::RandomInput;
//using Nodes::Output;
using Flags::NodeFlag;
//using Nodes::Fireable;
//using Nodes::FireableNode;
//using Nodes::ActionNode;


Node::Node(unsigned int i) : Structures::Part(i), turn(0), value(0), lastValue(0) {}
Input::Input(unsigned int i) : Node(i) {}

float NotInputNode::getValue(unsigned long long int curTurn) {
    // If the map isn't empty, it means we reached this point in a cycle
    if (!synCheckStatus.empty()) {
        unordered_map<unsigned int, char> nm;
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

    if (turn == curTurn) {
        //cout << "Net turn: " << DataBits::getTurn() << " Node turn: " << turn << endl;
        lastValue = value;
        return value;
    }

    turn = curTurn;

    if (UtilFunctions::LDRandomFloat() < dropoutChance) {
        lastValue = value;
        value = 0;
        return value;
    }

    for (auto syn : synapses) {
        synCheckStatus[syn->getID()] = 'u';
    }

    // TODO: add if statement to check injection, don't reset val if true
    value = 0;

	for (auto& syn : synapses) {
        synCheckStatus[syn->getID()] = 'v';
        value += syn->getData(curTurn);
        synCheckStatus[syn->getID()] = 'c';
	}

	lastValue = value;

    synCheckStatus.clear();

	return value;

}

float NotInputNode::getValueInLoop(unordered_map<unsigned int, char> statChecks) {
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
                    tempVal += syn->getData(turn);
                    synCheckStatus[syn->getID()] = 'c';
                }
            }

            lastValue = value + tempVal;
            return lastValue;
    }
}

float Node::getLastValue() const { return lastValue; }

float Input::getValue(unsigned long long int curTurn) { return value; }

float RandomInput::getValue(unsigned long long int curTurn) {
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

float Input::getValueInLoop(std::unordered_map<unsigned int, char> statChecks) { return 0; }
float RandomInput::getValueInLoop(std::unordered_map<unsigned int, char> statChecks) { return 0; }

void Node::setValue(float v) { value = v; }

void Node::setFlags(const std::vector<Flags::NodeFlag>& f) {

    for (NodeFlag flag : f) {
        addFlag(flag);
    }
}
void Node::addFlag(Flags::NodeFlag f) {
    switch (f) {
        case Flags::NodeFlag::NONE_FLAG:
        case NodeFlag::IGNORE_ON_CYCLE:
        case NodeFlag::PARTIAL_ON_CYCLE:
            cycleFlag = f;
            break;
        case NodeFlag::NO_DROPOUT:
        case Flags::NodeFlag::DROPOUT_20:
            dropoutFlag = f;
            dropoutChance = 0.2;
            break;
        case Flags::NodeFlag::DROPOUT_50:
            dropoutFlag = f;
            dropoutChance = 0.5;
            break;
        case Flags::NodeFlag::DROPOUT_80:
            dropoutFlag = f;
            dropoutChance = 0.8;
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
        case Flags::NodeFlag::DROPOUT_20:
        case Flags::NodeFlag::DROPOUT_50:
        case Flags::NodeFlag::DROPOUT_80:
            dropoutFlag = Flags::NodeFlag::NO_DROPOUT;
            dropoutChance = 0;
            break;
        default:
            break;
    }



    //flags.erase(remove(flags.begin(), flags.end(), f), flags.end());
}
//bool Node::hasFlag(Flags::NodeFlag f) { return find(flags.begin(), flags.end(), f) != flags.end(); }


void Node::addSynapse(Synapses::Synapse* syn) {
    syn->setOutput(this);
    synapses.push_back(syn);
}
void Node::addOutputSynapse(Synapses::Synapse *syn) {
    outputs.push_back(syn);
}
void Node::removeSynapse(Synapses::Synapse* syn) {
	//const Synapses::Synapse s = syn;

	synapses.erase(remove(synapses.begin(), synapses.end(), syn), synapses.end());
}

float RandomInput::randFloat() { return UtilFunctions::LDRandomFloat(); }
float RandomInput::randFloat(float min, float max) { return min + (randFloat() * (float)(max - min)); }

float RandomInput::randInt(int min, int max) { return UtilFunctions::LDRandomInt(min, max); }


float RandomInput::genDefault() { return randFloat(); }
float RandomInput::genNegDefault() { return -1 * genDefault(); }
float RandomInput::genExpanded() { return randFloat(-1, 1); }
float RandomInput::genUpperbound(float max) { return randFloat(0, max); }
float RandomInput::genBounded(float min, float max) { return randFloat(min, max); }
float RandomInput::genBoundedInts(int min, int max) { return randInt(min, max); }


RandomInput::RandomInput(unsigned int i, int m) : Input(i), mode(m), minimum(0), maximum(1) {}
RandomInput::RandomInput(unsigned int i, int m, float min, float max) : Input(i), mode(m) {
	if (min < max) {
		minimum = min;
		maximum = max;
	}
	else {
		minimum = max;
		maximum = min;
	}
}

void Output::getOutput(unsigned long long int curTurn) {
	value = getValue(curTurn);
	//if (value) cout << "Output " << getID() << ": " << value << endl;
}

string Node::getFlagListString() {
    string s;

    s += to_string((int)cycleFlag) + " " + to_string((int)dropoutFlag) + " ";

    return s;
}

void Node::totalSave(ofstream& saveFile) {

    saveFile << saveNode();

    /*if (totalSaving || lastTotalSave == DataBits::getTurn()) return;

    totalSaving = true;

    saveFile << saveNode();

    for (auto syn : synapses) {
        syn->totalSave(saveFile);
    }
    for (auto syn : outputs) {
        syn->totalSave(saveFile);
    }

    lastTotalSave = DataBits::getTurn();
    totalSaving = false;*/

}

void Node::graphSave(std::ofstream &graphFile, std::ofstream& graphFile2) {
    graphFile << "\"" + to_string(getID()) + "\"\n";
    graphFile2 << "\"" + to_string(getID()) + "\"\n";
    if (outputs.empty()) return;
    bool withOutput = false;
    string listString;
    string listString2 = "\"" + to_string(getID()) + "\" -> {";
    for (auto syn : outputs) {
        Node* o = syn->getOutput();
        if (o) {
            withOutput = true;
            graphFile << "\"" + to_string(getID()) + "\" -> \"" + to_string(o->getID()) + "\"\n";
            listString2 += " \"" + to_string(o->getID()) + "\"";

        }
    }
    if (withOutput) graphFile2 << listString2 + "}\n";
}

string NotInputNode::saveNode() {
    return "+n0 " + to_string(getID()) + " " + getFlagListString();
}

string Input::saveNode() {
    return "+n1 " + to_string(getID()) + " " + getFlagListString() + to_string(getValue(turn)) + " ";
}

string RandomInput::saveNode() {
    return "+n2 " + to_string(getID()) + " " + getFlagListString()
    + to_string(mode) + " " + to_string(minimum) + " " + to_string(maximum) + " ";
}

string Output::saveNode() {
    return "+n3 " + to_string(getID()) + " " + getFlagListString();
}

Fireable::Fireable(float t) : fireThreshold(t) {}

bool FireableNode::checkFire() {
    float sigmoidValue = 1 / (1 + exp(-value));

    return sigmoidValue >= fireThreshold;
}

float FireableNode::getValue(unsigned long long int curTurn) {
    NotInputNode::getValue(curTurn);

    if (!checkFire()) {
        value = 0;
        lastValue = 0;
    }

    return value;
}

void Fireable::setThreshold(float t) { fireThreshold = t; }

string  FireableNode::saveNode() {
    return "+n4 " + to_string(getID()) + " " + getFlagListString() + to_string(fireThreshold) + " ";
}

bool ActionNode::checkFire() {
    float sigmoidValue = 1 / (1 + exp(-value));

    return sigmoidValue >= fireThreshold;
}

ActionNode::ActionNode(unsigned int i, float t, int type) : Output(i), Fireable(t), actionType((Flags::ActionFlag)type) {}

Flags::ActionFlag ActionNode::getActionType() { return actionType; }

float ActionNode::getValue(unsigned long long int curTurn) {
    NotInputNode::getValue(curTurn);

    if (!checkFire()) {
        value = 0;
        lastValue = 0;
    }

    return value;
}

string ActionNode::saveNode() {
    return "+n5 " + to_string(getID()) + " " + getFlagListString()
            + to_string(fireThreshold) + " " + to_string((int)actionType) + " ";
}

void AddNodeNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    nodeType = nodeTypeInput ? (int)(abs(nodeTypeInput->getData(curTurn)) * DataBits::NUM_NODE_TYPES) % DataBits::NUM_NODE_TYPES : 0;
    nodeCycleFlag = cycleFlagInput ? (NodeFlag)((int)(cycleFlagInput->getData(curTurn) * 3) % 3) : NodeFlag::PARTIAL_ON_CYCLE;
    nodeValue = valueInput ? valueInput->getData(curTurn) : 0;
    mode = modeInput ? (int)(modeInput->getData(curTurn) * 5) % 5 : 0;
    min = minInput ? minInput->getData(curTurn) : 0;
    max = maxInput ? maxInput->getData(curTurn) : 0;
    thresholdValue = thresholdInput ? 1 / (1 + exp(-1 * thresholdInput->getData(curTurn))) : 0.5f;
    actionTypeValue = actionTypeInput ? (int)(abs(actionTypeInput->getData(curTurn)) * DataBits::NUM_ACTION_TYPES) % DataBits::NUM_ACTION_TYPES : 0;
}

//string AddNodeNode::saveNode() {
//    return "+n6 " + to_string(getID()) + " " + getFlagListString()
//            + to_string(fireThreshold) + " " + to_string((int)actionType);
//}

int AddNodeNode::getNodeType() const { return nodeType; }

ParamPackages::NodeParams AddNodeNode::getParams() {
    ParamPackages::NodeParams params;

    params.basicNodeParams.value = nodeValue;
    params.basicNodeParams.cycleFlag = nodeCycleFlag;
    params.randInputParams.mode = mode;
    params.randInputParams.min = min;
    params.randInputParams.max = max;
    params.fireableNodeParams.threshold = thresholdValue;
    params.actionNodeParams.actionType = actionTypeValue;

    return params;
}

void AddSynapseNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    synType = synTypeInput ? (int)(synTypeInput->getData(curTurn) * DataBits::NUM_SYN_TYPES) % DataBits::NUM_SYN_TYPES : 0;
    weight = weightInput ? weightInput->getData(curTurn) : 0;
}

int AddSynapseNode::getSynType() const { return synType; }

ParamPackages::SynapseParams AddSynapseNode::getParams() {
    ParamPackages::SynapseParams params;

    params.weightedSynapseParams.weight = 2 * UtilFunctions::sigmoid(weight) - 1;

    return params;
}

void AddNodeNode::addSynapse(Synapses::Synapse *syn) {
    if (!nodeTypeInput) {
        nodeTypeInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!cycleFlagInput) {
        cycleFlagInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!valueInput) {
        valueInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!modeInput) {
        modeInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!minInput) {
        minInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!maxInput) {
        maxInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!thresholdInput) {
        thresholdInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!thresholdInput) {
        thresholdInput = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

void AddSynapseNode::addSynapse(Synapses::Synapse *syn) {
    if (!synTypeInput) {
        synTypeInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!weightInput) {
        weightInput = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

void MakeConnectionNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    connectionType = connectionTypeInput ?
            (int)(connectionTypeInput->getData(curTurn) * DataBits::NUM_CONN_TYPES) % DataBits::NUM_CONN_TYPES : 0;
    id1 = id1Input ? id1Input->getData(curTurn) : 0;
    uu1 = uu1Input != nullptr && uu1Input->getData(curTurn) >= 0;
    id2 = id2Input ? id2Input->getData(curTurn) : 0;
    uu2 = uu2Input != nullptr && uu2Input->getData(curTurn) >= 0;
    id3 = (connectionType == 2 && id3Input) ? id3Input->getData(curTurn) : 0;
    uu3 = uu3Input != nullptr && uu3Input->getData(curTurn) >= 0;
}

void MakeConnectionNode::addSynapse(Synapses::Synapse *syn) {
    if (!connectionTypeInput) {
        connectionTypeInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!id1Input) {
        id1Input = syn;
        syn->setOutput(this);
        return;
    }
    if (!uu1Input){
        uu1Input = syn;
        syn->setOutput(this);
        return;
    }
    if (!id2Input) {
        id2Input = syn;
        syn->setOutput(this);
        return;
    }
    if (!uu2Input){
        uu2Input = syn;
        syn->setOutput(this);
        return;
    }
    if (!id3Input) {
        id3Input = syn;
        syn->setOutput(this);
        return;
    }
    if (!uu3Input){
        uu3Input = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

int MakeConnectionNode::getConnectionType() const { return connectionType; }
float MakeConnectionNode::getID1() const { return id1; }
float MakeConnectionNode::getID2() const { return id2; }
float MakeConnectionNode::getID3() const { return id3; }
bool MakeConnectionNode::getUU1() const { return uu1; }
bool MakeConnectionNode::getUU2() const { return uu2; }
bool MakeConnectionNode::getUU3() const { return uu3; }

void SetFlagNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    targetID = targetIDInput ? targetIDInput->getData(curTurn) : 0;
    flagVal = flagValInput ? (int)(flagValInput->getData(curTurn) * DataBits::NUM_NODE_FLAGS) % DataBits::NUM_NODE_FLAGS : 0;
}

void SetFlagNode::addSynapse(Synapses::Synapse *syn) {
    if (!targetIDInput) {
        targetIDInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!flagValInput) {
        flagValInput = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

float SetFlagNode::getTargetID() const { return targetID; }
int SetFlagNode::getFlagVal() const { return flagVal; }


bool Node::isUnused() {
    return !(synapses.empty() || outputs.empty());
}

void Node::removeOutputSynapse(Synapses::Synapse *syn) {
    outputs.erase(remove(outputs.begin(), outputs.end(), syn), outputs.end());
}

void UpdateWeightNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    targetID = targetIDInput ? abs(targetIDInput->getData(curTurn)) : 0;
    float unclamppedWeightModifier = weightModifierInput ? weightModifierInput->getData(curTurn) : 1;
    weightModifier = 2 * UtilFunctions::sigmoid(unclamppedWeightModifier) - 1;
    replaceWeight = replaceWeightInput != nullptr && (replaceWeightInput->getData(curTurn) <= 0);
}

void UpdateWeightNode::addSynapse(Synapses::Synapse *syn) {
    if (!targetIDInput) {
        targetIDInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!weightModifierInput) {
        weightModifierInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!replaceWeightInput) {
        replaceWeightInput = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

float UpdateWeightNode::getTargetID() const { return targetID; }
float UpdateWeightNode::getWeightModifier() const { return weightModifier; }
bool UpdateWeightNode::replacingWeight() const { return replaceWeight; }

void UpdateNodeValueNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    targetID = targetIDInput ? abs(targetIDInput->getData(curTurn)) : 0;
    float unclamppedValueModifier = valueModifierInput ? valueModifierInput->getData(curTurn) : 1;
    valueModifier = 2 * UtilFunctions::sigmoid(unclamppedValueModifier) - 1;
    replaceValue = replaceValueInput != nullptr && (replaceValueInput->getData(curTurn) <= 0);
}

void UpdateNodeValueNode::addSynapse(Synapses::Synapse *syn) {
    if (!targetIDInput) {
        targetIDInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!valueModifierInput) {
        valueModifierInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!replaceValueInput) {
        replaceValueInput = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

float UpdateNodeValueNode::getTargetID() const { return targetID; }
float UpdateNodeValueNode::getValueModifier() const { return valueModifier; }
bool UpdateNodeValueNode::replacingValue() const { return replaceValue; }


void TurtleNode::getOutput(unsigned long long int curTurn) {
    if (value == 0) return;

    instruction = instructionInput ? (int)(instructionInput->getData(curTurn) * 18) % 18 : -1;

    float pv = paramValueInput ? paramValueInput->getData(curTurn) : 0;
    switch (instruction) {
        case 0:
        case 1:
            paramValue =  1 + (int)(abs(pv) * 100) % 100;
            break;
        case 2:
        case 3:
            paramValue =  1 + (int)(abs(pv) * 360) % 360;
            break;
        case 4:
        case 5:
            paramValue =  -500 + (int)(abs(pv) * 1000) % 1000;
            break;
        case 6:
            paramValue =  (int)(abs(pv) * 361) % 361;
            break;
        case 8:
        case 9:
            paramValue =  1 + (int)(abs(pv) * 10) % 10;
            break;
        case 12:
            paramValue =  1 + (int)(abs(pv) * 100) % 100;
            break;
        case 13:
        case 14:
        case 15:
            paramValue =  1 + (int)(abs(pv) * 8) % 8;
            break;
        default:
            paramValue = 0;
    }
}

void TurtleNode::addSynapse(Synapses::Synapse *syn) {
    if (!instructionInput) {
        instructionInput = syn;
        syn->setOutput(this);
        return;
    }
    if (!paramValueInput) {
        paramValueInput = syn;
        syn->setOutput(this);
        return;
    }

    Node::addSynapse(syn);
}

int TurtleNode::getInstruction() const { return instruction; }
int TurtleNode::getParamValue() const { return paramValue; }

