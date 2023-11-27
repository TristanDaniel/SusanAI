#pragma once

namespace Flags {

    enum class NodeFlag {
        NONE_FLAG,
        IGNORE_ON_CYCLE,
        PARTIAL_ON_CYCLE
    };

}

namespace ParamPackages {

    union NodeParams {
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

    union SynapseParams {
        struct PassthroughSynapseParams {

        } passthroughSynapseParams;

        struct WeightedSynapseParams {
            float weight = 0;
        } weightedSynapseParams;

        SynapseParams() {

        }
    };
}