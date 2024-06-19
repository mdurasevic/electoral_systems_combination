package framework.core;

import java.util.Map;

import dominio.Instancia;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_0;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_1;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_2;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_25;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_3;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_4;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_5;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_6;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_7;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_75;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_8;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante0_9;
import framework.calculo_reglas.representacion.nodos.constantes.NodoConstante1_0;

public class GestorParametrosReglasEnsembles {

	public Nodo[] constantes;
	public double pasoConstantes = 0.1;

	// PARAMETROS
	
	// reglas
	public int profundidadMaximaGlobal = 8;
	public int profundidadMaximaCompletasReglas = 5;
	public int profundidadMaximaInicialReglas = 4;
	public int indiceNullHoja = 95;
	public int nConstantes = 9;
	public boolean metodoKoza = true;
	
	// para la SSHE
	public boolean podaConsistencia = true;
	public int tamañoMaximoReglas;
	
	// ensembles
	public int cardinalidadEnsemblesColaborativos = 10;
	public int cardinalidadEnsemblesCoordinados = 10;
	public int combinationMethod = 0;
	
	
		
	public GestorParametrosReglasEnsembles() {
		asignaConstantes();
	}
		
	public void asignaConstantes() {
		switch (nConstantes) {
		case 11:
			constantes = new Nodo[] { NodoConstante0_0.getInstance(), NodoConstante0_1.getInstance(),
					NodoConstante0_2.getInstance(), NodoConstante0_3.getInstance(), NodoConstante0_4.getInstance(),
					NodoConstante0_5.getInstance(), NodoConstante0_6.getInstance(), NodoConstante0_7.getInstance(),
					NodoConstante0_8.getInstance(), NodoConstante0_9.getInstance(), NodoConstante1_0.getInstance() };
			pasoConstantes = 0.1;
			break;
		case 10:
			constantes = new Nodo[] { NodoConstante0_1.getInstance(), NodoConstante0_2.getInstance(),
					NodoConstante0_3.getInstance(), NodoConstante0_4.getInstance(), NodoConstante0_5.getInstance(),
					NodoConstante0_6.getInstance(), NodoConstante0_7.getInstance(), NodoConstante0_8.getInstance(),
					NodoConstante0_9.getInstance(), NodoConstante1_0.getInstance() };
			pasoConstantes = 0.1;
			break;
		case 9:
			constantes = new Nodo[] { NodoConstante0_1.getInstance(), NodoConstante0_2.getInstance(),
					NodoConstante0_3.getInstance(), NodoConstante0_4.getInstance(), NodoConstante0_5.getInstance(),
					NodoConstante0_6.getInstance(), NodoConstante0_7.getInstance(), NodoConstante0_8.getInstance(),
					NodoConstante0_9.getInstance() };
			pasoConstantes = 0.1;
			break;
		case 4:
			constantes = new Nodo[] { NodoConstante0_2.getInstance(), NodoConstante0_4.getInstance(),
					NodoConstante0_6.getInstance(), NodoConstante0_8.getInstance() };
			pasoConstantes = 0.2;
			break;
		case 3:
			constantes = new Nodo[] { NodoConstante0_25.getInstance(), NodoConstante0_5.getInstance(),
					NodoConstante0_75.getInstance() };
			pasoConstantes = 0.25;
			break;
		case 1:
			constantes = new Nodo[] { NodoConstante0_5.getInstance() };
			pasoConstantes = 0.0;
			break;
		default:
			constantes = new Nodo[] {};
			pasoConstantes = 0.0;
			break;
		}
	}

	public Nodo getConstanteRandom() {
		return constantes[(int) (Math.random() * constantes.length)];
	}

	public Nodo getConstante(double value) {
		value = Math.round(value * 100d) / 100d;
		if (value == 0.00)
			return NodoConstante0_0.getInstance();
		if (value == 0.10)
			return NodoConstante0_1.getInstance();
		if (value == 0.20)
			return NodoConstante0_2.getInstance();
		if (value == 0.25)
			return NodoConstante0_25.getInstance();
		if (value == 0.30)
			return NodoConstante0_3.getInstance();
		if (value == 0.40)
			return NodoConstante0_4.getInstance();
		if (value == 0.50)
			return NodoConstante0_5.getInstance();
		if (value == 0.6)
			return NodoConstante0_6.getInstance();
		if (value == 0.70)
			return NodoConstante0_7.getInstance();
		if (value == 0.75)
			return NodoConstante0_75.getInstance();
		if (value == 0.80)
			return NodoConstante0_8.getInstance();
		if (value == 0.90)
			return NodoConstante0_9.getInstance();
		if (value == 1.00)
			return NodoConstante1_0.getInstance();
		return null;
	}
}
