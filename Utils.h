#pragma once

#include <random>
#include <queue>
#include <vector>
//#include "Nodes.h"

namespace Flags {

    enum class NodeFlag {
        NONE_FLAG,
        IGNORE_ON_CYCLE,
        PARTIAL_ON_CYCLE,
        NO_DROPOUT,
        DROPOUT_20,
        DROPOUT_50,
        DROPOUT_80
    };

    enum class ActionFlag {
        DO_NOTHING,
        ADD_NODE,
        ADD_SYNAPSE,
        MAKE_CONNECTION,
        SET_FLAG_FOR_NODE,
        UPDATE_WEIGHT,
        UPDATE_NODE_VALUE,
        TURTLE
    };

}

namespace ParamPackages {

    struct NodeParams {
        struct BasicNodeParams {
            // used for all basic node types (NotInputNode, Input, Output)
            float value = 0;

            Flags::NodeFlag cycleFlag = Flags::NodeFlag::NONE_FLAG;
        } basicNodeParams;

        struct RandInputParams {
            int mode = 0;
            float min = 0, max = 0;

            Flags::NodeFlag cycleFlag = Flags::NodeFlag::NONE_FLAG; // remove at some point
        } randInputParams;

        struct FireableNodeParams {
            float threshold = 0.5;
        } fireableNodeParams;

        struct ActionNodeParams {
            int actionType = 0;
        } actionNodeParams;

        NodeParams() = default;
    };

    struct SynapseParams {
        struct PassthroughSynapseParams {

        } passthroughSynapseParams;

        struct WeightedSynapseParams {
            float weight = 0;
        } weightedSynapseParams;

        SynapseParams() = default;
    };
}

namespace DataBits {
    static const int NUM_NODE_TYPES = 6;
    static const int NUM_ACTION_TYPES = 7;
    static const int NUM_SYN_TYPES = 2;
    static const int NUM_CONN_TYPES = 4;

    static const int NUM_FLAG_TARGET_TYPES = 0;
    static const int NUM_NODE_FLAGS = 7;
}

namespace UtilFunctions {

    float LDRandomFloat();

    float LDRandomInt(int min, int max);

    float sigmoid(float val);
}

namespace Nodes {
    class ActionNode;
}

namespace UtilClasses {

    template <typename T> class RunningAverage {
    protected:
        std::queue<T> que;
        T total{0};
        int buffSize{0};

    public:
        RunningAverage() = default;
        explicit RunningAverage(int bs) : total(0), buffSize(bs) {}

        void addValue(T val) {
            total += val;
            que.push(val);

            if (que.size() > buffSize) {
                total -= que.front();
                que.pop();
            }
        }
        float getAverage() {
            return ((float)total / (que.size() ? que.size() : 1));
        }
    };

    class ActionGroup {

    protected:
        std::vector<Nodes::ActionNode*> nodes;

    public:

        void addNode(Nodes::ActionNode* node);

        Nodes::ActionNode* getActionNode(unsigned long long int curTurn);

        std::vector<Nodes::ActionNode*> getActionNodes();
    };

    class TurnTracker {

    private:
        unsigned long long int turn;

    public:
        void initTurn();
        void incrTurn();
        unsigned long long int getTurn();

    };
}