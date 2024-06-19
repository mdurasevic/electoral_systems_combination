#include <ecf/ECF.h>
#include <ecf/ECF_base.h>
#include <cmath>
#include <sstream>
#include "IntUniqueGenotype.h"

namespace IntUniqueGenotype
{
	
		bool IntUniqueGenotypeMutOp::mutate(GenotypeP gene)
		{
			IntUniqueGenotype* gen = (IntUniqueGenotype*) (gene.get());

			std::set<int> inserted;
			for (uint i = 0; i < gen->intValues.size(); i++) {
				inserted.insert(gen->intValues[i]);
			}

			
			int a = state_->getRandomizer()->getRandomInteger(gen->intValues.size());

			int value = state_->getRandomizer()->getRandomInteger(gen->getLBound(), gen->getUBound());
			while(inserted.find(value)!=inserted.end()){
				value = state_->getRandomizer()->getRandomInteger(gen->getLBound(), gen->getUBound());
			}

			gen->intValues[a] = value;
			

			return true;
		}
	
}
