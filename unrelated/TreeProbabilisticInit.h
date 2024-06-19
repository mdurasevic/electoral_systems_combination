#pragma once
#include <ECF/ECF.h>

#include "ProbabilisticPrimitiveGenerator.h"

namespace Tree
{
	class ProbabilisticTree : public Tree
	{
	public:
		ProbabilisticTree();
		bool initialize(StateP state) override;
		void registerParameters(StateP) override;
	protected:
		static ProbabilisticPrimitiveGenerator generator;
		uint growBuild(PrimitiveSetP primitiveSet, int myDepth) override;
		uint fullBuild(PrimitiveSetP primitiveSet, int myDepth) override;
		pair<string, int> growBuild1(PrimitiveSetP primitiveSet, int myDepth, string context);
		pair<string, int> fullBuild1(PrimitiveSetP primitiveSet, int myDepth, string context);
		Tree* copy() override;
	};


}
typedef boost::shared_ptr<Tree::ProbabilisticTree> ProbabilisticTreeP;