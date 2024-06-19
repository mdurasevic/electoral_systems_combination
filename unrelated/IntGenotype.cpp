#include <ecf/ECF.h>
#include <ecf/ECF_base.h>
#include <cmath>
#include <sstream>
#include "IntUniqueGenotype.h"
#include <algorithm>
#include <time.h>
#include <set>

namespace IntUniqueGenotype
{
	int myrandom (int i) { return rand()%i;}
	void IntUniqueGenotype::registerParameters(StateP state)
		{
			registerParameter(state, "lbound", (voidP) new int(-10), ECF::INT);
			registerParameter(state, "ubound", (voidP) new int(10), ECF::INT);
			registerParameter(state, "size", (voidP)(new uint(1)), ECF::UINT);
		}

	bool IntUniqueGenotype::initialize(StateP state)
		{
			if (!isParameterDefined(state, "lbound") ||
				!isParameterDefined(state, "ubound") ||
				!isParameterDefined(state, "size")) {
				ECF_LOG_ERROR(state, "Error: required parameters for IntUniqueGenotype genotype not defined (lbound, ubound, size)!");
				throw("");
		}

			voidP genp = getParameterValue(state, "lbound");
			minValue_ = *((int*)genp.get());

			genp = getParameterValue(state, "ubound");
			maxValue_ = *((int*)genp.get());

			if (minValue_ >= maxValue_) {
				ECF_LOG_ERROR(state, "Error: 'lbound' must be smaller than 'ubound' for IntUniqueGenotype genotype!");
				throw("");
			}

			genp = getParameterValue(state, "size");
			nSize_ = *((uint*)genp.get());

			if (nSize_ < 1) {
				ECF_LOG_ERROR(state, "Error: 'size' must be > 0 for IntUniqueGenotype genotype!");
				throw("");
			}

			intValues.resize(nSize_);
			// randomly create each dimension
			
			for(int i=minValue_; i<=maxValue_; i++){
				possibleValues.push_back(i);
			}
			
			std::set<int> selected;

			while(selected.size()<nSize_){
				selected.insert(state->getRandomizer()->getRandomInteger(minValue_, maxValue_));
			}

			std::set<int>::iterator it = selected.begin();
			int index = 0;
			while (it != selected.end())
			{
				intValues[index] = *(it);
				index++;
				it++;
			}

			return true;
	}

	void IntUniqueGenotype::write(XMLNode &xIntUniqueGenotype)
	{
		xIntUniqueGenotype = XMLNode::createXMLTopNode("IntUniqueGenotype");
		std::stringstream sValue;
		sValue << intValues.size();
		xIntUniqueGenotype.addAttribute("size", sValue.str().c_str());

		sValue.str("");
		for (uint iVar = 0; iVar < intValues.size(); iVar++)
			sValue << "\t" << intValues[iVar];
		xIntUniqueGenotype.addText(sValue.str().c_str());
	}

		// mandatory if running parallel ECF or reading population from a milestone file
	void IntUniqueGenotype::read(XMLNode& xIntUniqueGenotype)
	{
		XMLCSTR values = xIntUniqueGenotype.getText();
		std::stringstream sValues;
		sValues << values;

		for (uint iVar = 0; iVar < intValues.size(); iVar++)
			sValues >> intValues[iVar];
	}
	
}