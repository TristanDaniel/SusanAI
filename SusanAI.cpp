// SusanAI.cpp : Defines the entry point for the application.
//

#include "SusanAI.h"

using namespace std;
using namespace Nodes;
using namespace Synapses;

int main()
{
	Output o = Output();
	Synapse* syn = new WeightedSynapse(0, new RandomInput(4, 5, 10), 0.5);

	o.addSynapse(syn);

	//RandomInput rin = RandomInput(0);

	for (int i = 0; i < 10; i++) {
		o.getOutput();
	}


	cout << "Done" << endl;
	return 0;
}
