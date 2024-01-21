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
    //Controller c("base_controller", true, false, false);
    Controller c("spike", (string)"spike_post_training", false, true);
    c.addTurtleInterface();
    c.mainLoop(750);
    c.totalSave();


    //ControllerTrainer::train();

	cout << "Done" << endl;
	return 0;
}
