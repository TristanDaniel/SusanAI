#pragma once

namespace Flags {

    enum class NodeFlag {
        NONE_FLAG,
        IGNORE_ON_CYCLE,
        PARTIAL_ON_CYCLE
    };

    enum class ActionFlag {
        DO_NOTHING,
        ADD_NODE,
        ADD_SYNAPSE,
        NODE_TO_SYN,
        SYN_TO_NODE,
        NODE_TO_NODE,
        SYN_TO_SYN,
        SET_FLAG_FOR_NODE
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

            Flags::NodeFlag cycleFlag = Flags::NodeFlag::NONE_FLAG;
        } randInputParams;

        NodeParams() {

        }
    };

    struct SynapseParams {
        struct PassthroughSynapseParams {

        } passthroughSynapseParams;

        struct WeightedSynapseParams {
            float weight = 0;
        } weightedSynapseParams;

        SynapseParams() {

        }
    };
}

namespace DataBits {
    static const int NUM_NODE_TYPES = 12;
}