#include <ecf/ECF.h>
#include <ecf/ECF_base.h>
#include <cmath>
#include <sstream>

#pragma once

namespace IntUniqueGenotype
{
	class IntUniqueGenotypeCrxOp : public CrossoverOp
	{
	public:
		bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
		
	};
	typedef boost::shared_ptr<IntUniqueGenotypeCrxOp> IntUniqueGenotypeCrxOpP;

}