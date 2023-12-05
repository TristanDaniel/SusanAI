
#include "Utils.h"

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

    return dist(mt);

}

