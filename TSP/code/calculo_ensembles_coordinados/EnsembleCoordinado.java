package calculo_ensembles_coordinados;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;
import domain.Graph;

public class EnsembleCoordinado {

//	boolean calculado;
	boolean calculadoTest;
	double coste, costeTest;
	List<Regla> reglas;

	public EnsembleCoordinado() {
		reglas = new ArrayList<Regla>(AlgoUtil.cardinalidadEnsembles);
	}

	public EnsembleCoordinado(List<Regla> reglas) {
		this.reglas = new ArrayList<Regla>(reglas.size());
		for (Regla regla : reglas)
			this.reglas.add(regla);
	}

	public EnsembleCoordinado(Regla[] reglas) {
		int sizeReal = 0;
		for (Regla regla : reglas)
			if (regla == null)
				break;
			else
				sizeReal++;
		this.reglas = new ArrayList<Regla>(sizeReal);
		for (Regla regla : reglas)
			if (regla == null)
				break;
			else
				this.reglas.add(regla);
	}

	public EnsembleCoordinado(EnsembleCoordinado ensemble) {
		reglas = new ArrayList<Regla>(ensemble.reglas.size());
		for (int i = 0; i < ensemble.reglas.size(); i++)
			reglas.add(ensemble.reglas.get(i));
	}

	public EnsembleCoordinado random() {
		for (int i = 0; i < AlgoUtil.cardinalidadEnsembles; i++)
			reglas.add(AlgoUtil.getReglaRandom());
		return this;
	}

	public void inserta(int i, Regla regla) {
		reglas.set(i, regla);
	}

	public double getCoste() {
		if (GA.cache.containsKey(this))
			return GA.cache.get(this).doubleValue();
		coste = 0;
		double value, min;
		for (Graph instancia : AlgoUtil.set_train) {
			min = Integer.MAX_VALUE;
			for (Regla regla : reglas) {
				value = regla.getCoste(instancia);
				if (value < min)
					min = regla.getCoste(instancia);
			}
//			min = min / instancia.getTotalCost() * Math.pow(instancia.getSize(), 2);  // normalize? 
			coste += min;
		}
		coste /= AlgoUtil.set_train.size();
		GA.cache.put(this, coste);
		return coste;
	}

	public double getCoste(Graph instancia) {
		double value, min;
		min = Integer.MAX_VALUE;
		for (Regla regla : reglas) {
			value = regla.getCoste(instancia);
			if (value < min)
				min = regla.getCoste(instancia);
		}
		return min;
	}
	
	public int dominanciaTest;

	public double getCosteTest() {
//		if (!calculadoTest) {
			dominanciaTest = 0;
			costeTest = 0;
			for (Graph instancia : AlgoUtil.set_test) {
				double min = Integer.MAX_VALUE;
				for (Regla regla : reglas)
					if (regla.getCoste(instancia) < min)
						min = regla.getCoste(instancia);
				costeTest += min;
				if (AlgoUtil.getPromedioPaperReferencia(instancia) > min) {
					dominanciaTest++;
				}
			}
			costeTest /= AlgoUtil.set_test.size();
//			calculadoTest = true;
//		}
		return costeTest;
	}
	
	public EnsembleCoordinado getClon() {
		return new EnsembleCoordinado(this);
	}

	public void mezclar() {
		Collections.shuffle(reglas);
	}

	public String representa() {
		String ensemble = "[";
		for (Regla regla : reglas)
			ensemble += regla.representa() + ",";
		return ensemble + "]";
	}

	public String representaMonticulo() {
		String ensemble = "[";
		for (Regla regla : reglas)
			ensemble += regla.representaMonticulo() + ",";
		return ensemble + "]";
	}

	public void cargaReglas(String string) {
		String[] r = string.replace("[", "").replace("]", "").split(",");
		reglas = new ArrayList<Regla>();
		for (int i = 0; i<r.length; i++)
			reglas.add(new Regla().generarRegla(r[i]));
	}
	
	public Regla get(int regla) {
		return reglas.get(regla);
	}

	public int getSize() {
		int size = 0;
		for (Regla regla : reglas)
			size += regla.getSize();
		return size;
	}
	
	public int getDepth() {
		int size = 0;
		for (Regla regla : reglas)
			size += regla.getProfundidad();
		return size;
	}

	boolean calculadaReglaMenosAporta;
	int indice_peorRegla;

	public int indiceReglaMenosAporta() {
		if (!calculadaReglaMenosAporta) {

			// 1- inicializar las estructuras
			List<List<Integer>> reglas_instancias = new ArrayList<List<Integer>>();
			// se inicializa una lista que contiene una lista por cada una de las instancias
			for (int i = 0; i < AlgoUtil.set_train.size(); i++)
				reglas_instancias.add(new ArrayList<Integer>());
			// 2- calculo de los INDICES de las reglas mejores para cada instancia y las
			// añado en su respectiva lista
			for (int instancia = 0; instancia <AlgoUtil.set_train.size(); instancia++) {
				double min = Integer.MAX_VALUE;
				double costeRegla;
				for (int indice_regla = 0; indice_regla < AlgoUtil.cardinalidadEnsembles; indice_regla++) {
					costeRegla = get(indice_regla).getCoste(AlgoUtil.set_train.get(instancia));
					if (costeRegla < min) {
						min = costeRegla;
						reglas_instancias.get(instancia).clear();
						reglas_instancias.get(instancia).add(indice_regla);
					} else if (costeRegla == min)
						reglas_instancias.get(instancia).add(indice_regla);
				}
			}

			// 3- Se calculan las veces que cada regla es la mejor para una instancia
			int[] vecesMejorRegla = new int[reglas.size()];
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

	public List<Regla> getReglas() {
		return reglas;
	}

	@Override
	public String toString() {
		return representa();
	}
	
	@Override
	public boolean equals(Object obj) {
		return toString().equals(obj.toString());
	}
	
}
