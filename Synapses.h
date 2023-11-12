#pragma once

#include "Nodes.h"

namespace Synapses {


    class Synapse {

	protected:
		int id{ 0 };

		Nodes::Node* input{};

	public:

		Synapse() = default;
		explicit Synapse(int i) : id(i) {}
		Synapse(int i, Nodes::Node* inNode) : id(i), input(inNode) {}

        virtual float getData() = 0;
		[[nodiscard]] int getID() const;

		void setInput(Nodes::Node* n);

		bool operator==(const Synapse& s) const;
	};

    class PassthroughSynapse : public Synapse {

    public:

        PassthroughSynapse() = default;
        explicit PassthroughSynapse(int i);
        PassthroughSynapse(int i, Nodes::Node* inNode);

        float getData() override;
    };

	class WeightedSynapse : public Synapse {

	protected:
		float weight{ 0 };

	public:

		WeightedSynapse() = default;
		explicit WeightedSynapse(int i);
		WeightedSynapse(int i, Nodes::Node* inNode, float w);

		float getData() override;
	};
	
}