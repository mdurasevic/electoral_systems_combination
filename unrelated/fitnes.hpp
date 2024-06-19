#ifndef SchedulingEvalOp_h
#define SchedulingEvalOp_h

#include <string>
#include <vector>

#include "ElectoralSystem.h"
#include "readpar.h"
#include "matrice.h"
#include "rpn.h"





class SchedulingEvalOp : public EvaluateOp 
{
public:
	SchedulingEvalOp();
	FitnessP evaluate(IndividualP individual);
	FitnessP evaluate(vector<IndividualP> individuals);
	void registerParameters(StateP);
	bool initialize(StateP);
	~SchedulingEvalOp();
	bool ff;
	unsigned int edited,total,nItems;
	RPN Evaluator;
	void write(std::string &output);
	void ReadTerminals(TreeP);
	void ReadTerminals(StateP);
	void DefineNodeNames(void);
	void ReadConstraints(Matrica &Constraints, int set, int jobs, Matrica &Precedence);
	void MakeSetup(Matrica &Duration, int set, int jobs, double faktor, Matrica &Setup);
	double NodeLevel(int set, int node);
	//void StartTerminalStatistic(Beagle::GP::Context& ioContext, double dSubsetSize=0);
	//void GetTerminalStatistic(double *pValues);
	//void UpdateTerminalStatistic(double &dFitness);
	void ReadIndividual(IndividualP individual);
	void ReadIndividuals(vector<IndividualP> individuals);
	void CalcTimedTerminals(uint &nNiz, uint &nPoslova, uint &nJob, double &dClock, uint nMachine=0, uint nMachines=1);
	void CalcTimedTerminals(uint& nNiz, uint& nPoslova, uint& nJob, double& dClock, uint nMachine, uint nMachines, double endTime);
	void CalcTimedTerminals(uint& nNiz, uint& nPoslova, uint& nJob, double& dClock, uint nMachine, uint nMachines, Matrica& MachineReady);
	void CalcStaticTerminals(uint& nNiz, uint& nPoslova, uint& nJob, double& dClock, uint& nMachine, uint& nMachines, double endTime, double trueEnd);
	bool isShortestExecutingJob(uint& nNiz, uint& nMachine, uint& index);
	//void CalcStaticTerminals(uint& nNiz, uint& nPoslova, uint& nJob, double& dClock, uint nMachine, uint nMachines, Matrica MachineReady, double endTime);
	
	void EvaluateSingle(double &dRawFitness);
	void EvaluateSingleVote(double &dRawFitness);
	void EvaluateSingleSum(double &dRawFitness);

	void EvaluateUniform(double &dRawFitness);
	void EvaluateUniformVote(double &dRawFitness);
	void EvaluateUniformSum(double &dRawFitness);

	void EvaluateUnrelated(double &dRawFitness);
	void EvaluateUnrelatedStatic(double& dRawFitness);
	void EvaluateUnrelatedIterativeDR(double& dRawFitness);
	void EvaluateUnrelatedIterativeDRSt(double& dRawFitness);
	void copyRow(Matrica& M1, Matrica& M2, int row, int numEl);
	void copyMachineReady(Matrica& M1, Matrica& M2, unsigned int row);
	void EvaluateUnrelatedRollout(double& dRawFitness);
	void EvaluateUnrelatedRolloutH(double& dRawFitness, unsigned nNiz, double lookahead, int& largestReleaseTime, Matrica& Schedule, Matrica& MachineReady, unsigned& prvi);
	void EvaluateUnrelatedRolloutIterative(double& dRawFitness);
	void EvaluateUnrelatedRolloutHIterative(double& dRawFitness, unsigned nNiz, double lookahead, int& largestReleaseTime, Matrica& Schedule, Matrica& MachineReady, unsigned& prvi);
	void EvaluateUnrelated2(double &dRawFitness);
	void EvaluateUnrelatedElectionMethods (double& dRawFitness);
	void EvaluateUnrelatedElectionMethodsV2(double& dRawFitness);
	void EvaluateUnrelated2Confidences(double &dRawFitness);
	void EvaluateUnrelatedBoosting(double &dRawFitness);
	void EvaluateUnrelatedWeightedvoting(double &dRawFitness);
	void EvaluateUnrelatedSumOfPriorities(double &dRawFitness);
	void EvaluateUnrelatedSumOfPrioritiesConfidences(double &dRawFitness);
	void EvaluateUnrelatedWeightedSumOfPriorities(double &dRawFitness);
	void EvaluateUnrelatedFP(FloatingPointP fp, double &dRawFitness);
	void EvaluateJobShop(double &dRawFitness);
	void EvaluateJobShopSum(double &dRawFitness);
	void EvaluateJobShopVote(double &dRawFitness);
	void EvaluateEnsembleCollaboration(double &dRawFitness);

	void SetElectoralSystem(shared_ptr<ElectoralSystem> electoralSystem);


	int Before(const void *arg1, const void *arg2);
	std::string in_file;
		//Marko: broj ensembleova
	int ensenbleCount;
public:
	int nJobsCollab;
enum environment
{	SINGLE,
	UNIFORM,
	UNRELATED,
	JOBSHOP
};

	unsigned int sets,max_jobs,total_jobs,max_machines,max_length;
	unsigned int m_fitness;	// koja fitnes funkcija? definicije u nodes.h
	unsigned int m_genotip; // koji genotip (0 - FP, 1 - GP)
	unsigned int m_primjer; // koji ispitni primjer (iz fitness.txt)
	unsigned int m_BestSubset;	// koliko najboljih jedinki gledamo za statistiku terminala
	unsigned int m_SubsetSize;	// vlastita proracunata velicina (default vrijednost)
	unsigned int m_InSubset;		// koliko ih trenutno ima u matrici
	unsigned int m_WorstInSubset;	// koji je najlosiji
	double m_WorstSubsetFitness;	// fitnes najlosije jedinke u podskupu najboljih
	double m_BestSubsetFitness;	// treba li komentar?
	environment m_Environment;	// okruzenje: single, uniform, unrelated, job shop
	unsigned int m_SortSet;		// oznaka skupa za potrebe qsort-a
	unsigned int m_PopSize;		// ukupna velicina populacije



	bool m_Normalized;		// je li fitnes normiran brojem i trajanjem poslova
	bool m_Evaluation;		// pisanje rezultata za svaki skup u datoteku
	bool m_TermUsage;			// prikupljanje statistike koristenja terminala
	bool m_editing;			// editiranje?
	bool m_LEF;				// limited error fitness (1 ili 0)
	double m_LEFVal;			// vrijednost za LEF
	bool m_setup;				// postoje li trajanja postavljanja izmedyu poslova
	double m_setup_faktor;	// prosjecni odnos izmedju trajanja postavljanja i trajanja posla
	bool m_dynamic;			// dinamicka okolina?
	bool m_constrained;		// ogranicenja u rasporedu?
	bool m_stsampling;		// stochastic sampling?
	double m_sampling;		// koliki postotak test skupova se uzima
	bool *pSamples;			// koji tocno skupovi idu
	bool m_Idleness;			// uzimamo li u obzir i cekanje operacija (job shop)
	bool *pRasporedjen;
	double *pVrijednosti, *pVrijednosti1, *pVrijednosti2, *pArray, *pSlack, *pSlackSpeed, *pArrival, *pLevel, *pSetupAvg;
	double *pOperationReady, *pJobReady, *pTotalWorkRemaining, *pTotalWorkDone, *pTotalMachineWork;
	double *pMachineWorkRemaining, *pMachineValues;
	unsigned int *pIndex, *pUsed, *pLastJob, *pPTimeMinMachine, *pMachineScheduled, *pOperationsScheduled;
	unsigned int *pOperationsWaiting;
	Matrica N,Duration,Deadline,WeightT,WeightF,WeightE,WeightN,Ready,Constraints,Machines,Speed;
	Matrica SP,SD,Fitness,Schedule,Precedence,Level,Setup,Terminals,MachineReady,PTimeAvg,PTimeMinMachine;
	Matrica SortedReady, Values, Values1, Values2, Durations, MachineIndex;
	Matrica ValuesArray[50];
	double *pVrijednostiArray[50];
	Matrica shortestExecutingJob;

	Matrica newMachineReady, newSchedule;
	Matrica previousSchedule, previousMachineReady;

	vector<IndividualP> individuals;
	vector<int> occur;

	bool firstTime;
	bool initialised;
	string method;
	


	int bagSize;
	int *bagIndices;


	int count[60];

	double bestislate[100];
	double bestindLate[100];
	double bestjobfinish[100];
	double bestindwtard[100];
	double bestindtard[100];
	double bestflow[100];
	double bestLateness;
	double bestTardiness;
	double bestNLateJobs;

	double lbestislate[100];
	double lbestindLate[100];
	double lbestjobfinish[100];
	double lbestindwtard[100];
	double lbestindtard[100];
	double lbestflow[100];
	double lbestLateness;
	double lbestTardiness;
	double lbestNLateJobs;

	double llbestislate[100];
	double llbestindLate[100];
	double llbestjobfinish[100];
	double llbestindwtard[100];
	double llbestindtard[100];
	double llbestflow[100];
	double llbestLateness;
	double llbestTardiness;
	double llbestNLateJobs;

	double lllbestislate[100];
	double lllbestindLate[100];
	double lllbestjobfinish[100];
	double lllbestindwtard[100];
	double lllbestindtard[100];
	double lllbestflow[100];
	double lllbestLateness;
	double lllbestTardiness;
	double lllbestNLateJobs;

	double bestOverall;
	double bestbestbest;
	int bestover;

	public:
		int first;
		double confidences[10];
			double lookaheadFactor, rolloutFactor;
	int lookaheadJobs, rolloutJobs;
	bool useLookaheadFactor, useRolloutFactor;

	int ruleUsed;
	int cClock;


	shared_ptr<ElectoralSystem> electoralSystem;

};
typedef boost::shared_ptr<SchedulingEvalOp> SchedulingEvalOpP;

#endif // SchedulingEvalOp_h
