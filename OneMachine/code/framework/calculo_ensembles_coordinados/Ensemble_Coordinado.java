package framework.calculo_ensembles_coordinados;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Set;
import java.util.concurrent.ThreadLocalRandom;

public class Ensemble_Coordinado {

	private int[] reglas;
	private Instancia instancia;
	private int[] solucionesNuevoEnsemble;

	public Ensemble_Coordinado(int cardinalidadEnsembles, Instancia instancia) {
		this.reglas = new int[cardinalidadEnsembles];
		this.instancia = instancia;
		for (int i = 0; i < cardinalidadEnsembles; i++)
			reglas[i] = -1;
	}

	public Ensemble_Coordinado random() {
		for (int i = 0; i < reglas.length; i++)
			reglas[i] = instancia.getReglaRandom();
		return this;
	}

	public Ensemble_Coordinado(Ensemble_Coordinado ensemble) {
		this.instancia = ensemble.instancia;
		this.reglas = new int[ensemble.reglas.length];
		for (int i = 0; i < ensemble.reglas.length; i++)
			reglas[i] = ensemble.reglas[i];
	}

	public int size() {
		return reglas.length;
	}

	public int sizeReal() {
		Set<Integer> unicas = new HashSet<Integer>();
		for (int i = 0; i < reglas.length; i++)
			if (reglas[i] != -1)
				unicas.add(i);
		return unicas.size();
	}

	public void inserta(int index, int regla) {
		reglas[index] = regla;
		calculadaReglaMenosAporta = false;
		calculado = false;
		calculadoTest = false;
	}

	boolean calculado;
	double coste;

	public double train() {
		if (!calculado) {
			coste = 0;
			solucionesNuevoEnsemble = new int[instancia.getNinstanciasTraining()];
			for (int i = 0; i < instancia.getNinstanciasTraining(); i++) {
				int min = Integer.MAX_VALUE;
				for (int regla : reglas) {
					if (regla != -1) {
						if (instancia.training(regla, i) < min)
							min = instancia.training(regla, i);
					} else
						break;
				}
				coste += min;
				solucionesNuevoEnsemble[i] = min;
			}
			coste /= instancia.getNinstanciasTraining();
			calculado = true;
		}
		return coste;
	}

	boolean calculadoTest;
	double costeTest;

	public double test() {
		if (!calculadoTest) {
			costeTest = 0;
			for (int i = 0; i < instancia.getNinstanciasTest(); i++) {
				int min = Integer.MAX_VALUE;
				for (int regla : reglas)
					if (instancia.test(regla, i) < min)
						min = instancia.test(regla, i);
				costeTest += min;
			}
			costeTest /= instancia.getNinstanciasTest();
			calculadoTest = true;
		}
		return costeTest;
	}

	public Ensemble_Coordinado getClon() {
		return new Ensemble_Coordinado(this);
	}

	// Implementing Fisher–Yates shuffle
	public void mezclar() {
		Random rnd = ThreadLocalRandom.current();
		for (int i = reglas.length - 1; i > 0; i--) {
			int index = rnd.nextInt(i + 1);
			int a = reglas[index];
			reglas[index] = reglas[i];
			reglas[i] = a;
		}
	}

	public String getReglas() {
		String ensemble = "";
		for (Integer regla : reglas)
			ensemble += regla + " ";
		return ensemble;
	}

	public int[] getEnsemble() {
		return reglas;
	}

	public int get(int index) {
		return reglas[index];
	}

	boolean calculadaReglaMenosAporta;
	int indice_peorRegla;

	public int indiceReglaMenosAporta() {
		if (!calculadaReglaMenosAporta) {
			// 1- inicializar las estructuras
			List<List<Integer>> reglas_instancias = new ArrayList<List<Integer>>();
			// se inicializa una lista que contiene una lista por cada una de las instancias
			for (int i = 0; i < instancia.getNinstanciasTraining(); i++)
				reglas_instancias.add(new ArrayList<Integer>());
			// 2- calculo de los INDICES de las reglas mejores para cada instancia y las
			// añado en su respectiva lista
			for (int i = 0; i < instancia.getNinstanciasTraining(); i++) {
				int min = Integer.MAX_VALUE;
				int costeRegla;
				for (int indice_regla = 0; indice_regla < reglas.length; indice_regla++) {
					costeRegla = instancia.training(reglas[indice_regla], i);
					if (costeRegla < min) {
						min = costeRegla;
						reglas_instancias.get(i).clear();
						reglas_instancias.get(i).add(indice_regla);
					} else if (costeRegla == min)
						reglas_instancias.get(i).add(indice_regla);
				}
			}
			// 3- Se calculan las veces que cada regla es la mejor para una instancia
			int[] vecesMejorRegla = new int[reglas.length];
			for (List<Integer> lista_indices_reglas : reglas_instancias)
				for (Integer indice_regla : lista_indices_reglas)
					vecesMejorRegla[indice_regla] = vecesMejorRegla[indice_regla] + 1;

			// 4- Se calcula la regla que menos veces es la mejor en las instancias, la peor
			// regla
			int min = Integer.MAX_VALUE;
			indice_peorRegla = -1;
			for (int indice_regla = 0; indice_regla < vecesMejorRegla.length; indice_regla++) {
				if (vecesMejorRegla[indice_regla] < min) {
					min = vecesMejorRegla[indice_regla];
					indice_peorRegla = indice_regla;
				}
			}
			calculadaReglaMenosAporta = true;
		}
		return indice_peorRegla;
	}

	public int getCosteMin(int inst) {
		int min = Integer.MAX_VALUE;
		for (Integer regla : reglas)
			if (instancia.test(regla, inst) < min)
				min = instancia.test(regla, inst);
		return min;
	}

	public int[] getSolucionesMejores() {
		train();
		return solucionesNuevoEnsemble;
	}

	public void mutar() {
		mezclar();
		int n = (int) (Math.random() * size() / 2);
		for (int i = 0; i < n; i++)
			inserta(i, instancia.getReglaRandom());
	}
}
