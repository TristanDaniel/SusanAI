#include "Synapses.h"

using namespace std;
using Synapses::Synapse;
using Synapses::PassthroughSynapse;
using Synapses::WeightedSynapse;

PassthroughSynapse::PassthroughSynapse(unsigned int i) { id = i; }
PassthroughSynapse::PassthroughSynapse(unsigned int i, Nodes::Node *inNode) { id = i; input = inNode; }

WeightedSynapse::WeightedSynapse(unsigned int i) { id = i; }
WeightedSynapse::WeightedSynapse(unsigned int i, float w) : weight(w)  { id = i; }
WeightedSynapse::WeightedSynapse(unsigned int i, Nodes::Node* inNode, float w) : weight(w) { id = i; input = inNode; }


float PassthroughSynapse::getData() {
    return input ? input->getValue() : 0;
}

float WeightedSynapse::getData() {
	return input ? input->getValue() * weight : 0;
}


unsigned int Synapse::getID() const { return id; }

void Synapse::setInput(Nodes::Node* n) {
    if (input) input->removeOutputSynapse(this);
    input = n;
}

void Synapse::setOutput(Nodes::Node *n) {
    if (output) output->removeSynapse(this);
    output = n;
}

bool Synapse::operator==(const Synapse& s) const { return id == s.id; }

string PassthroughSynapse::saveSynapse() {
    return "+s0 " + to_string(getID()) + " ";
}

string WeightedSynapse::saveSynapse() {
    return "+s1 " + to_string(getID()) + " " + to_string(weight) + " ";
}

bool Synapse::isUnused() {
    return !(input && (output && !dynamic_cast<Nodes::Input*>(output)));
}

void WeightedSynapse::setWeight(float w) { weight = w; }
float WeightedSynapse::getWeight() const { return weight; }

Nodes::Node* Synapse::getOutput() const { return output; }
Nodes::Node* Synapse::getInput() const { return input; }

void Synapse::totalSave(std::ofstream& saveFile) {
    if (totalSaving || lastTotalSave == DataBits::getTurn()) return;

    totalSaving = true;

    saveFile << saveSynapse();

    if (input != nullptr) saveFile << ">ns " + to_string(input->getID()) + " " + to_string(id) + " ";
    if (output != nullptr) saveFile << ">sn" + to_string(id) + " " + to_string(output->getID()) + " ";

    lastTotalSave = DataBits::getTurn();
    totalSaving = false;
}


