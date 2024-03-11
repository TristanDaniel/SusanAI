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
    Controller c("gate_test", true, false, false);
    //Controller c("byte", (string)"byte_post_training", false, true);
    //c.addTurtleInterface();
    c.mainLoop(5000);
    c.totalSave();


    //ControllerTrainer::train();

	cout << "Done" << endl;
	return 0;
}
