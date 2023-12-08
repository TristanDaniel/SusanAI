#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "Structures.h"
#include "Utils.h"

//#include "Synapses.h"

namespace Synapses {
	class Synapse;
    class PassthroughSynapse;
    class WeightedSynapse;
}


namespace Nodes {

	class Node : public Structures::Part {

	protected:
        unsigned long long int turn{};
		float value{}, lastValue{};
		std::vector<Synapses::Synapse*> synapses{};

        std::unordered_map<unsigned int, char> synCheckStatus;

        Flags::NodeFlag cycleFlag = Flags::NodeFlag::NONE_FLAG;
        //std::vector<Flags::NodeFlag> flags;

        std::string getFlagListString();

	public:
		
		Node()= default;
		explicit Node(unsigned int i);

		virtual float getValue() = 0;
		void setValue(float v);

        void setFlags(const std::vector<Flags::NodeFlag>& f);
        void addFlag(Flags::NodeFlag f);
        void removeFlag(Flags::NodeFlag f);
        //bool hasFlag(Flags::NodeFlag f);

        virtual float getValueInLoop(std::unordered_map<unsigned int, char> statChecks) = 0;

        float getLastValue() const;

		virtual void addSynapse(Synapses::Synapse* syn);
		void removeSynapse(Synapses::Synapse* syn);

        virtual std::string saveNode() = 0;
	};

	class NotInputNode : public Node {
	public:
        explicit NotInputNode(unsigned int i) : Node(i) {};

		float getValue() override;
        float getValueInLoop(std::unordered_map<unsigned int, char> statChecks) override;

        std::string saveNode() override;
	};

	class Input : public Node {

	public:
        explicit Input(unsigned int i);

		float getValue() override;
        float getValueInLoop(std::unordered_map<unsigned int, char> statChecks) override;

        std::string saveNode() override;
	};

	class RandomInput : public Input {
	private:
		int mode{ 0 };
		float minimum{ 0 }, maximum{ 1 };

		//Returns float between 0 and 1
		static float randFloat();

		//Returns float between min and max
		static float randFloat(float min, float max);

		//Returns int between min and max
		static float randInt(int min, int max);



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
        float getValueInLoop(std::unordered_map<unsigned int, char> statChecks) override;

        std::string saveNode() override;
	};

	class Output : public NotInputNode {

	public:

        explicit Output(unsigned int i) : NotInputNode(i) {}

		virtual void getOutput();

        std::string saveNode() override;
	};

    class Fireable {

    protected:
        float fireThreshold = 0.5;


        virtual bool checkFire() = 0;

    public:
        explicit Fireable(float t);

        void setThreshold(float t);
    };

    class FireableNode : public Nodes::NotInputNode, public Nodes::Fireable {

    protected:
        bool checkFire() override;

    public:
        FireableNode(unsigned int i, float t) : Nodes::NotInputNode(i), Nodes::Fireable(t) {}

        float getValue() override;

        std::string  saveNode() override;

    };

    class ActionNode : public Nodes::Output, public Nodes::Fireable {

    protected:
        Flags::ActionFlag actionType = Flags::ActionFlag::DO_NOTHING;

        bool checkFire() override;

    public:

        ActionNode(unsigned int i, float t, int type);

        Flags::ActionFlag getActionType();

        float getValue() override;

        std::string  saveNode() override;

    };

    class AddNodeNode : public ActionNode {

    protected:
        int nodeType = 0, mode = 0, actionTypeValue = 0;
        Flags::NodeFlag nodeCycleFlag = Flags::NodeFlag::PARTIAL_ON_CYCLE;
        float nodeValue = 0, min = 0, max = 0, thresholdValue = 0;

        Synapses::Synapse* nodeTypeInput = nullptr,
                         * cycleFlagInput = nullptr,
                         * valueInput = nullptr,
                         * modeInput = nullptr,
                         * minInput = nullptr,
                         * maxInput = nullptr,
                         * thresholdInput = nullptr,
                         * actionTypeInput = nullptr;

    public:

        AddNodeNode(unsigned int i, float t) : ActionNode(i, t, 1) {}

        void getOutput() override;

        void addSynapse(Synapses::Synapse* syn) override;

        //std::string  saveNode() override;

        int getNodeType() const;
        ParamPackages::NodeParams getParams();

    };

    class AddSynapseNode : public ActionNode {

    protected:
        int synType = 0;
        float weight = 0;

        Synapses::Synapse* synTypeInput = nullptr,
                         * weightInput = nullptr;

    public:
        AddSynapseNode(unsigned int i, float t) : ActionNode(i, t, 2) {}

        void getOutput() override;

        void addSynapse(Synapses::Synapse* syn) override;

        int getSynType() const;
        ParamPackages::SynapseParams getParams();
    };

    class MakeConnectionNode : public ActionNode {

    protected:
        int connectionType = 0;
        float id1 = 0, id2 = 0, id3 = 0; // id3 only used for nn con (type 2)

        Synapses::Synapse* connectionTypeInput = nullptr,
                         * id1Input = nullptr,
                         * id2Input = nullptr,
                         * id3Input = nullptr;

    public:
        MakeConnectionNode(unsigned int i, float t) : ActionNode(i, t, 3) {}

        void getOutput() override;

        void addSynapse(Synapses::Synapse* syn) override;

        int getConnectionType() const;
        float getID1() const;
        float getID2() const;
        float getID3() const;
    };

    class SetFlagNode : public ActionNode {

    protected:
        int targetType = 0; // useless for now, once synapses exist this will be useful
        float targetID = 0;
        int flagVal = 0;

        Synapses::Synapse* targetIDInput = nullptr,
                         * flagValInput = nullptr;

    public:
        SetFlagNode(unsigned int i, float t) : ActionNode(i, t, 4) {}

        void getOutput() override;

        void addSynapse(Synapses::Synapse* syn) override;

        float getTargetID() const;
        int getFlagVal() const;
    };


}

