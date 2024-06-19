#include <ECF\AlgSteadyStateTournament.h>
#include <ECF/ECF.h>

using namespace std;

class CustomAlgorithm : public SteadyStateTournament{
public:
	void Evaluate(vector<IndividualP> individuals, int ensembleCount);

};

typedef boost::shared_ptr<CustomAlgorithm> CustomAlgorithmP;

void CustomAlgorithm::Evaluate(vector<IndividualP> individuals, int ensembleCount){
		IndividualP ind = individuals.at(0);	
		state_->getContext()->evaluatedIndividual = ind;
		//ECF_LOG(state_, 5, "Evaluating individual: " + ind->toString());
		((SchedulingEvalOp*)evalOp_.get())->ensenbleCount = ensembleCount;
		ind->fitness = ((SchedulingEvalOp*)evalOp_.get())->evaluate(individuals);
		//ECF_LOG(state_, 5, "New fitness: " + ind->fitness->toString());
		state_->increaseEvaluations();
}