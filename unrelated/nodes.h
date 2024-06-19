// definicija svih funkcija i terminala koji se mogu koristiti
#ifndef NODES
#define NODES
#include<string>
using namespace std;

const int MAX_JOBS = 200;	// najveci broj poslova

//Marko: 27.12. promijenio iz razloga sto mi je trebalo za ensemle learning
const int MAX_TREES = 50;	// zasada :) najvise 3 stabla


//#define TREES				// ako koristimo visestablenu strukturu za jobshop!
//#define ADAPTATION			// ako koristimo prilagodbu operatora i cvorova

// Postupak za dodavanje novog terminala:
// - definirati konstantu (ovdje)
// - definirati ime terminala (fitnes.cpp: konstruktor)
// - omoguciti citanje terminala iz stabla (fitnes.cpp:ReadIndividual)
const int ONE = 1;		// specijalni terminali
const int NUL = 0;
const int OFFSET = 2;	// pocetak terminala
// za SINGLE
const int T_N = OFFSET + 0;
const int T_SP = OFFSET + 1;
const int T_SD = OFFSET + 2;
const int T_pt = OFFSET + 3;
const int T_dd = OFFSET + 4;
const int T_SL = OFFSET + 5;
const int T_w = OFFSET + 6;
const int T_Nr = OFFSET + 7;
const int T_SPr = OFFSET + 8;
const int T_L = OFFSET + 9;		// kasnjenje
const int T_SLr = OFFSET + 10;	// prosjecni slack svih preostalih poslova
const int T_AR = OFFSET + 11;	// (ready - clock)_+
const int T_SC = OFFSET + 12;	// koliko sljedbenika
const int T_LVL = OFFSET + 13;	// level of a node
const int T_STP = OFFSET + 14;	// trajanje postavljanja sa prethodnog na promatrani zadatak
const int T_Sav = OFFSET + 15;	// prosjecno trajanje postavljanja sa prethodnog na sve ostale zadatke
const int T_CLK = OFFSET + 16;
const int T_TF = OFFSET + 17;	// tardines faktor (procjena): 1 - dd_avg/SP
// UNIFORM
const int T_SPD = OFFSET + 18;	// brzina konkretnog stroja
const int T_Msm = OFFSET + 19;	// suma brzina svih strojeva
const int T_SLs = OFFSET + 20;	// slack koji uzima u obzir brzinu stroja
// UNRELATED
const int T_pmin = OFFSET + 21;	// najkrace trajanje izvodjenja zadatka (ovisno o stroju)
const int T_pavg = OFFSET + 22;	// srednje trajanje izvodjenja zadatka
const int T_PAT  = OFFSET + 23; // za koliko ce stroj koji daje najmanje trajanje izvodjenja biti slobodan
const int T_MR = OFFSET + 24;	// za koliko ce doticni stroj biti slobodan (machine ready - clock)
const int T_age = OFFSET + 25;	// koliko dugo je posao vec u sustavu (clock - ready)_+
// JOBSHOP
// pt	- trajanje operacije
// dd	- due date posla
// w	- tezina
// AR	- trajanje do dolaska operacije (idle time)
// CLK	- trenutno vrijeme
// STP	- trajanje postavljanja
// Sav	- prosjecno t. p. sa prethodne na ostale operacije (ovisno o poslu)
// age	- vrijeme proteklo od dolaska posla kojemu operacija pripada
const int T_NOPr = OFFSET + 26;	// ukupni broj preostalih operacija posla
const int T_TWK = OFFSET + 27;	// ukupno trajanje posla
const int T_TWKr = OFFSET + 28;	// preostalo trajanje posla (ukljucujuci i trenutnu operaciju)
const int T_PTav = OFFSET + 29;	// prosjecno trajanje operacija na stroju
const int T_HTR = OFFSET + 30;	// head time ratio - omjer vremena provedenog u sustavu i kolicine prethodnog posla
								// HTR = age / (suma trajanja obavljenih operacija)
								// HTR = 1 ako do sada nije bilo obavljenog posla
// terminali za strojeve
const int T_MNOPr = OFFSET + 31;	// preostali broj operacija na stroju
const int T_MNOPw = OFFSET + 32;	// broj operacija na cekanju
const int T_MTWK = OFFSET + 33;		// ukupno trajanje svih operacija na stroju
const int T_MTWKr = OFFSET + 34;	// trajanje preostalih operacija na stroju
const int T_MTWKav = OFFSET + 35;	// prosjecno trajanje svih operacija za sve strojeve
const int T_MUTL = OFFSET + 36;		// omjer rada i ukupnog vremena do sada

//staticni terminali
//broj nereleasanih (izvan horizonta) poslova za neki stroj koji imaju najkrace vrijeme izvodenja
const int T_NSHORT = OFFSET + 37;
//slack iduceg posla koji dolazi u sustav
const int T_SLNEXT = OFFSET + 38;
//slack iduceg posla koji dolayi u sustav i ima minimalno izvodenje za dani stroj
const int T_SLNEXTM = OFFSET + 39;
//time till arrival of next job
const int T_TTAR = OFFSET + 40;
//time till arrival of next job with the min execution time for machine
const int T_TTARM = OFFSET + 41;
//machine execution time off all unreleased jobs
const int T_MLOAD = OFFSET + 42;
//possible future lateness if current job is scheduled
const int T_FUTLATE = OFFSET + 43; 
//number of jobs which will be released during the execution of the selected job
const int T_NREL = OFFSET +44;
//number of jobs with fastest execution time which will be released during the execution of the selected job
const int T_NRELM = OFFSET +45;
//number of weighted late jobs during the execution of the current job
const int T_WLATE = OFFSET +46;
//diff between lateness if current job were not scheduled and if jobs which are released during its execution would not be scheduled
const int T_FLD = OFFSET + 47;
//diff between the weight of current job and T_WLATE
const int T_WLD = OFFSET + 48;
//average of slack times of jobs with shortest execution times for the gioven machine
const int T_SLAVGM = OFFSET + 49;
//average of slacks of jobs which appear during the execution of the next job
const int T_SLAVGD = OFFSET + 50;
//machine load of jobs  which appear only during the execution of the next job
const int T_MLOADD = OFFSET + 51;
//FLD but only with a single job
const int T_FLDS = OFFSET + 52;
//WLD but only with a single job
const int T_WLDS = OFFSET +53;
//FLD alternative
const int T_FLDL = OFFSET + 54;
//WLD alternative
const int T_WLDL = OFFSET +55;
//
const int T_FUTLATES = OFFSET + 56; 
const int T_FUTLATEL = OFFSET + 57;
const int T_WLATES = OFFSET + 58;
const int T_WLATEL = OFFSET + 59;

//IDR terminals
const int T_NLATE = OFFSET+60;
const int T_ISLATE = OFFSET+61;
const int T_LATENESS = OFFSET+62;
const int T_INDLATE = OFFSET+63;
const int T_JOBFINISH = OFFSET +64;
const int T_INDTARD = OFFSET +65;
const int T_INDWTARD = OFFSET +66;
const int T_FLOWTIME = OFFSET +67;
const int T_TARDINESS = OFFSET + 68;

const int TERMINALS = 69;		// broj terminala BEZ pocetna dva (OFFSET)

const int ADD = 100;		// pocetak funkcija
const int SUB = 101;
const int MUL = 102;
const int DIV = 103;
const int POS = 104;		// POS(x) = max{x,0}
const int SIN = 105;
const int COS = 106;
const int EXP = 107;
const int LOG = 108;
const int SQR = 109;		// SQR(x) = 1 ako x<0
const int IFGT = 110;
const int IFLT = 111;
const int MAX = 112;
const int MIN = 113;
const int AVG = 114;
const int ABS = 115;
const int LATE = 116;
const int FUNC_START = 100;
const int FUNC_END = 117;
const int TOTAL_NODES = FUNC_END;	// ukupan broj terminala i funkcija (zapravo velicina polja Nodes)

struct _node
{	std::string name;	// ime terminala
	double value;		// vrijednost - zasada samo za 0 i 1
	bool active;		// jesu li ukljuceni u terminal set
	unsigned int frequency;	// brojac terminala
};
typedef struct _node node;

// definicije oznaka fitnes funkcija
const int FUNCTIONS = 7;
const int Twt = 0;
const int Nwt = 1;
const int FTwt = 2;
const int ETwt = 3;
const int Fwt = 4;
const int Cmax = 5;
const int TwtCmax = 6;
const int NwtCmax = 7;

#endif // NODES