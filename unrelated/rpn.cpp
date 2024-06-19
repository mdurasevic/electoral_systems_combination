#include "rpn.h"

//#define STARO	// ovo treba ukljuciti ako se provjeravaju stara rjesenja (sa greskom u editiranju)

// evaluiramo za sve poslove odjednom!
// broj poslova je konstantan za pojedini test skup: m_iArraySize
// m_iOffset je prvi posao kojega uzimamo u obzir; prethodni su vec rasporedjeni!
// pIndex je polje u kojemu su do m_iOffset-a indeksi svih rasporedjenih, a iza svi ostali
// dodano 10.08.: idemo od Offset do End (ne gledamo poslove iza End u polju indeksa)
// dodano 06.09.: m_nTree odredjuje koje se stablo evaluira (default = 0)
void RPN::evaluate_array(double dResult[])
{
	double d1[MAX_JOBS],d2[MAX_JOBS],d3[MAX_JOBS],d4[MAX_JOBS];
	uint i;
	m_iPosition++; // pomicemo se na sljedeci simbol; pocetna vrijednost mora biti -1 !!!
	switch(m_pExpression[m_nTree][m_iPosition])
	{
		case ADD:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = d1[pIndex[i]] + d2[pIndex[i]];
			break;
		case SUB:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = d1[pIndex[i]] - d2[pIndex[i]];
			break;
		case MUL:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = d1[pIndex[i]] * d2[pIndex[i]];
			break;
		case DIV:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
			{	if(fabs(d2[pIndex[i]]) < 0.000001)
					dResult[pIndex[i]] = 1;
				else
					dResult[pIndex[i]] = d1[pIndex[i]] / d2[pIndex[i]];
			}
			break;
		case POS:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]] > 0)
					dResult[pIndex[i]] = d1[pIndex[i]];
				else
					dResult[pIndex[i]] = 0;
			break;
		case SIN:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = sin(d1[pIndex[i]]);
			break;
		case COS:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = cos(d1[pIndex[i]]);
			break;
		case EXP:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]] < 80.)
					dResult[pIndex[i]] = cos(d1[pIndex[i]]);
				else
					dResult[pIndex[i]] = 1;
			break;
		case LOG:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(fabs(d1[pIndex[i]]) > 0.000001)
					dResult[pIndex[i]] = log(fabs(d1[pIndex[i]]));
				else
					dResult[pIndex[i]] = 1;
			break;
		case SQR:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]] < 0)
					dResult[pIndex[i]] = 1;
				else
					dResult[pIndex[i]] = sqrt(d1[pIndex[i]]);
			break;
		case MAX:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]]>d2[pIndex[i]])
					dResult[pIndex[i]]= d1[pIndex[i]];
				else
					dResult[pIndex[i]]= d2[pIndex[i]];
			break;
		case MIN:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]]>d2[pIndex[i]])
					dResult[pIndex[i]]= d2[pIndex[i]];
				else
					dResult[pIndex[i]]= d1[pIndex[i]];
			break;
		case AVG:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = (d1[pIndex[i]] + d2[pIndex[i]])/2;
			break;

		case ABS:
			evaluate_array(d1);
			for(i=m_iOffset; i<m_iEnd; i++)
				dResult[pIndex[i]] = fabs(d1[pIndex[i]]);
			break;

		case IFGT:
			evaluate_array(d1);
			evaluate_array(d2);
			evaluate_array(d3);
			evaluate_array(d4);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]] > d2[pIndex[i]])
					dResult[pIndex[i]] = d3[pIndex[i]];
				else
					dResult[pIndex[i]] = d4[pIndex[i]];
			break;
		case IFLT:
			evaluate_array(d1);
			evaluate_array(d2);
			evaluate_array(d3);
			evaluate_array(d4);
			for(i=m_iOffset; i<m_iEnd; i++)
				if(d1[pIndex[i]] < d2[pIndex[i]])
					dResult[pIndex[i]] = d3[pIndex[i]];
				else
					dResult[pIndex[i]] = d4[pIndex[i]];
			break;

		case LATE:
			evaluate_array(d1);
			evaluate_array(d2);
			for(i=m_iOffset; i<m_iEnd; i++){
				//std::cout<<"Ja sam: "<<m_dTermValuesArray[T_ISLATE][i]<<std::endl;
				if(m_dTermValuesArray[T_ISLATE][i]!=0){
					dResult[pIndex[i]] = d1[pIndex[i]];
				}
				else{
					dResult[pIndex[i]] = d2[pIndex[i]];
				}
			}
			
		default:	// terminal
			memcpy(dResult, m_dTermValuesArray[m_pExpression[m_nTree][m_iPosition]], m_iArraySize*sizeof(double));
			break;
	}
}


double RPN::evaluate()
{
	double d1,d2;
	m_iPosition++; // pomicemo se na sljedeci simbol; pocetna vrijednost mora biti -1 !!!
	switch(m_pExpression[m_nTree][m_iPosition])
	{
		case ADD:
			return evaluate() + evaluate();
			break;
		case SUB:
			return evaluate() - evaluate();
			break;
		case MUL:
			return evaluate() * evaluate();
			break;
		case DIV:
			d1 = evaluate();
			d2 = evaluate();
			if(fabs(d2) < 0.000001)
				return 1;
			else
				return d1/d2;
			break;
		case SQR:
			d1 = evaluate();
			if(d1<0)
				return 1;
			return sqrt(d1);
		case POS:
			d1 = evaluate();
			if(d1 > 0)
				return d1;
			else
				return 0;
			break;
		case MAX:
			d1 = evaluate();
			d2 = evaluate();
			if(d1>d2)
				return d1;
			else 
				return d2;
			break;
		case MIN:
			d1 = evaluate();
			d2 = evaluate();
			if(d2>d1)
				return d1;
			else 
				return d2;
			break;
		case IFGT:
			d1 = evaluate();
			d2 = evaluate();
			if(d1>d2)
			{ d1 = evaluate();
			d2 = evaluate();
				return d1;
			}
			else
				
			{ d1 = evaluate();
			d2 = evaluate();
				return d2;
			}
			break;
		case IFLT:
			d1 = evaluate();
			d2 = evaluate();
			if(d1<d2)
			{ d1 = evaluate();
			d2 = evaluate();
				return d1;
			}
			else
				
			{ d1 = evaluate();
			d2 = evaluate();
				return d2;
			}
			break;
		case AVG:
			d1=evaluate();
			d2=evaluate();
			return (d1+d2)/2;
			break;
		default:	// terminal
			return m_pTermValues[m_pExpression[m_nTree][m_iPosition]];
			break;
	}
}

int RPN::edit()
{
	int r1,r2;
	uint iCurrent,iOp1,iOp2,i;
	m_iPosition++;	// pomicemo se na sljedeci simbol; pocetna vrijednost mora biti -1 !!!
	iCurrent = ++m_iEditedPos;	// usput pamtimo gdje smo u edited polju
	m_pEdited[m_iEditedPos] = m_pExpression[m_nTree][m_iPosition];	// prepisujemo tekuci simbol
	switch(m_pExpression[m_nTree][m_iPosition])
	{
		case ADD:
			r1 = edit(); iOp1 = m_iEditedPos;
			r2 = edit(); iOp2 = m_iEditedPos;
			if(r1==NUL)				// 0+A = A
			{	for(i=0; i<iOp2-iOp1; i++)
					m_pEdited[iCurrent+i] = m_pEdited[iOp1+1+i];
				m_iEditedPos = m_iEditedPos - (iOp1 - iCurrent) - 1;
				return r2;
			}
			else if(r2==NUL)		// A+0 = A
			{	for(i=0; i<iOp1-iCurrent; i++)
					m_pEdited[iCurrent+i] = m_pEdited[iCurrent+i+1];
				m_iEditedPos = m_iEditedPos - (iOp2 - iOp1) - 1;
				return r1;
			}
			else
				return ADD;
			break;
		case SUB:
			r1 = edit(); iOp1 = m_iEditedPos;
			r2 = edit(); iOp2 = m_iEditedPos;
#ifdef STARO
			if(r1<100 && r1==r2)	// ovo je bilo prije; krivo! 27.07.
#else
			if(r1<m_iTermNum && r1==r2)	// A-A = 0
#endif
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = NUL;
				return NUL;
			}
			else if(r2==NUL)		// A-0 = A
			{	for(i=0; i<iOp1-iCurrent; i++)
					m_pEdited[iCurrent+i] = m_pEdited[iCurrent+i+1];
				m_iEditedPos = m_iEditedPos - (iOp2 - iOp1) - 1;
				return r1;
			}
			// prosirenje za simbolicku jednakost A-A
			else if(r1==r2 && (iOp1-iCurrent)==(iOp2-iOp1))	
			{	for(i=0; i<iOp2-iOp1; i++)
					if(m_pEdited[iCurrent+1+i] != m_pEdited[iOp1+1+i])
						break;
				if(i != iOp2-iOp1)	// neuspjeh...
					return SUB;
				m_iEditedPos = iCurrent;	// uspjeh!!
				m_pEdited[m_iEditedPos] = NUL;
				return NUL;
			}
			else
				return SUB;
			break;
		case MUL:
			r1 = edit(); iOp1 = m_iEditedPos;
			r2 = edit(); iOp2 = m_iEditedPos;
			if(r1==ONE)				// 1*A = A
			{	for(i=0; i<iOp2-iOp1; i++)
					m_pEdited[iCurrent+i] = m_pEdited[iOp1+1+i];
				m_iEditedPos = m_iEditedPos - (iOp1 - iCurrent) - 1;
				return r2;
			}
			else if(r2==ONE)		// A*1 = A
			{	for(i=0; i<iOp1-iCurrent; i++)
					m_pEdited[iCurrent+i] = m_pEdited[iCurrent+i+1];
				m_iEditedPos = m_iEditedPos - (iOp2 - iOp1) - 1;
				return r1;
			}
#ifndef STARO
			// dodano 27.07.
			else if(r1==NUL || r2==NUL)	// A*0 = 0*A = 0
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = NUL;
				return NUL;
			}
#endif
			else
				return MUL;
			break;
		case DIV:
			r1 = edit(); iOp1 = m_iEditedPos;
			r2 = edit(); iOp2 = m_iEditedPos;
#ifdef STARO
			if(r1<100 && r1==r2)	// ovo je bilo prije; krivo! 27.07.
#else
			if(r1<m_iTermNum && r1==r2)	// A/A = 1
#endif
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = ONE;
				return ONE;
			}
			// prosirenje za simbolicku jednakost A/A
			else if(r1==r2 && (iOp1-iCurrent)==(iOp2-iOp1))	
			{	for(i=0; i<iOp2-iOp1; i++)
					if(m_pEdited[iCurrent+1+i] != m_pEdited[iOp1+1+i])
						break;
				if(i != iOp2-iOp1)	// neuspjeh...
					return DIV;
				m_iEditedPos = iCurrent;	// uspjeh!!
				m_pEdited[m_iEditedPos] = ONE;
				return ONE;
			}
			else if(r2 == NUL)	// A/0 = 1
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = ONE;
				return ONE;
			}
#ifndef STARO
			// dodano 27.07.
			else if(r2 == ONE)	// A/1 = A
			{	for(i=0; i<iOp1-iCurrent; i++)
					m_pEdited[iCurrent+i] = m_pEdited[iCurrent+i+1];
				m_iEditedPos = m_iEditedPos - (iOp2 - iOp1) - 1;
				return r1;
			}
			else if(r1 == NUL)	// 0/A = 0
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = NUL;
				return NUL;
			}
#endif
			else
				return DIV;
			break;
		case POS:
			r1 = edit();
			// POS(0) = 0, POS(1) = 1
			// a i svi ostali terminali (do T_age) su po definiciji pozitivne vrijednosti
			if(r1 <= TERMINALS + OFFSET)
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = r1;
				return r1;
			}
			else	// ako je ispod funkcija, nista
				return POS;
			break;
		case SQR:
			r1 = edit();
			// SQR(0,1) = 0,1
			if(r1 == NUL || r1 == ONE)
			{	m_iEditedPos = iCurrent;
				m_pEdited[m_iEditedPos] = r1;
				return r1;
			}
			else
				return SQR;
			break;
		case IFGT:
			r1 = edit();
			r1 = edit();
			r1 = edit();
			r1 = edit();
			return IFGT;
			break;
		case IFLT:
			r1 = edit();
			r1 = edit();
			r1 = edit();
			r1 = edit();
			return IFLT;
			break;
		default:	// terminal
			return m_pExpression[m_nTree][m_iPosition];
			break;
	}
}

// kopira editirani izraz u izvorni
// usput prebroji terminale!
void RPN::copy()
{
	// obrisi brojace od prosle jedinke
	for(int i = 0; i<TOTAL_NODES; i++)
		if(Nodes[i].active)
			m_pNodeFreq[i] = 0;
	// prebroji za ovoga i kumulativno
	for(int i = 0; i<m_iEditedPos+1; i++)
	{	Nodes[m_pEdited[i]].frequency++;
		m_pNodeFreq[m_pEdited[i]] += 1;
		m_pExpression[m_nTree][i] = m_pEdited[i];
	}
}

// broj terminala postavlja na nulu
void RPN::ResetNodeFreq()
{
	int i;
	for(i = 0; i<TERMINALS+OFFSET; i++)
		Nodes[i].frequency = 0;
	for(i = FUNC_START; i<FUNC_END; i++)
		Nodes[i].frequency = 0;
}

// ispis stabla u infix notaciji
void RPN::write()
{
	m_output = " ";
	m_iPosition = -1;
	r_write();
}

void RPN::r_write()
{
	m_iPosition++; // pomicemo se na sljedeci simbol; pocetna vrijednost mora biti -1 !!!
	switch(m_pExpression[m_nTree][m_iPosition])
	{
		case ADD:
			m_output += "(";
			r_write();
			m_output += "+";
			r_write();
			m_output += ")";
			break;
		case SUB:
			m_output += "(";
			r_write();
			m_output += "-";
			r_write();
			m_output += ")";
			break;
		case MUL:
			m_output += "(";
			r_write();
			m_output += "*";
			r_write();
			m_output += ")";
			break;
		case DIV:
			m_output += "(";
			r_write();
			m_output += "/";
			r_write();
			m_output += ")";
			break;
		case POS:
			m_output += "pos(";
			r_write();
			m_output += ")";
			break;
		case SQR:
			m_output += "sqr(";
			r_write();
			m_output += ")";
			break;
		default:	// terminal
			//itoa(m_pExpression[m_nTree][m_iPosition],pom,10);
			m_output += Nodes[m_pExpression[m_nTree][m_iPosition]].name;
			break;
	}
}