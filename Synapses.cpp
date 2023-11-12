#include "Synapses.h"

using namespace std;
using Synapses::Synapse;
using Synapses::PassthroughSynapse;
using Synapses::WeightedSynapse;

PassthroughSynapse::PassthroughSynapse(int i) { id = i; }
PassthroughSynapse::PassthroughSynapse(int i, Nodes::Node *inNode) { id = i; input = inNode; }

WeightedSynapse::WeightedSynapse(int i) { id = i; }
WeightedSynapse::WeightedSynapse(int i, Nodes::Node* inNode, float w) : weight(w) { id = i; input = inNode; }


float PassthroughSynapse::getData() {
    return input->getValue();
}

float WeightedSynapse::getData() {
	return input->getValue() * weight;
}


int Synapse::getID() const { return id; }

void Synapse::setInput(Nodes::Node* n) { input = n; }

bool Synapse::operator==(const Synapse& s) const { return id == s.id; }



