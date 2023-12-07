#pragma once

#include <string>

#include "Nodes.h"

namespace Synapses {


    class Synapse {

	protected:
		unsigned int id{ 0 };

		Nodes::Node* input{}, * output{};

	public:

		Synapse() = default;
		explicit Synapse(unsigned int i) : id(i), input(nullptr), output(nullptr) {}
		Synapse(unsigned int i, Nodes::Node* inNode) : id(i), input(inNode), output(nullptr) {}

        virtual float getData() = 0;
		[[nodiscard]] unsigned int getID() const;

		void setInput(Nodes::Node* n);
        void setOutput(Nodes::Node* n);

        virtual std::string saveSynapse() = 0;

		bool operator==(const Synapse& s) const;
	};

    class PassthroughSynapse : public Synapse {

    public:

        PassthroughSynapse() = default;
        explicit PassthroughSynapse(unsigned int i);
        PassthroughSynapse(unsigned int i, Nodes::Node* inNode);

        float getData() override;

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

		float getData() override;

        std::string  saveSynapse() override;
	};
	
}