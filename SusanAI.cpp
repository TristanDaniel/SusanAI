// SusanAI.cpp : Defines the entry point for the application.
//

#include "SusanAI.h"
#include "Trainer.h"

using namespace std;
using namespace Nodes;
using namespace Synapses;
using LemonDrop::Controller;

int main()
{
    //Controller c("test_controller", true, false, true);
    //Controller c("test_controller", (string)"best_g14_9065.430664_controller_g9_a15", false, true);
    //c.mainLoop(30);


    ControllerTrainer::train();

	cout << "Done" << endl;
	return 0;
}
