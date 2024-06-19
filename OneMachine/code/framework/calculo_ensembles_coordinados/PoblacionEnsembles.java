package framework.calculo_ensembles_coordinados;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class PoblacionEnsembles {

	private Ensemble_Coordinado[] poblacion;
	private Ensemble_Coordinado mejor;

	public PoblacionEnsembles(int tamañoPoblacion) {
		this.poblacion = new Ensemble_Coordinado[tamañoPoblacion];
	}

	public void inicializa(Instancia instancia, int cardinalidad) {
		for (int i = 0; i < poblacion.length; i++) {
			poblacion[i] = new Ensemble_Coordinado(cardinalidad, instancia).random();
		}
	}

	public Ensemble_Coordinado get(int index) {
		return poblacion[index];
	}

	public void save(int index, Ensemble_Coordinado ensemble) {
		poblacion[index] = ensemble;
		mejor = null;
	}

	public int size() {
		return poblacion.length;
	}

	public void mezclarPoblacion() {
		List<Ensemble_Coordinado> pop = Arrays.asList(poblacion);
		Collections.shuffle(pop);
		poblacion = (Ensemble_Coordinado[]) pop.toArray();
	}

	public void remove(Ensemble_Coordinado ensemble) {
		for (int i = 0; i < size(); i++)
			if (poblacion[i].equals(ensemble)) {
				poblacion[i] = null;
				break;
			}
		Ensemble_Coordinado[] aux = new Ensemble_Coordinado[size() - 1];
		int j = 0;
		for (int i = 0; i < size(); i++)
			if (poblacion[i] != null) {
				aux[j] = poblacion[i];
				j++;
			}
		poblacion = aux;
		mejor = null;
	}

	public Ensemble_Coordinado getFittest() {
		if (mejor == null) {
			mejor = get(0);
			for (Ensemble_Coordinado ensemble : poblacion)
				if (ensemble.train() < mejor.train())
					mejor = ensemble;
		}
		return mejor;
	}

	public double getCosteMedio() {
		double costeMedio = 0;
		for (Ensemble_Coordinado ensemble : poblacion)
			costeMedio += ensemble.train();
		return costeMedio / size();
	}

}
