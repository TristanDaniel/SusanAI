
#include "Utils.h"
#include "Nodes.h"

unsigned long long int turn;

void DataBits::initTurn() { turn = 0; }
void DataBits::incrTurn() { turn++; }
unsigned long long int DataBits::getTurn() { return turn; }

std::random_device rd;
std::mt19937 mt(rd());

float UtilFunctions::LDRandomFloat() {
    std::uniform_real_distribution<float> dist(0, 1);

    return dist(mt);
}

float UtilFunctions::LDRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);

    return (float)dist(mt);

}

float UtilFunctions::sigmoid(float val) {
    return 1 / (1 + std::exp(-val));
}

void UtilClasses::ActionGroup::addNode(Nodes::ActionNode* node) {
    nodes.push_back(node);
}

Nodes::ActionNode* UtilClasses::ActionGroup::getActionNode() {
    float highestValue = 0;

    Nodes::ActionNode* firingNode = nullptr;

    for (auto n : nodes) {
        float v = n->getValue();
        if (v > highestValue) {
            highestValue = v;
            firingNode = n;
        }
    }

    return firingNode;
}

