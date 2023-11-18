#pragma once

#include <vector>
#include <unordered_map>

#include "Structures.h"
#include "Utils.h"

//#include "Synapses.h"

namespace Synapses {
	class Synapse;
}


namespace Nodes {

	class Node : public Structures::Part {

	protected:
		float value{ 0 }, lastValue{ 0 };
		std::vector<Synapses::Synapse*> synapses{};

        std::unordered_map<int, char> synCheckStatus;

        Flags::NodeFlag cycleFlag = Flags::NodeFlag::PARTIAL_ON_CYCLE;
        //std::vector<Flags::NodeFlag> flags;

	public:
		
		Node()= default;
		Node(unsigned int i);

		virtual float getValue() = 0;
		void setValue(float v);

        void setFlags(std::vector<Flags::NodeFlag> f);
        void addFlag(Flags::NodeFlag f);
        void removeFlag(Flags::NodeFlag f);
        //bool hasFlag(Flags::NodeFlag f);

        virtual float getValueInLoop(std::unordered_map<int, char> statChecks) = 0;

        float getLastValue() const;

		void addSynapse(Synapses::Synapse* syn);
		void removeSynapse(Synapses::Synapse* syn);
	};

	class NotInputNode : public Node {
	public:
		float getValue() override;
        float getValueInLoop(std::unordered_map<int, char> statChecks) override;
	};

	class Input : public Node {

	public:
        Input(unsigned int i);

		float getValue() override;
	};

	class RandomInput : public Input {
	private:
		int mode{ 0 };
		float minimum{ 0 }, maximum{ 1 };

		//Returns float between 0 and 1
		static float randFloat();

		//Returns float between min and max
		static float randFloat(float min, float max);

		//Returns int between 0 and max int value
		static int randInt();

		//Returns int between min and max
		static int randInt(int min, int max);



		static float genDefault();

		static float genNegDefault();

		static float genExpanded();

        static float genUpperbound(float max);

        static float genBounded(float min, float max);

        static float genBoundedInts(int min, int max);

	public:

		RandomInput(unsigned int i, int m);
		RandomInput(unsigned int i, int m, float min, float max);

		float getValue() override;
        float getValueInLoop(std::unordered_map<int, char> statChecks) override;

	};

	class Output : public NotInputNode {

	public:
		virtual void getOutput();
	};
}