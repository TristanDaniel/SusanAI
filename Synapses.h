#pragma once

#include <string>
#include <fstream>

#include "Nodes.h"

namespace Synapses {


    class Synapse {

	protected:
		unsigned int id{ 0 };

		Nodes::Node* input{}, * output{};

        bool totalSaving = false;
        unsigned long long int lastTotalSave;

	public:

		Synapse() = default;
		explicit Synapse(unsigned int i) : id(i), input(nullptr), output(nullptr) {}
		Synapse(unsigned int i, Nodes::Node* inNode) : id(i), input(inNode), output(nullptr) {}

        virtual float getData(unsigned long long int curTurn) = 0;
		[[nodiscard]] unsigned int getID() const;

		void setInput(Nodes::Node* n);
        [[nodiscard]] Nodes::Node* getInput() const;
        void setOutput(Nodes::Node* n);
        [[nodiscard]] Nodes::Node* getOutput() const;

        bool isUnused();

        virtual std::string saveSynapse() = 0;
        void totalSave(std::ofstream& saveFile, std::ofstream& graphFile);

		bool operator==(const Synapse& s) const;
	};

    class PassthroughSynapse : public Synapse {

    public:

        PassthroughSynapse() = default;
        explicit PassthroughSynapse(unsigned int i);
        PassthroughSynapse(unsigned int i, Nodes::Node* inNode);

        float getData(unsigned long long int curTurn) override;

        std::string  saveSynapse() override;
    };

	class WeightedSynapse : public Synapse {

	protected:
		float weight{ 0 };

	public:

		WeightedSynapse() = default;
		explicit WeightedSynapse(unsigned int i);
        WeightedSynapse(unsigned int i, float w);
		WeightedSynapse(unsigned int i, Nodes::Node* inNode, float w);

        void setWeight(float w);
        float getWeight() const;

		float getData(unsigned long long int curTurn) override;

        std::string  saveSynapse() override;
	};
	
}