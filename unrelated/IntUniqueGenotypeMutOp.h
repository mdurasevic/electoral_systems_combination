#include <ecf/ECF.h>
#include <ecf/ECF_base.h>
#include <cmath>
#include <sstream>

#pragma once

namespace IntUniqueGenotype
{
	class IntUniqueGenotypeMutOp : public MutationOp
	{
	public:
		bool mutate(GenotypeP gene);
	};
	typedef boost::shared_ptr<IntUniqueGenotypeMutOp> IntUniqueGenotypeMutOpP;

}