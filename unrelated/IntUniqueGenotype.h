#include <ecf/ECF.h>
#include <ecf/ECF_base.h>
#include <cmath>
#include <sstream>

#include "IntUniqueGenotypeCrxOp.h"
#include "IntUniqueGenotypeMutOp.h"

#pragma once

namespace IntUniqueGenotype
{
	class IntUniqueGenotype : public Genotype
	{
	public:
		int minValue_, maxValue_;
		uint nSize_;
		std::vector<int> intValues;
		std::vector<int> possibleValues;

		IntUniqueGenotype()
		{
			name_ = "IntUniqueGenotype";
		}

		IntUniqueGenotype* copy()
		{
			IntUniqueGenotype *newObject = new IntUniqueGenotype(*this);
			return newObject;
		}

		/// return lower bound of the defined interval
		double getLBound()
		{
			return minValue_;
		}

		/// return upper bound of the defined interval
		double getUBound()
		{
			return maxValue_;
		}
		std::vector<CrossoverOpP> getCrossoverOp()
		{
			std::vector<CrossoverOpP> crx;
			crx.push_back(static_cast<CrossoverOpP> (new IntUniqueGenotypeCrxOp));
			return crx;
		}

		std::vector<MutationOpP> getMutationOp()
		{
			std::vector<MutationOpP> mut;
			mut.push_back(static_cast<MutationOpP> (new IntUniqueGenotypeMutOp));
			return mut;
		}

		void registerParameters(StateP state);

		bool initialize(StateP state);

		void write(XMLNode &xIntUniqueGenotype);

		// mandatory if running parallel ECF or reading population from a milestone file
		void read(XMLNode& xIntUniqueGenotype);
	};
}

typedef boost::shared_ptr<IntUniqueGenotype::IntUniqueGenotype> IntUniqueGenotypeP;
