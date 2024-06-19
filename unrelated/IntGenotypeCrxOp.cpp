#include <ecf/ECF.h>
#include <ecf/ECF_base.h>
#include <cmath>
#include <sstream>
#include "IntUniqueGenotype.h"
#include <algorithm>
#include <unordered_set>

namespace IntUniqueGenotype
{
	
		bool IntUniqueGenotypeCrxOp::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {

			IntUniqueGenotype* p1 = (IntUniqueGenotype*)(gen1.get());
			IntUniqueGenotype* p2 = (IntUniqueGenotype*)(gen2.get());
			IntUniqueGenotype* ch = (IntUniqueGenotype*)(child.get());

			std::vector<int> values; 

			for (uint i = 0; i < p1->intValues.size(); i++) {
				values.push_back(p1->intValues[i]);
				values.push_back(p2->intValues[i]);
			}

			std::random_shuffle(values.begin(), values.end());

			std::set<int> inserted;

			int childIndex = 0;

			for(int i = 0; i<values.size();i++){
				if(inserted.find(values[i])!=inserted.end()){
					inserted.insert(values[i]);
					ch->intValues[childIndex] = values[i];
					childIndex++;
				}
				if(childIndex==p1->intValues.size()){
					break;
				}
			}

			return true;
		}
	
}