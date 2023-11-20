
#include "LemonDrop.h"
#include "Utils.h"

using LemonDrop::Controller;

bool Controller::newNode(unsigned int type, ParamPackages::NodeParams params) {

    unsigned int id;
    Nodes::Node* n;

    switch (type) {
        case 0:
            //basic node
            id = nodes.getNextID();

            n = new Nodes::NotInputNode(id);

            nodes.addNode(n);

            return true;

        case 1:
        {
            //Random input
            id = nodes.getNextID();

            int mode = params.randInputParams.mode;
            float min = params.randInputParams.min;
            float max = params.randInputParams.max;

            n = new Nodes::RandomInput(id, mode, min, max);

            nodes.addNode(n);

            return true;
        }

        default:
            return false;
    }
}

bool Controller::newOutput(unsigned int type, ParamPackages::NodeParams params) {
    unsigned int id;
    Nodes::Output* n;

    switch (type) {
        case 0:
            //basic output
            id = outputs.getNextID();

            n = new Nodes::Output(id);

            outputs.addNode(n);

            return true;

        default:
            return false;
    }
}