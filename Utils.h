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
            // No params needed for basic node yet
        } basicNodeParams;

        struct RandInputParams {
            int mode = 0;
            float min = 0, max = 0;
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