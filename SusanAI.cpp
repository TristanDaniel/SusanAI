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
    //Controller c("test_controller", (string)"best_final_3208.315186_controller_g99_a26", false, true);
    //c.mainLoop(100);


    ControllerTrainer::train();

	cout << "Done" << endl;
	return 0;
}
