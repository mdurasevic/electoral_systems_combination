#include <ecf/ECF.h>
#include "fitnes.hpp"
#include <ecf/AlgGenHookeJeeves.h>
#include "IFGT.h"
#include "IFLT.h"
#include "SQR.h"
#include "AVG.h"
#include "ABS.h"
#include "CustomAlgorithm.h"
#include <time.h>
#include "LateOperator.h"
#include "IntUniqueGenotype.h"
#include <ctime>
#include <regex>

#include "AntiPlurality.h"
#include "ApprovalVoting.h"
#include "BaldwinMethod.h"
#include "BordaCount.h"
#include "BucklinVoting.h"
#include "CombinedApprovalVoting.h"
#include "CondorcetMethod.h"
#include "ContigentVoting.h"
#include "CoombsMethod.h"
#include "CopelandMethod.h"
#include "DistributedVoting.h"
#include "ElectionMethodTests.h"
#include "ElectoralSystemUtils.h"
#include "InstantRunoff.h"
#include "JudgementMethod.h"
#include "KemenyYoungMethod.h"
#include "LowestPriorityStrategy.h"
#include "MajorityVote.h"
#include "MiniMax.h"
#include "NansonMethod.h"
#include "OklahomaMethod.h"
#include "PluralityVote.h"
#include "PreferentialBlockVoting.h"
#include "RankedPairs.h"
#include "SchulzeMethod.h"
#include "ScoreVoting.h"
#include "StarVoting.h"
#include "SumMethod.h"
#include "TidemanAlternativeSmith.h"
#include "TreeProbabilisticInit.h"
#include "UnifiedPrimary.h"


class WinnerStrategy;
using namespace std;

//
// pomocni operator za ispitivanje trenutnih rjesenja iz HoF-a na skupu za evaluaciju
//
class Zapis : public Operator
{
private:
	StateP state_;
	std::string logfilename;
	SchedulingEvalOpP testEvalOp;
public:
	void registerParameters(StateP state)
	{
		state->getRegistry()->registerEntry("test_cases2", (voidP) (new std::string("fitness2.txt")), ECF::STRING);
	}

	bool initialize(StateP state)
	{
		state_ = state;
		voidP sptr = state->getRegistry()->getEntry("log.filename");
		logfilename = *((std::string*) sptr.get());

		// setup test cases evaluator
		testEvalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
		sptr = state->getRegistry()->getEntry("test_cases");
		voidP sptr2 = state->getRegistry()->getEntry("test_cases2");
		//state->getRegistry()->modifyEntry("test_cases", (voidP) (new std::string("fitness2.txt")));
		state->getRegistry()->modifyEntry("test_cases", sptr2);
		testEvalOp->initialize(state);
		state->getRegistry()->modifyEntry("test_cases", sptr);

		return true;
	}

	bool operate(StateP state)
	{
		ECF_LOG(state, 1, "Podaci\t" + uint2str(state->getGenerationNo()));
		IndividualP bestInd = state->getPopulation()->getHof()->getBest().at(0);
		ECF_LOG(state, 1, bestInd->toString());

		TreeP bestTree = boost::dynamic_pointer_cast<Tree::Tree> (bestInd->getGenotype(0));
		uint sizeOfBest = (uint) bestTree->size();

		DemeP deme = state->getPopulation()->getLocalDeme();
		uint popSize = (uint) deme->size();
		uint totalSize = 0;
		for(uint i = 0; i < popSize; i++) {
			TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (deme->at(i)->getGenotype(0));
			totalSize += (uint) tree->size();
		}
		uint averageSize = (uint) (1. * totalSize / popSize);

		// evaluate best individual on test data set
		double bestValue = testEvalOp->evaluate(bestInd)->getValue();

		ECF_LOG(state, 1, uint2str(sizeOfBest) + "\t" + uint2str(averageSize) + "\t" + dbl2str(bestValue));

		return true;
	}
};



//
// pomocni operator za ispis trenutno najboljeg rjesenja
//
class WriteBest : public Operator
{
private:
	StateP state_;
	std::string logfilename;
	SchedulingEvalOpP testEvalOp;
public:

	bool initialize(StateP state)
	{
		state_ = state;
		return true;
	}

	bool operate(StateP state)
	{
		ECF_LOG(state, 1, "Best in " + uint2str(state->getGenerationNo()));
		IndividualP bestInd = state->getPopulation()->getHof()->getBest().at(0);
		ECF_LOG(state, 1, bestInd->toString());

		return true;
	}
};


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


 typedef struct ensembleParams{
	std::string method;
	std::string combination;
	int size;
	int repetitions;
	int jobCount;
} EnsembleParams;


int testEval(int argc, char **argv)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);

	vector<IndividualP> individuals;
	
	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();
	
	vector<double> fitnesses;

	vector<IndividualP> ensemble;

	clock_t begin = clock();

	for(int i = 0; i<10; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		ca->evaluate(ind);
		fitnesses.push_back(ind->fitness->getValue());
		cout<<ind->fitness->getValue()<<endl;
		individuals.push_back(ind);
		ensemble.push_back(ind);	
	}

	clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed individual "<< elapsed_secs; 

	begin = clock();

	ca->Evaluate(ensemble, ensemble.size());
	cout<<"Total: "<<ensemble[0]->fitness->getValue()<<endl;
	end = clock();
 elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 
	
	
	return 0;
}

int random(int argc, char **argv, EnsembleParams params)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	//int ensembleSize = atoi(argv[2]);
	//int tries = atoi(argv[3]);

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();

	vector<IndividualP> individuals;
	//read individuals
	for(int i = 0; i<30; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
		//ca->evaluate(ind);
	}




	ofstream myfile;
	myfile.open ("out.txt");

	srand(time(NULL));

	vector<int> indices;

	for(int i=0; i<30; i++)
	{
		indices.push_back(i);
	}

	evalOp->method = params.combination;
	evalOp->nJobsCollab = params.jobCount;


	
	clock_t begin = clock();
	for(int r=0; r<30; r++){

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		for(int k = 0; k<params.repetitions; k++){
			int counter = 1;
			vector<int> copiedIndices(indices);
			vector<int> selectedIndices;
			//vector<IndividualP> ensemble;
			vector<IndividualP> ens;

			while(counter<=params.size)
			{
				int randIndex = rand()%copiedIndices.size();
				int index = copiedIndices[randIndex];
				copiedIndices.erase(copiedIndices.begin() + randIndex);
				selectedIndices.push_back(index);
				counter++;
			}


			

			for(int i = 0; i<selectedIndices.size(); i++)
			{
				ens.push_back(individuals[selectedIndices[i]]);

			}


			ca->Evaluate(ens, params.size);
			if(ens[0]->fitness->getValue()<bestFit)
			{
				bestFit = ens[0]->fitness->getValue();
				bestEnsembleIndices = vector<int>(selectedIndices);
				cout<<bestFit<<endl;
						for(int l=0; l<bestEnsembleIndices.size(); l++)
		{
			cout<<bestEnsembleIndices[l]<< " ";
		
		}
						cout<<endl;
			}
		}

		vector<IndividualP> ensemble;
		for(int i = 0; i<bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(individuals[bestEnsembleIndices[i]]);
		}
		ca->Evaluate(ensemble, params.size);

		cout<<"For run i=" << r<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		myfile<<r<<" " << ensemble[0]->fitness->getValue()<<" ";
			//cout<<"With the enslemle: ";
		
		for(int l=0; l<bestEnsembleIndices.size(); l++)
		{
			cout<<bestEnsembleIndices[l]<< " ";
			myfile<<bestEnsembleIndices[l]<< " ";
		}
		cout<<endl;
		myfile<<endl;

	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 

	ofstream duration;
	duration.open("ensembleTime.txt");
	duration << elapsed_secs << endl;
	duration.close();

	return 0;
}

int random_varsize(int argc, char **argv, EnsembleParams params)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	//int ensembleSize = atoi(argv[2]);
	//int tries = atoi(argv[3]);

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();

	vector<int> sizeCounter(50, 0);
	vector<double> sizeFitness(50, 0);

	vector<IndividualP> individuals;
	//read individuals
	for(int i = 0; i<50; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
		//ca->evaluate(ind);
	}




	ofstream myfile;
	myfile.open ("out.txt");

	srand(time(NULL));

	vector<int> indices;

	for(int i=0; i<50; i++)
	{
		indices.push_back(i);
	}

	evalOp->method = params.combination;
	evalOp->nJobsCollab = params.jobCount;


	
	clock_t begin = clock();
	for(int r=0; r<30; r++){

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		

		for(int k = 0; k<params.repetitions; k++){
			int ensembleSize = rand() % (params.size-2)+2;
			int counter = 1;
			vector<int> copiedIndices(indices);
			vector<int> selectedIndices;
			//vector<IndividualP> ensemble;
			vector<IndividualP> ens;

			while(counter<=ensembleSize)
			{
				int randIndex = rand()%copiedIndices.size();
				int index = copiedIndices[randIndex];
				copiedIndices.erase(copiedIndices.begin() + randIndex);
				selectedIndices.push_back(index);
				counter++;
			}


			

			for(int i = 0; i<selectedIndices.size(); i++)
			{
				ens.push_back(individuals[selectedIndices[i]]);

			}


			ca->Evaluate(ens, params.size);
			if(ens[0]->fitness->getValue()<bestFit)
			{
				bestFit = ens[0]->fitness->getValue();
				bestEnsembleIndices = vector<int>(selectedIndices);
				cout<<bestFit<<endl;
						for(int l=0; l<bestEnsembleIndices.size(); l++)
				{
					cout<<bestEnsembleIndices[l]<< " ";
				
				}
						cout<<endl;
			}

			sizeCounter[ensembleSize]++;
			sizeFitness[ensembleSize]+=ens[0]->fitness->getValue();
		}

		vector<IndividualP> ensemble;
		for(int i = 0; i<bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(individuals[bestEnsembleIndices[i]]);
		}
		ca->Evaluate(ensemble, params.size);

		cout<<"For run i=" << r<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		myfile<<r<<" " << ensemble[0]->fitness->getValue()<<" ";
			//cout<<"With the enslemle: ";
		
		for(int l=0; l<bestEnsembleIndices.size(); l++)
		{
			cout<<bestEnsembleIndices[l]<< " ";
			myfile<<bestEnsembleIndices[l]<< " ";
		}
		cout<<endl;
		myfile<<endl;

	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 


	ofstream ensStatitstics;
	ensStatitstics.open ("ensembleStatistics.txt");
	for(int i=0; i<50; i++)
	{
		double ratio = 0;
		if(sizeCounter[i]!=0)
		{
			ratio =sizeFitness[i]/sizeCounter[i];
		}
		ensStatitstics<<i<< " " << sizeCounter[i] << " " <<  ratio<<endl;
	}

	ensStatitstics.close();

	ofstream duration;
	duration.open ("ensembleTime.txt");
	duration<<elapsed_secs<<endl;
	duration.close();

	
	return 0;
}

int grow(int argc, char **argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	//int ensembleSize = ensembleSize;
	
	vector<IndividualP> individuals;
	//read individuals
	for(int i = 0; i<50; i++){

		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
	}



	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();
	ofstream myfile;
	myfile.open ("out.txt");

	//evalOp->method = params.combination;
	//evalOp->nJobsCollab = params.jobCount;
	evalOp->electoralSystem = electoralSystem;

		clock_t begin = clock();
	for(int k = 0; k<individuals.size(); k++){
		int counter = 2;
		vector<int> ensembleIndices;
		vector<IndividualP> copiedIndividuals(individuals);
		vector<IndividualP> ensemble;
		ensemble.push_back(copiedIndividuals[k]);
		ensembleIndices.push_back(k);
		copiedIndividuals.erase(copiedIndividuals.begin()+k);

		while(counter<=ensembleSize)
		{
			int bestIndex = 0;
			double bestValue = DBL_MAX;
			for(int i = 0; i<copiedIndividuals.size(); i++)
			{
				vector<IndividualP> testEnsemble(ensemble);	
				testEnsemble.push_back(copiedIndividuals[i]);
				ca->Evaluate(testEnsemble, counter);
				double fitnessVal =  testEnsemble.at(0)->fitness->getValue();
				//cout<< "Best for size: " <<  counter << " "<< fitnessVal<<endl;
				if(bestValue>fitnessVal)
				{
					bestValue = fitnessVal;
					bestIndex = i;
				}
			}
			counter++;
			ensemble.push_back(copiedIndividuals[bestIndex]);
			int index = find(individuals.begin(), individuals.end(), copiedIndividuals[bestIndex])-individuals.begin();
			ensembleIndices.push_back(index);
			copiedIndividuals.erase(copiedIndividuals.begin()+bestIndex);
		}
		ca->Evaluate(ensemble, ensembleSize);
		cout<<"For ind i=" << k<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		myfile<<k<<" " << ensemble[0]->fitness->getValue()<<" ";
		//cout<<"With the enslemle: ";
		
		for(int l=0; l<ensembleIndices.size(); l++)
		{
			cout<<ensembleIndices[l]<< " ";
			myfile<<ensembleIndices[l]<< " ";
		}
		cout<<endl;
		myfile<<endl;
	}
			clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 


	return 0;
}


int grow_destroy(int argc, char **argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	
	
	vector<IndividualP> individuals;
	//read individuals
	for(int i = 0; i<50; i++){

		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
	}



	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();

		ofstream myfile;
	myfile.open ("out.txt");

	evalOp->electoralSystem = electoralSystem;

		clock_t begin = clock();
		int k = 0;
	{
		int counter = 2;
		vector<int> ensembleIndices;
		vector<IndividualP> copiedIndividuals(individuals);
		vector<IndividualP> ensemble;
		ensemble.push_back(copiedIndividuals[k]);
		ensembleIndices.push_back(k);
		copiedIndividuals.erase(copiedIndividuals.begin()+k);

		while(counter<=(ensembleSize*2)&&counter<=50)
		{
			int bestIndex = 0;
			double bestValue = DBL_MAX;
			for(int i = 0; i<copiedIndividuals.size(); i++)
			{
				vector<IndividualP> testEnsemble(ensemble);	
				testEnsemble.push_back(copiedIndividuals[i]);
				ca->Evaluate(testEnsemble, counter);
				double fitnessVal =  testEnsemble.at(0)->fitness->getValue();
				//cout<< "Best for size: " <<  counter << " "<< fitnessVal<<endl;
				if(bestValue>fitnessVal)
				{
					bestValue = fitnessVal;
					bestIndex = i;
				}
			}
			counter++;
			ensemble.push_back(copiedIndividuals[bestIndex]);
			int index = find(individuals.begin(), individuals.end(), copiedIndividuals[bestIndex])-individuals.begin();
			ensembleIndices.push_back(index);
			copiedIndividuals.erase(copiedIndividuals.begin()+bestIndex);
		}

		ca->Evaluate(ensemble, ensemble.size());
		//cout<<"For ind i=" << k<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		//cout<<"With the enslemle: ";

		//counter = 0;
		counter--;
		while(counter>ensembleSize)
		{
			int bestIndex = 0;
			double bestValue = DBL_MAX;
			for(int i = 0; i<ensemble.size(); i++)
			{
				vector<IndividualP> testEnsemble(ensemble);	
				testEnsemble.erase(testEnsemble.begin()+i);
				ca->Evaluate(testEnsemble, counter);
				double fitnessVal =  testEnsemble.at(0)->fitness->getValue();
				//cout<< "Best for size: " <<  counter << " "<< fitnessVal<<endl;
				if(bestValue>fitnessVal)
				{
					bestValue = fitnessVal;
					bestIndex = i;
				}
			}
			counter--;
			ensemble.erase(ensemble.begin()+bestIndex);
			//int index = find(ensembleIndices.begin(), ensembleIndices.end(), bestIndex)-ensembleIndices.begin();
			ensembleIndices.erase(ensembleIndices.begin() + bestIndex);
			//ensemble.erase(copiedIndividuals.begin()+bestIndex);
		}

		ca->Evaluate(ensemble, ensembleSize);
		cout<<"For ind i=" << k<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		myfile<< k<<" " << ensemble[0]->fitness->getValue()<<" ";
		
		//cout<<"With the enslemle: ";
		for(int l=0; l<ensembleIndices.size(); l++)
		{
			cout<<ensembleIndices[l]<< " ";
			myfile<<ensembleIndices[l]<< " ";

		}
		cout<<endl;
		myfile<<endl;

	}
		clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 

	
	return 0;
}

int grow_destroy_reverse(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb(new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl(new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr(new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg(new Avg);
	tree->addFunction(avg);
	Tree::PrimitiveP abs(new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);




	vector<IndividualP> individuals;
	//read individuals
	for (int i = 0; i < 50; i++) {

		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
	}



	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	ofstream myfile;
	myfile.open("out.txt");

	evalOp->electoralSystem = electoralSystem;

	clock_t begin = clock();
	int k = 0;
	{
		int counter = 2;
		vector<int> ensembleIndices;
		vector<IndividualP> copiedIndividuals(individuals);
		vector<IndividualP> ensemble;
		
		for(int i = 0; i<50; i++)
		{
			ensemble.push_back(individuals[i]);
			ensembleIndices.push_back(i);
		}
		
		ca->Evaluate(ensemble, ensemble.size());
		//cout<<"For ind i=" << k<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		//cout<<"With the enslemle: ";

		counter = 50;
		counter--;
		while (counter > ensembleSize)
		{
			int bestIndex = 0;
			double bestValue = DBL_MAX;
			for (int i = 0; i < ensemble.size(); i++)
			{
				vector<IndividualP> testEnsemble(ensemble);
				testEnsemble.erase(testEnsemble.begin() + i);
				ca->Evaluate(testEnsemble, counter);
				double fitnessVal = testEnsemble.at(0)->fitness->getValue();
				//cout<< "Best for size: " <<  counter << " "<< fitnessVal<<endl;
				if (bestValue > fitnessVal)
				{
					bestValue = fitnessVal;
					bestIndex = i;
				}
			}
			counter--;
			ensemble.erase(ensemble.begin() + bestIndex);
			//int index = find(ensembleIndices.begin(), ensembleIndices.end(), bestIndex)-ensembleIndices.begin();
			ensembleIndices.erase(ensembleIndices.begin() + bestIndex);
			//ensemble.erase(copiedIndividuals.begin()+bestIndex);
		}

		ca->Evaluate(ensemble, ensembleSize);
		cout << "For ind i=" << k << " the result was: " << ensemble[0]->fitness->getValue() << endl;
		myfile << k << " " << ensemble[0]->fitness->getValue() << " ";

		//cout<<"With the enslemle: ";
		for (int l = 0; l < ensembleIndices.size(); l++)
		{
			cout << ensembleIndices[l] << " ";
			myfile << ensembleIndices[l] << " ";

		}
		cout << endl;
		myfile << endl;

	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Elapsed ensemble " << elapsed_secs;


	return 0;
}

std::vector<shared_ptr<ElectoralSystem>> getElectoralSystemList();


int evalEnsemble(int argc, char **argv, EnsembleParams params)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	//int ensembleSize = atoi(argv[2]);
	//int tries = atoi(argv[3]);

	vector<IndividualP> individuals;
	//read individuals
	
	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();
	
	vector<double> fitnesses;


	evalOp->method = params.combination;
	evalOp->nJobsCollab = params.jobCount;

	double time = 0;
	
	
	for(int i = 0; i<50; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		clock_t tStart = clock();	
		ca->evaluate(ind);
		clock_t tEnd = clock();
		time+=(double)(clock() - tStart)/CLOCKS_PER_SEC;
		fitnesses.push_back(ind->fitness->getValue());
		individuals.push_back(ind);
	}

	cout<<"Individual DR eval time: " << time<<endl;


	std::string line;
	std::ifstream nameFileout;

	nameFileout.open("out.txt");
	
	ofstream myfile;
	//ofstream occur;
	myfile.open ("outEval.txt");
	//occur.open ("outoccur.txt");
	time = 0;

	auto electoralSystems = getElectoralSystemList();

	while (std::getline(nameFileout, line))
	{
		vector<string> splitted ;
		vector<IndividualP> ensemble;

//			std::getline(nameFileout, line);
			splitted = split(line, ' ');
			for(int i = 2; i<splitted.size(); i++)
			{
				ensemble.push_back(individuals[atoi(splitted[i].c_str())]);
			}
		for(auto electoralSystem :electoralSystems)
		{
			evalOp->electoralSystem = electoralSystem;
			ca->Evaluate(ensemble, ensemble.size());

			cout << ensemble[0]->fitness->getValue()<<" ";
			myfile << ensemble[0]->fitness->getValue() << " ";

		}

		cout << endl;
		myfile << endl;
		
	}

	cout<<"Ensemble: " << time<<endl;


	myfile.close();
	nameFileout.close();
	return 0;
}



int probabilistic(int argc, char **argv, EnsembleParams params)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	int ensembleSize = params.size;
	int tries = params.repetitions;

	vector<IndividualP> individuals;
	//read individuals
	
	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();
	
	vector<double> fitnesses;

	for(int i = 0; i<50; i++){

		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		ca->evaluate(ind);
		fitnesses.push_back(ind->fitness->getValue());
		individuals.push_back(ind);
	}

		evalOp->method = params.combination;
	evalOp->nJobsCollab = params.jobCount;


	double maxFit = 0;
	for(int i = 0; i< 50; i++)
	{
		if(maxFit<fitnesses[i])
		{
			maxFit = fitnesses[i];
		}
	}

	maxFit+=1;

	vector<double> probabilites;
	double aggregate = 0;
	for(int i = 0; i< 50; i++)
	{
		aggregate+=maxFit - fitnesses[i];
		probabilites.push_back(aggregate);
	}

	ofstream myfile;
	myfile.open ("out.txt");

	srand(time(NULL));

	vector<int> indices;

	evalOp->method = params.combination;

	for(int i=0; i<50; i++)
	{
		indices.push_back(i);
	}

		clock_t begin = clock();
	for(int r=0; r<30; r++){

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		for(int k = 0; k<tries; k++){
			int counter = 1;
			vector<int> copiedIndices(indices);
			vector<int> selectedIndices;

			while(counter<=ensembleSize)
			{
				int randIndex = 0;
				do{
					double f = (double)rand() / RAND_MAX;
					double randVal = f * (aggregate);

					
					for(int i = 0; i< 50; i++)
					{
						if(randVal<=probabilites[i])
						{
							randIndex = i;
							break;
						}
					}
				}while(find(selectedIndices.begin(), selectedIndices.end(), randIndex) != selectedIndices.end());

				selectedIndices.push_back(randIndex);
				counter++;
			}


			vector<IndividualP> ensemble;

			for(int i = 0; i<selectedIndices.size(); i++)
			{
				ensemble.push_back(individuals[selectedIndices[i]]);
			}


			ca->Evaluate(ensemble, ensembleSize);
			if(ensemble[0]->fitness->getValue()<bestFit)
			{
				bestFit = ensemble[0]->fitness->getValue();
				bestEnsembleIndices = vector<int>(selectedIndices);
			}
		}

		vector<IndividualP> ensemble;
		for(int i = 0; i<bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(individuals[bestEnsembleIndices[i]]);
		}
		ca->Evaluate(ensemble, ensembleSize);

		cout<<"For run i=" << r<<" the result was: " << ensemble[0]->fitness->getValue()<<endl;
		myfile<<"For run i=" << r<<" the result was: " << ensemble[0]->fitness->getValue()<<" ";
			//cout<<"With the enslemle: ";
		
		for(int l=0; l<bestEnsembleIndices.size(); l++)
		{
			cout<<bestEnsembleIndices[l]<< " ";
			myfile<<bestEnsembleIndices[l]<< " ";
		}
		cout<<endl;
		myfile<<endl;

	}

		clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 


	return 0;
}







//
int evolveGpEnsemble(int argc, char **argv)
{
	StateP state (new State);

	GenHookeJeevesP alg (new GenHookeJeeves);
	state->addAlgorithm(alg);

	SchedulingEvalOpP evalOp (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	evalOp->ensenbleCount = 5;

//	Zapis *z = new Zapis;
//	state->addOperator((OperatorP) z);

	WriteBest *b = new WriteBest;
	state->addOperator((OperatorP) b);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
	Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->addGenotype(tree);

	if(argc > 2) {
		uint primjer = atoi(argv[2]);
		state->getContext()->environment = new uint(primjer);
	}

	state->initialize(argc, argv);
	state->run();

	return 0;
}


//
// Evolution of scheduling heuristics
//
int evalGA(int argc, char **argv, EnsembleParams params)
{
	StateP state (new State);

	char *params1 = "parametersTree.txt";
	char *backup = argv[1];
	char *argv2[2];
	argv[1] = params1;
	//argv2[0] = argv[0];
	SchedulingEvalOpP evalOp (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	
 	WriteBest *b = new WriteBest;
	state->addOperator((OperatorP) b);

	// integer values genotype
	IntUniqueGenotypeP gen = (IntUniqueGenotypeP) (new IntUniqueGenotype::IntUniqueGenotype);
	state->addGenotype(gen);

	vector<IndividualP> individuals;
	evalOp->firstTime = true;
	state->initialize(argc, argv);
	for(int i = 0; i<50; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);

		//treeGen->setGenotypeId(0);
		//treeGen->initialize(state);
		XMLNode values = xInd.getChildNode("Tree");
		ind->getGenotype()->read(values);
		//ind->read(xInd);
		//Tree::Tree* indTree = (Tree::Tree*)ind->getGenotype().get();
		
		individuals.push_back(ind);
	}
	evalOp->firstTime = false;
	evalOp->ensenbleCount = params.size;
	evalOp->individuals = individuals;
	evalOp->nJobsCollab = params.jobCount;


	// if 3rd argument is present, read that as a single test case to evaluate
	if(argc > 2) {
		uint primjer = atoi(argv[2]);
		state->getContext()->environment = new uint(primjer);
	}

	evalOp->method = params.combination;
	argv[1] = backup;
	StateP state2 (new State);
	
	state2->addOperator((OperatorP) b);
	state2->addGenotype(gen);
	state2->setEvalOp(evalOp);
	state2->initialize(argc, argv);

		clock_t begin = clock();
	state2->run();
		clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 


	return 0;
}


int instanceBased(int argc, char **argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);

	
	vector<IndividualP> individuals;
	//read individuals

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();

	vector<vector<double>> fitnesses;

	for(int i = 0; i<50; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		ca->evaluate(ind);
		individuals.push_back(ind);
		fitnesses.push_back(vector<double>());
		vector<double> indFit;
		indFit.push_back(ind->fitness->getValue());
		for(int j = 0; j<60; j++)
		{
			fitnesses[i].push_back(evalOp->Fitness[j][Twt]);
		}
	}
	evalOp->electoralSystem = electoralSystem;

	vector<double> optimal;
	for(int i = 0; i<100; i++)
	{
		double optVal = DBL_MAX;

	
		for(int j = 0; j<50; j++)
		{
			if(optVal>fitnesses[j][i])
			{
				optVal = fitnesses[j][i];
			}
		}
		optimal.push_back(optVal);
	}


	ofstream myfile;
	myfile.open ("out.txt");

		clock_t begin = clock();
	for(int i = 0; i<50; i++)
	{
		vector<int> ensembleIndices;
		ensembleIndices.push_back(i);
		vector<int> nonOpt;
		for(int j=0; j<100; j++)
		{
			if(fabs(fitnesses[i][j]-optimal[j])>10e-6)
			{
				nonOpt.push_back(1);
			} else
			{
				nonOpt.push_back(0);
			}
		} 

		int counter = 2;
		while (counter<=ensembleSize)
		{
			double nonOptValue=DBL_MAX;
			int nonOptIndex = 0;

			for(int j=0; j<50; j++)
			{
				double indNonOptValue = 0;
				if(find(ensembleIndices.begin(), ensembleIndices.end(), j)!=ensembleIndices.end())
				{
					continue;
				}
				for(int k = 0; k<100; k++)
				{
					if(!nonOpt[k])
					{
						continue;
					}
					indNonOptValue+=fitnesses[j][k]-optimal[k];
				}
				if(indNonOptValue<nonOptValue)
				{
					nonOptValue = indNonOptValue;
					nonOptIndex = j;
				}
			}

			ensembleIndices.push_back(nonOptIndex);
			vector<IndividualP> ensemble;
			for (int j=0; j<ensembleIndices.size(); j++)
			{
				ensemble.push_back(individuals[ensembleIndices[j]]);
			}

			ca->Evaluate(ensemble, ensemble.size());

			nonOpt.clear();

			for(int j=0;j<60; j++)
			{
				if(evalOp->Fitness[j][Twt]<=optimal[j])
				{
					nonOpt.push_back(0);
				} else
				{
					nonOpt.push_back(1);
				}
			}
			counter++;
		}
		
		vector<IndividualP> ensemble;
		for(int j = 0; j<ensembleIndices.size(); j++)
		{
			ensemble.push_back(individuals[ensembleIndices[j]]);
		}

		ca->Evaluate(ensemble, ensemble.size());

		cout<<i<<" "<< ensemble[0]->fitness->getValue()<<endl;

		myfile<<i<<" "<< ensemble[0]->fitness->getValue()<<" ";
			//cout<<"With the enslemle: ";
		
		for(int l=0; l<ensembleIndices.size(); l++)
		{
			cout<<ensembleIndices[l]<< " ";
			myfile<<ensembleIndices[l]<< " ";
		}
		cout<<endl;
		myfile<<endl;

	}	

			clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout<<"Elapsed ensemble "<< elapsed_secs; 

	return 0;
}


int instanceBasedEnsemble(int argc, char **argv, EnsembleParams params)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);

	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);

	int ensembleSize = params.size;
	
	vector<IndividualP> individuals;
	//read individuals

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();

	vector<vector<double>> fitnesses;

	for(int i = 0; i<50; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		ca->evaluate(ind);
		individuals.push_back(ind);
		fitnesses.push_back(vector<double>());
		vector<double> indFit;
		indFit.push_back(ind->fitness->getValue());
		for(int j = 0; j<60; j++)
		{
			fitnesses[i].push_back(evalOp->Fitness[j][Twt]);
		}
	}
		evalOp->method = params.combination;

	vector<double> optimal;
	for(int i = 0; i<60; i++)
	{
		double optVal = DBL_MAX;

	
		for(int j = 0; j<50; j++)
		{
			if(optVal>fitnesses[j][i])
			{
				optVal = fitnesses[j][i];
			}
		}
		optimal.push_back(optVal);
	}


	ofstream myfile;
	myfile.open ("out.txt");

	for(int i = 0; i<50; i++)
	{
		vector<int> ensembleIndices;
		ensembleIndices.push_back(i);
		vector<int> nonOpt;
		for(int j=0; j<60; j++)
		{
			if(fabs(fitnesses[i][j]-optimal[j])>10e-6)
			{
				nonOpt.push_back(1);
			} else
			{
				nonOpt.push_back(0);
			}
		} 

		int counter = 2;
		while (counter<=ensembleSize)
		{
			double nonOptValue=DBL_MAX;
			int nonOptIndex = 0;
			int bsetOptimalCount = 0;

			for(int j=0; j<50; j++)
			{
				
				double indNonOptValue = 0;
				if(find(ensembleIndices.begin(), ensembleIndices.end(), j)!=ensembleIndices.end())
				{
					continue;
				}

				ensembleIndices.push_back(j);
				vector<IndividualP> ensemble;
				for (int j=0; j<ensembleIndices.size(); j++)
				{
					ensemble.push_back(individuals[ensembleIndices[j]]);
				}

				ca->Evaluate(ensemble, ensemble.size());

				ensembleIndices.pop_back();


				int optimalCount = 0;
				for(int k = 0; k<60; k++)
				{
					if(optimal[k]>=evalOp->Fitness[k][Twt])
					{
						optimalCount++;
					} else{
						indNonOptValue+=evalOp->Fitness[k][Twt]-optimal[k];
					}
				}
				indNonOptValue = ensemble.at(0)->fitness->getValue();
				if(bsetOptimalCount<optimalCount)
				{
					bsetOptimalCount = optimalCount;
					nonOptValue = indNonOptValue;
					nonOptIndex = j;
				}
				if(bsetOptimalCount==optimalCount&&nonOptValue<indNonOptValue) 
				{
					bsetOptimalCount = optimalCount;
					nonOptValue = indNonOptValue;
					nonOptIndex = j;	
				}

			}

			ensembleIndices.push_back(nonOptIndex);
			vector<IndividualP> ensemble;
			for (int j=0; j<ensembleIndices.size(); j++)
			{
				ensemble.push_back(individuals[ensembleIndices[j]]);
			}

			ca->Evaluate(ensemble, ensemble.size());

			nonOpt.clear();

			for(int j=0;j<60; j++)
			{
				if(evalOp->Fitness[j][Twt]<=optimal[j])
				{
					nonOpt.push_back(0);
				} else
				{
					nonOpt.push_back(1);
				}
			}
			counter++;
		}
		
		vector<IndividualP> ensemble;
		for(int j = 0; j<ensembleIndices.size(); j++)
		{
			ensemble.push_back(individuals[ensembleIndices[j]]);
		}

		ca->Evaluate(ensemble, ensemble.size());

		cout<<i<<" "<< ensemble[0]->fitness->getValue()<<endl;

		myfile<<i<<" "<< ensemble[0]->fitness->getValue()<<" ";
			//cout<<"With the enslemle: ";
		
		for(int l=0; l<ensembleIndices.size(); l++)
		{
			cout<<ensembleIndices[l]<< " ";
			myfile<<ensembleIndices[l]<< " ";
		}
		cout<<endl;
		myfile<<endl;

	}	
	return 0;
}



EnsembleParams initEnsembleParams()
{
	EnsembleParams params;

	std::ifstream input( string("ensParam.txt") );
	for(int i = 0; i<5; i++)
	{
		std::string line; 
		getline( input, line );
		vector<string> lineSplit = split(line, ' ');
		if(strcmp(lineSplit[0].c_str(), "method")==0)
		{	
			params.method = lineSplit[1].c_str();
		}
		if(strcmp(lineSplit[0].c_str(), "combination")==0)
		{	
			params.combination = lineSplit[1].c_str();
		}
		if(strcmp(lineSplit[0].c_str(), "size")==0)
		{	
			params.size = stoi( lineSplit[1].c_str());
		}
		if(strcmp(lineSplit[0].c_str(), "repetitions")==0)
		{	
			params.repetitions = stoi(lineSplit[1].c_str());
		}
		if(strcmp(lineSplit[0].c_str(), "jobCount")==0)
		{	
			params.jobCount = stoi(lineSplit[1].c_str());
		}
	}
	return params;
}


int writeFitnesses(int argc, char **argv)
{
	StateP state (new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree (new Tree::Tree);
		IntUniqueGenotypeP gen = (IntUniqueGenotypeP) (new IntUniqueGenotype::IntUniqueGenotype);
	state->addGenotype(gen);
	// citanje terminala iz 'fitness.txt'
	// -- ne radimo jer se cita iz 'terminalset' parametra!
	//TreeP tree = (TreeP) new Tree::Tree;
	//evalOp->in_file = "fitness.txt";
	//evalOp->ReadTerminals(tree);
	//state->addGenotype(tree);
	Tree::PrimitiveP ifb (new Ifgt);
	tree->addFunction(ifb);
	Tree::PrimitiveP ifl (new Iflt);
	tree->addFunction(ifl);
	Tree::PrimitiveP sqr (new Sqr);
	tree->addFunction(sqr);
	state->addGenotype(tree);
	Tree::PrimitiveP avg (new Avg);
	tree->addFunction(avg);
		Tree::PrimitiveP abs (new Abs);
	tree->addFunction(abs);

	state->initialize(argc, argv);


	//int ensembleSize = atoi(argv[2]);
	//int tries = atoi(argv[3]);

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm *ca = (CustomAlgorithm*)alg.get();

	vector<IndividualP> individuals;
	//read individuals

	ofstream myfile;
	myfile.open ("fitnesses.txt");

	for(int i = 0; i<50; i++){
		std::ostringstream stringStream;
		stringStream << "./ind"<<i<<".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
		ca->evaluate(ind);
		for(int j = 0; j<60;j++){
			myfile<<evalOp->Fitness[j][Nwt]<<" ";
		}
		myfile<<endl;
	}
	myfile.close();

	return 0;
}

int runInd(int argc, char **argv)
{
	StateP state (new State);

	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);

	state->setEvalOp(evalOp);
	TreeP tree (new Tree::Tree);

	//Tree::PrimitiveP late (new LateOperator);
	//tree->addFunction(late);
	state->addGenotype(tree);
	state->initialize(argc, argv);
	// citanje jedinke iz 'ind.txt' i provjera dobrote
	
	vector<double> execs;

	for(int i = 0; i<50; i++){
		evalOp->firstTime = true;
		std::ostringstream oss;
		oss << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(oss.str().c_str(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		evalOp->firstTime = false;
		clock_t begin = clock();
		state->getAlgorithm()->evaluate(ind);
		
		clock_t end = clock();
		double elapsed_secs = double(end - begin);
		execs.push_back(elapsed_secs);

		std::cout << ind->toString();
		cout<<ind->fitness->getValue(); 
	}
	
	
	cout<<"EXECUTION TIMES: "<<endl;
	for(int i = 0; i<30; i++){
		cout<<execs[i]<< endl;
	}


	return 0;
}

int evolveGp(int argc, char** argv)
{
	StateP state(new State);

	GenHookeJeevesP alg(new GenHookeJeeves);
	state->addAlgorithm(alg);

	SchedulingEvalOpP evalOp(new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	ProbabilisticTreeP tree(new Tree::ProbabilisticTree);
	state->addGenotype(tree);


	//	Zapis *z = new Zapis;
	//	state->addOperator((OperatorP) z);

	WriteBest* b = new WriteBest;
	state->addOperator((OperatorP)b);

	//TreeP tree(new Tree::Tree);

	//// citanje terminala iz 'fitness.txt'
	//// -- ne radimo jer se cita iz 'terminalset' parametra!
	////TreeP tree = (TreeP) new Tree::Tree;
	////evalOp->in_file = "fitness.txt";
	////evalOp->ReadTerminals(tree);
	////state->addGenotype(tree);
	//Tree::PrimitiveP ifb(new Ifgt);
	//tree->addFunction(ifb);
	//Tree::PrimitiveP ifl(new Iflt);
	//tree->addFunction(ifl);
	//Tree::PrimitiveP sqr(new Sqr);
	//tree->addFunction(sqr);
	//Tree::PrimitiveP avg(new Avg);
	//tree->addFunction(avg);
	//Tree::PrimitiveP abs(new Abs);
	//tree->addFunction(abs);

	//state->addGenotype(tree);

	if (argc > 2) {
		uint primjer = atoi(argv[2]);
		state->getContext()->environment = new uint(primjer);
	}

	state->initialize(argc, argv);
	state->run();

	return 0;
}


shared_ptr<ElectoralSystem> createElectoralSystem()
{
	std::ifstream input(string("electoral.txt"));
	std::string line;
	getline(input, line);

	auto params = StringUtils::Split(line, " ");
	string method = params[0];

	std::transform(method.begin(), method.end(), method.begin(),
		[](unsigned char c) { return std::tolower(c); });

	shared_ptr<ElectoralSystem> electoralSystem;

	if (method == "antiplurality")
	{
		electoralSystem = make_shared<AntiPlurality>();
	}
	else if (method == "approval")
	{
		double approvalRate = std::stod(params[1]);
		bool useRange = std::stoi(params[2]);
		bool secondRound = std::stoi(params[3]);

		electoralSystem = make_shared<ApprovalVoting>(approvalRate, useRange, secondRound);
	}
	else if (method == "baldwin")
	{
		electoralSystem = make_shared<BaldwinMethod>();
	}
	else if (method == "borda")
	{
		string bordaScoringString = params[1];
		std::transform(bordaScoringString.begin(), bordaScoringString.end(), bordaScoringString.begin(),
			[](unsigned char c) { return std::tolower(c); });

		BordaCountStrategy strategy;
		if (bordaScoringString == "standard")
		{
			strategy = BordaCountStrategy::StandardBorda;
		}
		else if (bordaScoringString == "binary")
		{
			strategy = BordaCountStrategy::BinaryNumberSystem;
		}
		else if (bordaScoringString == "nauru")
		{
			strategy = BordaCountStrategy::Nauru;
		}
		else if (bordaScoringString == "eurovision")
		{
			strategy = BordaCountStrategy::Eurovision;
		}
		else
		{
			throw "Unknown borda strategy specified";
		}
		electoralSystem = make_shared<BordaCount>(strategy);
	}
	else if (method == "bucklin")
	{
		electoralSystem = make_shared<BucklinVoting>();
	}
	else if (method == "combinedapproval")
	{
		double approvalRate = std::stod(params[1]);
		double disapprovalRate = std::stod(params[2]);
		double useApprovalInterval = std::stod(params[3]);

		electoralSystem = make_shared<CombinedApprovalVoting>(approvalRate, disapprovalRate, useApprovalInterval);
	}
	else if (method == "condorcet")
	{
		electoralSystem = make_shared<CondorcetMethod>();
	}
	else if (method == "contigent")
	{
		electoralSystem = make_shared<ContigentVoting>();
	}
	else if (method == "coombs")
	{
		electoralSystem = make_shared<CoombsMethod>();
	}
	else if (method == "copeland")
	{
		electoralSystem = make_shared<CopelandMethod>();
	}
	else if (method == "instantrunoff")
	{
		electoralSystem = make_shared<InstantRunoff>();
	}
	else if (method == "judgement")
	{
		int maximumRating = std::stoi(params[1]);
		int minimumRating = std::stoi(params[2]);
		string medianRuleString = params[3];
		MedianRule medianRule;
		if (medianRuleString == "typical")
		{
			medianRule = MedianRule::TypicalJudgement;
		}
		else if (medianRuleString == "usual")
		{
			medianRule = MedianRule::UsualJudgement;
		}
		else if (medianRuleString == "central")
		{
			medianRule = MedianRule::CentralJudgement;
		}
		else if (medianRuleString == "majority")
		{
			medianRule = MedianRule::MajorityJudgement;
		}
		else
		{
			throw "Unknown median rule specified for the judgement method";
		}
		electoralSystem = make_shared<JudgementMethod>(maximumRating, minimumRating, medianRule);
	}
	else if (method == "kemenyyoung")
	{
		int maximumConsideredCandidates = std::stoi(params[1]);
		electoralSystem = make_shared<KemenyYoungMethod>(maximumConsideredCandidates);
	}
	else if (method == "majority")
	{
		electoralSystem = make_shared<MajorityVote>();
	}
	else if (method == "minimax")
	{
		electoralSystem = make_shared<MiniMax>();
	}
	else if (method == "nanson")
	{
		electoralSystem = make_shared<NansonMethod>();
	}
	else if (method == "oklahoma")
	{
		electoralSystem = make_shared<OklahomaMethod>();
	}
	else if (method == "plurality")
	{
		bool useSecondRound = std::stoi(params[1]);
		electoralSystem = make_shared<PluralityVote>(useSecondRound);
	}
	else if (method == "preferential")
	{
		electoralSystem = make_shared<PreferentialBlockVoting>();
	}
	else if (method == "rankedpairs")
	{
		electoralSystem = make_shared<RankedPairs>();
	}
	else if (method == "schulze")
	{
		electoralSystem = make_shared<SchulzeMethod>();
	}
	else if (method == "score")
	{
		int maxScore = std::stoi(params[1]);
		int minScore = std::stoi(params[2]);
		electoralSystem = make_shared<ScoreVoting>(maxScore, minScore);
	}
	else if (method == "star")
	{
		electoralSystem = make_shared<StarVoting>();
	}
	else if (method == "tideman")
	{
		electoralSystem = make_shared<TidemanAlternativeSmith>();
	}
	else if (method == "unifiedprimary")
	{
		double approvalRate = std::stod(params[1]);
		bool useApprovalInterval = std::stoi(params[2]);
		electoralSystem = make_shared<UnifiedPrimary>(approvalRate, useApprovalInterval);
	}
	else if(method == "sum")
	{
		electoralSystem = make_shared<SumMethod>();
	}
	return electoralSystem;
}


int evaluateAllGenerations(int argc, char** argv)
{
	StateP state(new State);

	GenHookeJeevesP alg(new GenHookeJeeves);
	state->addAlgorithm(alg);

	SchedulingEvalOpP evalOp(new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	ProbabilisticTreeP tree(new Tree::ProbabilisticTree);
	state->addGenotype(tree);


	//	Zapis *z = new Zapis;
	//	state->addOperator((OperatorP) z);

	WriteBest* b = new WriteBest;
	state->addOperator((OperatorP)b);

	//TreeP tree(new Tree::Tree);

	//// citanje terminala iz 'fitness.txt'
	//// -- ne radimo jer se cita iz 'terminalset' parametra!
	////TreeP tree = (TreeP) new Tree::Tree;
	////evalOp->in_file = "fitness.txt";
	////evalOp->ReadTerminals(tree);
	////state->addGenotype(tree);
	//Tree::PrimitiveP ifb(new Ifgt);
	//tree->addFunction(ifb);
	//Tree::PrimitiveP ifl(new Iflt);
	//tree->addFunction(ifl);
	//Tree::PrimitiveP sqr(new Sqr);
	//tree->addFunction(sqr);
	//Tree::PrimitiveP avg(new Avg);
	//tree->addFunction(avg);
	//Tree::PrimitiveP abs(new Abs);
	//tree->addFunction(abs);

	//state->addGenotype(tree);

	if (argc > 2) {
		uint primjer = atoi(argv[2]);
		state->getContext()->environment = new uint(primjer);
	}



	state->initialize(argc, argv);
	vector<string> lines(100, "");
	for(int i=1;i<=30;i++)
	{
		string prefix = i < 10 ? "0" : "";
		string fileName = "log_" + prefix + to_string(i) + ".txt";
		ifstream file(fileName);
		string line;
		int counter = 0;
		while(getline(file, line))
		{
			string individualString;
			if(line.find("<Individual") != string::npos)
			{
				individualString += line;
				getline(file, line);
				individualString += line;
				getline(file, line);
				individualString += line;
				getline(file, line);
				individualString += line;
				
				XMLNode xInd = XMLNode::parseString(individualString.c_str(), "Individual");
				IndividualP ind = (IndividualP) new Individual(state);
				ind->read(xInd);
				
				state->getEvalOp()->evaluate(ind);

				//std::cout << ind->toString();
				//cout << ind->fitness->getValue();
				lines[counter] += to_string(ind->fitness->getValue()) + "\t";
				counter++;
			}
		}
	}

	for (auto line : lines)
	{
		cout << line << endl;
	}



	return 0;
}


void runTests()
{
	ElectionMethodTests test;
	test.testElectionMethods();
}

std::vector<shared_ptr<ElectoralSystem>> getElectoralSystemList()
{
	std::vector<shared_ptr<ElectoralSystem>> electoralSystems;

	electoralSystems.push_back(make_shared<AntiPlurality>());
	electoralSystems.push_back(make_shared<ApprovalVoting>());
	electoralSystems.push_back(make_shared<BaldwinMethod>());
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::StandardBorda));
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::BinaryNumberSystem));
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::Nauru));
	electoralSystems.push_back(make_shared<BordaCount>(BordaCountStrategy::Eurovision));
	electoralSystems.push_back(make_shared<BucklinVoting>());
	electoralSystems.push_back(make_shared<CombinedApprovalVoting>());
	electoralSystems.push_back(make_shared<CondorcetMethod>());
	electoralSystems.push_back(make_shared<ContigentVoting>());
	electoralSystems.push_back(make_shared<CoombsMethod>());
	electoralSystems.push_back(make_shared<CopelandMethod>());
	electoralSystems.push_back(make_shared<InstantRunoff>());
	electoralSystems.push_back(make_shared<JudgementMethod>(4,0,MedianRule::MajorityJudgement));
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::CentralJudgement));
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::TypicalJudgement));
	electoralSystems.push_back(make_shared<JudgementMethod>(4, 0, MedianRule::UsualJudgement));
	electoralSystems.push_back(make_shared<KemenyYoungMethod>());
	electoralSystems.push_back(make_shared<MajorityVote>());
	electoralSystems.push_back(make_shared<MiniMax>());
	electoralSystems.push_back(make_shared<NansonMethod>());
	electoralSystems.push_back(make_shared<OklahomaMethod>());
	electoralSystems.push_back(make_shared<PluralityVote>());
	electoralSystems.push_back(make_shared<PreferentialBlockVoting>());
	electoralSystems.push_back(make_shared<RankedPairs>());
	electoralSystems.push_back(make_shared<SchulzeMethod>());
	electoralSystems.push_back(make_shared<ScoreVoting>());
	electoralSystems.push_back(make_shared<StarVoting>());
	electoralSystems.push_back(make_shared<TidemanAlternativeSmith>());
	electoralSystems.push_back(make_shared<UnifiedPrimary>());

	return  electoralSystems;
}

int randomEnsembleElectoral(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);
	
	state->initialize(argc, argv);

	evalOp->electoralSystem = electoralSystem;
	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	vector<IndividualP> individuals;
	for (int i = 0; i < 50; i++) {
		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
	}

	ofstream myfile;
	string fileName = "out_" + std::to_string(ensembleSize) + ".txt";
	myfile.open(fileName);

	srand(time(NULL));

	vector<int> indices;

	for (int i = 0; i < 50; i++)
	{
		indices.push_back(i);
	}
		
	clock_t begin = clock();
	for (int r = 0; r < 30; r++) {

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		for (int k = 0; k < 1000; k++) {
			int counter = 1;
			vector<int> copiedIndices(indices);
			vector<int> selectedIndices;
			vector<IndividualP> ens;

			while (counter <= ensembleSize)
			{
				int randIndex = rand() % copiedIndices.size();
				int index = copiedIndices[randIndex];
				copiedIndices.erase(copiedIndices.begin() + randIndex);
				selectedIndices.push_back(index);
				counter++;
			}

			for (int i = 0; i < selectedIndices.size(); i++)
			{
				ens.push_back(individuals[selectedIndices[i]]);
			}

			ca->Evaluate(ens, ensembleSize);
			if (ens[0]->fitness->getValue() < bestFit)
			{
				bestFit = ens[0]->fitness->getValue();
				bestEnsembleIndices = vector<int>(selectedIndices);
				cout << bestFit << endl;
				for (int l = 0; l < bestEnsembleIndices.size(); l++)
				{
					cout << bestEnsembleIndices[l] << " ";
				}
				cout << endl;
			}
		}

		vector<IndividualP> ensemble;
		for (int i = 0; i < bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(individuals[bestEnsembleIndices[i]]);
		}
		ca->Evaluate(ensemble, ensembleSize);

		cout << "For run i=" << r << " the result was: " << ensemble[0]->fitness->getValue() << endl;
		myfile << r << " " << ensemble[0]->fitness->getValue() << " ";

		for (int l = 0; l < bestEnsembleIndices.size(); l++)
		{
			cout << bestEnsembleIndices[l] << " ";
			myfile << bestEnsembleIndices[l] << " ";
		}
		cout << endl;
		myfile << endl;
		myfile.flush();
	}
	myfile.close();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Elapsed ensemble " << elapsed_secs;

	ofstream duration;
	string fileNameTime = "out_" + std::to_string(ensembleSize) + "_time.txt";
	duration.open(fileNameTime);
	duration << elapsed_secs << endl;
	duration.close();

	return 0;
}


int randomEnsembleElectoralPreliminary(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);

	state->initialize(argc, argv);

	evalOp->electoralSystem = electoralSystem;
	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	vector<IndividualP> individuals;
	for (int i = 0; i < 50; i++) {
		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		individuals.push_back(ind);
	}

	ofstream myfile;
	string fileName = "out_" + std::to_string(ensembleSize) + ".txt";
	myfile.open(fileName);

	srand(3);

	vector<int> indices;

	for (int i = 0; i < 50; i++)
	{
		indices.push_back(i);
	}

	clock_t begin = clock();
	for (int r = 0; r < 10000; r++) {

		vector<int> bestEnsembleIndices;
		double bestFit = DBL_MAX;

		
		int counter = 1;
		vector<int> copiedIndices(indices);
		vector<int> selectedIndices;
		vector<IndividualP> ens;

		while (counter <= ensembleSize)
		{
			int randIndex = rand() % copiedIndices.size();
			int index = copiedIndices[randIndex];
			copiedIndices.erase(copiedIndices.begin() + randIndex);
			selectedIndices.push_back(index);
			counter++;
		}

		for (int i = 0; i < selectedIndices.size(); i++)
		{
			ens.push_back(individuals[selectedIndices[i]]);
		}

		ca->Evaluate(ens, ensembleSize);
		if (ens[0]->fitness->getValue() < bestFit)
		{
			bestFit = ens[0]->fitness->getValue();
			bestEnsembleIndices = vector<int>(selectedIndices);
			cout << bestFit << endl;
			for (int l = 0; l < bestEnsembleIndices.size(); l++)
			{
				cout << bestEnsembleIndices[l] << " ";
			}
			cout << endl;
		}
	

		vector<IndividualP> ensemble;
		for (int i = 0; i < bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(individuals[bestEnsembleIndices[i]]);
		}
		ca->Evaluate(ensemble, ensembleSize);

		cout << "For run i=" << r << " the result was: " << ensemble[0]->fitness->getValue() << endl;
		myfile << r << " " << ensemble[0]->fitness->getValue() << " ";

		for (int l = 0; l < bestEnsembleIndices.size(); l++)
		{
			cout << bestEnsembleIndices[l] << " ";
			myfile << bestEnsembleIndices[l] << " ";
		}
		cout << endl;
		myfile << endl;
		myfile.flush();
	}
	myfile.close();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Elapsed ensemble " << elapsed_secs;

	ofstream duration;
	string fileNameTime = "out_" + std::to_string(ensembleSize) + "_time.txt";
	duration.open(fileNameTime);
	duration << elapsed_secs << endl;
	duration.close();

	return 0;
}

int evaluateEnsembleElectoral(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);
	state->initialize(argc, argv);

	vector<IndividualP> individuals;

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	vector<double> fitnesses;
	double time = 0;

	for (int i = 0; i < 50; i++) {
		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		clock_t tStart = clock();
		ca->evaluate(ind);
		clock_t tEnd = clock();
		time += (double)(clock() - tStart) / CLOCKS_PER_SEC;
		fitnesses.push_back(ind->fitness->getValue());
		individuals.push_back(ind);
	}

	cout << "Individual DR eval time: " << time << endl;


	std::string line;
	std::ifstream nameFileout;

	nameFileout.open("out_" + to_string(ensembleSize) + ".txt");

	ofstream myfile;
	//ofstream occur;
	myfile.open("outEval_" + to_string(ensembleSize) + ".txt");
	//occur.open ("outoccur.txt");
	time = 0;

	auto electoralSystems = getElectoralSystemList();

	while (std::getline(nameFileout, line))
	{
		vector<string> splitted;
		vector<IndividualP> ensemble;

		//			std::getline(nameFileout, line);
		splitted = split(line, ' ');
		for (int i = 2; i < splitted.size(); i++)
		{
			ensemble.push_back(individuals[atoi(splitted[i].c_str())]);
		}
		
		evalOp->electoralSystem = electoralSystem;
		ca->Evaluate(ensemble, ensemble.size());

		cout << ensemble[0]->fitness->getValue() << " " << endl;
		myfile << ensemble[0]->fitness->getValue() << " " << endl;
	}

	cout << "Ensemble: " << time << endl;


	myfile.close();
	nameFileout.close();
	return 0;
}


int evaluateEnsembleElectoralAllSizes(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);
	state->initialize(argc, argv);

	vector<IndividualP> individuals;

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	vector<double> fitnesses;
	double time = 0;

	for (int i = 0; i < 50; i++) {
		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		clock_t tStart = clock();
		ca->evaluate(ind);
		clock_t tEnd = clock();
		time += (double)(clock() - tStart) / CLOCKS_PER_SEC;
		fitnesses.push_back(ind->fitness->getValue());
		individuals.push_back(ind);
	}

	cout << "Individual DR eval time: " << time << endl;


	std::string line;
	std::ifstream nameFileout;

	nameFileout.open("out_" + to_string(ensembleSize) + ".txt");

	ofstream myfile;
	//ofstream occur;
	
	//occur.open ("outoccur.txt");
	time = 0;

	auto electoralSystems = getElectoralSystemList();

	int index = 0;
	while (std::getline(nameFileout, line))
	{
		vector<string> splitted;
		vector<IndividualP> ensemble;
		myfile.open("outEval_" + to_string(index) + ".txt");
		index++;
		//			std::getline(nameFileout, line);
		splitted = split(line, ' ');
		for (int i = 2; i < splitted.size(); i++)
		{
			ensemble.push_back(individuals[atoi(splitted[i].c_str())]);

			evalOp->electoralSystem = electoralSystem;
			ca->Evaluate(ensemble, ensemble.size());

			cout << ensemble[0]->fitness->getValue() << " " << endl;
			myfile << ensemble[0]->fitness->getValue() << " " << endl;
		}
		myfile.close();

	}

	cout << "Ensemble: " << time << endl;


	nameFileout.close();
	return 0;
}


int ElectoralEnsembleConstruction(int argc, char** argv, int ensembleSize, shared_ptr <ElectoralSystem> electoralSystem)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);

	state->initialize(argc, argv);

	evalOp->electoralSystem = make_shared<SumMethod>();
	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	vector<vector<double>> fitnesses;
	for(int i = 0;i<100;i++)
	{
		fitnesses.push_back(vector<double>(50));
	}


	vector<IndividualP> individuals;
	for (int i = 0; i < 50; i++) {
		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		ca->evaluate(ind);
		individuals.push_back(ind);
		
		vector<double> indFit;
		indFit.push_back(ind->fitness->getValue());
		for (int j = 0; j < 100; j++)
		{
			fitnesses[j][i] = evalOp->Fitness[j][Twt];
		}
	}
	shared_ptr<WinnerStrategy> winnerStrategy = make_shared<LowestPriorityStrategy>();

	ofstream myfile;
	string fileName = "out_" + std::to_string(ensembleSize) + ".txt";
	myfile.open(fileName, std::ios_base::app);

	srand(time(NULL));

	vector<int> indices;

	for (int i = 0; i < 50; i++)
	{
		indices.push_back(i);
	}
	vector<int> bestEnsembleIndices;

	clock_t begin = clock();
	for(int i = 0; i<ensembleSize; i++)
	{
		int elected = electoralSystem->elect(fitnesses, winnerStrategy);
		int index = indices[elected];
		bestEnsembleIndices.push_back(index);
		indices.erase(indices.begin() + elected);
		//cout << indices.size() << endl;
		for(int j = 0;j<100;j++)
		{
			fitnesses[j].erase(fitnesses[j].begin() + elected);
		}
	}
	clock_t end = clock();

	vector<IndividualP> ensemble;
		
		for (int i = 0; i < bestEnsembleIndices.size(); i++)
		{
			ensemble.push_back(individuals[bestEnsembleIndices[i]]);
		}
		ca->Evaluate(ensemble, ensembleSize);

		cout << "For run i=" <<  " the result was: " << ensemble[0]->fitness->getValue() << endl;
		myfile << " " << ensemble[0]->fitness->getValue() << " ";

		for (int l = 0; l < bestEnsembleIndices.size(); l++)
		{
			cout << bestEnsembleIndices[l] << " ";
			myfile << bestEnsembleIndices[l] << " ";
		}
		cout << endl;
		myfile << endl;
		myfile.flush();
	
	myfile.close();
	
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Elapsed ensemble " << elapsed_secs;

	ofstream duration;
	string fileNameTime = "out_" + std::to_string(ensembleSize) + "_time.txt";
	duration.open(fileNameTime);
	//duration << elapsed_secs << endl;
	duration.close();

	return 0;
}

void runMethod(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	std::vector<int> ensembleSizes{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30,35,40,45};
	for (int ensemble_size : ensembleSizes)
	//for(int ensemble_size = 2; ensemble_size<=50;ensemble_size++)
	{
		cout << "************************************************************************************" << endl;
		cout << "running for esnemble size " << ensemble_size << endl;
		cout << "************************************************************************************" << endl;
		randomEnsembleElectoral(argc, argv, ensemble_size, electoralSystem);
	}
}

void runPreliminaryMethod(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	std::vector<int> ensembleSizes{ 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30,35,40,45 };
	for (int ensemble_size : ensembleSizes)
		//for(int ensemble_size = 2; ensemble_size<=50;ensemble_size++)
	{
		cout << "************************************************************************************" << endl;
		cout << "running for esnemble size " << ensemble_size << endl;
		cout << "************************************************************************************" << endl;
		randomEnsembleElectoralPreliminary(argc, argv, ensemble_size, electoralSystem);
	}
}

void evaluateMethod(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	std::vector<int> ensembleSizes{ 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30, 35, 40, 45};
	for (int ensemble_size : ensembleSizes)
	{
		cout << "************************************************************************************" << endl;
		cout << "running for esnemble size " << ensemble_size << endl;
		cout << "************************************************************************************" << endl;
		evaluateEnsembleElectoral(argc, argv, ensemble_size, electoralSystem);
	}
}

void evaluateAllSizesMethod(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	
		evaluateEnsembleElectoralAllSizes(argc, argv, 45, electoralSystem);

}

void runElectoralSystemConstructionMethod(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	std::vector<int> ensembleSizes{ 45 };
	for(auto electoralSystem : getElectoralSystemList())
	{
		for (int ensemble_size : ensembleSizes)
		{
			cout << "************************************************************************************" << endl;
			cout << "running for esnemble size " << ensemble_size << endl;
			cout << "************************************************************************************" << endl;
			clock_t begin = clock();
			ElectoralEnsembleConstruction(argc, argv, ensemble_size, electoralSystem);
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			cout << "Elapsed total ensemble " << elapsed_secs;
		}
	}
	
}
void runElectoralSystemConstructionMethodGrow(int argc, char** argv)
{
	auto electoralSystem = createElectoralSystem();
	std::vector<int> ensembleSizes{ 45, 40, 35, 30, 25 };
	//for (auto electoralSystem : getElectoralSystemList())
	{
		for (int ensemble_size : ensembleSizes)
		{
			cout << "************************************************************************************" << endl;
			cout << "running for esnemble size " << ensemble_size << endl;
			cout << "************************************************************************************" << endl;
			clock_t begin = clock();
			grow_destroy_reverse(argc, argv, ensemble_size, electoralSystem);
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			cout << "Elapsed total ensemble " << elapsed_secs;
		}
	}

}

int printMatrix(int argc, char** argv)
{
	StateP state(new State);
	state->addAlgorithm(CustomAlgorithmP(new CustomAlgorithm));
	SchedulingEvalOpP evalOp = static_cast<SchedulingEvalOpP> (new SchedulingEvalOp);
	state->setEvalOp(evalOp);

	TreeP tree(new Tree::Tree);

	state->initialize(argc, argv);



	vector<IndividualP> individuals;
	//read individuals

	AlgorithmP alg = state->getAlgorithm();
	CustomAlgorithm* ca = (CustomAlgorithm*)alg.get();

	vector<vector<double>> fitnesses;
	vector<double> indFit;


	//evalOp->combinationMethod = argv[3];
	ofstream out("./matrix.txt");

	for (int i = 0; i < 50; i++) {
		std::ostringstream stringStream;
		stringStream << "./ind" << i << ".txt";
		XMLNode xInd = XMLNode::parseFile(stringStream.str().data(), "Individual");
		IndividualP ind = (IndividualP) new Individual(state);
		ind->read(xInd);
		ca->evaluate(ind);
		individuals.push_back(ind);
		fitnesses.push_back(vector<double>());
		indFit.push_back(ind->fitness->getValue());
		for (int j = 0; j < 100; j++)
		{
			fitnesses[i].push_back(evalOp->Fitness[j][Twt]);
			out << evalOp->Fitness[j][Twt] << " ";
		}
		out << endl;
		cout << ind->fitness->getValue() << endl;
	}

	out.close();

	return 0;
}

int main(int argc, char **argv){

	printMatrix(argc, argv);
	//runElectoralSystemConstructionMethodGrow(argc, argv );
	//runPreliminaryMethod(argc, argv);
	//evaluateAllSizesMethod(argc, argv);
	//runElectoralSystemConstructionMethod(argc, argv);
	//runMethod(argc, argv);
	//evaluateMethod(argc, argv);
	return 0;


	//evaluateAllGenerations(argc, argv);
	//evolveGp(argc, argv);
	//return 0;

	//std::vector<double> A { 1, 2, 5, 2, 1, 25, 2 };
	//
	//for (int i = 0; i < A.size(); i++)
	//	cout << A[i] << ' ';
	//cout << '\n';

	//shared_ptr<WinnerStrategy> winnerStrategy = make_shared<LowestPriorityStrategy>();

	//auto res= ElectoralSystemUtils::calculateRanksUnique(A, winnerStrategy);

	//for (int i = 0; i < res.size(); i++)
	//	cout << res[i] << ' ';
	//cout << '\n';

	//return 0;

	//runTests();
	//return 0;

	vector<vector<double>> priorities{
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},
{1,2,4,3},
{1,2,4,3},{1,2,4,3},
{1,2,4,3},


		{4,1,3,2},
		{4,1,3,2},
		{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},
{4,1,3,2},
		{4,1,3,2},


		{4,3,2,1},
		{4,3,2,1},
		{4,3,2,1},
		{4,3,2,1},
{4,3,2,1},
		{4,3,2,1},
{4,3,2,1},
		{4,3,2,1},
{4,3,2,1},
		{4,3,2,1},
{4,3,2,1},
		{4,3,2,1},
{4,3,2,1},
		{4,3,2,1},
{4,3,2,1},
		


		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},
		{4,3,1,2},

	};

	vector<vector<double>> priorities2{
{2,4,3,1},
{2,4,3,1},
{2,4,3,1},
{2,4,3,1},
{2,4,3,1},
{2,4,3,1},
{2,4,3,1},



{3,2,4,1},
{3,2,4,1},

{1,2,3,4},
{1,2,3,4},
{1,2,3,4},
{1,2,3,4},


{1,4,2, 3},
{1,4,2, 3},
{1,4,2, 3},
{1,4,2, 3},
{1,4,2, 3},


{3,1,4,2},

{4,1,2,3},
{4,1,2,3},
{4,1,2,3},
{4,1,2,3},
{4,1,2,3},
{4,1,2,3},
{4,1,2,3},
{4,1,2,3}

	};

	vector<vector<double>> priorities3{
{1,3,2,5,4},
{1,3,2,5,4},
{1,3,2,5,4},
{1,3,2,5,4},
{1,3,2,5,4},

{1,5,4,2,3},
{1,5,4,2,3},
{1,5,4,2,3},
{1,5,4,2,3},
{1,5,4,2,3},


{4,1,5,3,2},
{4,1,5,3,2},
{4,1,5,3,2},
{4,1,5,3,2},
{4,1,5,3,2},
{4,1,5,3,2},
{4,1,5,3,2},
{4,1,5,3,2},


{2,3,1,5,4},
{2,3,1,5,4},
{2,3,1,5,4},


{2,4,1,5,3},
{2,4,1,5,3},
{2,4,1,5,3},
{2,4,1,5,3},
{2,4,1,5,3},
{2,4,1,5,3},
{2,4,1,5,3},


{3,2,1,4,5},
{3,2,1,4,5},

{5,4,2,1,3},
{5,4,2,1,3},
{5,4,2,1,3},
{5,4,2,1,3},
{5,4,2,1,3},
{5,4,2,1,3},
{5,4,2,1,3},


{3,2,5,4,1},
{3,2,5,4,1},
{3,2,5,4,1},
{3,2,5,4,1},
{3,2,5,4,1},
{3,2,5,4,1},
{3,2,5,4,1},
{3,2,5,4,1},


	};



	vector<vector<double>> priorities4{
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},
		{1, 5, 3, 4, 2},

		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},
		{2, 1, 4, 5, 3},

		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},
		{2, 1, 5, 4, 3},

		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},
		{4, 3, 1, 2, 5},

		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},
		{5, 3, 1, 2, 4},

		{2, 4, 5, 1, 3},
		{2, 4, 5, 1, 3},
		{2, 4, 5, 1, 3},
		{2, 4, 5, 1, 3},
		{2, 4, 5, 1, 3},

		{2, 5, 4, 1, 3},
		{2, 5, 4, 1, 3},
		{ 2, 5, 4, 1, 3 },
		{ 2, 5, 4, 1, 3 },
		{ 2, 5, 4, 1, 3 },

	};


	shared_ptr<WinnerStrategy> winnerStrategy = make_shared<LowestPriorityStrategy>();
	
	CombinedApprovalVoting coombs(0.5, 0.5, false);
	auto res = coombs.elect(priorities, winnerStrategy);
	cout << res << endl;

	//return 0;

	EnsembleParams params = initEnsembleParams();
	
	if(params.method == "eval"){
		evalEnsemble(argc, argv, params);	
	} else if(params.method == "ga"){
		evalGA(argc, argv, params);	
	} else if(params.method == "random"){
		random(argc, argv, params);
	} else if(params.method == "grow"){
		//grow(argc, argv, params);
	} else if(params.method == "growd"){
		//grow_destroy(argc, argv, params);
	} else if(params.method == "probabilistic"){
		probabilistic(argc, argv, params);
	} else if(params.method == "instance"){
		//instanceBased(argc, argv, params);
	} else if (params.method == "ind"){
		runInd(argc, argv);
	} else if(params.method == "randomSize")
	{
		random_varsize(argc, argv, params);
	}

	//evolveGpEnsemble(argc,argv);
	//evalEnsemble(argc, argv);
	//testEval(argc, argv);
	//random(argc, argv);
	//main232(argc, argv);
	return 0;
}
