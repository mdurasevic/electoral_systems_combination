	/*
Radi za:
SINGLE okruzenje:
- staticke i dinamicke dolaske poslova
- sa i bez ogranicenja u rasporedu
- trajanja postavljanja za staticki i dinamicki slucaj
UNIFORM okruzenje:
- staticke i dinamicke dolaske poslova
- trajanja postavljanja za staticki i dinamicki slucaj
treba napraviti:
- promijeniti racunanje SPr ! (otkomentirati)
UNRELATED okruzenje:
- dinamicki uvjeti rada (simulacija online schedulinga)
JOBSHOP okruzenje:
- staticki dolasci poslova
- sa ili bez idle time
*/

#include <ecf/ECF.h>
#include "fitnes.hpp"
#include <cmath>
#include <time.h>

// globalna varijabla - koriste je svi zivi...
node Nodes[TOTAL_NODES];

// makroi za uvjetnu provjeru
#define CHECKMSG(condition, text) \
if(!(condition)) {fprintf(stderr,"file: " __FILE__ "\nline: %d\nmsg:  " text "\n",__LINE__); exit(1);}
#define CHECK(condition) \
if(!(condition)) {fprintf(stderr,"Nesto ne valja!\nfile: " __FILE__ "\nline: %d\n" ,__LINE__); exit(1);}
// fja max{x,0}
#define POS(x)		(x>0 ? x : 0)
#define MIN(x,y)	(x<y ? x : y)
#define MAX(x,y)	(x>y ? x : y)


// fja za uporabu qsort-a
double *pVal;
int Before(const void *arg1, const void *arg2)
{
	if(pVal[*(uint*)arg1] < pVal[*(uint*)arg2])
		return -1;
	else if(pVal[*(uint*)arg1] > pVal[*(uint*)arg2])
		return 1;
	return 0;
}




// definira imena terminala i funkcija; takodjer postavlja aktivne funkcije
SchedulingEvalOp::SchedulingEvalOp()
{
	for(int i=0; i<TERMINALS+OFFSET; i++)
	{	Nodes[i].active = false;
	}
	for(int i=TERMINALS+OFFSET; i<TOTAL_NODES; i++)
	{	Nodes[i].active = true;
	}
	// definiramo imena mogucih funkcija i terminala
	Nodes[NUL].name = "0";
	Nodes[NUL].value = 0;
	Nodes[ONE].name = "1";
	Nodes[ONE].value = 1;
	Nodes[T_N].name = "N";
	Nodes[T_SP].name = "SP";
	Nodes[T_SD].name = "SD";
	Nodes[T_pt].name = "pt";
	Nodes[T_dd].name = "dd";
	Nodes[T_w].name = "w";
	Nodes[T_Nr].name = "Nr";
	Nodes[T_SPr].name = "SPr";
	Nodes[T_SL].name = "SL";
	Nodes[T_AR].name = "AR";
	Nodes[T_SC].name = "SC";
	Nodes[T_LVL].name = "LVL";
	Nodes[T_STP].name = "STP";
	Nodes[T_Sav].name = "Sav";
	Nodes[T_SLs].name = "SLs";
	Nodes[T_SPD].name = "SPD";
	Nodes[T_Msm].name = "Msm";
	Nodes[T_pmin].name = "pmin";
	Nodes[T_pavg].name = "pavg";
	Nodes[T_PAT].name = "PAT";
	Nodes[T_MR].name = "MR";
	Nodes[T_age].name = "age";
	Nodes[T_L].name = "L";
	Nodes[T_SLr].name = "SLr";
	Nodes[T_CLK].name = "CLK";
	Nodes[T_NOPr].name = "NOPr";
	Nodes[T_TWK].name = "TWK";
	Nodes[T_TWKr].name = "TWKR";
	Nodes[T_PTav].name = "PTav";
	Nodes[T_HTR].name = "HTR";
	Nodes[T_TF].name = "TF";
	//staticni terminali

	// terminali za strojeve
	Nodes[T_MNOPr].name = "MNOPr";
	Nodes[T_MNOPw].name = "MNOPw";
	Nodes[T_MTWK].name = "MTWK";
	Nodes[T_MTWKr].name = "MTWKr";
	Nodes[T_MTWKav].name = "MTWKav";
	Nodes[T_MUTL].name = "MUTL";

	//staticki terminali
	Nodes[T_NSHORT].name = "NSHORT";
	Nodes[T_SLNEXT].name = "SLNEXT";
	Nodes[T_SLNEXTM].name = "SLNEXTM";
	Nodes[T_TTAR].name = "TTAR";
	Nodes[T_TTARM].name = "TTARM";
	Nodes[T_MLOAD].name = "MLOAD";
	Nodes[T_FUTLATE].name = "FUTLATE";
	Nodes[T_NREL].name = "NREL";
	Nodes[T_NRELM].name = "NRELM";
	Nodes[T_WLATE].name = "WLATE";
	Nodes[T_FLD].name = "FLD";
	Nodes[T_WLD].name = "WLD";
	Nodes[T_SLAVGM].name = "SLAVGM";
	Nodes[T_SLAVGD].name = "SLAVGD";
	Nodes[T_MLOADD].name = "MLOADD";
	Nodes[T_FLDS].name = "FLDS";
	Nodes[T_WLDS].name = "WLDS";
	Nodes[T_FLDL].name = "FLDL";
	Nodes[T_WLDL].name = "WLDL";

	// funkcije
	Nodes[ADD].name = "+";
	Nodes[SUB].name = "-";
	Nodes[MUL].name = "*";
	Nodes[DIV].name = "/";
	Nodes[POS].name = "pos";
	Nodes[SQR].name = "sqr";
	Nodes[IFGT].name = "ifgt";
	Nodes[IFLT].name = "iflt";
	Nodes[MAX].name = "max";
	Nodes[MIN].name = "min";
	Nodes[AVG].name = "avg";
	Nodes[ABS].name = "abs";
	// default parametri
	m_Idleness = false;		// po defaultu ne cekamo u job shop okruzenju
	m_TermUsage = false;	// iskljucena statistika koristenja terminala
	m_BestSubset = 100;		// po defaultu gledamo 100 najboljih
	m_LEF = 0;	// nema rezanja fitnesa po defaultu
	m_editing = false;	// niti editiranja
	m_Evaluation = false;	// pisanje detaljnih rezultata u fajl je iskljuceno
	Evaluator.SetExprSize(2000);	// postavimo max velicinu izraza... :)
	edited = 0;	// koliko cvorova je editirano
	total = 0;	// koliko cvorova je ukupno bilo

	ff = false;
}


SchedulingEvalOp::~SchedulingEvalOp()
{
	delete [] pRasporedjen;
	delete [] pVrijednosti;
	delete [] pIndex;
	delete [] pUsed;
	delete [] pArray;
	delete [] pSlack;
	delete [] pSlackSpeed;
	delete [] pSamples;
	delete [] pArrival;
	delete [] pLevel;
	delete [] pSetupAvg;
	delete [] pLastJob;
	delete [] pMachineScheduled;
	delete [] pOperationReady;
	delete [] pJobReady;
	delete [] pOperationsScheduled;
	delete [] pTotalWorkRemaining;
	delete [] pTotalWorkDone;
	delete [] pTotalMachineWork;
	delete [] pMachineWorkRemaining;
	delete [] pOperationsWaiting;
	delete [] pMachineValues;
}


void SchedulingEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("test_cases", (voidP) (new std::string), ECF::STRING);
	state->getRegistry()->registerEntry("normalized", (voidP) (new uint(1)), ECF::UINT);
	state->getRegistry()->registerEntry("genotip", (voidP) (new uint(1)), ECF::UINT);
	state->getRegistry()->registerEntry("primjer", (voidP) (new uint(0)), ECF::UINT);
}


// inicijalizacija
// zove se nakon ucitavanja GP parametara, prije pocetka evolucije
bool SchedulingEvalOp::initialize(StateP state)
{
	std::string configFile;

	// check if the parameters are defined in the conf. file
	if(!state->getRegistry()->isModified("test_cases"))
		return false;

	voidP sptr = state->getRegistry()->getEntry("test_cases"); // get parameter value
	configFile = *((std::string*) sptr.get()); // convert from voidP to user defined type
	in_file = configFile;

	sptr = state->getRegistry()->getEntry("normalized"); // get parameter value
	m_Normalized = (bool) *((uint*) sptr.get()); // convert from voidP to user defined type

	sptr = state->getRegistry()->getEntry("genotip"); // get parameter value
	m_genotip = (uint) *((uint*) sptr.get()); // convert from voidP to user defined type

	sptr = state->getRegistry()->getEntry("primjer"); // get parameter value
	m_primjer = (uint) *((uint*) sptr.get()); // convert from voidP to user defined type

	if(m_primjer > 60) {
		m_primjer = *((uint*) state->getContext()->environment);
	}

	if(m_genotip == 1)	// ako koristimo GP
		// procitaj terminale iz registrija
		ReadTerminals(state);

// originalni dio iz BEAGLE implementacije:
	std::string input,sp,duration,deadline,weightT,weightF,weightE,weightN,term,ready,cons,speed;
	char pom[256];
	ReadPar p;
	unsigned int i,j;
	double d_niz[2][1000];

	//// procitajmo ukupnu velicinu populacije i odredimo velicinu BestSubset-a
	//// a moze biti ucitan i iz datoteke - nize dolje
	//IntegerVector::Handle hPopSize;
	//hPopSize = castHandleT<IntegerVector>(ioSystem.getRegister().getEntry("ec.pop.size"));
	//unsigned int nDemes = (unsigned int) (*hPopSize).size();
	//m_PopSize = 0;
	//for(i=0; i<nDemes; i++)
	//	m_PopSize += (*hPopSize)[i];
	//// broj 'najboljih' jedinki za brojanje terminala = korijen od populacije
	////m_SubsetSize = 4 * (uint) sqrt((float)m_PopSize);
	//m_SubsetSize = (uint) (0.1 * (float)m_PopSize) + 1;

	// inicijalizacija default vrijednosti
	input = configFile;	// glavni ulazni fajl, mora ga biti

	// ucitavanje parametara
	p.OpenFile(input.c_str());
	if(p.ReadParameter("single",ReadPar::INTEGER,&i))
		m_Environment = SINGLE;
	else if(p.ReadParameter("uniform",ReadPar::INTEGER,&i))
		m_Environment = UNIFORM;
	else if(p.ReadParameter("unrelated",ReadPar::INTEGER,&i))
		m_Environment = UNRELATED;
	else if(p.ReadParameter("jobshop",ReadPar::INTEGER,&i))
		m_Environment = JOBSHOP;
	p.ReadParameter("sets",ReadPar::INTEGER,&sets);
	p.ReadParameter("max_jobs",ReadPar::INTEGER,&max_jobs);
	if(!p.ReadParameter("max_machines",ReadPar::INTEGER,&max_machines))
		max_machines = 1;
	p.ReadParameter("max_length",ReadPar::INTEGER,&max_length);
	p.ReadParameter("duration",ReadPar::STRING,pom); duration = pom;
	p.ReadParameter("deadline",ReadPar::STRING,pom); deadline = pom;
	p.ReadParameter("weight_T",ReadPar::STRING,pom); weightT = pom;
	p.ReadParameter("weight_F",ReadPar::STRING,pom); weightF = pom;
	p.ReadParameter("weight_E",ReadPar::STRING,pom); weightE = pom;
	p.ReadParameter("weight_N",ReadPar::STRING,pom); weightN = pom;
	p.ReadParameter("SP",ReadPar::STRING,pom); sp = pom;
	p.ReadParameter("machine_file",ReadPar::STRING,pom); speed = pom;
	// dinamicki dolasci poslova
	if(p.ReadParameter("ready",ReadPar::STRING,pom))
	{	ready = pom;
		m_dynamic = true;
	}
	else
		m_dynamic = false;
	// limited error fitness izracunavanje
	if(p.ReadParameter("LEF",ReadPar::INTEGER,&i))
	{	if(i==1)
		{	m_LEF = true;
			if(!p.ReadParameter("LEF_value",ReadPar::DOUBLE,&m_LEFVal))
				CHECKMSG(0,"LEF vrijednost nije zadana!");
		}
	}
	// evaluacija - ispis rjesenja za svaku jedinku
	if(p.ReadParameter("evaluation",ReadPar::INTEGER,&i))
		if(i==1) m_Evaluation = true;
	// fitness - mora biti definiran
	if(p.ReadParameter("fitness",ReadPar::STRING,pom))
	{	input = pom;
		if(input == "Twt")
			m_fitness = Twt;
		else if(input == "Nwt")
			m_fitness = Nwt;
		else if(input == "FTwt")
			m_fitness = FTwt;
		else if(input == "ETwt")
			m_fitness = ETwt;
		else if(input == "Cmax")
			m_fitness = Cmax;
		else if(input == "Fwt")
			m_fitness = Fwt;
		else if(input == "TwtCmax")
			m_fitness = TwtCmax;
		else if(input == "NwtCmax")
			m_fitness = NwtCmax;
		else
			CHECKMSG(0,"Nepoznata fitnes funkcija!");
	}
	else CHECKMSG(0,"Nije definirana fitnes funkcija!");
	// editiranje jedinke
	if(p.ReadParameter("editing",ReadPar::INTEGER,&i))
		if(i==1) m_editing = true;
	// stochastic sampling, koliko
	if(p.ReadParameter("stsampling",ReadPar::DOUBLE,&m_sampling))
		m_stsampling = true;
	else
		m_stsampling = false;
	// ogranicenja u rasporedu
	if(p.ReadParameter("constraints",ReadPar::STRING,pom))
	{	cons = pom;
		m_constrained = true;
	}
	else
		m_constrained = false;
	// trajanja postavljanja
	if(p.ReadParameter("setup",ReadPar::DOUBLE,&m_setup_faktor))
		m_setup = true;
	else
		m_setup = false;
	// eventualno definiranje podskupa jedinki za brojanje terminala
	p.ReadParameter("bestsubset",ReadPar::INTEGER,&m_BestSubset);
	if(p.ReadParameter("idleness",ReadPar::INTEGER, &i))
		if(i == 1)	m_Idleness = true;

	N.Init(sets);
	SP.Init(sets);
	SD.Init(sets);
	Machines.Init(sets);
	MachineReady.Init(max_machines);
	Speed.Init(sets,max_jobs);
	Duration.Init(sets,max_jobs);
	Deadline.Init(sets,max_jobs);
	Durations.Init(max_jobs, max_machines);
	MachineIndex.Init(max_jobs, max_machines);
	WeightT.Init(sets,max_jobs);
	WeightF.Init(sets,max_jobs);
	WeightE.Init(sets,max_jobs);
	WeightN.Init(sets,max_jobs);
	Fitness.Init(sets+1,FUNCTIONS);
	Values.Init(max_machines,max_jobs);
	for(int i=0; i<12; i++){
		ValuesArray[i].Init(max_machines,max_jobs);
	}
	Precedence.Reset(max_jobs,max_jobs);	// prazna matrica znaci da nema ogranicenja!
	Setup.Init(max_jobs+1,max_jobs);
	if(m_Environment == JOBSHOP)
	{	Schedule.Init(sets, max_machines*max_jobs);
		PTimeAvg.Reset(sets, max_machines);
	}
	else
	{	Schedule.Init(sets,max_jobs);
		PTimeAvg.Init(sets,max_jobs);
		PTimeMinMachine.Init(sets,max_jobs);
	}
	SortedReady.Init(sets,max_jobs);

	pVrijednosti = new double[max_jobs];
	pVrijednosti1 = new double[max_jobs];
	pVrijednosti2 = new double[max_jobs];


	//Marko: enslemble
	for(int kk=0; kk< 12; kk++){
		pVrijednostiArray[kk] = new double[max_jobs];
	}


	pRasporedjen = new bool[max_jobs];
	pIndex = new unsigned int[max_jobs];
	pUsed = new unsigned int[max_jobs];
	pArray = new double[max_jobs];
	pSlack = new double[max_jobs];
	pSlackSpeed = new double[max_jobs];
	pArrival = new double[max_jobs];
	pLevel = new double[max_jobs];
	pSetupAvg = new double[max_jobs + 1];
	pSamples = new bool[sets];
	pLastJob = new unsigned int[max_machines];
	pMachineScheduled = new unsigned int[max_machines];
	pOperationReady = new double[max_machines];
	pJobReady = new double[max_jobs];
	pOperationsScheduled = new unsigned int[max_jobs];
	pTotalWorkRemaining = new double[max_jobs];
	pTotalWorkDone = new double[max_jobs];
	pTotalMachineWork = new double[max_machines];
	pOperationsWaiting = new unsigned int[max_machines];
	pMachineWorkRemaining = new double[max_machines];
	pMachineValues = new double[max_machines];
	p.ReadParameter("jobs",ReadPar::DOUBLE,&d_niz[0][0],sets);
	p.ReadParameter("machines",ReadPar::DOUBLE,&d_niz[1][0],sets);
	total_jobs = 0;
	for(i=0; i<sets; i++)
	{	N[i][0] = d_niz[0][i];
		total_jobs += (int) d_niz[0][i];
		Machines[i][0] = d_niz[1][i];
	}
	Duration.Load(duration.c_str());
	Deadline.Load(deadline.c_str());
	if(m_Environment==UNIFORM)
	{	Speed.Load(speed.c_str());
	}
	WeightT.Load(weightT.c_str());
	WeightF.Load(weightF.c_str());
	WeightE.Load(weightE.c_str());
	WeightN.Load(weightN.c_str());
	SP.Load(sp.c_str());
	if(m_dynamic) Ready.Load(ready.c_str());
	else Ready.Reset(sets,max_jobs);
	if(m_constrained) Constraints.Load(cons.c_str());
	// racunamo sumu deadline-a
	Level.Init(sets,max_jobs);
	for(i=0; i<sets; i++)
	{	SD.Set(i,0);
		for(j=0; j<(unsigned int)N.Get(i); j++)
		{	SD.data[i][0] += Deadline.data[i][j];
			Level[i][j] = -1;	// oznacimo da je neizracunato
		}
	}
	// racunamo level cvorova u grafu ovisnosti
	for(i=0; i<sets; i++)
	{	if(m_constrained)
			ReadConstraints(Constraints,i,(unsigned int)N.Get(i),Precedence);
		for(j=0; j<(unsigned int)N.Get(i); j++)
			Level[i][j] = NodeLevel(i,j);
	}
	// racunamo prosjek i minimalno trajanje izvodjenja za UNRELATED
	if(m_Environment == UNRELATED)
	{	for(uint set=0; set<sets; set++)
		{	uint jobs = (uint) N[set][0];
			uint machines = (uint) Machines[set][0];
			for(j=0; j<jobs; j++)
			{	PTimeAvg[set][j] = 0;
				uint min_machine = 0;
				for(uint machine=0; machine<machines; machine++)
				{	PTimeAvg[set][j] += Duration[set][j*machines + machine];
					if(Duration[set][j*machines + machine] < Duration[set][j*machines + min_machine])
						min_machine = machine;
				}
				PTimeAvg[set][j] /= machines;
				PTimeMinMachine[set][j] = min_machine;
			}
		}
	}
	if(m_Environment == JOBSHOP)	// prosjek trajanja operacija po strojevima
	{	for(uint set=0; set<sets; set++)
		{	uint jobs = (uint) N[set][0];
			uint machines = (uint) Machines[set][0];
			for(j=0; j<jobs; j++)
			{	uint operations = machines;
				for(uint op=0; op<operations; op++)
				{	double dur = Duration[set][j*operations + op];
					uint machine = (uint) dur / 1000;
					dur = (int)dur % 1000;	// dobijemo trajanje
					PTimeAvg[set][machine] += dur;
				}
			}
			for(uint m=0; m<machines; m++)
				PTimeAvg[set][m] /= jobs;
		}
	}

	// sortiramo indekse poslova po dolascima - treba za ubrzano izracunavanje
	for(i=0; i<sets; i++)
	{	::pVal = Ready[i];
		uint jobs = (uint) N[i][0];
		for(j=0; j<jobs; j++)
			pIndex[j] = j;
		qsort(pIndex,jobs,sizeof(unsigned int),::Before);
		for(j=0; j<jobs; j++)
			SortedReady[i][j] = pIndex[j];
	}
				
	p.CloseFile();

	return true;
}



// zove se iz main() prije inicijalizacije populacije
void SchedulingEvalOp::DefineNodeNames(void)
{
}


// citanje terminala iz konf. datoteke
// radi se prije inicijalizacije populacije; poziva se iz main()
void SchedulingEvalOp::ReadTerminals(TreeP tree)
{
	int i,dummy;
	ReadPar p;
	std::string term;
	p.OpenFile(in_file.c_str());

	// citanje aktivnih terminala
	for(i = OFFSET; i < TERMINALS + OFFSET; i++)
	{	term = "T_" + Nodes[i].name;
		if(p.ReadParameter(term.c_str(),ReadPar::INTEGER,&dummy))
		{	Nodes[i].active = true;

			// zadavanje terminala ECF-u
			Tree::PrimitiveP newTerm = (Tree::PrimitiveP) new Tree::Primitives::Terminal;
			newTerm->setName(Nodes[i].name);
			tree->addTerminal(newTerm);
		}
	}
	p.CloseFile();
}


// citanje terminala iz ECF parametra 'tree.terminalset'
// poziva se iz initialize()
void SchedulingEvalOp::ReadTerminals(StateP state)
{
	int i;
	std::string term;

	GenotypeP gen = (GenotypeP) (state->getGenotypes()[0]);
	TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (gen);
	voidP val = tree->getParameterValue(state, "terminalset");
	std::string terminals = *((std::string*) val.get());
	terminals = " " + terminals + " ";

	// citanje aktivnih terminala
	for(i = OFFSET; i < TERMINALS + OFFSET; i++)
	{	if(terminals.find(" " + Nodes[i].name + " ") != string::npos)
		{	Nodes[i].active = true;
		}
	}
}

//
// dio za provjeru duplikata
//
vector<IndividualP> jedinke;    // globalna...
// fja vraca true ako nadje istu, a prepise joj fitness od stare
bool provjeriDuplikate(IndividualP individual, StateP state_)
{
   /*     static uint gen = 0;
        static uint jednakih = 0;

        bool jednaka = false;

        // provjeri generaciju
        if(state_->getGenerationNo() != gen) {
                gen = state_->getGenerationNo();

                ECF_LOG(state_, 1, "jednakih: " + uint2str(jednakih) + ", " + 
uint2str(100*jednakih/jedinke.size()));

                jedinke.clear();
                jednakih = 0;
        }

        // provjeri sadrzaj
        uint broj = (uint) jedinke.size();
        Tree::Tree* nova = (Tree::Tree*) individual->getGenotype().get();
        for(uint i = 0; i < broj; i++) {
                Tree::Tree* stara = (Tree::Tree*) jedinke[i]->getGenotype().get();
                if(nova->size() != stara->size())
                        continue;
                uint n, size = (uint) nova->size();
                Tree::NodeP cvor1, cvor2;
                for(n = 0; n < size; n++) {
                        cvor1 = (*nova)[n];
                        cvor2 = (*stara)[n];
                        if(cvor1->primitive_ != cvor2->primitive_)
                                break;
                }
                // ako smo nasli jednaku, dodijeli joj Fitness od stare
                if(n == size) {
                        jednakih++;
                        individual->fitness = (FitnessP) jedinke[i]->fitness->copy();
                        jednaka = true;
                        break;
                }
        }

        return jednaka;*/
	return false;
}


FitnessP SchedulingEvalOp::evaluate(IndividualP individual)
{ 
/*	double dClock, dRawFitness=0, dFitness, dRez, dSetFitness, dAvgWeights;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dMsum, dSetupTime, dFwt;
    Double DResult, DBest;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
*/
	// stvaranje zeljenog Fintess objekta:
	
	/*if(provjeriDuplikate(individual, state_)) {
         FitnessP fitness = individual->fitness;
         return fitness;
    }
	*/
	FitnessP fitness = static_cast<FitnessP> (new FitnessMin);

	if(m_genotip == 0) { // koristimo FP, unrelated okruzenje

		FloatingPointP fp = boost::dynamic_pointer_cast<FloatingPoint::FloatingPoint> (individual->getGenotype());
		double dFitness;

		EvaluateUnrelatedFP(fp, dFitness);

		fitness->setValue(dFitness);

		return fitness;
	}


	// dohvat genotipa jedinke
	TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (individual->getGenotype());

// oroginalni kod iz BEAGLE implementacije
	unsigned int i;
	double dRawFitness, dFitness;
	ReadIndividual(individual);	// procitaj i zapisi jedinku

	// stochastic sampling?
	if(m_stsampling)
	{	int koliko = (int) (m_sampling*sets);
		int razmak = sets / koliko;
		int pocetni = rand()%razmak;
		for(i=0; i<sets; i++)
			pSamples[i] = false;
		for(i=pocetni; i<sets; i+=razmak)
			pSamples[i] = true;
	}

	switch(m_Environment)
	{	case SINGLE:
			EvaluateSingle(dRawFitness);
		break;
		case UNIFORM:
			EvaluateUniform(dRawFitness);
		break;
		case UNRELATED:
			//EvaluateUnrelatedStatic(dRawFitness);
			
			if(first)
			{
				EvaluateUnrelatedStatic(dRawFitness);
				//EvaluateUnrelated(dRawFitness);
			}
			else
			{
				//EvaluateUnrelated(dRawFitness);
				//EvaluateUnrelatedRollout(dRawFitness);
				EvaluateUnrelatedStatic(dRawFitness);
			}
		break;
		case JOBSHOP:
			EvaluateJobShop(dRawFitness);
		break;
	}

	//dFitness = dRawFitness /= nJobS*SETS;	// prosjek
	//double lRMSE = sqrt(sqrt(dRawFitness));		// irelevantno za turnir selekciju
	//dFitness = (1.0 / (lRMSE + 1.0));	
	dFitness = dRawFitness;	// (trazimo minimum)

	if(m_Evaluation)	// samo za usporedbu heuristika! pise rezultate svih skupova u fajl
	{	Fitness.Save("rezultat_GP.txt");
		std::ostream *file = new std::ofstream("rezultat_GP.txt", std::ios_base::app);
		Evaluator.write();
		*file << std::endl << "-- infix: " << Evaluator.m_output << " --" << std::endl;
		*file << "Editirano: " << edited << ", ukupno: " << total << std::endl;
		*file << std::flush;
		delete file;
		Schedule.Save("raspored_GP.txt");
	}

	// pogledamo je li ukljuceno brojanje terminala
//	if(m_TermUsage)
//		UpdateTerminalStatistic(dFitness);

	fitness->setValue(dFitness);

        // ova je bila nova, zapisi fitnes i zapamti ovu jedinku:
    individual->fitness = fitness;
    jedinke.push_back((IndividualP) individual->copy());

    return fitness;
}










//za ensemble

FitnessP SchedulingEvalOp::evaluate(vector<IndividualP> individuals)
{ 
/*	double dClock, dRawFitness=0, dFitness, dRez, dSetFitness, dAvgWeights;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dMsum, dSetupTime, dFwt;
    Double DResult, DBest;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
*/
	// stvaranje zeljenog Fintess objekta:
	
	/*if(provjeriDuplikate(individual, state_)) {
         FitnessP fitness = individual->fitness;
         return fitness;
    }
	*/
	FitnessP fitness = static_cast<FitnessP> (new FitnessMin);

	// dohvat genotipa jedinke
	//TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (individual->getGenotype());

// oroginalni kod iz BEAGLE implementacije
	unsigned int i;
	double dRawFitness, dFitness;
	ReadIndividuals(individuals);	// procitaj i zapisi jedinku

	// stochastic sampling?
	if(m_stsampling)
	{	int koliko = (int) (m_sampling*sets);
		int razmak = sets / koliko;
		int pocetni = rand()%razmak;
		for(i=0; i<sets; i++)
			pSamples[i] = false;
		for(i=pocetni; i<sets; i+=razmak)
			pSamples[i] = true;
	}

	switch(m_Environment)
	{	case SINGLE:
			//EvaluateSingle(dRawFitness);
//			EvaluateSingleVote(dRawFitness);
			EvaluateSingleSum(dRawFitness);

		break;
		case UNIFORM:
//			EvaluateUniform(dRawFitness);
//			EvaluateUniformVote(dRawFitness);
			EvaluateUniformSum(dRawFitness);
		break;
		case UNRELATED:
			EvaluateUnrelated2(dRawFitness);
//			EvaluateUnrelatedWeightedvoting(dRawFitness);
//			EvaluateUnrelatedSumOfPriorities(dRawFitness);
//			EvaluateUnrelatedWeightedSumOfPriorities(dRawFitness);

//			EvaluateUnrelatedSumOfPrioritiesConfidences(dRawFitness);
//			EvaluateUnrelated2Confidences(dRawFitness);
		break;
		case JOBSHOP:
//			EvaluateJobShop(dRawFitness);
			EvaluateJobShopSum(dRawFitness);
//			EvaluateJobShopVote(dRawFitness);

		break;
	}

	//dFitness = dRawFitness /= nJobS*SETS;	// prosjek
	//double lRMSE = sqrt(sqrt(dRawFitness));		// irelevantno za turnir selekciju
	//dFitness = (1.0 / (lRMSE + 1.0));	
	dFitness = dRawFitness;	// (trazimo minimum)

	if(m_Evaluation)	// samo za usporedbu heuristika! pise rezultate svih skupova u fajl
	{	Fitness.Save("rezultat_GP.txt");
		std::ostream *file = new std::ofstream("rezultat_GP.txt", std::ios_base::app);
		Evaluator.write();
		*file << std::endl << "-- infix: " << Evaluator.m_output << " --" << std::endl;
		*file << "Editirano: " << edited << ", ukupno: " << total << std::endl;
		*file << std::flush;
		delete file;
		Schedule.Save("raspored_GP.txt");
	}

	// pogledamo je li ukljuceno brojanje terminala
//	if(m_TermUsage)
//		UpdateTerminalStatistic(dFitness);

	fitness->setValue(dFitness);

        // ova je bila nova, zapisi fitnes i zapamti ovu jedinku:
	individuals.at(0)->fitness = fitness;
	jedinke.push_back((IndividualP) individuals.at(0)->copy());

    return fitness;
}









void SchedulingEvalOp::write(std::string &output)
{
}


// dekodira matricu Constraints i definira matricu Precedence
void SchedulingEvalOp::ReadConstraints(Matrica &Constraints, int set, int jobs, Matrica &Precedence)
{
	int i,j,prethodnika,prethodnik,pom;
	unsigned int podatak;
	//Precedence.Init(jobs,jobs);
	// prvo ocistimo prva dva stupca! gdje su brojevi prethodnika i sljedbenika
	for(i=0; i<jobs; i++)
		for(j=0; j<2; j++)
			Precedence[i][j] = 0;
	for(i=0; i<jobs; i++)
	{	podatak = (unsigned int) Constraints[set][i];
		prethodnik = 1;	// koji po redu posao iza mene
		prethodnika = 0;
		while(podatak != 0)
		{	if(podatak%2 != 0)
			{	prethodnika++;	// povecam broj svojih prethodnika
				pom = (int) Precedence[i-prethodnik][1] + 1;
				Precedence[i-prethodnik][pom+1] = i;
				Precedence[i-prethodnik][1] = pom;	// i broj sljedbenika od moga prethodnika
			}
			prethodnik++;
			podatak /= 2;
		}
		Precedence[i][0] = prethodnika;
	}
}


// generira matricu sequence dependant setup trajanja
// i polje prosjecnih trajanja postavljanja za svaki posao prema ostalima
void SchedulingEvalOp::MakeSetup(Matrica &Duration, int set, int jobs, double faktor, Matrica &Setup)
{
	int i,j;
	pSetupAvg[jobs] = 0;
	if(m_Environment == JOBSHOP)
	{	srand(set);
		for(i=0; i<jobs; i++)
		{	Setup[jobs][i] = (int) ((rand()%max_length+1) * faktor);	// polazno trajanje postavljanja
			pSetupAvg[jobs] += Setup[jobs][i];
			for(j=0; j<=i; j++)
			{	Setup[i][j] = (int) ((rand()%max_length+1) * faktor);
				Setup[j][i] = (int) ((rand()%max_length+1) * faktor);
				pSetupAvg[i] += Setup[i][j];
				pSetupAvg[j] += Setup[j][i];
			}
		}
	}
	else
		for(i=0; i<jobs; i++)
		{	pSetupAvg[i] = 0;
			Setup[jobs][i] = Duration[set][(i+1) % jobs];	// polazno trajanje postavljanja
			pSetupAvg[jobs] += Setup[jobs][i];
			for(j=0; j<=i; j++)
			{	Setup[i][j] = ceil( fabs( Duration[set][i] - Duration[set][j] ) * faktor);
				Setup[j][i] = ceil( fabs( Duration[set][(i+1) % jobs] - Duration[set][(j+1) % jobs] ) * faktor);
				pSetupAvg[i] += Setup[i][j];
				pSetupAvg[j] += Setup[j][i];
			}
		}
	pSetupAvg[jobs] /= jobs;
	for(i=0; i<jobs; i++)
		pSetupAvg[i] /= (jobs-1);
}


// procita jedinku i zapise u RPN; takodjer editira i prebroji terminale
void SchedulingEvalOp::ReadIndividual(IndividualP individual)
{
	TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (individual->getGenotype());
	static std::string strTerminal;
	unsigned int nTreeSize,i,j,nTree;
	uint nTrees = (uint) individual->size();
	for(nTree = 0; nTree<nTrees; nTree++)	// vrtimo kroz stabla
	{	TreeP pTree = boost::dynamic_pointer_cast<Tree::Tree> (individual->getGenotype(nTree)); // pokazivac na tree
		nTreeSize = (uint) pTree->size();
		if(nTreeSize > Evaluator.m_iExprSize)	// jel imamo dovoljno mjesta za cvorove
			Evaluator.SetExprSize(nTreeSize);

		// procitamo cijelo stablo i zapisemo u rpn
		for(i=0; i<nTreeSize; i++)
		{	//strTerminal = (*pTree)[i].mPrimitive->getName();
			strTerminal = (*pTree)[i]->primitive_->getName();
			for(j=OFFSET; j<TOTAL_NODES; j++)
			{	if(!Nodes[j].active)
					continue;
				if(strTerminal == Nodes[j].name)
				{	Evaluator.m_pExpression[nTree][i] = j;
					break;
				}
			}
			assert(j<TOTAL_NODES);
		}
		// editiranje?
		if(m_editing)
		{	Evaluator.m_nTree = nTree;	// zadamo na kojem se stablu radi
			Evaluator.m_iPosition = Evaluator.m_iEditedPos = -1;
			Evaluator.edit();
			Evaluator.copy();	// automatski prebrojava terminale i funkcije
			total += Evaluator.m_iPosition;
			edited += Evaluator.m_iPosition - Evaluator.m_iEditedPos;
		}
	}

}


// procita jedinku i zapise u RPN; takodjer editira i prebroji terminale
void SchedulingEvalOp::ReadIndividuals(vector<IndividualP> individuals)
{
	ff = true;
	for(int k = 0;  k< individuals.size(); k++){
		TreeP tree = boost::dynamic_pointer_cast<Tree::Tree> (individuals.at(k)->getGenotype());
	static std::string strTerminal;
	unsigned int nTreeSize,i,j,nTree;
	uint nTrees = (uint) individuals.at(k)->size();
	for(nTree = 0; nTree<nTrees; nTree++)	// vrtimo kroz stabla
	{	TreeP pTree = boost::dynamic_pointer_cast<Tree::Tree> (individuals.at(k)->getGenotype(nTree)); // pokazivac na tree
		nTreeSize = (uint) pTree->size();
		if(nTreeSize > Evaluator.m_iExprSize)	// jel imamo dovoljno mjesta za cvorove
			Evaluator.SetExprSize(nTreeSize, k);

		// procitamo cijelo stablo i zapisemo u rpn
		for(i=0; i<nTreeSize; i++)
		{	//strTerminal = (*pTree)[i].mPrimitive->getName();
			strTerminal = (*pTree)[i]->primitive_->getName();
			for(j=OFFSET; j<TOTAL_NODES; j++)
			{	if(!Nodes[j].active)
					continue;
				if(strTerminal == Nodes[j].name)
				{	Evaluator.m_pExpression[k][i] = j;
					break;
				}
			}
			assert(j<TOTAL_NODES);
		}
		// editiranje?
		if(m_editing)
		{	Evaluator.m_nTree = nTree;	// zadamo na kojem se stablu radi
			Evaluator.m_iPosition = Evaluator.m_iEditedPos = -1;
			Evaluator.edit();
			Evaluator.copy();	// automatski prebrojava terminale i funkcije
			total += Evaluator.m_iPosition;
			edited += Evaluator.m_iPosition - Evaluator.m_iEditedPos;
		}
	}

	}
	
}


// rekurzivno racunanje 'level' vrijednosti svakog posla
// ima smisla samo u problemima s ogranicenjima
// promjena 27.07: level cvora ukljucuje i trajanje cvora
double SchedulingEvalOp::NodeLevel(int set, int node)
{	double value,level;
	int succ,i,next;
	if(Level[set][node] > -1)	// ako je vec izracunato
		return Level[set][node];
	if(Precedence[node][1] == 0)	// ako nema sljedbenika
		return Duration[set][node];
	succ = (int)Precedence[node][1];	// koliko sljedbenika
	next = (int)Precedence[node][2];	// prvi sljedbenik
	level = NodeLevel(set,next) + Duration[set][node];	// level zbog prvog sljedbenika
	for(i=1; i<succ; i++)
	{	next = (int)Precedence[node][i+2];
		value = NodeLevel(set,next) + Duration[set][node];
		if(value > level)
			level = value;
	}
	return level;
}


// racuna vremenski i drugacije ovisne terminale
void SchedulingEvalOp::CalcTimedTerminals(uint &nNiz, uint &nPoslova, uint &nJob, double &dClock, \
									uint nMachine, uint nMachines)
{	uint i,j;
	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
	{	j = pIndex[i];	// 'pravi' indeks posla
		if(m_Environment == UNIFORM)
		{	Evaluator.m_dTermValuesArray[T_SPD][j] = Speed[nNiz][nMachine];	// brzina stroja (neovisna o poslu)
			pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j]);
			pSlackSpeed[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j] * Speed[nNiz][nMachine]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j]*Speed[nNiz][nMachine]- Deadline[nNiz][j]);	// kasnjenje
		}
		if(m_Environment == UNRELATED)
		{	Evaluator.m_dTermValuesArray[T_PAT][j] = POS(MachineReady[(uint)PTimeMinMachine[nNiz][j]][0] - dClock);
			Evaluator.m_dTermValuesArray[T_MR][j] = POS(MachineReady[nMachine][0] - dClock);
			Evaluator.m_dTermValuesArray[T_pt][j] = Duration[nNiz][j*nMachines + nMachine];
			Evaluator.m_dTermValuesArray[T_age][j] = POS(dClock - Ready[nNiz][j]);
			pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j*nMachines + nMachine]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j*nMachines + nMachine]- Deadline[nNiz][j]);	// kasnjenje
		}
		if(m_Environment == SINGLE)
		{	pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j]- Deadline[nNiz][j]);	// kasnjenje
			//Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock - Deadline[nNiz][j]);	// kasnjenje
		}

		// 'zajednicki' terminali
		Evaluator.m_dTermValuesArray[T_CLK][j] = dClock;
		pArrival[j] = POS(Ready[nNiz][j] - dClock);	// pozitivna vrijednost dolaska
		Evaluator.m_dTermValuesArray[T_AR][j] = pArrival[j];	// za koliko dolazi
		if(pSlack[j]<0) pSlack[j] = 0;	// uzimamo samo pozitivni slack?
		if(pSlackSpeed[j]<0) pSlackSpeed[j] = 0;	// uzimamo samo pozitivni slack?
		// pokazalo se malo boljim!
		Evaluator.m_dTermValuesArray[T_SL][j] = pSlack[j];	// slack
		Evaluator.m_dTermValuesArray[T_SLs][j] = pSlackSpeed[j];	// slack sa brzinom

		if(m_Environment != SINGLE)	// za SINGLE se racunaju u glavnoj funkciji
		{	// trajanje postavljanja u odnosu na zadnji posao na zadanom stroju
			Evaluator.m_dTermValuesArray[T_STP][j] = Setup[pLastJob[nMachine]][j];
			Evaluator.m_dTermValuesArray[T_Sav][j] = pSetupAvg[pLastJob[nMachine]];	// prosjecno t.p.
		}
	}
}


// racuna vremenski i drugacije ovisne terminale
void SchedulingEvalOp::CalcTimedTerminals(uint &nNiz, uint &nPoslova, uint &nJob, double &dClock, \
									uint nMachine, uint nMachines, double endTime)
{	uint i,j;
	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
	{	j = pIndex[i];	// 'pravi' indeks posla
	if(Ready[nNiz][j]>endTime)
	{
		break;
	}	
	if(m_Environment == UNIFORM)
		{	Evaluator.m_dTermValuesArray[T_SPD][j] = Speed[nNiz][nMachine];	// brzina stroja (neovisna o poslu)
			pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j]);
			pSlackSpeed[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j] * Speed[nNiz][nMachine]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j]*Speed[nNiz][nMachine]- Deadline[nNiz][j]);	// kasnjenje
		}
		if(m_Environment == UNRELATED)
		{	Evaluator.m_dTermValuesArray[T_PAT][j] = POS(MachineReady[(uint)PTimeMinMachine[nNiz][j]][0] - dClock);
			Evaluator.m_dTermValuesArray[T_MR][j] = POS(MachineReady[nMachine][0] - dClock);
			Evaluator.m_dTermValuesArray[T_pt][j] = Duration[nNiz][j*nMachines + nMachine];
			Evaluator.m_dTermValuesArray[T_age][j] = POS(dClock - Ready[nNiz][j]);
			pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j*nMachines + nMachine]);
			//Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j*nMachines + nMachine]- Deadline[nNiz][j]);	// kasnjenje
		}
		if(m_Environment == SINGLE)
		{	pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j]- Deadline[nNiz][j]);	// kasnjenje
			//Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock - Deadline[nNiz][j]);	// kasnjenje
		}

		// 'zajednicki' terminali
		Evaluator.m_dTermValuesArray[T_CLK][j] = dClock;
		pArrival[j] = POS(Ready[nNiz][j] - dClock);	// pozitivna vrijednost dolaska
		Evaluator.m_dTermValuesArray[T_AR][j] = pArrival[j];	// za koliko dolazi
		if(pSlack[j]<0) pSlack[j] = 0;	// uzimamo samo pozitivni slack?
		//if(pSlackSpeed[j]<0) pSlackSpeed[j] = 0;	// uzimamo samo pozitivni slack?
		// pokazalo se malo boljim!
		Evaluator.m_dTermValuesArray[T_SL][j] = pSlack[j];	// slack
		//Evaluator.m_dTermValuesArray[T_SLs][j] = pSlackSpeed[j];	// slack sa brzinom

		if(m_Environment != SINGLE)	// za SINGLE se racunaju u glavnoj funkciji
		{	// trajanje postavljanja u odnosu na zadnji posao na zadanom stroju
			Evaluator.m_dTermValuesArray[T_STP][j] = Setup[pLastJob[nMachine]][j];
			Evaluator.m_dTermValuesArray[T_Sav][j] = pSetupAvg[pLastJob[nMachine]];	// prosjecno t.p.
		}
	}
}

void SchedulingEvalOp::CalcTimedTerminals(uint &nNiz, uint &nPoslova, uint &nJob, double &dClock, \
									uint nMachine, uint nMachines, Matrica MachineReady)
{	uint i,j;
	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
	{	j = pIndex[i];	// 'pravi' indeks posla
		if(m_Environment == UNIFORM)
		{	Evaluator.m_dTermValuesArray[T_SPD][j] = Speed[nNiz][nMachine];	// brzina stroja (neovisna o poslu)
			pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j]);
			pSlackSpeed[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j] * Speed[nNiz][nMachine]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j]*Speed[nNiz][nMachine]- Deadline[nNiz][j]);	// kasnjenje
		}
		if(m_Environment == UNRELATED)
		{	Evaluator.m_dTermValuesArray[T_PAT][j] = POS(MachineReady[(uint)PTimeMinMachine[nNiz][j]][0] - dClock);
			Evaluator.m_dTermValuesArray[T_MR][j] = POS(MachineReady[nMachine][0] - dClock);
			Evaluator.m_dTermValuesArray[T_pt][j] = Duration[nNiz][j*nMachines + nMachine];
			Evaluator.m_dTermValuesArray[T_age][j] = POS(dClock - Ready[nNiz][j]);
			pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j*nMachines + nMachine]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j*nMachines + nMachine]- Deadline[nNiz][j]);	// kasnjenje
		}
		if(m_Environment == SINGLE)
		{	pSlack[j] = Deadline[nNiz][j] - (dClock + Duration[nNiz][j]);
			Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock + Duration[nNiz][j]- Deadline[nNiz][j]);	// kasnjenje
			//Evaluator.m_dTermValuesArray[T_L][j] = POS(dClock - Deadline[nNiz][j]);	// kasnjenje
		}

		// 'zajednicki' terminali
		Evaluator.m_dTermValuesArray[T_CLK][j] = dClock;
		pArrival[j] = POS(Ready[nNiz][j] - dClock);	// pozitivna vrijednost dolaska
		Evaluator.m_dTermValuesArray[T_AR][j] = pArrival[j];	// za koliko dolazi
		if(pSlack[j]<0) pSlack[j] = 0;	// uzimamo samo pozitivni slack?
		if(pSlackSpeed[j]<0) pSlackSpeed[j] = 0;	// uzimamo samo pozitivni slack?
		// pokazalo se malo boljim!
		Evaluator.m_dTermValuesArray[T_SL][j] = pSlack[j];	// slack
		Evaluator.m_dTermValuesArray[T_SLs][j] = pSlackSpeed[j];	// slack sa brzinom

		if(m_Environment != SINGLE)	// za SINGLE se racunaju u glavnoj funkciji
		{	// trajanje postavljanja u odnosu na zadnji posao na zadanom stroju
		//	Evaluator.m_dTermValuesArray[T_STP][j] = Setup[pLastJob[nMachine]][j];
		//	Evaluator.m_dTermValuesArray[T_Sav][j] = pSetupAvg[pLastJob[nMachine]];	// prosjecno t.p.
		}
	}
}

void SchedulingEvalOp::CalcStaticTerminals(uint &nNiz, uint &nPoslova, uint &nJob, double &dClock, \
									uint nMachine, uint nMachines, Matrica MachineReady, double endTime)
{
	int VERY_BIG_VALUE = 5000;

	uint i,j, k;

	//racunamo T_NSHORT
	int nShort = 0;
	
	int slackNext = 0;
	int slackNextReady = 99999;

	int slackNextM = 0;
	int slackNextReadyM = 99999;

	int minRelease = 99999;

	int minReleaseM = 99999;

	int mload = 0;


	int slacksum = 0;
	//int counter = 0;

	int slCounter = 0;
	int slCounterM = 0;

	int startInd = -1;

	for(i=nJob; i<nPoslova; i++)
	{
		//calculate T_NSHORT
		j = pIndex[i];

		bool isReady= Ready[nNiz][j]>dClock;


		if(isReady)
		{
			if(isShortestExecutingJob(nNiz, nMachine, nMachines, j))
			{
				nShort++;
				mload+=Duration[nNiz][j*nMachines + nMachine];
				slacksum+=pSlack[j];
				//counter++;
			}
			if(startInd==-1)
			{
				startInd = i;
			}
		}

		//int timeMax = dClock>Ready[nNiz][j]?dClock:Ready[nNiz][j];
		//int slack = Deadline[nNiz][j] - (timeMax + Duration[nNiz][j*nMachines + nMachine]);

		//Calculate slack next
		if(isReady && slackNextReady>Ready[nNiz][j])
		{
			slackNext = pSlack[j];
			//slackNext = slack;
			slackNextReady = Ready[nNiz][j];
			slCounter++;
		}
		
		if(isReady && slackNextReadyM>Ready[nNiz][j] && isShortestExecutingJob(nNiz, nMachine, nMachines, j))
		{
			slackNextM = pSlack[j];
			//slackNextM = slack;
			slackNextReadyM = Ready[nNiz][j];
			slCounterM++;
		}
	
		if(isReady && minRelease > (Ready[nNiz][j]-dClock))
		{
			minRelease = Ready[nNiz][j]-dClock;
		}

		if(isReady && minReleaseM > (Ready[nNiz][j]-dClock) && isShortestExecutingJob(nNiz, nMachine, nMachines, j))
		{
			minReleaseM = Ready[nNiz][j]-dClock;
		}
	}

	
	if(startInd==-1)
	{
		startInd = nPoslova;
	}

	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
	{	j = pIndex[i];	// 'pravi' indeks posla
		
		if(Ready[nNiz][j]>endTime)
		{
			break;;
		}
		Evaluator.m_dTermValuesArray[T_NSHORT][j] = nShort;
		if(slCounter!=0){
			Evaluator.m_dTermValuesArray[T_SLNEXT][j] = slackNext;
		} else
		{
			Evaluator.m_dTermValuesArray[T_SLNEXT][j] = VERY_BIG_VALUE;
		}
		if(slCounterM!=0){
			Evaluator.m_dTermValuesArray[T_SLNEXTM][j] = slackNextM;
		} else
		{
			Evaluator.m_dTermValuesArray[T_SLNEXTM][j] = VERY_BIG_VALUE;
			
		}
		if(minRelease!=0){
			Evaluator.m_dTermValuesArray[T_TTAR][j] = minRelease;
		} else{
			Evaluator.m_dTermValuesArray[T_TTAR][j] = VERY_BIG_VALUE;			
		}
		if(minReleaseM!=0){
			Evaluator.m_dTermValuesArray[T_TTARM][j] = minReleaseM;
		} else
		{
			Evaluator.m_dTermValuesArray[T_TTARM][j] = VERY_BIG_VALUE;
			
		}
		if(nShort!=0){
			Evaluator.m_dTermValuesArray[T_SLAVGM][j] = ((double)slacksum)/nShort;
		} else
		{
			Evaluator.m_dTermValuesArray[T_SLAVGM][j] = VERY_BIG_VALUE;
		}
		Evaluator.m_dTermValuesArray[T_MLOAD][j] = mload;

		
		int nrel = 0;
		int nrelM = 0;
		double wTard = 0;
		double wLate = 0;
		int mloadd = 0;
		double slavgd = 0;
		int counter1 = 0;
		double startTime = 0;

		double wTards = 0;
		double wLates = 0;
		bool wSetS = false;
		int wend = 0;

		double wtardL = 0;
		double wlateL = 0;
		double tsum = 0;
		
		int maxTime = dClock>Ready[nNiz][j]?dClock:Ready[nNiz][j];
		tsum = maxTime+Duration[nNiz][j*nMachines + nMachine];
		for(k = startInd; k< nPoslova; k++){
			int kIndex = pIndex[k];

			if(Ready[nNiz][kIndex]>maxTime && Ready[nNiz][kIndex]<(maxTime+Duration[nNiz][j*nMachines + nMachine]))
			{
				nrel++;
				if(isShortestExecutingJob(nNiz, nMachine, nMachines, kIndex))
				{
					nrelM++;
					mloadd+= Duration[nNiz][kIndex*nMachines + nMachine];

					slavgd+=pSlack[kIndex];
					counter1++;

					if(Ready[nNiz][kIndex]>startTime)
					{
						startTime = Ready[nNiz][kIndex];
					}
					startTime+=Duration[nNiz][kIndex*nMachines + nMachine];

					if(Deadline[nNiz][kIndex]<(Duration[nNiz][kIndex*nMachines + nMachine]+tsum))
					{
						double initialLate = 0;
						if(Deadline[nNiz][kIndex]<(Duration[nNiz][kIndex*nMachines + nMachine]+Ready[nNiz][kIndex]))
						{
							initialLate = ((Duration[nNiz][kIndex*nMachines + nMachine]+Ready[nNiz][kIndex])-Deadline[nNiz][kIndex])*WeightT[nNiz][kIndex];
						}
						wtardL+=(-Deadline[nNiz][kIndex]+(Duration[nNiz][kIndex*nMachines + nMachine]+tsum))*WeightT[nNiz][kIndex]-initialLate;
						
						if(initialLate==0){
							wlateL+=WeightT[nNiz][kIndex];
						}
					} 
						tsum+=Duration[nNiz][kIndex*nMachines + nMachine];						


					if(Deadline[nNiz][kIndex]<(Duration[nNiz][kIndex*nMachines + nMachine]+maxTime+Duration[nNiz][j*nMachines + nMachine]))
					{
						//true tardiness!
						//in the worst case the job can be late even if immediately released when it arrives, so we subtract the initial tardiness
						double initialLate = 0;
						if(Deadline[nNiz][kIndex]<(Duration[nNiz][kIndex*nMachines + nMachine]+Ready[nNiz][kIndex]))
						{
							initialLate = ((Duration[nNiz][kIndex*nMachines + nMachine]+Ready[nNiz][kIndex])-Deadline[nNiz][kIndex])*WeightT[nNiz][kIndex];
						}
						
						wTard+=(-Deadline[nNiz][kIndex]+(Duration[nNiz][kIndex*nMachines + nMachine]+maxTime+Duration[nNiz][j*nMachines + nMachine]))*WeightT[nNiz][kIndex]-initialLate;

						if(initialLate==0){
							wLate+=WeightT[nNiz][kIndex];
						}
						if(!wSetS)
						{
							wSetS = true;
							wTards+=wTard;
							wLates += wLate;
							wend = Ready[nNiz][kIndex]+Duration[nNiz][kIndex*nMachines + nMachine];
						}

					}
				}
			} else{break;}
		}
		Evaluator.m_dTermValuesArray[T_NREL][j] = nrel;
		Evaluator.m_dTermValuesArray[T_NRELM][j] = nrelM;
		Evaluator.m_dTermValuesArray[T_FUTLATE][j] = wTard;
		Evaluator.m_dTermValuesArray[T_WLATE][j] = wLate;
		//int maxTime = dClock>Ready[nNiz][j]?dClock:Ready[nNiz][j];
//		double lateWeight = (Deadline[nNiz][j]<(maxTime+2*Duration[nNiz][j*nMachines + nMachine]))?WeightT[nNiz][j]:0;

		double jInitialLate = 0;
		if(Deadline[nNiz][j]<(Duration[nNiz][j*nMachines + nMachine]+Ready[nNiz][j])){
			jInitialLate = ((Duration[nNiz][j*nMachines + nMachine]+Ready[nNiz][j])-Deadline[nNiz][j])*WeightT[nNiz][j];
		}

		double lateWeight=0;
		double lateWeightS = 0;
		if(jInitialLate==0){
			lateWeight = (Deadline[nNiz][j]<(startTime + Duration[nNiz][j*nMachines + nMachine]))?WeightT[nNiz][j]:0;
			lateWeightS = (Deadline[nNiz][j]<(wend + Duration[nNiz][j*nMachines + nMachine]))?WeightT[nNiz][j]:0;
		}
		
//		double tardinessWeight = (Deadline[nNiz][j]<(maxTime+2*Duration[nNiz][j*nMachines + nMachine]))?(WeightT[nNiz][j]*((-Deadline[nNiz][j])+(maxTime+2*Duration[nNiz][j*nMachines + nMachine]))):0;
		double tardinessWeight = (Deadline[nNiz][j]<(startTime+Duration[nNiz][j*nMachines + nMachine]))?(WeightT[nNiz][j]*((-Deadline[nNiz][j])+(startTime+Duration[nNiz][j*nMachines + nMachine])))-jInitialLate:0;

		Evaluator.m_dTermValuesArray[T_FLD][j] = tardinessWeight - wTard;
		Evaluator.m_dTermValuesArray[T_WLD][j] = lateWeight - wLate;

		if(counter1!=0){
			Evaluator.m_dTermValuesArray[T_SLAVGD][j] = slavgd/counter1;
		}else
		{
			Evaluator.m_dTermValuesArray[T_SLAVGD][j] = VERY_BIG_VALUE;			
		}

		Evaluator.m_dTermValuesArray[T_MLOADD][j] = mloadd;

		double tardinessWeightS = (Deadline[nNiz][j]<(wend+Duration[nNiz][j*nMachines + nMachine]))?(WeightT[nNiz][j]*((-Deadline[nNiz][j])+(wend+Duration[nNiz][j*nMachines + nMachine])))-jInitialLate:0;

		Evaluator.m_dTermValuesArray[T_FLDS][j] = tardinessWeightS - wTards;
		Evaluator.m_dTermValuesArray[T_WLDS][j] = lateWeightS - wLates;

		Evaluator.m_dTermValuesArray[T_FLDL][j] = tardinessWeight - wtardL;
		Evaluator.m_dTermValuesArray[T_WLDL][j] = lateWeight - wlateL;
	}
	 

	return ;
}

bool SchedulingEvalOp::isShortestExecutingJob(uint &nNiz, uint nMachine, uint nMachines, uint index)
{
	uint i;
	uint minIndex = 0;
	uint minValue = 101;
	for(i = 0; i<nMachines; i++)
	{
		if(minValue>Duration[nNiz][index*nMachines + i])
		{
			minValue = Duration[nNiz][index*nMachines + i];
			minIndex = i;
		}
	}
	return nMachine == minIndex;
}

// obrada za SINGLE okruzenje
void SchedulingEvalOp::EvaluateSingle(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration, dAvgDueDate;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSDr;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nOdabrani;

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		dAvgDueDate = 0;
		for(i=0; i<nPoslova; i++)
		{	dAvgDueDate += Deadline[nNiz][i];
			pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		dAvgDueDate /= nPoslova;
	// postavljanje pocetnih vrijednosti terminala
		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		dSPr = Evaluator.m_pTermValues[T_SP] = SP.Get(nNiz);
		dSDr = Evaluator.m_pTermValues[T_SD] = SD.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			Evaluator.m_dTermValuesArray[T_TF][i] = 1 - dAvgDueDate / SP[nNiz][0];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
				Evaluator.m_dTermValuesArray[T_STP][j] = Setup[nLastJob][j];	// trajanje postavljanja
				Evaluator.m_dTermValuesArray[T_Sav][j] = pSetupAvg[nLastJob];	// prosjecno t.p.
				//Evaluator.m_dTermValuesArray[T_SD][j] = dSDr;	// proba
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; + uzeta u obzir eventualna ogranicenja
			{	unsigned int raspolozivi = nJob, prvi = nJob;
				unsigned int najkraci;	// najkraci raspolozivi
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				double najdulje = 0, najkrace = 0;
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}
				if(kada > dClock)	// nema raspolozivih 
				{	dClock = kada;	// sat postavimo na najblize vrijeme dolaska
				}
				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				// pronadjimo najduljeg i najkraceg raspolozivog
				najdulje = najkrace = Duration[nNiz][pIndex[prvi]];
				najkraci = prvi;
				for(i=nJob; i<nPoslova; i++)
				{	k = pIndex[i];
					if(dClock < Ready[nNiz][k] || Precedence[k][0] > 0)
						continue;
					if(Duration[nNiz][k] < najkrace)	// najkrace
					{	najkrace = Duration[nNiz][k];
						najkraci = i;
					}
				}
				// sad pogledamo najduljega koji pocinje <= zavrsetka najkraceg raspolozivog
				for(i=nJob; i<nPoslova; i++)
				{	k = pIndex[i];
					if(Precedence[k][0] > 0)
						continue;
					if(Duration[nNiz][k] > najdulje && Ready[nNiz][k] <= (dClock+najkrace))	// gledamo najdulje trajanje
						najdulje = Duration[nNiz][k];
				}
				// sada je (dClock + najkrace + najdulje) limit za gledanje u buducnost!

				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				// prva verzija (kompliciranija)
				// gledat cemo sve cije vrijeme dolaska je prije zavrsetka najduljeg trenutno raspolozivog
					// MODIFIKACIJA (09.09.): gledamo sve koji mogu poceti prije zavrsetka najkraceg + trajanje do tada spremnog najduljeg!
					// (pronadjemo najduljeg koji moze poceti prije zavrsetka najkraceg)
				// tada: ako se prije odabranoga moze ugurati neki kraci, odaberemo najboljeg kraceg
				//kada = najdulje + 1;	// poc. vrijednost za dolazak trenutno odabranog
				kada = najkrace + najdulje;
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
				{	k = pIndex[i];
					if(Precedence[k][0] == 0 && (pVrijednosti[k] < dBest) && (Ready[nNiz][k] < dClock + kada))
					{	dBest = pVrijednosti[k];
						nOdabrani = i;
					}
				}
				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?
				if(kada >= najkrace)	// ako stane jos barem jedan, odaberimo najbolji koji ce zavrsiti prije pocetka odabranog
				{	dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
					nOdabrani = najkraci;
					for(i=nJob; i<nPoslova; i++)
					{	k = pIndex[i];
						if(Precedence[k][0] == 0 && (Ready[nNiz][k] + Duration[nNiz][k] <= dClock + kada) \
							&& pVrijednosti[k] < dBest \
							&& Ready[nNiz][k] - dClock < najkrace)	// pocinje prije zavrsetka najkraceg!
						{	dBest = pVrijednosti[k];
							nOdabrani = i;
						}
					}
					kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// novi odabrani
				}

/*				// druga verzija (jednostavnija)
				// samo gledamo najboljega koji moze poceti prije zavrsetka najkraceg raspolozivog
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + najkrace)
				{	if((pVrijednosti[pIndex[i]] < dBest) && (Ready[nNiz][pIndex[i]] < dClock + najkrace) \
						&& Precedence[pIndex[i]][0] == 0)
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?
*/
				// redovni nastavak (iza 1. i 2. verzije)
				if(kada > 0)	// odabrali smo cekati
					dClock += kada;	// ili: dClock = Ready[nNiz][pIndex[nOdabrani]];
			}	// endif - m_dynamic

			else	// staticki
			{	CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			dClock += Duration[nNiz][pIndex[nJob]];	// update vremena
			dSPr -= Duration[nNiz][pIndex[nJob]];		// update trajanja preostalih
			dSDr -= Deadline[nNiz][pIndex[nJob]];		// update deadlinea
			nNr--;											// update broja preostalih
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			if(m_setup)	// trajanje postavljanja
			{	dClock += Setup[nLastJob][pIndex[nJob]];
				nLastJob = pIndex[nJob];	// sljedeci prethodni posao
			}
			Schedule[nNiz][nJob] = pIndex[nJob];	// zapisemo u raspored
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = dAvgDuration = 0;
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = pIndex[nJob];
				dAvgWeights += WeightT[nNiz][index];
				dAvgDuration += Duration[nNiz][index];
				if(m_dynamic && dClock < Ready[nNiz][index])	// ako jos nije raspoloziv
					dClock = Ready[nNiz][index];
				if(m_setup)
					dClock += Setup[nLastJob][index];
				nLastJob = index;
				dClock += Duration.data[nNiz][index];	// update vremena
				dRez = dClock - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
			}
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// promijenjeno (valjda konacno) 04.09.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dAvgDuration /= nPoslova;
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			double Cmax = dClock / (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case TwtCmax: dRawFitness += dTardiness * Cmax; break;
				case NwtCmax: dRawFitness += dNwt * Cmax; break;
				default: exit(1);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
		}
	}
// kraj petlje koja vrti skupove poslova
	Fitness.data[sets][Twt] = dTotalTardiness;
	Fitness.data[sets][Nwt] = dTotalNwt;
}

// obrada za SINGLE okruzenje
void SchedulingEvalOp::EvaluateSingleSum(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration, dAvgDueDate;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSDr;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nOdabrani;
					double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		dAvgDueDate = 0;
		for(i=0; i<nPoslova; i++)
		{	dAvgDueDate += Deadline[nNiz][i];
			pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		dAvgDueDate /= nPoslova;
	// postavljanje pocetnih vrijednosti terminala
		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		dSPr = Evaluator.m_pTermValues[T_SP] = SP.Get(nNiz);
		dSDr = Evaluator.m_pTermValues[T_SD] = SD.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			Evaluator.m_dTermValuesArray[T_TF][i] = 1 - dAvgDueDate / SP[nNiz][0];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
				Evaluator.m_dTermValuesArray[T_STP][j] = Setup[nLastJob][j];	// trajanje postavljanja
				Evaluator.m_dTermValuesArray[T_Sav][j] = pSetupAvg[nLastJob];	// prosjecno t.p.
				//Evaluator.m_dTermValuesArray[T_SD][j] = dSDr;	// proba
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; + uzeta u obzir eventualna ogranicenja
			{	unsigned int raspolozivi = nJob, prvi = nJob;
				unsigned int najkraci;	// najkraci raspolozivi
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				double najdulje = 0, najkrace = 0;
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}
				if(kada > dClock)	// nema raspolozivih 
				{	dClock = kada;	// sat postavimo na najblize vrijeme dolaska
				}
				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				// pronadjimo najduljeg i najkraceg raspolozivog
				najdulje = najkrace = Duration[nNiz][pIndex[prvi]];
				najkraci = prvi;
				for(i=nJob; i<nPoslova; i++)
				{	k = pIndex[i];
					if(dClock < Ready[nNiz][k] || Precedence[k][0] > 0)
						continue;
					if(Duration[nNiz][k] < najkrace)	// najkrace
					{	najkrace = Duration[nNiz][k];
						najkraci = i;
					}
				}
				// sad pogledamo najduljega koji pocinje <= zavrsetka najkraceg raspolozivog
				for(i=nJob; i<nPoslova; i++)
				{	k = pIndex[i];
					if(Precedence[k][0] > 0)
						continue;
					if(Duration[nNiz][k] > najdulje && Ready[nNiz][k] <= (dClock+najkrace))	// gledamo najdulje trajanje
						najdulje = Duration[nNiz][k];
				}
				// sada je (dClock + najkrace + najdulje) limit za gledanje u buducnost!


				for(int kk = 0; kk<ensenbleCount; kk++){
					Evaluator.m_iPosition = -1;
					Evaluator.m_nTree = kk;
					Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
					memcpy((ValuesArray[kk])[0],pVrijednostiArray[kk],nPoslova*sizeof(double));					
				}


				//Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				// prva verzija (kompliciranija)
				// gledat cemo sve cije vrijeme dolaska je prije zavrsetka najduljeg trenutno raspolozivog
					// MODIFIKACIJA (09.09.): gledamo sve koji mogu poceti prije zavrsetka najkraceg + trajanje do tada spremnog najduljeg!
					// (pronadjemo najduljeg koji moze poceti prije zavrsetka najkraceg)
				// tada: ako se prije odabranoga moze ugurati neki kraci, odaberemo najboljeg kraceg
				//kada = najdulje + 1;	// poc. vrijednost za dolazak trenutno odabranog
				kada = najkrace + najdulje;
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				
				vector<double> prioritet;

				for(int kk=0; kk<nPoslova; kk++){
					prioritet.push_back(0);
				}
				
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
				{	k = pIndex[i];
					if(Precedence[k][0] == 0 && /*(pVrijednosti[k] < dBest)*/ (Ready[nNiz][k] < dClock + kada))
					{
						for(int kk=0; kk<ensenbleCount; kk++){
							prioritet[i]+=(ValuesArray[kk])[0][k];
							//cout<<"ValuesArrat: "<<(ValuesArray[kk])[0][i]<<endl;
						}
					}
				}

				dBest = 9999999999999;
				for(i = nJob; i<nPoslova; i++){
					k = pIndex[i];
					if(Precedence[k][0] == 0 && (prioritet[i] < dBest) && (Ready[nNiz][k] < dClock + kada))
					{
						nOdabrani = i; 
						dBest = prioritet[i];
					}
				}

				//cout<<"nOdabrani: "<<nOdabrani<<endl;

				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?
				if(kada >= najkrace)	// ako stane jos barem jedan, odaberimo najbolji koji ce zavrsiti prije pocetka odabranog
				{	dBest=prioritet[najkraci];
					nOdabrani = najkraci;
					for(i=nJob; i<nPoslova; i++)
					{	k = pIndex[i];
						if(Precedence[k][0] == 0 && (Ready[nNiz][k] + Duration[nNiz][k] <= dClock + kada) \
							&& prioritet[i] < dBest \
							&& Ready[nNiz][k] - dClock < najkrace)	// pocinje prije zavrsetka najkraceg!
						{	dBest = prioritet[i];
							nOdabrani = i;
						}
					}
					kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// novi odabrani
				}

/*				// druga verzija (jednostavnija)
				// samo gledamo najboljega koji moze poceti prije zavrsetka najkraceg raspolozivog
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + najkrace)
				{	if((pVrijednosti[pIndex[i]] < dBest) && (Ready[nNiz][pIndex[i]] < dClock + najkrace) \
						&& Precedence[pIndex[i]][0] == 0)
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?
*/
				// redovni nastavak (iza 1. i 2. verzije)
				if(kada > 0)	// odabrali smo cekati
					dClock += kada;	// ili: dClock = Ready[nNiz][pIndex[nOdabrani]];
			}	// endif - m_dynamic

			else	// staticki
			{	CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			dClock += Duration[nNiz][pIndex[nJob]];	// update vremena
			dSPr -= Duration[nNiz][pIndex[nJob]];		// update trajanja preostalih
			dSDr -= Deadline[nNiz][pIndex[nJob]];		// update deadlinea
			nNr--;											// update broja preostalih
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			if(m_setup)	// trajanje postavljanja
			{	dClock += Setup[nLastJob][pIndex[nJob]];
				nLastJob = pIndex[nJob];	// sljedeci prethodni posao
			}
			Schedule[nNiz][nJob] = pIndex[nJob];	// zapisemo u raspored
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = dAvgDuration = 0;
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = pIndex[nJob];
				dAvgWeights += WeightT[nNiz][index];
				dAvgDuration += Duration[nNiz][index];
				if(m_dynamic && dClock < Ready[nNiz][index])	// ako jos nije raspoloziv
					dClock = Ready[nNiz][index];
				if(m_setup)
					dClock += Setup[nLastJob][index];
				nLastJob = index;
				dClock += Duration.data[nNiz][index];	// update vremena
				dRez = dClock - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
			}
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// promijenjeno (valjda konacno) 04.09.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dAvgDuration /= nPoslova;
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			double Cmax = dClock / (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case TwtCmax: dRawFitness += dTardiness * Cmax; break;
				case NwtCmax: dRawFitness += dNwt * Cmax; break;
				default: exit(1);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
		}
	}
// kraj petlje koja vrti skupove poslova
	Fitness.data[sets][Twt] = dTotalTardiness;
	Fitness.data[sets][Nwt] = dTotalNwt;
}



void SchedulingEvalOp::EvaluateSingleVote(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration, dAvgDueDate;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSDr;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nOdabrani;

	double dBestArray[12];
	int odabraniArray[12];

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		dAvgDueDate = 0;
		for(i=0; i<nPoslova; i++)
		{	dAvgDueDate += Deadline[nNiz][i];
			pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		dAvgDueDate /= nPoslova;
	// postavljanje pocetnih vrijednosti terminala
		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		dSPr = Evaluator.m_pTermValues[T_SP] = SP.Get(nNiz);
		dSDr = Evaluator.m_pTermValues[T_SD] = SD.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			Evaluator.m_dTermValuesArray[T_TF][i] = 1 - dAvgDueDate / SP[nNiz][0];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo vrem. ovisne terminale, samo za nerasporedjene poslove
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
				Evaluator.m_dTermValuesArray[T_STP][j] = Setup[nLastJob][j];	// trajanje postavljanja
				Evaluator.m_dTermValuesArray[T_Sav][j] = pSetupAvg[nLastJob];	// prosjecno t.p.
				//Evaluator.m_dTermValuesArray[T_SD][j] = dSDr;	// proba
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; + uzeta u obzir eventualna ogranicenja
			{	unsigned int raspolozivi = nJob, prvi = nJob;
				unsigned int najkraci;	// najkraci raspolozivi
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				double najdulje = 0, najkrace = 0;
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}
				if(kada > dClock)	// nema raspolozivih 
				{	dClock = kada;	// sat postavimo na najblize vrijeme dolaska
				}
				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				// pronadjimo najduljeg i najkraceg raspolozivog
				najdulje = najkrace = Duration[nNiz][pIndex[prvi]];
				najkraci = prvi;
				for(i=nJob; i<nPoslova; i++)
				{	k = pIndex[i];
					if(dClock < Ready[nNiz][k] || Precedence[k][0] > 0)
						continue;
					if(Duration[nNiz][k] < najkrace)	// najkrace
					{	najkrace = Duration[nNiz][k];
						najkraci = i;
					}
				}
				// sad pogledamo najduljega koji pocinje <= zavrsetka najkraceg raspolozivog
				for(i=nJob; i<nPoslova; i++)
				{	k = pIndex[i];
					if(Precedence[k][0] > 0)
						continue;
					if(Duration[nNiz][k] > najdulje && Ready[nNiz][k] <= (dClock+najkrace))	// gledamo najdulje trajanje
						najdulje = Duration[nNiz][k];
				}
				// sada je (dClock + najkrace + najdulje) limit za gledanje u buducnost!

				for(int kk = 0; kk<ensenbleCount; kk++){
					Evaluator.m_iPosition = -1;
					Evaluator.m_iPosition = -1;
					Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!
					Evaluator.m_nTree = kk;
					Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
					memcpy((ValuesArray[kk])[0],pVrijednostiArray[kk],nPoslova*sizeof(double));					
				}

				//Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				// prva verzija (kompliciranija)
				// gledat cemo sve cije vrijeme dolaska je prije zavrsetka najduljeg trenutno raspolozivog
					// MODIFIKACIJA (09.09.): gledamo sve koji mogu poceti prije zavrsetka najkraceg + trajanje do tada spremnog najduljeg!
					// (pronadjemo najduljeg koji moze poceti prije zavrsetka najkraceg)
				// tada: ako se prije odabranoga moze ugurati neki kraci, odaberemo najboljeg kraceg
				//kada = najdulje + 1;	// poc. vrijednost za dolazak trenutno odabranog
				kada = najkrace + najdulje;
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				
				vector<int> votes;
				for(int kk = 0; kk<nPoslova; kk++){
					votes.push_back(0);
				}

				vector<double> prioritet;
				for(int kk=0; kk<nPoslova; kk++){
					prioritet.push_back(0);
				}
				
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
				{	k = pIndex[i];
					if(Precedence[k][0] == 0 && /*(pVrijednosti[k] < dBest)*/ (Ready[nNiz][k] < dClock + kada))
					{
						for(int kk=0; kk<ensenbleCount; kk++){
							prioritet[i]+=(ValuesArray[kk])[0][k];
							//cout<<"ValuesArrat: "<<(ValuesArray[kk])[0][i]<<endl;
						}
					}
				}


				for(int kk = 0; kk<ensenbleCount; kk++){				
					nOdabrani = najkraci;
					dBest = (ValuesArray[kk])[0][pIndex[najkraci]];
					for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
					{	k = pIndex[i];
						if(Precedence[k][0] == 0 && ((ValuesArray[kk])[0][k] < dBest) && (Ready[nNiz][k] < dClock + kada))
						{	
							dBest = (ValuesArray[kk])[0][k];
							nOdabrani = i;
						}
					}
					votes[nOdabrani]++;
				}

				int od = 0;

				for(int kk = 0; kk<nPoslova; kk++){
					if(votes[kk]>votes[od]/*||(votes[kk]==votes[od]&&prioritet[kk]<prioritet[od])*/){
						od = kk;
						nOdabrani = od;
					}
				}



				for(int kk = 0; kk<nPoslova; kk++){
					votes[kk]=0;
				}

				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?
				if(kada >= najkrace)	// ako stane jos barem jedan, odaberimo najbolji koji ce zavrsiti prije pocetka odabranog
				{	dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
					//nOdabrani = najkraci;
					
					for(int kk = 0; kk<ensenbleCount; kk++){				
						nOdabrani = najkraci;
						dBest = (ValuesArray[kk])[0][pIndex[najkraci]];
						for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
						{	k = pIndex[i];
							if(Precedence[k][0] == 0 && (Ready[nNiz][k] + Duration[nNiz][k] <= dClock + kada) \
							&& ((ValuesArray[kk])[0][k] < dBest)
							&& Ready[nNiz][k] - dClock < najkrace)
							{	dBest = (ValuesArray[kk])[0][k];
								nOdabrani = i;
							}
						}
						votes[nOdabrani]++;
					}


				for(int kk = 0; kk<nPoslova; kk++){
					if(votes[kk]>votes[od]){
						od = kk;
						nOdabrani = od;
					}
				}

					kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// novi odabrani
				}

/*				// druga verzija (jednostavnija)
				// samo gledamo najboljega koji moze poceti prije zavrsetka najkraceg raspolozivog
				dBest = pVrijednosti[pIndex[najkraci]]; // poc. vrijednost za usporedbu
				nOdabrani = najkraci;
				for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + najkrace)
				{	if((pVrijednosti[pIndex[i]] < dBest) && (Ready[nNiz][pIndex[i]] < dClock + najkrace) \
						&& Precedence[pIndex[i]][0] == 0)
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
				kada = Ready[nNiz][pIndex[nOdabrani]] - dClock;	// za koliko pocinje odabrani?
*/
				// redovni nastavak (iza 1. i 2. verzije)
				if(kada > 0)	// odabrali smo cekati
					dClock += kada;	// ili: dClock = Ready[nNiz][pIndex[nOdabrani]];
			}	// endif - m_dynamic

			else	// staticki
			{	CalcTimedTerminals(nNiz,nPoslova,nJob,dClock);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			dClock += Duration[nNiz][pIndex[nJob]];	// update vremena
			dSPr -= Duration[nNiz][pIndex[nJob]];		// update trajanja preostalih
			dSDr -= Deadline[nNiz][pIndex[nJob]];		// update deadlinea
			nNr--;											// update broja preostalih
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			if(m_setup)	// trajanje postavljanja
			{	dClock += Setup[nLastJob][pIndex[nJob]];
				nLastJob = pIndex[nJob];	// sljedeci prethodni posao
			}
			Schedule[nNiz][nJob] = pIndex[nJob];	// zapisemo u raspored
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = dAvgDuration = 0;
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = pIndex[nJob];
				dAvgWeights += WeightT[nNiz][index];
				dAvgDuration += Duration[nNiz][index];
				if(m_dynamic && dClock < Ready[nNiz][index])	// ako jos nije raspoloziv
					dClock = Ready[nNiz][index];
				if(m_setup)
					dClock += Setup[nLastJob][index];
				nLastJob = index;
				dClock += Duration.data[nNiz][index];	// update vremena
				dRez = dClock - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
			}
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// promijenjeno (valjda konacno) 04.09.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dAvgDuration /= nPoslova;
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			double Cmax = dClock / (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case TwtCmax: dRawFitness += dTardiness * Cmax; break;
				case NwtCmax: dRawFitness += dNwt * Cmax; break;
				default: exit(1);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
		}
	}
// kraj petlje koja vrti skupove poslova
	Fitness.data[sets][Twt] = dTotalTardiness;
	Fitness.data[sets][Nwt] = dTotalNwt;
}





// obrada za UNIFORM okruzenje
// implementirani terminali: pt,w,dd,SPD,SL,Sls,Msm,STP,Sav,Nr,SP
void SchedulingEvalOp::EvaluateUniform(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0, dTotalFwt=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dMsum, dSetupTime, dFwt, dCmax, dTotalCmax=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		nMachines = (uint) Machines[nNiz][0];
		dMsum = 0;
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			dMsum += 1/Speed[nNiz][j];	// suma brzina svih strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}
	// postavljanje pocetnih vrijednosti terminala
		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		dSPr = Evaluator.m_pTermValues[T_SP] = SP.Get(nNiz);
		Evaluator.m_pTermValues[T_SD] = SD.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina;
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				if(kada > dClock)	// prvo vrijeme kad imamo raspoloziv i stroj i posao
					dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine);

				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				// samo gledamo najboljega koji moze poceti 
				dBest = pVrijednosti[pIndex[prvi]]; // poc. vrijednost za usporedbu
				nOdabrani = prvi;
				for(i=nJob; i<nPoslova; i++)
				{	if((pVrijednosti[pIndex[i]] < dBest) && Precedence[pIndex[i]][0] == 0 \
						&& Ready[nNiz][pIndex[i]] <= dClock)
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// endif - m_dynamic
			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;

			//dSPr -= Duration.data[nNiz][pIndex[nJob]];		// update trajanja preostalih
			dSPr -= Duration.data[nNiz][pIndex[nJob]]*dMsum;	// zapravo bi trebalo ovako!
			nNr--;											// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][pIndex[nJob]];
				pLastJob[nMachine] = pIndex[nJob];	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][pIndex[nJob]] * Speed[nNiz][nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = pIndex[nJob] + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu
		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index] * Speed[nNiz][nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				dFwt = (MachineReady[nMachine][0] - Ready[nNiz][index]) * WeightT[nNiz][index];	// tezinsko protjecanje
			}
			dCmax = 0;	// odredjivanje Cmax
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// i o prosjecnom trajanju - dodano 15.09.
			dAvgDuration = SP[nNiz][0] / nPoslova;	// prosjecno trajanje posla
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= (nPoslova * dAvgWeights * dAvgDuration);
			dCmax /= (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: throw;
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][FTwt] = 0;
	Fitness[sets][ETwt] = 0;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}

// obrada za UNIFORM okruzenje
// implementirani terminali: pt,w,dd,SPD,SL,Sls,Msm,STP,Sav,Nr,SP
void SchedulingEvalOp::EvaluateUniformSum(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0, dTotalFwt=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dMsum, dSetupTime, dFwt, dCmax, dTotalCmax=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		nMachines = (uint) Machines[nNiz][0];
		dMsum = 0;
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			dMsum += 1/Speed[nNiz][j];	// suma brzina svih strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}
	// postavljanje pocetnih vrijednosti terminala
		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		dSPr = Evaluator.m_pTermValues[T_SP] = SP.Get(nNiz);
		Evaluator.m_pTermValues[T_SD] = SD.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina;
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				if(kada > dClock)	// prvo vrijeme kad imamo raspoloziv i stroj i posao
					dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine);


				
				for(int kk = 0; kk<ensenbleCount; kk++){
					Evaluator.m_iPosition = -1;
					Evaluator.m_nTree = kk;
					Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
					memcpy((ValuesArray[kk])[0],pVrijednostiArray[kk],nPoslova*sizeof(double));					
				}
				//Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				// samo gledamo najboljega koji moze poceti 
				
				
				
				vector<double> prioritet;

				for(int kk=0; kk<nPoslova; kk++){
					prioritet.push_back(0);
				}
				
				for(i=nJob; i<nPoslova; i++)
				{	if(/*(pVrijednosti[pIndex[i]] < dBest) && */Precedence[pIndex[i]][0] == 0 \
						&& Ready[nNiz][pIndex[i]] <= dClock)
					{
						for(int kk=0; kk<ensenbleCount; kk++){
							prioritet[i]+=ValuesArray[kk][0][pIndex[i]];
						}
					}
				}
				
				dBest = prioritet[prvi]; // poc. vrijednost za usporedbu
				nOdabrani = prvi;
				for(i=nJob; i<nPoslova; i++)
				{	if((prioritet[i] < dBest) && Precedence[pIndex[i]][0] == 0 \
						&& Ready[nNiz][pIndex[i]] <= dClock)
					{	dBest = prioritet[i];
						nOdabrani = i;
					}
				}
			}	// endif - m_dynamic
			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;

			//dSPr -= Duration.data[nNiz][pIndex[nJob]];		// update trajanja preostalih
			dSPr -= Duration.data[nNiz][pIndex[nJob]]*dMsum;	// zapravo bi trebalo ovako!
			nNr--;											// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][pIndex[nJob]];
				pLastJob[nMachine] = pIndex[nJob];	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][pIndex[nJob]] * Speed[nNiz][nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = pIndex[nJob] + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu
		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index] * Speed[nNiz][nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				dFwt = (MachineReady[nMachine][0] - Ready[nNiz][index]) * WeightT[nNiz][index];	// tezinsko protjecanje
			}
			dCmax = 0;	// odredjivanje Cmax
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// i o prosjecnom trajanju - dodano 15.09.
			dAvgDuration = SP[nNiz][0] / nPoslova;	// prosjecno trajanje posla
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= (nPoslova * dAvgWeights * dAvgDuration);
			dCmax /= (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: throw;
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][FTwt] = 0;
	Fitness[sets][ETwt] = 0;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}


// obrada za UNIFORM okruzenje
// implementirani terminali: pt,w,dd,SPD,SL,Sls,Msm,STP,Sav,Nr,SP
void SchedulingEvalOp::EvaluateUniformVote(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dAvgDuration;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0, dTotalFwt=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dMsum, dSetupTime, dFwt, dCmax, dTotalCmax=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			pIndex[i] = i;	// inicijalni poredak
		}
		nMachines = (uint) Machines[nNiz][0];
		dMsum = 0;
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			dMsum += 1/Speed[nNiz][j];	// suma brzina svih strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}
	// postavljanje pocetnih vrijednosti terminala
		Evaluator.m_pTermValues[T_N] = N.Get(nNiz);
		dSPr = Evaluator.m_pTermValues[T_SP] = SP.Get(nNiz);
		Evaluator.m_pTermValues[T_SD] = SD.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
		}
		memcpy(Evaluator.m_dTermValuesArray[T_pt], Duration.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina;
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				if(kada > dClock)	// prvo vrijeme kad imamo raspoloziv i stroj i posao
					dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine);

				for(int kk = 0; kk<ensenbleCount; kk++){
					Evaluator.m_iPosition = -1;
					Evaluator.m_nTree = kk;
					Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
					memcpy((ValuesArray[kk])[0],pVrijednostiArray[kk],nPoslova*sizeof(double));					
				}


				vector<double> prioritet;
				vector<int> votes;
				for(int kk=0; kk< nPoslova; kk++){
					votes.push_back(0);	
					prioritet.push_back(0);
				}

				for(i=nJob; i<nPoslova; i++)
				{	if(/*(pVrijednosti[pIndex[i]] < dBest) && */Precedence[pIndex[i]][0] == 0 \
						&& Ready[nNiz][pIndex[i]] <= dClock)
					{
						for(int kk=0; kk<ensenbleCount; kk++){
							prioritet[i]+=ValuesArray[kk][0][pIndex[i]];
						}
					}
				}

				for(int kk = 0; kk < ensenbleCount; kk++){
					dBest = ValuesArray[kk][0][pIndex[prvi]]; // poc. vrijednost za usporedbu
					nOdabrani = prvi;
					for(i=nJob; i<nPoslova; i++)
					{	if((ValuesArray[kk][0][pIndex[i]] < dBest) && Precedence[pIndex[i]][0] == 0 \
							&& Ready[nNiz][pIndex[i]] <= dClock)
					{	dBest = ValuesArray[kk][0][pIndex[i]];
							nOdabrani = i;
						}

					}
					votes[nOdabrani]++;
				}

				int od = 0;
				nOdabrani= 0;
				for(int kk = 0; kk<nPoslova; kk++){
					if(votes[kk]>votes[od]){
						od = kk;
						nOdabrani = od;
					}
					
				}


				//	vector<double> prioritet;

				//for(int kk=0; kk<nPoslova; kk++){
				//	prioritet.push_back(0);
				//}
				//
				//for(i=nJob; i<nPoslova; i++)
				//{	if(/*(pVrijednosti[pIndex[i]] < dBest) && */Precedence[pIndex[i]][0] == 0 \
				//		&& Ready[nNiz][pIndex[i]] <= dClock)
				//	{
				//		for(int kk=0; kk<ensenbleCount; kk++){
				//			prioritet[i]+=ValuesArray[kk][0][pIndex[i]];
				//		}
				//	}
				//}
				//
				//dBest = prioritet[prvi]; // poc. vrijednost za usporedbu
				//nOdabrani = prvi;
				//for(i=nJob; i<nPoslova; i++)
				//{	if((prioritet[i] < dBest) && Precedence[pIndex[i]][0] == 0 \
				//		&& Ready[nNiz][pIndex[i]] <= dClock)
				//	{	dBest = prioritet[i];
				//		nOdabrani = i;
				//	}
				//}


				//Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				// samo gledamo najboljega koji moze poceti 
				//dBest = pVrijednosti[pIndex[prvi]]; // poc. vrijednost za usporedbu
				//nOdabrani = prvi;
				//for(i=nJob; i<nPoslova; i++)
				//{	if((pVrijednosti[pIndex[i]] < dBest) && Precedence[pIndex[i]][0] == 0 \
				//		&& Ready[nNiz][pIndex[i]] <= dClock)
				//	{	dBest = pVrijednosti[pIndex[i]];
				//		nOdabrani = i;
				//	}
				//}
			}	// endif - m_dynamic
			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;

			//dSPr -= Duration.data[nNiz][pIndex[nJob]];		// update trajanja preostalih
			dSPr -= Duration.data[nNiz][pIndex[nJob]]*dMsum;	// zapravo bi trebalo ovako!
			nNr--;											// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][pIndex[nJob]];
				pLastJob[nMachine] = pIndex[nJob];	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[pIndex[nJob]][1]; i++)
				{	j = (int) Precedence[pIndex[nJob]][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][pIndex[nJob]] * Speed[nNiz][nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = pIndex[nJob] + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu
		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index] * Speed[nNiz][nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				dFwt = (MachineReady[nMachine][0] - Ready[nNiz][index]) * WeightT[nNiz][index];	// tezinsko protjecanje
			}
			dCmax = 0;	// odredjivanje Cmax
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			// i o prosjecnom trajanju - dodano 15.09.
			dAvgDuration = SP[nNiz][0] / nPoslova;	// prosjecno trajanje posla
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= (nPoslova * dAvgWeights * dAvgDuration);
			dCmax /= (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: throw;
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][FTwt] = 0;
	Fitness[sets][ETwt] = 0;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}



// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelated(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = 0;
					Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
					if(false){
					Evaluator.m_nTree = 1;
					Evaluator.m_iPosition = -1;
					Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					Evaluator.m_iPosition = -1;
					Evaluator.m_nTree = 2;
					Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					}
					memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				uint nBestMachine, nBestJobMachine;
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					for(nMachine=1; nMachine<nMachines; nMachine++)
						if(Values[nBestJobMachine][pIndex[i]] < Values[nMachine][pIndex[i]])
							nBestJobMachine = nMachine;
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
						dBest = Values[nBestMachine][pIndex[i]];
						nOdabrani = i;
						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	if(Values[nBestJobMachine][pIndex[i]] < dBest)
						{	nBestMachine = nBestJobMachine;
							dBest = Values[nBestJobMachine][pIndex[i]];
							nOdabrani = i;
						}
					}
				}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}


void SchedulingEvalOp::EvaluateUnrelatedStatic(double &dRawFitness)
{
	double lookahead=0;
	int largestReleaseTime = 0;

	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}
		largestReleaseTime = Ready[nNiz][pIndex[nPoslova-1]];

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=(dClock+(largestReleaseTime-dClock)*lookahead); i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines,dClock+(largestReleaseTime-dClock)*lookahead);
					CalcStaticTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines, MachineReady,dClock+(largestReleaseTime-dClock)*lookahead);
					Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = 0;
					Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
					if(false){
					Evaluator.m_nTree = 1;
					Evaluator.m_iPosition = -1;
					Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					Evaluator.m_iPosition = -1;
					Evaluator.m_nTree = 2;
					Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					}
					memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				uint nBestMachine, nBestJobMachine;
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > (dClock+(largestReleaseTime-dClock)*lookahead))
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					for(nMachine=1; nMachine<nMachines; nMachine++)
						if(Values[nBestJobMachine][pIndex[i]] < Values[nMachine][pIndex[i]])
							nBestJobMachine = nMachine;
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
						dBest = Values[nBestMachine][pIndex[i]];
						nOdabrani = i;
						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	if(Values[nBestJobMachine][pIndex[i]] < dBest)
						{	nBestMachine = nBestJobMachine;
							dBest = Values[nBestJobMachine][pIndex[i]];
							nOdabrani = i;
						}
					}
				}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
			//cout<< "Tardi"<< dTardiness <<endl;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}


// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelatedRollout(double &dRawFitness)
{
	double lookahead = 0;
	 int largestReleaseTime = 0;


	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr, zadnji;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	cout<<nNiz;
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		int dOldClock = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}
		largestReleaseTime = Ready[nNiz][pIndex[nPoslova-1]];

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

				Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;


		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));


		double previousBestPriority = DBL_MAX;
		double previousClock = 0;
		Matrica previousSchedule;
		Matrica previousMachineReady;
		unsigned int *previouspIndex = new unsigned int[nPoslova];
		unsigned int *previousPlastJob = new unsigned int[nMachines];


		for(nJob=0; nJob<nPoslova; nJob++){	// rasporedjujemo svaki posao unutar skupa
			
			
				// dinamicka okolina; zapravo simulacija online rasporedjivanja

				// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dOldClock = dClock;
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=prvi; i<nPoslova && Ready[nNiz][pIndex[i]]<=(dClock+(largestReleaseTime-dClock)*lookahead); i++) NULL;
				Evaluator.m_iEnd = i;
				zadnji = i;

				int chosenMachine = 0;
				int chosenJob = 0;
				double priority = DBL_MAX;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				//cout<<prvi<<endl;





				for(nMachine=0; nMachine<nMachines; nMachine++)
				{
					for(unsigned int jj = nJob; jj<nPoslova && Ready[nNiz][pIndex[jj]]<=(dClock+(largestReleaseTime-dClock)*0.05); jj++)
					{
						
						// prepisivanje u nove
						double fitnessValue = 0;
						int trenutniNiz = nNiz;
						unsigned int *novipIndex = new unsigned int[nPoslova];
						Matrica newMachineReady = Matrica(MachineReady);
						Matrica newSchedule = Matrica(Schedule);
						unsigned int *newPlastJob = new unsigned int[nMachines];
						int startTime = dOldClock;

						if(dOldClock<newMachineReady[nMachine][0])
						{
							startTime= newMachineReady[nMachine][0];
						}
						if(startTime< Ready[nNiz][pIndex[jj]])
						{
							startTime = Ready[nNiz][pIndex[jj]];
						}

						newMachineReady[nMachine][0] = startTime + Duration[nNiz][pIndex[jj]*nMachines + nMachine];

						for(int bb = 0; bb<nPoslova; bb++)
						{
							novipIndex[bb] = pIndex[bb];
						}

						for(int bb = 0; bb<nMachines; bb++)
						{
							newPlastJob[bb] = pLastJob[bb];
						}

						//inicijalizacija novih!
						newSchedule[nNiz][nJob] = pIndex[jj]+nMachine*1000;
						
						

						unsigned int od = novipIndex[jj];

						/*i = pIndex[nJob];
						pIndex[nJob] = pIndex[jj];
						pIndex[jj] = i;*/

						i = pIndex[jj];
						for(int ll = jj; ll>nJob; ll--)
						{
							pIndex[ll] = pIndex[ll-1];
						}
												
						pIndex[nJob] = i;
			
		EvaluateUnrelatedRolloutH(fitnessValue, nNiz, lookahead, largestReleaseTime, newSchedule, newMachineReady, nJob+1);

					//	cout<<"Machine "<<nMachine<<" job " << pIndex[nJob]<< " fitness "<< fitnessValue <<endl;

						if(priority>fitnessValue)
						{
							priority = fitnessValue;
							chosenJob = jj;
							chosenMachine = nMachine;
						}

						//vrati nazad
						for(int bb = 0; bb<nPoslova; bb++)
						{
							pIndex[bb] = novipIndex[bb];
						}

						for(int bb = 0; bb<nMachines; bb++)
						{
							pLastJob[bb] = newPlastJob[bb];
						}

						//obrisi 
						delete[] novipIndex;
						delete[] newPlastJob;


					}
				}
				//cout<<"Prior: "<<priority<<endl;
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				
			
				
				
				nOdabrani = chosenJob;
				nMachine = chosenMachine;


			
			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			//i = pIndex[nJob];
			//pIndex[nJob] = pIndex[nOdabrani];
			//pIndex[nOdabrani] = i;
		//	pRasporedjen[pIndex[nJob]] = true;
		//	nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla



		if(previousBestPriority<priority)
		{
			Schedule = Matrica(previousSchedule);
			MachineReady = Matrica(previousMachineReady);
			for(int bb = 0; bb<nPoslova; bb++)
			{
				pIndex[bb] = previouspIndex[bb];
			}
			for(int bb = 0; bb<nMachines; bb++)
			{
				pLastJob[bb] = previousPlastJob[bb];
			}

			dClock = previousClock;

			//nJob--;


		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = nJob;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir


			unsigned int raspolozivi = nJob; //prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						//prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=(dClock+(largestReleaseTime-dClock)*lookahead); i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines, MachineReady);
					Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = 0;
					Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
					memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				uint nBestMachine, nBestJobMachine;
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > (dClock+(largestReleaseTime-dClock)*lookahead))
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					for(nMachine=1; nMachine<nMachines; nMachine++)
						if(Values[nBestJobMachine][pIndex[i]] < Values[nMachine][pIndex[i]])
							nBestJobMachine = nMachine;
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
						dBest = Values[nBestMachine][pIndex[i]];
						nOdabrani = i;
						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	if(Values[nBestJobMachine][pIndex[i]] < dBest)
						{	nBestMachine = nBestJobMachine;
							dBest = Values[nBestJobMachine][pIndex[i]];
							nOdabrani = i;
						}
					}
				}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
            


			previousSchedule = Matrica(Schedule);
			previousMachineReady = Matrica(MachineReady);
			for(int bb = 0; bb<nPoslova; bb++)
			{
				previouspIndex[bb] = pIndex[bb];
			}
			for(int bb = 0; bb<nMachines; bb++)
			{
				previousPlastJob[bb] = pLastJob[bb];
			}

			previousClock = dClock;

					
					
					
					continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;



		} else
		{




						i = pIndex[nOdabrani];
						for(int ll = nOdabrani; ll>nJob; ll--)
						{
							pIndex[ll] = pIndex[ll-1];
						}
												
						pIndex[nJob] = i;

			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}

			int startTime = 0;
			if(dClock<=MachineReady[nMachine][0])
			{
				startTime= MachineReady[nMachine][0];
			}
			if(startTime< Ready[nNiz][nOdabrani])
			{
				startTime = Ready[nNiz][nOdabrani];
			}

			MachineReady[nMachine][0] = startTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;



			previousBestPriority = priority;
			
			
			previousSchedule = Matrica(Schedule);
			previousMachineReady = Matrica(MachineReady);

			for(int bb = 0; bb<nPoslova; bb++)
			{
				previouspIndex[bb] = pIndex[bb];
			}
			for(int bb = 0; bb<nMachines; bb++)
			{
				previousPlastJob[bb] = pLastJob[bb];
			}
			
			previousClock = dOldClock;

		}



		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;

			cout<< "fit:" << dTardiness<< endl;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}


// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelatedRolloutH(double &dRawFitness, unsigned int nNiz, double lookahead, int largestReleaseTime, Matrica Schedule, Matrica MachineReady, unsigned int prvi)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness=0;

// vrtimo sve skupove

		nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling

	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nMachines = (uint) Machines[nNiz][0];
		nLastJob = nPoslova;
		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=prvi; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob; //prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						//prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=(dClock+(largestReleaseTime-dClock)*lookahead); i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines, MachineReady);
					Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = 0;
					Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve
					memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				uint nBestMachine, nBestJobMachine;
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > (dClock+(largestReleaseTime-dClock)*lookahead))
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					for(nMachine=1; nMachine<nMachines; nMachine++)
						if(Values[nBestJobMachine][pIndex[i]] < Values[nMachine][pIndex[i]])
							nBestJobMachine = nMachine;
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
						dBest = Values[nBestMachine][pIndex[i]];
						nOdabrani = i;
						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	if(Values[nBestJobMachine][pIndex[i]] < dBest)
						{	nBestMachine = nBestJobMachine;
							dBest = Values[nBestJobMachine][pIndex[i]];
							nOdabrani = i;
						}
					}
				}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			dSetupTime = 0;

			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
		}

}

// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelated2(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				

				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				int voting[12];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					for(int kk = 0; kk < ensenbleCount; kk++){
						nBestJobMachineArray[kk] = 0;
					}
					for(nMachine=0; nMachine<nMachines; nMachine++){
						for(int kk = 0; kk<ensenbleCount; kk++){
							//printf("%d", kk);
							if((ValuesArray[kk])[nBestJobMachineArray[kk]][pIndex[i]] < (ValuesArray[kk])[nMachine][pIndex[i]]){
								nBestJobMachineArray[kk] = nMachine;
							}
						}
					/*	if(Values1[nBestJobMachine1][pIndex[i]] < Values1[nMachine][pIndex[i]]){
							nBestJobMachine1 = nMachine;
						}
						if(Values2[nBestJobMachine2][pIndex[i]] < Values2[nMachine][pIndex[i]]){
							nBestJobMachine2 = nMachine;
						}*/
					}


					//implementation of voting
					voting[0] = 1;
					for(int kk=1; kk<ensenbleCount; kk++){
						voting[kk] = 0;
						for(int jj = 0; jj< kk; jj++){
							if(nBestJobMachineArray[jj]==nBestJobMachineArray[kk]){
								voting[jj]++;
								goto label;
							}
						}
						voting[kk] = 1;
						continue;
label: 
						voting[kk] =0;
					}
					
					nBestJobMachine = nBestJobMachineArray[0];
					int indexx = 0;
					for(int kk=1; kk<ensenbleCount; kk++){
						if(voting[kk]>voting[indexx]){
							indexx=kk;
							nBestJobMachine = nBestJobMachineArray[kk];
						}
					}
				//	printf("%d", indexx);

		/*			if(nBestJobMachine1==nBestJobMachine2){
						nBestJobMachine = nBestJobMachine1;
					}*/

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					//if(MachineReady[nBestJobMachine1][0] > dClock)
					//	continue;	// tebe necemo sada rasporediti...
					//if(MachineReady[nBestJobMachine2][0] > dClock)
					//	continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = ValuesArray[indexx][nBestMachine][pIndex[i]];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;
					//	nOdabrani1 = i;
					//	nOdabrani2 = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("1.dio \n");
					//	printf("%f \n", ValuesArray[indexx][nBestJobMachine][pIndex[i]]);
				//		printf("%f \n", dBest);
						//if(ValuesArray[indexx][nBestJobMachine][pIndex[i]] < dBest)
						//{	nBestMachine = nBestJobMachine;

						//	dBest = ValuesArray[indexx][nBestJobMachine][pIndex[i]];
						//	nOdabrani = i;
						//}
					



					//	printf("2.dio \n");
						//printf("db %f \n", dBest);
						int curr = 0; int next = 0;
						for(int kk=0; kk<ensenbleCount;kk++){
//							printf("%f \n", ValuesArray[kk][nBestJobMachine][pIndex[i]]);
//							printf("mat %f \n", ValuesArray[kk][nBestMachine][pindex]);
							
							
							if(ValuesArray[kk][nBestJobMachine][pIndex[i]] < ValuesArray[kk][nBestMachine][pindex])
							{	
								next++;
							} else{
								curr++;
							}	

							
						}
						
						if(next>curr){
						//	printf("tu sam \n");
								pindex = pIndex[i];

								nBestMachine = nBestJobMachine;
								dBest = ValuesArray[indexx][nBestJobMachine][pIndex[i]];
								nOdabrani = i;
							}

					//if(Values1[nBestJobMachine1][pIndex[i]] < dBest1)
					//	{	nBestMachine1 = nBestJobMachine1;
					//		dBest1 = Values1[nBestJobMachine1][pIndex[i]];
					//		nOdabrani1 = i;
					//	}
					//if(Values2[nBestJobMachine2][pIndex[i]] < dBest2)
					//	{	nBestMachine2 = nBestJobMachine2;
					//		dBest2 = Values2[nBestJobMachine2][pIndex[i]];
					//		nOdabrani2 = i;
					//	}
					}
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}



void SchedulingEvalOp::EvaluateUnrelated2Confidences(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				

				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				double voting[12];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					for(int kk = 0; kk < ensenbleCount; kk++){
						nBestJobMachineArray[kk] = 0;
					}
					for(nMachine=1; nMachine<nMachines; nMachine++){
						for(int kk = 0; kk<ensenbleCount; kk++){
							//printf("%d", kk);
							if((ValuesArray[kk])[nBestJobMachineArray[kk]][pIndex[i]] < (ValuesArray[kk])[nMachine][pIndex[i]]){
								nBestJobMachineArray[kk] = nMachine;
							}
						}
					/*	if(Values1[nBestJobMachine1][pIndex[i]] < Values1[nMachine][pIndex[i]]){
							nBestJobMachine1 = nMachine;
						}
						if(Values2[nBestJobMachine2][pIndex[i]] < Values2[nMachine][pIndex[i]]){
							nBestJobMachine2 = nMachine;
						}*/
					}


					//implementation of voting
					voting[0] = confidences[0];
					for(int kk=1; kk<ensenbleCount; kk++){
						voting[kk] = 0;
						for(int jj = 0; jj< kk; jj++){
							if(nBestJobMachineArray[jj]==nBestJobMachineArray[kk]){
								voting[jj]+=confidences[kk];
								goto label;
							}
						}
						voting[kk] = confidences[kk];
						continue;
label: 
						voting[kk] =0;
					}
					
					nBestJobMachine = nBestJobMachineArray[0];
					int indexx = 0;
					for(int kk=1; kk<ensenbleCount; kk++){
						if(voting[kk]>voting[indexx]){
							indexx=kk;
							nBestJobMachine = nBestJobMachineArray[kk];
						}
					}
				//	printf("%d", indexx);

		/*			if(nBestJobMachine1==nBestJobMachine2){
						nBestJobMachine = nBestJobMachine1;
					}*/

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					//if(MachineReady[nBestJobMachine1][0] > dClock)
					//	continue;	// tebe necemo sada rasporediti...
					//if(MachineReady[nBestJobMachine2][0] > dClock)
					//	continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = ValuesArray[indexx][nBestMachine][pIndex[i]];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;
					//	nOdabrani1 = i;
					//	nOdabrani2 = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("1.dio \n");
					//	printf("%f \n", ValuesArray[indexx][nBestJobMachine][pIndex[i]]);
				//		printf("%f \n", dBest);
						//if(ValuesArray[indexx][nBestJobMachine][pIndex[i]] < dBest)
						//{	nBestMachine = nBestJobMachine;

						//	dBest = ValuesArray[indexx][nBestJobMachine][pIndex[i]];
						//	nOdabrani = i;
						//}
					



					//	printf("2.dio \n");
						//printf("db %f \n", dBest);
						int curr = 0; int next = 0;
						for(int kk=0; kk<ensenbleCount;kk++){
//							printf("%f \n", ValuesArray[kk][nBestJobMachine][pIndex[i]]);
//							printf("mat %f \n", ValuesArray[kk][nBestMachine][pindex]);
							
							
							if(ValuesArray[kk][nBestJobMachine][pIndex[i]] < ValuesArray[kk][nBestMachine][pindex])
							{	
								next++;
							} else{
								curr++;
							}	

							
						}
						
						if(next>curr){
						//	printf("tu sam \n");
								pindex = pIndex[i];

								nBestMachine = nBestJobMachine;
								dBest = ValuesArray[indexx][nBestJobMachine][pIndex[i]];
								nOdabrani = i;
							}

					//if(Values1[nBestJobMachine1][pIndex[i]] < dBest1)
					//	{	nBestMachine1 = nBestJobMachine1;
					//		dBest1 = Values1[nBestJobMachine1][pIndex[i]];
					//		nOdabrani1 = i;
					//	}
					//if(Values2[nBestJobMachine2][pIndex[i]] < dBest2)
					//	{	nBestMachine2 = nBestJobMachine2;
					//		dBest2 = Values2[nBestJobMachine2][pIndex[i]];
					//		nOdabrani2 = i;
					//	}
					}
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}





// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelatedSumOfPriorities(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				
	


				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				//int voting[10];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
							double priority[12];	
				nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					//for(int kk = 0; kk < ensenbleCount; kk++){
					//	nBestJobMachineArray[kk] = 0;
					//}
					for(nMachine=0; nMachine<nMachines; nMachine++){
						priority[nMachine]=0.0f;
						for(int kk = 0; kk<ensenbleCount; kk++){
							priority[nMachine] +=(ValuesArray[kk])[nMachine][pIndex[i]];
						}
					}


					nBestJobMachine=0;
					for(nMachine=1; nMachine<nMachines; nMachine++){
						if(priority[nBestJobMachine]<priority[nMachine]){
							nBestJobMachine = nMachine;
						}
					}

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = priority[nBestJobMachine];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("nBestJobMachine: %f \n", priority[nBestJobMachine]);
						//printf("dBest: %f \n", dBest);

						if(priority[nBestJobMachine] < dBest){
							nBestMachine = nBestJobMachine;
							dBest = priority[nBestJobMachine];
							nOdabrani = i;
						}
					}
					//delete priority;
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}


void SchedulingEvalOp::EvaluateUnrelatedBoosting(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				
	


				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				//int voting[10];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
							double priority[12];	
				nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					//for(int kk = 0; kk < ensenbleCount; kk++){
					//	nBestJobMachineArray[kk] = 0;
					//}
					for(nMachine=0; nMachine<nMachines; nMachine++){
						priority[nMachine]=0.0f;
						for(int kk = 0; kk<ensenbleCount; kk++){
							priority[nMachine] +=(ValuesArray[kk])[nMachine][pIndex[i]];
						}
					}


					nBestJobMachine=0;
					for(nMachine=1; nMachine<nMachines; nMachine++){
						if(priority[nBestJobMachine]<priority[nMachine]){
							nBestJobMachine = nMachine;
						}
					}

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = priority[nBestJobMachine];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("nBestJobMachine: %f \n", priority[nBestJobMachine]);
						//printf("dBest: %f \n", dBest);

						if(priority[nBestJobMachine] < dBest){
							nBestMachine = nBestJobMachine;
							dBest = priority[nBestJobMachine];
							nOdabrani = i;
						}
					}
					//delete priority;
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}


void SchedulingEvalOp::EvaluateUnrelatedSumOfPrioritiesConfidences(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				
	


				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				//int voting[10];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
							double priority[12];	
				nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					//for(int kk = 0; kk < ensenbleCount; kk++){
					//	nBestJobMachineArray[kk] = 0;
					//}
					for(nMachine=0; nMachine<nMachines; nMachine++){
						priority[nMachine]=0.0f;
						for(int kk = 0; kk<ensenbleCount; kk++){
							priority[nMachine] +=confidences[kk]  * (ValuesArray[kk])[nMachine][pIndex[i]];
						}
					}


					nBestJobMachine=0;
					for(nMachine=1; nMachine<nMachines; nMachine++){
						if(priority[nBestJobMachine]<priority[nMachine]){
							nBestJobMachine = nMachine;
						}
					}

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = priority[nBestJobMachine];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("nBestJobMachine: %f \n", priority[nBestJobMachine]);
						//printf("dBest: %f \n", dBest);

						if(priority[nBestJobMachine] < dBest){
							nBestMachine = nBestJobMachine;
							dBest = priority[nBestJobMachine];
							nOdabrani = i;
						}
					}
					//delete priority;
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}





// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelatedWeightedvoting(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				

				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				double voting[20];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
					nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					for(int kk = 0; kk < ensenbleCount; kk++){
						nBestJobMachineArray[kk] = 0;
					}
					for(nMachine=1; nMachine<nMachines; nMachine++){
						for(int kk = 0; kk<ensenbleCount; kk++){
							//printf("%d", kk);
							if((ValuesArray[kk])[nBestJobMachineArray[kk]][pIndex[i]] < (ValuesArray[kk])[nMachine][pIndex[i]]){
								nBestJobMachineArray[kk] = nMachine;
							}
						}
					/*	if(Values1[nBestJobMachine1][pIndex[i]] < Values1[nMachine][pIndex[i]]){
							nBestJobMachine1 = nMachine;
						}
						if(Values2[nBestJobMachine2][pIndex[i]] < Values2[nMachine][pIndex[i]]){
							nBestJobMachine2 = nMachine;
						}*/
					}


					//implementation of voting
					voting[0] = 1;
					for(int kk=1; kk<ensenbleCount; kk++){
						voting[kk] = 0;
						for(int jj = 0; jj< kk; jj++){
							if(nBestJobMachineArray[jj]==nBestJobMachineArray[kk]){
								voting[jj]+=WeightT.data[nNiz][pIndex[i]];
								goto label;
							}
						}
						voting[kk] = WeightT.data[nNiz][pIndex[i]];
						continue;
label: 
						voting[kk] =0;
					}
					
					nBestJobMachine = nBestJobMachineArray[0];
					int indexx = 0;
					for(int kk=1; kk<ensenbleCount; kk++){
						if(voting[kk]>voting[indexx]){
							indexx=kk;
							nBestJobMachine = nBestJobMachineArray[kk];
						}
					}
				//	printf("%d", indexx);

		/*			if(nBestJobMachine1==nBestJobMachine2){
						nBestJobMachine = nBestJobMachine1;
					}*/

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	// tebe necemo sada rasporediti...
					//if(MachineReady[nBestJobMachine1][0] > dClock)
					//	continue;	// tebe necemo sada rasporediti...
					//if(MachineReady[nBestJobMachine2][0] > dClock)
					//	continue;	// tebe necemo sada rasporediti...
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = ValuesArray[indexx][nBestMachine][pIndex[i]];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;
					//	nOdabrani1 = i;
					//	nOdabrani2 = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("1.dio \n");
					//	printf("%f \n", ValuesArray[indexx][nBestJobMachine][pIndex[i]]);
				//		printf("%f \n", dBest);
						//if(ValuesArray[indexx][nBestJobMachine][pIndex[i]] < dBest)
						//{	nBestMachine = nBestJobMachine;

						//	dBest = ValuesArray[indexx][nBestJobMachine][pIndex[i]];
						//	nOdabrani = i;
						//}
					



					//	printf("2.dio \n");
						//printf("db %f \n", dBest);
						int curr = 0; int next = 0;
						for(int kk=0; kk<ensenbleCount;kk++){
//							printf("%f \n", ValuesArray[kk][nBestJobMachine][pIndex[i]]);
//							printf("mat %f \n", ValuesArray[kk][nBestMachine][pindex]);
							
							
							if(ValuesArray[kk][nBestJobMachine][pIndex[i]] < ValuesArray[kk][nBestMachine][pindex])
							{	
								next++;
							} else{
								curr++;
							}	

							
						}
						
						if(next>curr){
						//	printf("tu sam \n");
								pindex = pIndex[i];

								nBestMachine = nBestJobMachine;
								dBest = ValuesArray[indexx][nBestJobMachine][pIndex[i]];
								nOdabrani = i;
							}

					//if(Values1[nBestJobMachine1][pIndex[i]] < dBest1)
					//	{	nBestMachine1 = nBestJobMachine1;
					//		dBest1 = Values1[nBestJobMachine1][pIndex[i]];
					//		nOdabrani1 = i;
					//	}
					//if(Values2[nBestJobMachine2][pIndex[i]] < dBest2)
					//	{	nBestMachine2 = nBestJobMachine2;
					//		dBest2 = Values2[nBestJobMachine2][pIndex[i]];
					//		nOdabrani2 = i;
					//	}
					}
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova  * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}









// obrada za UNRELATED okruzenje
// implementirani terminali: w, dd, pt, SL, pmin, pavg, PAT, MR, age
// ne koristimo u dinamickom: N, Nr, SP, SPr, SD
void SchedulingEvalOp::EvaluateUnrelatedWeightedSumOfPriorities(double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;
					int nOdabrani1, nOdabrani2;
				double dBest1, dBest2;
				double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}

		nMachines = (uint) Machines[nNiz][0];
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
		}

		// postavljanje pocetnih vrijednosti terminala
		dSPr = SP.Get(nNiz);
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		for(i=0; i<nPoslova; i++)
		{	Evaluator.m_dTermValuesArray[T_N][i] = N.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SP][i] = SP.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SD][i] = SD.data[nNiz][0];
			Evaluator.m_dTermValuesArray[T_SC][i] = Precedence[i][1];	// broj sljedbenika
			// najkrace trajanje izvodjenja posla
			Evaluator.m_dTermValuesArray[T_pmin][i] = Duration[nNiz][i*nMachines + (uint)PTimeMinMachine[nNiz][i]];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_LVL], Level[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_pavg], PTimeAvg[nNiz], nPoslova*sizeof(double));

/////////////////////////////////////////////////////////////////////////////////////////////
// ako u terminalima ima vremenski ovisnih, moramo raditi posao po posao
// vektorska evaluacija!
		for(nJob=0; nJob<nPoslova; nJob++)	// rasporedjujemo svaki posao unutar skupa
		{	for(i=nJob; i<nPoslova; i++)	// racunamo nove terminale samo za nerasporedjene
			{	j = pIndex[i];
				Evaluator.m_dTermValuesArray[T_SPr][j] = dSPr;	// suma trajanja preostalih
				Evaluator.m_dTermValuesArray[T_Nr][j] = nNr;	// broj preostalih
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = nJob;	// od kojeg posla pocinjemo - prethodni su vec rasporedjeni!

			if(m_dynamic)	// dinamicka okolina; zapravo simulacija online rasporedjivanja
//
// daklem ovako: 
// - gledamo prvo vrijeme kad je raspoloziv i posao i stroj (barem jedan)
// - ocijenimo sve poslove za sve strojeve
// - gledamo najbolju vrijednost:
// 	- ako je za raspolozivi stroj, rasporedi
// 	- ako je za neraspolozivi stroj, gledaj sljedece vrijednosti (od drugih poslova)
// - ako svi poslovi imaju svoje najbolje vrijednosti za neraspolozive strojeve, NE RASPOREDI nego 
// pomakni vrijeme na prvi sljedeci raspolozivi stroj ILI POSAO i ponovi ispocetka (za sve pristigle poslove!)
// 
			{	// trebamo naci prvi raspolozivi stroj i prvi raspolozivi posao
				unsigned int raspolozivi = nJob, prvi = nJob;
				// trebamo pronaci prvog raspolozivog bez nezavrsenih prethodnika
				for(; Precedence[pIndex[raspolozivi]][0] > 0; raspolozivi++)	NULL;
				double kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
				for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
				{	k = pIndex[raspolozivi];
					if(Ready.data[nNiz][k] < kada && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
					{	kada = Ready.data[nNiz][k];
						prvi = raspolozivi;
					}
				}	// sada je pIndex[prvi] prvi raspolozivi posao
				nMachine = 0;
				for(i=0; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				// nMachine je prvi raspolozivi stroj
				if(kada < MachineReady[nMachine][0])
					kada = MachineReady[nMachine][0];
				dClock = kada;	// sat postavimo na najblize vrijeme raspolozivosti
				// sad odredimo koji zadnji posao gledamo iz polja indeksa
				for(i=Evaluator.m_iEnd; i<nPoslova && Ready[nNiz][pIndex[i]]<=dClock; i++) NULL;
				Evaluator.m_iEnd = i;

				// probajmo ovako: pronaci najbolju kombinaciju svih pristiglih poslova i raspolozivih strojeva
				// varijacija: gledamo sve strojeve (i one trenutno zauzete)
				for(nMachine=0; nMachine<nMachines; nMachine++)
				{	//if(MachineReady[nMachine][0] > dClock)	// varijanta samo sa slobodnim strojevima
					//	continue;
					// trebamo izracunati nove vrijednosti vremenski ovisnih terminala!
					CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
					for(int kk = 0; kk<ensenbleCount; kk++){
						Evaluator.m_iPosition = -1;
						Evaluator.m_nTree = kk;
						Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
						memcpy((ValuesArray[kk])[nMachine],pVrijednostiArray[kk],nPoslova*sizeof(double));					
					}
					
					
					//if(ff){
					//Evaluator.m_nTree = 1;
					//Evaluator.m_iPosition = -1;
					//Evaluator.evaluate_array(pVrijednosti1);	// ocijenimo sve
					//memcpy(Values1[nMachine],pVrijednosti1,nPoslova*sizeof(double));

					//Evaluator.m_iPosition = -1;
					//Evaluator.m_nTree = 2;
					//Evaluator.evaluate_array(pVrijednosti2);	// ocijenimo sve
					//memcpy(Values2[nMachine],pVrijednosti2,nPoslova*sizeof(double));

					//}
					//memcpy(Values[nMachine],pVrijednosti,nPoslova*sizeof(double));
					
					// pohranimo vrijednosti za taj stroj
				}
				bool BestSet = false;
				
				
	


				//nOdabrani2 = nOdabrani1 = nOdabrani;
				uint nBestMachineArray[12];
				uint nBestJobMachineArray[12];
				uint nBestMachine, nBestMachine1, nBestMachine2, nBestJobMachine, nBestJobMachine1, nBestJobMachine2;
				//int voting[10];
				for(i=nJob; i<nPoslova; i++)
				{	if(Precedence[pIndex[i]][0] != 0 || Ready[nNiz][pIndex[i]] > dClock)
						continue;
					// je li posao odabrao neraspolozivi stroj?
							float priority[12];	
				nBestJobMachine = 0;
					nBestJobMachine1 = 0;
					nBestJobMachine2 = 0;
					//for(int kk = 0; kk < ensenbleCount; kk++){
					//	nBestJobMachineArray[kk] = 0;
					//}
					for(nMachine=0; nMachine<nMachines; nMachine++){
						priority[nMachine]=0.0f;
						for(int kk = 0; kk<ensenbleCount; kk++){
							priority[nMachine] +=(ValuesArray[kk])[nMachine][pIndex[i]]*WeightT.data[nNiz][pIndex[i]];
						}
					}


					nBestJobMachine=0;
					for(nMachine=1; nMachine<nMachines; nMachine++){
						if(priority[nBestJobMachine]<priority[nMachine]){
							nBestJobMachine = nMachine;
						}
					}

					int dbsetJobMachine;
					int pindex; 
					if(MachineReady[nBestJobMachine][0] > dClock)
						continue;	
					if(!BestSet)	// ako je to prvi posao koji je odabrao raspolozivi stroj
					{	nBestMachine = nBestJobMachine;
					nBestMachine1 = nBestJobMachine1;
					nBestMachine2 = nBestJobMachine2;
						dBest = priority[nBestJobMachine];
						pindex = pIndex[i];
						//	dBest1 = Values1[nBestMachine1][pIndex[i]];
					//	dBest2 = Values2[nBestMachine2][pIndex[i]];
						nOdabrani = i;

						BestSet = true;
					}
					else	// inace vidi kolika je vrijednost
					{	
						//printf("nBestJobMachine: %f \n", priority[nBestJobMachine]);
						//printf("dBest: %f \n", dBest);

						if(priority[nBestJobMachine] < dBest){
							nBestMachine = nBestJobMachine;
							dBest = priority[nBestJobMachine];
							nOdabrani = i;
						}
					}
					//delete priority;
				}
				//printf("%d, %d, %d \n", nOdabrani, nOdabrani1, nOdabrani2);
				//if(nOdabrani1==nOdabrani2){
				//	nOdabrani = nOdabrani1;
				//	nBestJobMachine = nBestJobMachine1;
				//}
				// ako nijedan posao nije za raspolozivi stroj, prekidamo iteraciju petlje koja vrti poslove
				if(!BestSet)
				{	nJob--;	// azuriraj indeks for petlje (nismo rasporedili posao)
					// vremena svih raspolozivih strojeva prebaci na prvi sljedeci zavrsetak
					// prvo pronadji najblizi _sljedeci_ raspolozivi stroj (nMachine)
					for(i=0; i<nMachines && MachineReady[i][0] <= dClock; i++)	NULL;
					nMachine = i;
					for( ; i<nMachines; i++)
						if(MachineReady[i][0] > dClock && MachineReady[i][0] < MachineReady[nMachine][0])
							nMachine = i;

					// onda prvi _sljedeci_ raspolozivi posao
					unsigned int raspolozivi = nJob, prvi = nJob;
					// trebamo pronaci prvog sljedeceg raspolozivog bez nezavrsenih prethodnika
					for(; raspolozivi < nPoslova && (Precedence[pIndex[raspolozivi]][0] > 0 || Ready[nNiz][pIndex[raspolozivi]] <= dClock); raspolozivi++)	NULL;
					double kada;
					if(raspolozivi < nPoslova)	// ima li uopce novih?
					{	kada = Ready[nNiz][pIndex[raspolozivi]];	// pocetno vrijeme
						for( ; raspolozivi < nPoslova; raspolozivi++)	// koji je 'najstariji'?
						{	k = pIndex[raspolozivi];
							if(Ready[nNiz][k] < kada && Ready[nNiz][k] > dClock && Precedence[k][0] == 0)	// gledamo najblize vrijeme dolaska
							{	kada = Ready[nNiz][k];
								prvi = raspolozivi;
							}
						}	// sada je pIndex[prvi] prvi raspolozivi posao
					}

					// a onda odredimo manje od ta dva vremena (raspolozivost stroja ili posla)
					dClock = MachineReady[nMachine][0];
					if(raspolozivi < nPoslova && kada < dClock)
						dClock = kada;

					// azuriraj vremena raspolozivosti strojeva
					for(i=0; i<nMachines; i++)
						if(MachineReady[i][0] < dClock)
							MachineReady[i][0] = dClock;
                    continue;	// idi na pocetak petlje koja vrti poslove
				}
				// inace, rasporedi posao na raspolozivom stroju
				nMachine = nBestMachine;
			}	// endif - m_dynamic

			else	// staticki
			{	// pronadjemo prvi raspolozivi stroj
				nMachine = 0;
				for(i=1; i<nMachines; i++)
					if(MachineReady[i][0] < MachineReady[nMachine][0])
						nMachine = i;
				dClock = MachineReady[nMachine][0];	// to je trenutno vrijeme
				CalcTimedTerminals(nNiz,nPoslova,nJob,dClock,nMachine,nMachines);
				nOdabrani = nJob;
				if(m_constrained)	// trazimo prvi bez prethodnika
					for(; Precedence[pIndex[nOdabrani]][0] > 0; nOdabrani++)	NULL;
				
				
				
				dBest = pVrijednosti[pIndex[nOdabrani]];	// pretpostavimo da je neki najbolji
				for(i=nJob; i<nPoslova; i++)	// pa pogledamo ostale
				{	// trazimo najmanju vrijednost
					if(pVrijednosti[pIndex[i]] < dBest && Precedence[pIndex[i]][0] == 0)	// je li to najbolja vrijednost?
					{	dBest = pVrijednosti[pIndex[i]];
						nOdabrani = i;
					}
				}
			}	// zavrsen odabir posla

			// vrijednost pIndex[nOdabrani] je indeks posla koji ide sljedeci, na stroju nMachine
			// gledamo nOdabrani kao rezultat; zamijenimo nJob-ti i odabrani u polju indeksa
			i = pIndex[nJob];
			pIndex[nJob] = pIndex[nOdabrani];
			pIndex[nOdabrani] = i;
			pRasporedjen[pIndex[nJob]] = true;
			nOdabrani = pIndex[nJob];	// nOdabrani je pravi indeks posla

			// update trajanja preostalih - jos treba definirati
			//dSPr -= Duration.data[nNiz][nOdabrani]*dMsum;	
			nNr--;		// update broja preostalih
			if(m_setup)	// trajanje postavljanja
			{	dSetupTime = Setup[pLastJob[nMachine]][nOdabrani];
				pLastJob[nMachine] = nOdabrani;	// sljedeci prethodni posao
			}
			else dSetupTime = 0;
			if(m_constrained)	// smanjimo broj prethodnika svim sljedbenicima odabranog posla
				for(i=0; i<Precedence[nOdabrani][1]; i++)
				{	j = (int) Precedence[nOdabrani][i+2];
					Precedence[j][0] -= 1;
				}
			MachineReady[nMachine][0] = dClock + dSetupTime + \
				Duration[nNiz][nOdabrani*nMachines + nMachine];
			// odredimo sljedeci element u matrici rasporeda (indeks stroja je tisucica)
			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;
		} // kraj petlje koja vrti poslove u skupu

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			double dAvgDuration = SP[nNiz][0] / nPoslova;   // prosjecno trajanje posla
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			//printf("Tardiness: %f \n",  dTardiness);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= nPoslova  * dAvgDuration;
			dCmax /= nPoslova * dAvgDuration;
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}



//
// UNRELATED okruzenje, ali uz pomoc floating point prikaza
//
void SchedulingEvalOp::EvaluateUnrelatedFP(FloatingPointP fp, double &dRawFitness)
{
	double dClock, dRez, dSetFitness, dAvgWeights, dCmax, dTotalCmax=0;
	double dLateness, dTotalLateness=0, dTardiness, dTotalTardiness=0;
	double dNwt, dTotalNwt=0, dBest, dSPr, dSetupTime, dFwt, dTotalFwt=0;
	unsigned int nPoslova,nNiz,nJob,i,j,k,index,nLateJobs,nTotalLateJobs=0,nNr;
	unsigned int nLastJob,nMachines,nOdabrani,nMachine;

	dRawFitness = 0;

// vrtimo sve skupove
	//for(nNiz=0; nNiz<sets; nNiz++)

	nNiz = m_primjer;
	{

		nNr = nPoslova = (int) N.Get(nNiz);
		// preliminarna ispitivanja
		if(m_constrained)	// ima li ogranicenja
			ReadConstraints(Constraints,nNiz,nPoslova,Precedence);
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);

		// postavljanje pocetnih vrijednosti za pojedini skup
		nLateJobs = 0;
		dLateness = 0;
		dTardiness = 0;
		dNwt = 0;
		dFwt = 0;
		dClock = 0; dSetFitness = 0;
		nLastJob = nPoslova;
		for(i=0; i<nPoslova; i++)
		{	pRasporedjen[i] = false;	// svi nerasporedjeni
			// procitamo niz indeksa poslova sortiran po dolasku
			pIndex[i] = (uint) SortedReady[nNiz][i];	// inicijalni poredak
		}
		nMachines = (uint) Machines[nNiz][0];

		// redni broj rasporedjenog posla 
		nJob = 0;

		// odredi granice FP vrijednosti za svaki stroj
		std::vector<double> machineBounds;
		machineBounds.resize(nMachines + 1);
		for(uint m = 0; m <= nMachines; m++)
			machineBounds[m] = (m) * 1. / nMachines;
		
		std::vector<double> jobValues(nPoslova);
		std::vector<uint> jobIndexes(nPoslova);

		// odredimo poslove za svaki stroj posebno
		for(uint m = 0; m < nMachines; m++) {
			uint nJobs = 0;

			// koji poslovi idu na ovaj stroj i koji su njihovi indeksi
			for(uint job = 0; job < nPoslova; job++)
				if(fp->realValue[job] >= machineBounds[m] && fp->realValue[job] < machineBounds[m + 1]) {
					jobValues[nJobs] = fp->realValue[job];
					jobIndexes[nJobs] = job;
					nJobs++;
				}

			// ako nema poslova za ovaj stroj, odi dalje
			if(nJobs == 0)
				continue;

			// sortiraj poslove na stroju po vrijednostima
			double pd;
			uint pi;
			for(uint i = 0; i < nJobs - 1; i++)
				for(uint j = i + 1; j < nJobs; j++)
					if(jobValues[i] > jobValues[j]) {
						pd = jobValues[i];
						jobValues[i] = jobValues[j];
						jobValues[j] = pd;
						pi = jobIndexes[i];
						jobIndexes[i] = jobIndexes[j];
						jobIndexes[j] = pi;
					}

			// zapisi u raspored
			for(uint job = 0; job < nJobs; job++) {
				Schedule[nNiz][nJob] = jobIndexes[job] + m * 1000;
				nJob++;
			}

		}



//			Schedule[nNiz][nJob] = nOdabrani + nMachine * 1000;


		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(nJob=nPoslova ; nJob < this->max_jobs; nJob++)
					Schedule[nNiz][nJob] = 0;					// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dClock = 0; nLastJob = nPoslova; dAvgWeights = 0; dCmax = 0;
			for(i=0; i<nMachines; i++)
			{	MachineReady[i][0] = 0;
				pLastJob[i] = nPoslova;
			}
			for(nJob = 0; nJob<nPoslova; nJob++)
			{	index = (int) Schedule[nNiz][nJob];
				nMachine = index / 1000;	// na kojem stroju
				index = index % 1000;		// koji posao
				dAvgWeights += WeightT[nNiz][index];
				if(m_dynamic && MachineReady[nMachine][0] < Ready[nNiz][index])	// ako posao jos nije raspoloziv
					MachineReady[nMachine][0] = Ready[nNiz][index];
				MachineReady[nMachine][0] += Duration[nNiz][index*nMachines + nMachine];
				if(m_setup)
					MachineReady[nMachine][0] += Setup[pLastJob[nMachine]][index];
				pLastJob[nMachine] = index;
				dRez = MachineReady[nMachine][0] - Deadline.Get(nNiz,index);	// kasnjenje
				dLateness += dRez*WeightT.data[nNiz][index];	// tezinsko kasnjenje
				if(dRez > 0) dTardiness += dRez*WeightT.data[nNiz][index];	// tezinska zakasnjelost
				if(dRez > 0) nLateJobs ++;	// kao broj zakasnjelih poslova
				if(dRez > 0) dNwt += WeightN.data[nNiz][index];	// tezinski broj zakasnjelih
				if(m_dynamic)
					dFwt += MachineReady[nMachine][0] - Ready[nNiz][index];	// protjecanje, NIJE TEZINSKO
				else
					dFwt += MachineReady[nMachine][0];
			}
			for(i=0; i<nMachines; i++)
				if(MachineReady[i][0] > dCmax)
					dCmax = MachineReady[i][0];
			// normiranje fitnesa ovisno o broju poslova - dodano 27.07.
			double dAvgDuration = SP[nNiz][0] / nPoslova;	// prosjecno trajanje posla
			dAvgWeights /= nPoslova;	// prosjecni tezinski faktor skupa
			dTardiness /= (nPoslova * dAvgWeights * dAvgDuration);
			dNwt /= (nPoslova * dAvgWeights);
			dFwt /= (nPoslova * dAvgWeights * dAvgDuration);
			dCmax /= (nPoslova * dAvgDuration);
			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTardiness; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			nTotalLateJobs += nLateJobs;
			dTotalNwt += dNwt;
			dTotalFwt += dFwt;
			dTotalLateness += dLateness;
			dTotalTardiness += dTardiness;
			dTotalCmax += dCmax;
			Fitness[nNiz][Twt] = dTardiness;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness[sets][Twt] = dTotalTardiness;
	Fitness[sets][Nwt] = dTotalNwt;
	Fitness[sets][Fwt] = dTotalFwt;
	Fitness[sets][Cmax] = dTotalCmax;
}





void SchedulingEvalOp::EvaluateJobShop(double &dRawFitness)
{
	double dL, dT, dTwt, dF, dFwt, dN, dNwt, dCmax;
	double dTotalT, dTotalTwt, dTotalF, dTotalFwt, dTotalN, dTotalNwt, dTotalCmax;
	double dClock, dAvgWeights, dTotalAvgLoad, dBestMachineValue, dAvgDuration;
	double dBest, dSetupTime, m1, m2, dNajkrace, dBegin;
	unsigned int nPoslova, nNiz, nJob, i, j, k, nNr, nOperations, nOperation, nNextOperation;
	unsigned int nMachines, nMachine, nSchedule, nMachineIndex, nJobsToEval, nBestJob, nNextMachine;
	unsigned int nBottleneck;

	dRawFitness=0;
	dTotalT = dTotalTwt = dTotalF = dTotalFwt = dTotalN = dTotalNwt = dTotalCmax = 0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		dClock = 0;
		dTotalAvgLoad = 0;	// prosjecno opterecenje svih strojeva
		nOperations = nMachines = (uint) Machines[nNiz][0];
		dBestMachineValue = 0;
		nBottleneck = nMachines;	// nijedan nije bottleneck na pocetku
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
			pMachineScheduled[j] = 0;	// broj rasporedjenih operacija na stroju
			pOperationReady[j] = -1;	// raspolozivost operacija za strojeve tek treba odrediti
			pTotalMachineWork[j] = 0;	// ukupno opterecenje stroja
			pOperationsWaiting[j] = 0;	// broj operacija na cekanju
			pMachineValues[j] = 0;		// vrijednosti strojeva - racunaju se stablom odluke
		}
		for(i=0; i<nPoslova; i++)
		{	pTotalWorkRemaining[i] = 0;
			pTotalWorkDone[i] = 0;
			pIndex[i] = i;	// inicijalni poredak
			pJobReady[i] = Ready[nNiz][i];	// Ready su nule u statickom slucaju
			pOperationsScheduled[i] = 0;	// broj rasporedjenih operacija posla
			for(j=0; j<nOperations; j++)
			{	k = (int) Duration[nNiz][i*nOperations + j];
				nMachine = k / 1000;	// indeks stroja
				if(j == 0)	// za prvu operaciju
					pOperationsWaiting[nMachine]++;
				k = k % 1000;		// trajanje operacije
				pTotalWorkRemaining[i] += k;
				Durations[i][j] = k;
				dTotalAvgLoad += k;
				MachineIndex[i][j] = nMachine;
				pTotalMachineWork[nMachine] += k;
			}
			// pogledamo na kojem je stroju prva operacija posla
			nMachine = (int) MachineIndex[i][0];
			pOperationReady[nMachine] = pJobReady[i];
			// znaci taj stroj je trazen u trenutku dolaska posla (0 u statickom slucaju)
		}
		dAvgDuration = dTotalAvgLoad / nPoslova;	// prosjecno trajanje poslova
		dTotalAvgLoad /= nMachines;
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		//Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		// postavljanje pocetnih vrijednosti terminala - nepromjenjivi
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_TWK], pTotalWorkRemaining, nPoslova*sizeof(double));
		// nepromjenjivi terminali za strojeve
		for(i=0; i<nMachines; i++)
		{	Evaluator.m_dTermValuesArray[T_MTWKav][i] = dTotalAvgLoad;
			Evaluator.m_dTermValuesArray[T_MTWK][i] = pTotalMachineWork[i];
			pMachineWorkRemaining[i] = pTotalMachineWork[i];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_MTWK], pTotalMachineWork, nMachines*sizeof(double));

		for(nSchedule = 0; nSchedule < nPoslova*nMachines; nSchedule++)
		{	// treba pronaci stroj koji ima najranije vrijeme raspolozivosti i sebe i operacije za njega
			for(nMachine = 0; nMachine<nMachines; nMachine++)
				if(pOperationReady[nMachine] >= 0)	// je li stroj uopce trazen
					break;
			for(i = nMachine; i < nMachines; i++)
			{	if(pOperationReady[i] < 0)	// jos se nije pojavila potreba za tim strojem
					continue;
				m1 = MAX(MachineReady[i][0],pOperationReady[i]);
				m2 = MAX(MachineReady[nMachine][0],pOperationReady[nMachine]);
				if(m1 < m2)	// ima neki stroj koji je prije raspoloziv za rasporediti
					nMachine = i;
			}
			dClock = MAX(MachineReady[nMachine][0], pOperationReady[nMachine]);
			// pronadjemo kada bilo koja operacija moze najprije zavrsiti
			dNajkrace = -1;
			for(nJob=0; nJob<nPoslova; nJob++)
			{	nOperation = pOperationsScheduled[nJob];
				if(nOperation == nMachines) continue;		// ovaj posao je zavrsio
				nMachineIndex = (int) MachineIndex[nJob][nOperation];
				if(nMachineIndex != nMachine) continue;	// ovaj posao ne ceka na taj stroj
				if(dNajkrace < 0)
					dNajkrace = MAX(dClock, pJobReady[nJob]) + Durations[nJob][nOperation];
				else
				{	dBegin = MAX(dClock, pJobReady[nJob]);
					if(dNajkrace > dBegin + Durations[nJob][nOperation])
						dNajkrace = dBegin + Durations[nJob][nOperation];
				}
			}
			// sada treba pronaci operaciju najveceg prioriteta koja ceka na taj stroj
			// radimo tako da poslove cije operacije treba ocijeniti stavimo na pocetak pIndex
			// prije evaluacije odredimo koliko ih ima (kao m_iEnd u Evaluatoru)
			nJobsToEval = 0;
			for(nJob=0; nJob<nPoslova; nJob++)
			{	nOperation = pOperationsScheduled[nJob];
				if(nOperation == nMachines) continue;		// ovaj posao je zavrsio
				nMachineIndex = (int) MachineIndex[nJob][nOperation];
				if(nMachineIndex != nMachine) continue;	// ovaj posao ne ceka na taj stroj
				// mozemo uzeti u obzir samo one koji su vec spremni:
				if(!m_Idleness)
					if(pJobReady[nJob] > dClock) continue;	// jos nije zavrsila prethodna operacija
				// ili mozemo uzeti u obzir i one koji ce tek zavrsiti, ali ne kasnije od moguceg zavrsetka najkrace spremne operacije
				dBegin = MAX(pJobReady[nJob],dClock);
				if(dBegin > dNajkrace)	// nema smisla gledati ako pocinje iza najkraceg zavrsetka
					continue;
				// ako smo dosli ovdje, uzet cemo u obzir operaciju nOper. posla nJob na stroju nMachine
				pIndex[nJobsToEval] = nJob;
				nJobsToEval++;
			}
#ifdef TREES
			// pomocu prvog stabla odlucimo koja od dva preostala koristimo za prioritete
*
MNOPr[nMachine] = nOperations - pMachineScheduled[nMachine]
MTWKav = dTotalAvgLoad;
MTWK[nMachine] = pTotalMachineWork[nMachine];
MNOPw[nMachine] = pOperationsWaiting[nMachine];
MTWKr[nMachine] = pMachineWorkRemaining[nMachine];
MUTL[nMachine] = (pTotalMachineWork[nMachine] - pMachineWorkRemaining[nMachine]) / dClock;
*/
			Evaluator.m_nTree = 0;
			// prvo stablo racunamo samo za nMachine - pa za taj stroj racunamo terminale
			Evaluator.m_dTermValuesArray[T_MNOPr][nMachine] = nOperations - pMachineScheduled[nMachine];
			Evaluator.m_dTermValuesArray[T_MNOPw][nMachine] = pOperationsWaiting[nMachine];
			Evaluator.m_dTermValuesArray[T_MTWKr][nMachine] = pMachineWorkRemaining[nMachine];
			if(dClock == 0)
				Evaluator.m_dTermValuesArray[T_MUTL][nMachine] = 1;
			else
				Evaluator.m_dTermValuesArray[T_MUTL][nMachine] = (pTotalMachineWork[nMachine] - pMachineWorkRemaining[nMachine]) / dClock;

			Evaluator.m_iPosition = -1;
			nSavedIndex = pIndex[nMachine];	// pohranimo indeks posla koji se tu nalazi
			pIndex[nMachine] = nMachine;
			Evaluator.m_iOffset = nMachine;
			Evaluator.m_iEnd = nMachine + 1;
			Evaluator.evaluate_array(pVrijednosti);	// ocijenimo stroj u stablu odluke
			pMachineValues[nMachine] = pVrijednosti[nMachine];
			// MODIFIKACIJA 15.09.: uvijek pronadjemo novi dBestMachineValue kao Bottleneck
*			dBestMachineValue = pMachineValues[0];
			nBottleneck = 0;
			for(i=0; i<nMachine; i++)
				if(pMachineValues[i] > dBestMachineValue)
				{	dBestMachineValue = pMachineValues[i];
					nBottleneck = i;
				}
*/
			// MODIFIKACIJA 13.09.: uvijek pamtimo stroj koji je postigao najvecu vrijednost
			// (dok se ne postigne veca)
			if(pMachineValues[nMachine] >= dBestMachineValue)	// je li nova najveca vrijednost?
			{	dBestMachineValue = pMachineValues[nMachine];
				nBottleneck = nMachine;
			}

			if(nMachine == nBottleneck)	// jesam li ja bottleneck?
				Evaluator.m_nTree = 2;	// za najoptereceniji stroj
			else
				Evaluator.m_nTree = 1;	// za sve ostale
			pIndex[nMachine] = nSavedIndex;	// vratimo indeks posla
#else	// samo jedno stablo
			Evaluator.m_nTree = 0;	// vratimo na obicno stablo
#endif
			// vektorska evaluacija!
			// trebamo definirati vrijednosti terminala za trenutne operacije odabranih poslova
			for(i=0; i<nJobsToEval; i++)
			{	nJob = pIndex[i];
				nOperation = pOperationsScheduled[nJob];
				Evaluator.m_dTermValuesArray[T_pt][nJob] = Durations[nJob][nOperation];
				Evaluator.m_dTermValuesArray[T_CLK][nJob] = dClock;
				Evaluator.m_dTermValuesArray[T_AR][nJob] = POS(pJobReady[nJob] - dClock); 
				Evaluator.m_dTermValuesArray[T_STP][nJob] = Setup[pLastJob[nMachine]][nJob];
				Evaluator.m_dTermValuesArray[T_Sav][nJob] = pSetupAvg[pLastJob[nMachine]];
				Evaluator.m_dTermValuesArray[T_age][nJob] = POS(dClock - Ready[nNiz][nJob]);
				Evaluator.m_dTermValuesArray[T_NOPr][nJob] = nOperations - pOperationsScheduled[nJob];
				Evaluator.m_dTermValuesArray[T_PTav][nJob] = PTimeAvg[nNiz][nMachine];
				Evaluator.m_dTermValuesArray[T_TWKr][nJob] = pTotalWorkRemaining[nJob];
				if(pTotalWorkDone[nJob] == 0)
					Evaluator.m_dTermValuesArray[T_HTR][nJob] = 1;
				else
					Evaluator.m_dTermValuesArray[T_HTR][nJob] = POS(dClock - Ready[nNiz][nJob]) / pTotalWorkDone[nJob];
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = 0;
			Evaluator.m_iEnd = nJobsToEval;	// toliko ih ima za ocijeniti
			Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve u odabranom stablu
			dBest = pVrijednosti[pIndex[0]]; // poc. vrijednost za usporedbu
			nBestJob = pIndex[0];
			for(i=1; i<nJobsToEval; i++)
			{	if(pVrijednosti[pIndex[i]] < dBest)
				{	dBest = pVrijednosti[pIndex[i]];
					nBestJob = pIndex[i];
				}
			}

			// nBestJob na nMachine
			nOperation = pOperationsScheduled[nBestJob];	// sadasnja operacija (koja ce se izvesti)
			pOperationsScheduled[nBestJob] += 1;	// novi broj rasporedjenih operacija posla
			pTotalWorkRemaining[nBestJob] -= Durations[nBestJob][nOperation];
			pMachineWorkRemaining[nMachine] -= Durations[nBestJob][nOperation];
			pTotalWorkDone[nBestJob] += Durations[nBestJob][nOperation];
			if(m_setup)
			{	dSetupTime = Setup[pLastJob[nMachine]][nBestJob];
				pLastJob[nMachine] = nBestJob;
			}
			else
				dSetupTime = 0;
			dClock = MAX(dClock, pJobReady[nBestJob]);	// ako smo odlucili pricekati
			MachineReady[nMachine][0] = dClock + dSetupTime + Durations[nBestJob][nOperation];
			pJobReady[nBestJob] = MachineReady[nMachine][0];	// kada ce sljedeca operacija biti raspoloziva
			if(nOperation < nOperations-1)	// ako posao ima jos operacija
			{	nNextMachine = (int) MachineIndex[nBestJob][pOperationsScheduled[nBestJob]];
				if(pOperationReady[nNextMachine] < 0)	// ako stroj trenutno nije trazen...
					pOperationReady[nNextMachine] = pJobReady[nBestJob];	// ... bit ce tada
				else	// provjeri dolazi li moja sljedeca operacija prije trenutno najblize za taj stroj
					if(pOperationReady[nNextMachine] > pJobReady[nBestJob])
						pOperationReady[nNextMachine] = pJobReady[nBestJob];
			}
			// kada cu ja (stroj) opet biti trazen?
			pOperationReady[nMachine] = -1;
			pOperationsWaiting[nMachine] = 0;
			for(j=0; j<nPoslova; j++)
			{	nNextOperation = pOperationsScheduled[j];
				if(nNextOperation == nMachines) continue;	// ovaj nema vise operacija
				nNextMachine = (int) MachineIndex[j][nNextOperation];
				if(nNextMachine != nMachine) continue;
				pOperationsWaiting[nMachine]++;
				if(pOperationReady[nMachine] < 0)
					pOperationReady[nMachine] = pJobReady[j];
				else
					if(pOperationReady[nMachine] > pJobReady[j])
						pOperationReady[nMachine] = pJobReady[j];
			}
			nOperation = (int) pMachineScheduled[nMachine];
			pMachineScheduled[nMachine] += 1;
			Schedule[nNiz][nMachine * nPoslova + nOperation] = nBestJob;
		}	// kraj rasporedjivanja skupa

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(i=nPoslova*nMachines ; i < (uint)Schedule.GetCol(); i++)
					Schedule[nNiz][i] = 0;			// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dAvgWeights = 0;
			dCmax = dF = dFwt = dT = dTwt = dN = dNwt = 0;
			for(j=0; j<nPoslova; j++)
			{	dAvgWeights += WeightT[nNiz][j];
				if(dCmax < pJobReady[j])
					dCmax = pJobReady[j];
				dF += pJobReady[j] - Ready[nNiz][j];
				dFwt += (pJobReady[j] - Ready[nNiz][j]) * WeightT[nNiz][j];
				if(pJobReady[j] > Deadline[nNiz][j])
				{	dN += 1;
					dNwt += WeightT[nNiz][j];
					dL = pJobReady[j] - Deadline[nNiz][j];
					dT += dL;
					dTwt += WeightT[nNiz][j] * dL;
				}
			}
			dAvgWeights /= nPoslova;
			dCmax /= nPoslova*dAvgDuration;
			dF /= nPoslova;
			dFwt /= nPoslova*dAvgWeights*dAvgDuration;
			dT /= nPoslova;
			dTwt /= nPoslova*dAvgWeights*dAvgDuration;
			dN /= nPoslova;
			dNwt /= nPoslova*dAvgWeights;
			dTotalCmax += dCmax;
			dTotalF += dF;
			dTotalFwt += dFwt;
			dTotalT += dT;
			dTotalTwt += dTwt;
			dTotalN += dN;
			dTotalNwt += dNwt;

			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTwt; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			Fitness[nNiz][Twt] = dTwt;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness.data[sets][Twt] = dTotalTwt;
	Fitness.data[sets][Nwt] = dTotalNwt;
}


void SchedulingEvalOp::EvaluateJobShopVote(double &dRawFitness)
{
	double dL, dT, dTwt, dF, dFwt, dN, dNwt, dCmax;
	double dTotalT, dTotalTwt, dTotalF, dTotalFwt, dTotalN, dTotalNwt, dTotalCmax;
	double dClock, dAvgWeights, dTotalAvgLoad, dBestMachineValue, dAvgDuration;
	double dBest, dSetupTime, m1, m2, dNajkrace, dBegin;
	unsigned int nPoslova, nNiz, nJob, i, j, k, nNr, nOperations, nOperation, nNextOperation;
	unsigned int nMachines, nMachine, nSchedule, nMachineIndex, nJobsToEval, nBestJob, nNextMachine;
	unsigned int nBottleneck;

	double dBestArray[12];
	int odabraniArray[12];

	dRawFitness=0;
	dTotalT = dTotalTwt = dTotalF = dTotalFwt = dTotalN = dTotalNwt = dTotalCmax = 0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		dClock = 0;
		dTotalAvgLoad = 0;	// prosjecno opterecenje svih strojeva
		nOperations = nMachines = (uint) Machines[nNiz][0];
		dBestMachineValue = 0;
		nBottleneck = nMachines;	// nijedan nije bottleneck na pocetku
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
			pMachineScheduled[j] = 0;	// broj rasporedjenih operacija na stroju
			pOperationReady[j] = -1;	// raspolozivost operacija za strojeve tek treba odrediti
			pTotalMachineWork[j] = 0;	// ukupno opterecenje stroja
			pOperationsWaiting[j] = 0;	// broj operacija na cekanju
			pMachineValues[j] = 0;		// vrijednosti strojeva - racunaju se stablom odluke
		}
		for(i=0; i<nPoslova; i++)
		{	pTotalWorkRemaining[i] = 0;
			pTotalWorkDone[i] = 0;
			pIndex[i] = i;	// inicijalni poredak
			pJobReady[i] = Ready[nNiz][i];	// Ready su nule u statickom slucaju
			pOperationsScheduled[i] = 0;	// broj rasporedjenih operacija posla
			for(j=0; j<nOperations; j++)
			{	k = (int) Duration[nNiz][i*nOperations + j];
				nMachine = k / 1000;	// indeks stroja
				if(j == 0)	// za prvu operaciju
					pOperationsWaiting[nMachine]++;
				k = k % 1000;		// trajanje operacije
				pTotalWorkRemaining[i] += k;
				Durations[i][j] = k;
				dTotalAvgLoad += k;
				MachineIndex[i][j] = nMachine;
				pTotalMachineWork[nMachine] += k;
			}
			// pogledamo na kojem je stroju prva operacija posla
			nMachine = (int) MachineIndex[i][0];
			pOperationReady[nMachine] = pJobReady[i];
			// znaci taj stroj je trazen u trenutku dolaska posla (0 u statickom slucaju)
		}
		dAvgDuration = dTotalAvgLoad / nPoslova;	// prosjecno trajanje poslova
		dTotalAvgLoad /= nMachines;
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		//Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		// postavljanje pocetnih vrijednosti terminala - nepromjenjivi
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_TWK], pTotalWorkRemaining, nPoslova*sizeof(double));
		// nepromjenjivi terminali za strojeve
		for(i=0; i<nMachines; i++)
		{	Evaluator.m_dTermValuesArray[T_MTWKav][i] = dTotalAvgLoad;
			Evaluator.m_dTermValuesArray[T_MTWK][i] = pTotalMachineWork[i];
			pMachineWorkRemaining[i] = pTotalMachineWork[i];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_MTWK], pTotalMachineWork, nMachines*sizeof(double));

		for(nSchedule = 0; nSchedule < nPoslova*nMachines; nSchedule++)
		{	// treba pronaci stroj koji ima najranije vrijeme raspolozivosti i sebe i operacije za njega
			for(nMachine = 0; nMachine<nMachines; nMachine++)
				if(pOperationReady[nMachine] >= 0)	// je li stroj uopce trazen
					break;
			for(i = nMachine; i < nMachines; i++)
			{	if(pOperationReady[i] < 0)	// jos se nije pojavila potreba za tim strojem
					continue;
				m1 = MAX(MachineReady[i][0],pOperationReady[i]);
				m2 = MAX(MachineReady[nMachine][0],pOperationReady[nMachine]);
				if(m1 < m2)	// ima neki stroj koji je prije raspoloziv za rasporediti
					nMachine = i;
			}
			dClock = MAX(MachineReady[nMachine][0], pOperationReady[nMachine]);
			// pronadjemo kada bilo koja operacija moze najprije zavrsiti
			dNajkrace = -1;
			for(nJob=0; nJob<nPoslova; nJob++)
			{	nOperation = pOperationsScheduled[nJob];
				if(nOperation == nMachines) continue;		// ovaj posao je zavrsio
				nMachineIndex = (int) MachineIndex[nJob][nOperation];
				if(nMachineIndex != nMachine) continue;	// ovaj posao ne ceka na taj stroj
				if(dNajkrace < 0)
					dNajkrace = MAX(dClock, pJobReady[nJob]) + Durations[nJob][nOperation];
				else
				{	dBegin = MAX(dClock, pJobReady[nJob]);
					if(dNajkrace > dBegin + Durations[nJob][nOperation])
						dNajkrace = dBegin + Durations[nJob][nOperation];
				}
			}
			// sada treba pronaci operaciju najveceg prioriteta koja ceka na taj stroj
			// radimo tako da poslove cije operacije treba ocijeniti stavimo na pocetak pIndex
			// prije evaluacije odredimo koliko ih ima (kao m_iEnd u Evaluatoru)
			nJobsToEval = 0;
			for(nJob=0; nJob<nPoslova; nJob++)
			{	nOperation = pOperationsScheduled[nJob];
				if(nOperation == nMachines) continue;		// ovaj posao je zavrsio
				nMachineIndex = (int) MachineIndex[nJob][nOperation];
				if(nMachineIndex != nMachine) continue;	// ovaj posao ne ceka na taj stroj
				// mozemo uzeti u obzir samo one koji su vec spremni:
				if(!m_Idleness)
					if(pJobReady[nJob] > dClock) continue;	// jos nije zavrsila prethodna operacija
				// ili mozemo uzeti u obzir i one koji ce tek zavrsiti, ali ne kasnije od moguceg zavrsetka najkrace spremne operacije
				dBegin = MAX(pJobReady[nJob],dClock);
				if(dBegin > dNajkrace)	// nema smisla gledati ako pocinje iza najkraceg zavrsetka
					continue;
				// ako smo dosli ovdje, uzet cemo u obzir operaciju nOper. posla nJob na stroju nMachine
				pIndex[nJobsToEval] = nJob;
				nJobsToEval++;
			}
#ifdef TREES
			// pomocu prvog stabla odlucimo koja od dva preostala koristimo za prioritete
*
MNOPr[nMachine] = nOperations - pMachineScheduled[nMachine]
MTWKav = dTotalAvgLoad;
MTWK[nMachine] = pTotalMachineWork[nMachine];
MNOPw[nMachine] = pOperationsWaiting[nMachine];
MTWKr[nMachine] = pMachineWorkRemaining[nMachine];
MUTL[nMachine] = (pTotalMachineWork[nMachine] - pMachineWorkRemaining[nMachine]) / dClock;
*/
			Evaluator.m_nTree = 0;
			// prvo stablo racunamo samo za nMachine - pa za taj stroj racunamo terminale
			Evaluator.m_dTermValuesArray[T_MNOPr][nMachine] = nOperations - pMachineScheduled[nMachine];
			Evaluator.m_dTermValuesArray[T_MNOPw][nMachine] = pOperationsWaiting[nMachine];
			Evaluator.m_dTermValuesArray[T_MTWKr][nMachine] = pMachineWorkRemaining[nMachine];
			if(dClock == 0)
				Evaluator.m_dTermValuesArray[T_MUTL][nMachine] = 1;
			else
				Evaluator.m_dTermValuesArray[T_MUTL][nMachine] = (pTotalMachineWork[nMachine] - pMachineWorkRemaining[nMachine]) / dClock;

			Evaluator.m_iPosition = -1;
			nSavedIndex = pIndex[nMachine];	// pohranimo indeks posla koji se tu nalazi
			pIndex[nMachine] = nMachine;
			Evaluator.m_iOffset = nMachine;
			Evaluator.m_iEnd = nMachine + 1;
			Evaluator.evaluate_array(pVrijednosti);	// ocijenimo stroj u stablu odluke
			pMachineValues[nMachine] = pVrijednosti[nMachine];
			// MODIFIKACIJA 15.09.: uvijek pronadjemo novi dBestMachineValue kao Bottleneck
*			dBestMachineValue = pMachineValues[0];
			nBottleneck = 0;
			for(i=0; i<nMachine; i++)
				if(pMachineValues[i] > dBestMachineValue)
				{	dBestMachineValue = pMachineValues[i];
					nBottleneck = i;
				}
*/
			// MODIFIKACIJA 13.09.: uvijek pamtimo stroj koji je postigao najvecu vrijednost
			// (dok se ne postigne veca)
			if(pMachineValues[nMachine] >= dBestMachineValue)	// je li nova najveca vrijednost?
			{	dBestMachineValue = pMachineValues[nMachine];
				nBottleneck = nMachine;
			}

			if(nMachine == nBottleneck)	// jesam li ja bottleneck?
				Evaluator.m_nTree = 2;	// za najoptereceniji stroj
			else
				Evaluator.m_nTree = 1;	// za sve ostale
			pIndex[nMachine] = nSavedIndex;	// vratimo indeks posla
#else	// samo jedno stablo
			Evaluator.m_nTree = 0;	// vratimo na obicno stablo
#endif
			// vektorska evaluacija!
			// trebamo definirati vrijednosti terminala za trenutne operacije odabranih poslova
			for(i=0; i<nJobsToEval; i++)
			{	nJob = pIndex[i];
				nOperation = pOperationsScheduled[nJob];
				Evaluator.m_dTermValuesArray[T_pt][nJob] = Durations[nJob][nOperation];
				Evaluator.m_dTermValuesArray[T_CLK][nJob] = dClock;
				Evaluator.m_dTermValuesArray[T_AR][nJob] = POS(pJobReady[nJob] - dClock); 
				Evaluator.m_dTermValuesArray[T_STP][nJob] = Setup[pLastJob[nMachine]][nJob];
				Evaluator.m_dTermValuesArray[T_Sav][nJob] = pSetupAvg[pLastJob[nMachine]];
				Evaluator.m_dTermValuesArray[T_age][nJob] = POS(dClock - Ready[nNiz][nJob]);
				Evaluator.m_dTermValuesArray[T_NOPr][nJob] = nOperations - pOperationsScheduled[nJob];
				Evaluator.m_dTermValuesArray[T_PTav][nJob] = PTimeAvg[nNiz][nMachine];
				Evaluator.m_dTermValuesArray[T_TWKr][nJob] = pTotalWorkRemaining[nJob];
				if(pTotalWorkDone[nJob] == 0)
					Evaluator.m_dTermValuesArray[T_HTR][nJob] = 1;
				else
					Evaluator.m_dTermValuesArray[T_HTR][nJob] = POS(dClock - Ready[nNiz][nJob]) / pTotalWorkDone[nJob];
			}
			Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = 0;
			Evaluator.m_iEnd = nJobsToEval;	// toliko ih ima za ocijeniti
			Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve u odabranom stablu
			dBest = pVrijednosti[pIndex[0]]; // poc. vrijednost za usporedbu
			nBestJob = pIndex[0];
		
			
			for(int kk = 0; kk<ensenbleCount; kk++){
							Evaluator.m_iPosition = -1;
			Evaluator.m_iOffset = 0;
			Evaluator.m_iEnd = nJobsToEval;	// toliko ih ima za ocijeniti
				Evaluator.m_nTree = kk;
				Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
				memcpy((ValuesArray[kk])[0],pVrijednostiArray[kk],nPoslova*sizeof(double));					
			}


				vector<int> votes;
				for(int kk = 0; kk<nJobsToEval; kk++){
					votes.push_back(0);
				}

				//vector<double> prioritet;
				//for(int kk=0; kk<nPoslova; kk++){
				//	prioritet.push_back(0);
				//}
				
				//for(i=nJob; i<nPoslova; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
				//{	k = pIndex[i];
				//	if(Precedence[k][0] == 0 && /*(pVrijednosti[k] < dBest)*/ (Ready[nNiz][k] < dClock + kada))
				//	{
				//		for(int kk=0; kk<ensenbleCount; kk++){
				//			prioritet[i]+=(ValuesArray[kk])[0][k];
				//			//cout<<"ValuesArrat: "<<(ValuesArray[kk])[0][i]<<endl;
				//		}
				//	}
				//}


				for(int kk = 0; kk<ensenbleCount; kk++){				
					nBestJob = pIndex[0];
					int bestj = 0;
					dBest = (ValuesArray[kk])[0][nBestJob];
					for(i=1; i<nJobsToEval; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
					{	k = pIndex[i];
						if(((ValuesArray[kk])[0][k] < dBest))
						{	
							dBest = (ValuesArray[kk])[0][k];
							nBestJob = k;
							bestj = i;
						}
					}
					votes[bestj]++;
				}

				int od = 0;

				for(int kk = 1; kk<nJobsToEval; kk++){
					if(votes[kk]>votes[od]/*||(votes[kk]==votes[od]&&prioritet[kk]<prioritet[od])*/){
						od = kk;
						nBestJob = od;
					}
				}

				nBestJob = pIndex[od];

			
			
			
			//for(i=1; i<nJobsToEval; i++)
			//{	if(pVrijednosti[pIndex[i]] < dBest)
			//	{	dBest = pVrijednosti[pIndex[i]];
			//		nBestJob = pIndex[i];
			//	}
			//}

			// nBestJob na nMachine
			nOperation = pOperationsScheduled[nBestJob];	// sadasnja operacija (koja ce se izvesti)
			pOperationsScheduled[nBestJob] += 1;	// novi broj rasporedjenih operacija posla
			pTotalWorkRemaining[nBestJob] -= Durations[nBestJob][nOperation];
			pMachineWorkRemaining[nMachine] -= Durations[nBestJob][nOperation];
			pTotalWorkDone[nBestJob] += Durations[nBestJob][nOperation];
			if(m_setup)
			{	dSetupTime = Setup[pLastJob[nMachine]][nBestJob];
				pLastJob[nMachine] = nBestJob;
			}
			else
				dSetupTime = 0;
			dClock = MAX(dClock, pJobReady[nBestJob]);	// ako smo odlucili pricekati
			MachineReady[nMachine][0] = dClock + dSetupTime + Durations[nBestJob][nOperation];
			pJobReady[nBestJob] = MachineReady[nMachine][0];	// kada ce sljedeca operacija biti raspoloziva
			if(nOperation < nOperations-1)	// ako posao ima jos operacija
			{	nNextMachine = (int) MachineIndex[nBestJob][pOperationsScheduled[nBestJob]];
				if(pOperationReady[nNextMachine] < 0)	// ako stroj trenutno nije trazen...
					pOperationReady[nNextMachine] = pJobReady[nBestJob];	// ... bit ce tada
				else	// provjeri dolazi li moja sljedeca operacija prije trenutno najblize za taj stroj
					if(pOperationReady[nNextMachine] > pJobReady[nBestJob])
						pOperationReady[nNextMachine] = pJobReady[nBestJob];
			}
			// kada cu ja (stroj) opet biti trazen?
			pOperationReady[nMachine] = -1;
			pOperationsWaiting[nMachine] = 0;
			for(j=0; j<nPoslova; j++)
			{	nNextOperation = pOperationsScheduled[j];
				if(nNextOperation == nMachines) continue;	// ovaj nema vise operacija
				nNextMachine = (int) MachineIndex[j][nNextOperation];
				if(nNextMachine != nMachine) continue;
				pOperationsWaiting[nMachine]++;
				if(pOperationReady[nMachine] < 0)
					pOperationReady[nMachine] = pJobReady[j];
				else
					if(pOperationReady[nMachine] > pJobReady[j])
						pOperationReady[nMachine] = pJobReady[j];
			}
			nOperation = (int) pMachineScheduled[nMachine];
			pMachineScheduled[nMachine] += 1;
			Schedule[nNiz][nMachine * nPoslova + nOperation] = nBestJob;
		}	// kraj rasporedjivanja skupa

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(i=nPoslova*nMachines ; i < (uint)Schedule.GetCol(); i++)
					Schedule[nNiz][i] = 0;			// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dAvgWeights = 0;
			dCmax = dF = dFwt = dT = dTwt = dN = dNwt = 0;
			for(j=0; j<nPoslova; j++)
			{	dAvgWeights += WeightT[nNiz][j];
				if(dCmax < pJobReady[j])
					dCmax = pJobReady[j];
				dF += pJobReady[j] - Ready[nNiz][j];
				dFwt += (pJobReady[j] - Ready[nNiz][j]) * WeightT[nNiz][j];
				if(pJobReady[j] > Deadline[nNiz][j])
				{	dN += 1;
					dNwt += WeightT[nNiz][j];
					dL = pJobReady[j] - Deadline[nNiz][j];
					dT += dL;
					dTwt += WeightT[nNiz][j] * dL;
				}
			}
			dAvgWeights /= nPoslova;
			dCmax /= nPoslova*dAvgDuration;
			dF /= nPoslova;
			dFwt /= nPoslova*dAvgWeights*dAvgDuration;
			dT /= nPoslova;
			dTwt /= nPoslova*dAvgWeights*dAvgDuration;
			dN /= nPoslova;
			dNwt /= nPoslova*dAvgWeights;
			dTotalCmax += dCmax;
			dTotalF += dF;
			dTotalFwt += dFwt;
			dTotalT += dT;
			dTotalTwt += dTwt;
			dTotalN += dN;
			dTotalNwt += dNwt;

			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTwt; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			Fitness[nNiz][Twt] = dTwt;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness.data[sets][Twt] = dTotalTwt;
	Fitness.data[sets][Nwt] = dTotalNwt;
}

void SchedulingEvalOp::EvaluateJobShopSum(double &dRawFitness)
{
	double dL, dT, dTwt, dF, dFwt, dN, dNwt, dCmax;
	double dTotalT, dTotalTwt, dTotalF, dTotalFwt, dTotalN, dTotalNwt, dTotalCmax;
	double dClock, dAvgWeights, dTotalAvgLoad, dBestMachineValue, dAvgDuration;
	double dBest, dSetupTime, m1, m2, dNajkrace, dBegin;
	unsigned int nPoslova, nNiz, nJob, i, j, k, nNr, nOperations, nOperation, nNextOperation;
	unsigned int nMachines, nMachine, nSchedule, nMachineIndex, nJobsToEval, nBestJob, nNextMachine;
	unsigned int nBottleneck;
					double dBestArray[12];
				int odabraniArray[12];
	dRawFitness=0;
	dTotalT = dTotalTwt = dTotalF = dTotalFwt = dTotalN = dTotalNwt = dTotalCmax = 0;

// vrtimo sve skupove
	for(nNiz=0; nNiz<sets; nNiz++)
	{	nNr = nPoslova = (int) N.Get(nNiz);
	// preliminarna ispitivanja
		// radimo li stochastic sampling
		if(m_stsampling)
			if(pSamples[nNiz] == false)
				continue;	// jednostavno preskocimo taj test case
		// gleda li se limited error fitness
		if(m_LEF)
		{	if(dRawFitness > m_LEFVal)
				break;	// prekid ispitivanja
		}
		if(m_setup)	// trajanja postavljanja
			MakeSetup(Duration,nNiz,nPoslova,m_setup_faktor,Setup);
	// postavljanje pocetnih vrijednosti za pojedini skup
		dClock = 0;
		dTotalAvgLoad = 0;	// prosjecno opterecenje svih strojeva
		nOperations = nMachines = (uint) Machines[nNiz][0];
		dBestMachineValue = 0;
		nBottleneck = nMachines;	// nijedan nije bottleneck na pocetku
		for(j=0; j<nMachines; j++)
		{	MachineReady[j][0] = 0;		// pocetno vrijeme raspolozivosti strojeva
			pLastJob[j] = nPoslova;		// pocetni prethodni posao
			pMachineScheduled[j] = 0;	// broj rasporedjenih operacija na stroju
			pOperationReady[j] = -1;	// raspolozivost operacija za strojeve tek treba odrediti
			pTotalMachineWork[j] = 0;	// ukupno opterecenje stroja
			pOperationsWaiting[j] = 0;	// broj operacija na cekanju
			pMachineValues[j] = 0;		// vrijednosti strojeva - racunaju se stablom odluke
		}
		for(i=0; i<nPoslova; i++)
		{	pTotalWorkRemaining[i] = 0;
			pTotalWorkDone[i] = 0;
			pIndex[i] = i;	// inicijalni poredak
			pJobReady[i] = Ready[nNiz][i];	// Ready su nule u statickom slucaju
			pOperationsScheduled[i] = 0;	// broj rasporedjenih operacija posla
			for(j=0; j<nOperations; j++)
			{	k = (int) Duration[nNiz][i*nOperations + j];
				nMachine = k / 1000;	// indeks stroja
				if(j == 0)	// za prvu operaciju
					pOperationsWaiting[nMachine]++;
				k = k % 1000;		// trajanje operacije
				pTotalWorkRemaining[i] += k;
				Durations[i][j] = k;
				dTotalAvgLoad += k;
				MachineIndex[i][j] = nMachine;
				pTotalMachineWork[nMachine] += k;
			}
			// pogledamo na kojem je stroju prva operacija posla
			nMachine = (int) MachineIndex[i][0];
			pOperationReady[nMachine] = pJobReady[i];
			// znaci taj stroj je trazen u trenutku dolaska posla (0 u statickom slucaju)
		}
		dAvgDuration = dTotalAvgLoad / nPoslova;	// prosjecno trajanje poslova
		dTotalAvgLoad /= nMachines;
		Evaluator.SetTermArraySize(nPoslova);	// koliko poslova u skupu - za vektorsku evaluaciju
		Evaluator.pIndex = pIndex;	// polje indeksa poslova
		Evaluator.m_iOffset = 0;		// indeks prvog nerasporedjenog
		//Evaluator.m_iEnd = 1;	// pocetna vrijednost zadnjeg posla koji se uzima u o obzir
		// postavljanje pocetnih vrijednosti terminala - nepromjenjivi
		memcpy(Evaluator.m_dTermValuesArray[T_dd], Deadline.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_w],  WeightT.data[nNiz], nPoslova*sizeof(double));
		memcpy(Evaluator.m_dTermValuesArray[T_TWK], pTotalWorkRemaining, nPoslova*sizeof(double));
		// nepromjenjivi terminali za strojeve
		for(i=0; i<nMachines; i++)
		{	Evaluator.m_dTermValuesArray[T_MTWKav][i] = dTotalAvgLoad;
			Evaluator.m_dTermValuesArray[T_MTWK][i] = pTotalMachineWork[i];
			pMachineWorkRemaining[i] = pTotalMachineWork[i];
		}
		memcpy(Evaluator.m_dTermValuesArray[T_MTWK], pTotalMachineWork, nMachines*sizeof(double));

		for(nSchedule = 0; nSchedule < nPoslova*nMachines; nSchedule++)
		{	// treba pronaci stroj koji ima najranije vrijeme raspolozivosti i sebe i operacije za njega
			for(nMachine = 0; nMachine<nMachines; nMachine++)
				if(pOperationReady[nMachine] >= 0)	// je li stroj uopce trazen
					break;
			for(i = nMachine; i < nMachines; i++)
			{	if(pOperationReady[i] < 0)	// jos se nije pojavila potreba za tim strojem
					continue;
				m1 = MAX(MachineReady[i][0],pOperationReady[i]);
				m2 = MAX(MachineReady[nMachine][0],pOperationReady[nMachine]);
				if(m1 < m2)	// ima neki stroj koji je prije raspoloziv za rasporediti
					nMachine = i;
			}
			dClock = MAX(MachineReady[nMachine][0], pOperationReady[nMachine]);
			// pronadjemo kada bilo koja operacija moze najprije zavrsiti
			dNajkrace = -1;
			for(nJob=0; nJob<nPoslova; nJob++)
			{	nOperation = pOperationsScheduled[nJob];
				if(nOperation == nMachines) continue;		// ovaj posao je zavrsio
				nMachineIndex = (int) MachineIndex[nJob][nOperation];
				if(nMachineIndex != nMachine) continue;	// ovaj posao ne ceka na taj stroj
				if(dNajkrace < 0)
					dNajkrace = MAX(dClock, pJobReady[nJob]) + Durations[nJob][nOperation];
				else
				{	dBegin = MAX(dClock, pJobReady[nJob]);
					if(dNajkrace > dBegin + Durations[nJob][nOperation])
						dNajkrace = dBegin + Durations[nJob][nOperation];
				}
			}
			// sada treba pronaci operaciju najveceg prioriteta koja ceka na taj stroj
			// radimo tako da poslove cije operacije treba ocijeniti stavimo na pocetak pIndex
			// prije evaluacije odredimo koliko ih ima (kao m_iEnd u Evaluatoru)
			nJobsToEval = 0;
			for(nJob=0; nJob<nPoslova; nJob++)
			{	nOperation = pOperationsScheduled[nJob];
				if(nOperation == nMachines) continue;		// ovaj posao je zavrsio
				nMachineIndex = (int) MachineIndex[nJob][nOperation];
				if(nMachineIndex != nMachine) continue;	// ovaj posao ne ceka na taj stroj
				// mozemo uzeti u obzir samo one koji su vec spremni:
				if(!m_Idleness)
					if(pJobReady[nJob] > dClock) continue;	// jos nije zavrsila prethodna operacija
				// ili mozemo uzeti u obzir i one koji ce tek zavrsiti, ali ne kasnije od moguceg zavrsetka najkrace spremne operacije
				dBegin = MAX(pJobReady[nJob],dClock);
				if(dBegin > dNajkrace)	// nema smisla gledati ako pocinje iza najkraceg zavrsetka
					continue;
				// ako smo dosli ovdje, uzet cemo u obzir operaciju nOper. posla nJob na stroju nMachine
				pIndex[nJobsToEval] = nJob;
				nJobsToEval++;
			}
#ifdef TREES
			// pomocu prvog stabla odlucimo koja od dva preostala koristimo za prioritete
*
MNOPr[nMachine] = nOperations - pMachineScheduled[nMachine]
MTWKav = dTotalAvgLoad;
MTWK[nMachine] = pTotalMachineWork[nMachine];
MNOPw[nMachine] = pOperationsWaiting[nMachine];
MTWKr[nMachine] = pMachineWorkRemaining[nMachine];
MUTL[nMachine] = (pTotalMachineWork[nMachine] - pMachineWorkRemaining[nMachine]) / dClock;
*/
			Evaluator.m_nTree = 0;
			// prvo stablo racunamo samo za nMachine - pa za taj stroj racunamo terminale
			Evaluator.m_dTermValuesArray[T_MNOPr][nMachine] = nOperations - pMachineScheduled[nMachine];
			Evaluator.m_dTermValuesArray[T_MNOPw][nMachine] = pOperationsWaiting[nMachine];
			Evaluator.m_dTermValuesArray[T_MTWKr][nMachine] = pMachineWorkRemaining[nMachine];
			if(dClock == 0)
				Evaluator.m_dTermValuesArray[T_MUTL][nMachine] = 1;
			else
				Evaluator.m_dTermValuesArray[T_MUTL][nMachine] = (pTotalMachineWork[nMachine] - pMachineWorkRemaining[nMachine]) / dClock;

			Evaluator.m_iPosition = -1;
			nSavedIndex = pIndex[nMachine];	// pohranimo indeks posla koji se tu nalazi
			pIndex[nMachine] = nMachine;
			Evaluator.m_iOffset = nMachine;
			Evaluator.m_iEnd = nMachine + 1;
			Evaluator.evaluate_array(pVrijednosti);	// ocijenimo stroj u stablu odluke
			pMachineValues[nMachine] = pVrijednosti[nMachine];
			// MODIFIKACIJA 15.09.: uvijek pronadjemo novi dBestMachineValue kao Bottleneck
*			dBestMachineValue = pMachineValues[0];
			nBottleneck = 0;
			for(i=0; i<nMachine; i++)
				if(pMachineValues[i] > dBestMachineValue)
				{	dBestMachineValue = pMachineValues[i];
					nBottleneck = i;
				}
*/
			// MODIFIKACIJA 13.09.: uvijek pamtimo stroj koji je postigao najvecu vrijednost
			// (dok se ne postigne veca)
			if(pMachineValues[nMachine] >= dBestMachineValue)	// je li nova najveca vrijednost?
			{	dBestMachineValue = pMachineValues[nMachine];
				nBottleneck = nMachine;
			}

			if(nMachine == nBottleneck)	// jesam li ja bottleneck?
				Evaluator.m_nTree = 2;	// za najoptereceniji stroj
			else
				Evaluator.m_nTree = 1;	// za sve ostale
			pIndex[nMachine] = nSavedIndex;	// vratimo indeks posla
#else	// samo jedno stablo
			Evaluator.m_nTree = 0;	// vratimo na obicno stablo
#endif
			// vektorska evaluacija!
			// trebamo definirati vrijednosti terminala za trenutne operacije odabranih poslova
			for(i=0; i<nJobsToEval; i++)
			{	nJob = pIndex[i];
				nOperation = pOperationsScheduled[nJob];
				Evaluator.m_dTermValuesArray[T_pt][nJob] = Durations[nJob][nOperation];
				Evaluator.m_dTermValuesArray[T_CLK][nJob] = dClock;
				Evaluator.m_dTermValuesArray[T_AR][nJob] = POS(pJobReady[nJob] - dClock); 
				Evaluator.m_dTermValuesArray[T_STP][nJob] = Setup[pLastJob[nMachine]][nJob];
				Evaluator.m_dTermValuesArray[T_Sav][nJob] = pSetupAvg[pLastJob[nMachine]];
				Evaluator.m_dTermValuesArray[T_age][nJob] = POS(dClock - Ready[nNiz][nJob]);
				Evaluator.m_dTermValuesArray[T_NOPr][nJob] = nOperations - pOperationsScheduled[nJob];
				Evaluator.m_dTermValuesArray[T_PTav][nJob] = PTimeAvg[nNiz][nMachine];
				Evaluator.m_dTermValuesArray[T_TWKr][nJob] = pTotalWorkRemaining[nJob];
				if(pTotalWorkDone[nJob] == 0)
					Evaluator.m_dTermValuesArray[T_HTR][nJob] = 1;
				else
					Evaluator.m_dTermValuesArray[T_HTR][nJob] = POS(dClock - Ready[nNiz][nJob]) / pTotalWorkDone[nJob];
			}

			for(int kk = 0; kk<ensenbleCount; kk++){
				Evaluator.m_iPosition = -1;
				Evaluator.m_nTree = kk;
				Evaluator.m_iEnd = nJobsToEval;
				Evaluator.evaluate_array(pVrijednostiArray[kk]);	// ocijenimo sve
				memcpy((ValuesArray[kk])[0],pVrijednostiArray[kk],nPoslova*sizeof(double));					
			}

			//Evaluator.m_iPosition = -1;
			//Evaluator.m_iOffset = 0;
			//Evaluator.m_iEnd = nJobsToEval;	// toliko ih ima za ocijeniti
			//Evaluator.evaluate_array(pVrijednosti);	// ocijenimo sve u odabranom stablu
			dBest = pVrijednosti[pIndex[0]]; // poc. vrijednost za usporedbu
			nBestJob = pIndex[0];
			//for(i=1; i<nJobsToEval; i++)
			//{	if(pVrijednosti[pIndex[i]] < dBest)
			//	{	dBest = pVrijednosti[pIndex[i]];
			//		nBestJob = pIndex[i];
			//	}
			//}

			vector<double> prioritet;
			for(int kk=0; kk<nJobsToEval; kk++){
				prioritet.push_back(0);
			}
				

			for(i=0; i<nJobsToEval; i++)	// trazimo najbolju vrijednost unutar < (dClock + kada)
				{	k = pIndex[i];
						for(int kk=0; kk<ensenbleCount; kk++){
							prioritet[i]+=(ValuesArray[kk])[0][k];
							//cout<<"ValuesArrat: "<<(ValuesArray[kk])[0][i]<<endl;
						}
					
				}

				dBest = 9999999999999;
				for(i = 0; i<nJobsToEval; i++){
					k = pIndex[i];
					if(prioritet[i] < dBest)
					{
						nBestJob = pIndex[i]; 
						dBest = prioritet[i];
					}
				}

			// nBestJob na nMachine
			nOperation = pOperationsScheduled[nBestJob];	// sadasnja operacija (koja ce se izvesti)
			pOperationsScheduled[nBestJob] += 1;	// novi broj rasporedjenih operacija posla
			pTotalWorkRemaining[nBestJob] -= Durations[nBestJob][nOperation];
			pMachineWorkRemaining[nMachine] -= Durations[nBestJob][nOperation];
			pTotalWorkDone[nBestJob] += Durations[nBestJob][nOperation];
			if(m_setup)
			{	dSetupTime = Setup[pLastJob[nMachine]][nBestJob];
				pLastJob[nMachine] = nBestJob;
			}
			else
				dSetupTime = 0;
			dClock = MAX(dClock, pJobReady[nBestJob]);	// ako smo odlucili pricekati
			MachineReady[nMachine][0] = dClock + dSetupTime + Durations[nBestJob][nOperation];
			pJobReady[nBestJob] = MachineReady[nMachine][0];	// kada ce sljedeca operacija biti raspoloziva
			if(nOperation < nOperations-1)	// ako posao ima jos operacija
			{	nNextMachine = (int) MachineIndex[nBestJob][pOperationsScheduled[nBestJob]];
				if(pOperationReady[nNextMachine] < 0)	// ako stroj trenutno nije trazen...
					pOperationReady[nNextMachine] = pJobReady[nBestJob];	// ... bit ce tada
				else	// provjeri dolazi li moja sljedeca operacija prije trenutno najblize za taj stroj
					if(pOperationReady[nNextMachine] > pJobReady[nBestJob])
						pOperationReady[nNextMachine] = pJobReady[nBestJob];
			}
			// kada cu ja (stroj) opet biti trazen?
			pOperationReady[nMachine] = -1;
			pOperationsWaiting[nMachine] = 0;
			for(j=0; j<nPoslova; j++)
			{	nNextOperation = pOperationsScheduled[j];
				if(nNextOperation == nMachines) continue;	// ovaj nema vise operacija
				nNextMachine = (int) MachineIndex[j][nNextOperation];
				if(nNextMachine != nMachine) continue;
				pOperationsWaiting[nMachine]++;
				if(pOperationReady[nMachine] < 0)
					pOperationReady[nMachine] = pJobReady[j];
				else
					if(pOperationReady[nMachine] > pJobReady[j])
						pOperationReady[nMachine] = pJobReady[j];
			}
			nOperation = (int) pMachineScheduled[nMachine];
			pMachineScheduled[nMachine] += 1;
			Schedule[nNiz][nMachine * nPoslova + nOperation] = nBestJob;
		}	// kraj rasporedjivanja skupa

		// racunanje raznih kriterija za trenutni skup
		{	if(m_Evaluation)
			{	for(i=nPoslova*nMachines ; i < (uint)Schedule.GetCol(); i++)
					Schedule[nNiz][i] = 0;			// ostalo nule
			}
			// odredimo fitnes kriterij - sve moguce funkcije
			dAvgWeights = 0;
			dCmax = dF = dFwt = dT = dTwt = dN = dNwt = 0;
			for(j=0; j<nPoslova; j++)
			{	dAvgWeights += WeightT[nNiz][j];
				if(dCmax < pJobReady[j])
					dCmax = pJobReady[j];
				dF += pJobReady[j] - Ready[nNiz][j];
				dFwt += (pJobReady[j] - Ready[nNiz][j]) * WeightT[nNiz][j];
				if(pJobReady[j] > Deadline[nNiz][j])
				{	dN += 1;
					dNwt += WeightT[nNiz][j];
					dL = pJobReady[j] - Deadline[nNiz][j];
					dT += dL;
					dTwt += WeightT[nNiz][j] * dL;
				}
			}
			dAvgWeights /= nPoslova;
			dCmax /= nPoslova*dAvgDuration;
			dF /= nPoslova;
			dFwt /= nPoslova*dAvgWeights*dAvgDuration;
			dT /= nPoslova;
			dTwt /= nPoslova*dAvgWeights*dAvgDuration;
			dN /= nPoslova;
			dNwt /= nPoslova*dAvgWeights;
			dTotalCmax += dCmax;
			dTotalF += dF;
			dTotalFwt += dFwt;
			dTotalT += dT;
			dTotalTwt += dTwt;
			dTotalN += dN;
			dTotalNwt += dNwt;

			switch(m_fitness)	// sto uzimamo kao fitnes?
			{	case Twt: dRawFitness += dTwt; break;
				case Nwt: dRawFitness += dNwt; break;
				case Fwt: dRawFitness += dFwt; break;
				case Cmax: dRawFitness += dCmax; break;
				default: CHECK(0);
			}
			Fitness[nNiz][Twt] = dTwt;
			Fitness[nNiz][Nwt] = dNwt;
			Fitness[nNiz][FTwt] = 0;	// zasada
			Fitness[nNiz][ETwt] = 0;
			Fitness[nNiz][Fwt] = dFwt;
			Fitness[nNiz][Cmax] = dCmax;
		}

	} // kraj petlje koja vrti skupove poslova
	Fitness.data[sets][Twt] = dTotalTwt;
	Fitness.data[sets][Nwt] = dTotalNwt;
}
