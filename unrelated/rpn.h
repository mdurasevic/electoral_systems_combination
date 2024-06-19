#ifndef RPN_Class
#define RPN_Class

#include <cmath>
#include <stdlib.h>
#include <string>
#include "nodes.h"
extern node Nodes[TOTAL_NODES];
typedef unsigned int uint;

class RPN
{
public:
	int *m_pExpression[MAX_TREES], *m_pEdited;	// zapis originalnih odnosno editiranog stabla
	uint m_iExprSize, m_iEditSize;
	double m_pTermValues[TERMINALS+OFFSET];
	double m_dTermValuesArray[TERMINALS+OFFSET][MAX_JOBS];	// vrijednosti terminala za vektorsku evaluaciju
	int m_iTermNum;
	double m_pNodeFreq[TOTAL_NODES];	// double zbog lakseg kopiranja u matricu
	uint m_iArraySize;	// velicina polja za zajednicku evaluaciju
	uint m_iOffset, m_iEnd;	// pocetni i krajnji element u polju indeksa poslova za koje se racuna stablo
	uint *pIndex;			// polje indeksa poslova - svim terminalima pristupa se preko toga polja
	int m_iPosition, m_iEditedPos;	// pozicija u originalnom odnosno editiranom stablu
	uint m_nTree;		// oznacava aktivno stablo (default = 0)
	std::string m_output;
	
	double evaluate();
	void evaluate_array(double dResult[]);
	int edit();
	void copy();
	void ResetNodeFreq();
	void write();
	void r_write();
	
	RPN()
	{	for(int i=0; i<MAX_TREES; i++)
			m_pExpression[i] = NULL;
		m_pEdited = NULL;
		m_pTermValues[NUL] = 0;
		m_pTermValues[ONE] = 1;
		for(int i=0; i<MAX_JOBS; i++)
		{	m_dTermValuesArray[NUL][i] = 0;
			m_dTermValuesArray[ONE][i] = 1;
		}
		m_iTermNum = TERMINALS+OFFSET;
		ResetNodeFreq();		// brojace terminala stavljamo na nulu
		m_nTree = 0;			/// po defaultu aktivno stablo = 0
	}
	~RPN()
	{	for(int i=0; i<MAX_TREES; i++)
			if(m_pExpression[i] != NULL) delete [] m_pExpression[i];
		if(m_pEdited != NULL) delete [] m_pEdited;
	}
	void SetExprSize(uint _size)
	{	for(int i=0; i<MAX_TREES; i++)
		{	if(m_pExpression[i] != NULL) delete [] m_pExpression[i];
			m_pExpression[i] = new int[_size];
		}
		if(m_pEdited != NULL) delete [] m_pEdited;
		m_pEdited = new int[_size];
		m_iExprSize = _size;
	}
	void SetExprSize(uint _size, int tree)
	{	
			if(m_pExpression[tree] != NULL) delete [] m_pExpression[tree];
			m_pExpression[tree] = new int[_size];
		
		if(m_pEdited != NULL) delete [] m_pEdited;
		m_pEdited = new int[_size];
		m_iExprSize = _size;
	}

	void SetTermArraySize(uint _size)
	{	/*int i;
		for(i=0; i<TERMINALS+OFFSET; i++)
		{	if(m_pTermValuesArray[i])
				delete [] m_pTermValuesArray[i];
			m_pTermValuesArray[i] = new double[_size];
		}*/
		m_iArraySize = m_iEnd = _size;
	}
};

#endif // RPN