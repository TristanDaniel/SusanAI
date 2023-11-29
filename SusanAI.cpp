// SusanAI.cpp : Defines the entry point for the application.
//

#include "SusanAI.h"

using namespace std;
using namespace Nodes;
using namespace Synapses;
using LemonDrop::Controller;

int main()
{
    Controller ld = Controller();

//	ParamPackages::NodeParams outP1;
//    ld.newOutput(0, outP1);
//
//    ParamPackages::SynapseParams synP1;
//    synP1.weightedSynapseParams.weight = 0.5;
//    ld.newSynapse(1, synP1);
//
//    ParamPackages::NodeParams rinP1;
//    rinP1.randInputParams.mode = 4;
//    rinP1.randInputParams.min = 5;
//    rinP1.randInputParams.max = 10;
//    ld.newNode(1, rinP1);
//
//    ld.addNodeToSynapse(1, 0);
//    ld.addSynapseToNode(0, 0);
//
//    ld.mainLoop();


    //Output o = Output();
    //Synapse* syn = new WeightedSynapse(0, new RandomInput(0, 4, 5, 10), 0.5);
	//o.addSynapse(syn);

	//RandomInput rin = RandomInput(0);

	//for (int i = 0; i < 10; i++) {
		//o.getOutput();
	//}


	cout << "Done" << endl;
	return 0;
}
