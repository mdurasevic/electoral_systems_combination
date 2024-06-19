package calculo_ensembles_coordinados;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class PoblacionEnsembles {

	public EnsembleCoordinado[] poblacion;
	private EnsembleCoordinado mejor;

	public PoblacionEnsembles() {
		this.poblacion = new EnsembleCoordinado[GA.popSize];
	}
	
	public void generaAleatoria() {
		for (int i = 0; i < poblacion.length; i++)
			poblacion[i] = new EnsembleCoordinado().random();
	}

	public PoblacionEnsembles(int size) {
		this.poblacion = new EnsembleCoordinado[size];
	}

	public EnsembleCoordinado get(int index) {
		return poblacion[index];
	}

	public void save(int index, EnsembleCoordinado ensemble) {
		poblacion[index] = ensemble;
		mejor = null;
	}

	public int size() {
		return poblacion.length;
	}

	public void mezclarPoblacion() {
		List<EnsembleCoordinado> pop = Arrays.asList(poblacion);
		Collections.shuffle(pop);
		poblacion = (EnsembleCoordinado[]) pop.toArray();
	}

	public void remove(EnsembleCoordinado ensemble) {
		for (int i = 0; i < size(); i++)
			if (poblacion[i].equals(ensemble)) {
				poblacion[i] = null;
				break;
			}
		EnsembleCoordinado[] aux = new EnsembleCoordinado[size() - 1];
		int j = 0;
		for (int i = 0; i < size(); i++)
			if (poblacion[i] != null) {
				aux[j] = poblacion[i];
				j++;
			}
		poblacion = aux;
		mejor = null;
	}

	public double getCosteMedio() {
		double costeMedio = 0;
		for (EnsembleCoordinado ensemble : poblacion)
			costeMedio += ensemble.getCoste();
		return costeMedio / size();
	}

	public EnsembleCoordinado getMejor() {
		if (mejor == null) {
			mejor = get(0);
			for (EnsembleCoordinado ensemble : poblacion)
				if (ensemble.getCoste() < mejor.getCoste())
					mejor = ensemble;
		}
		return mejor;
	}

}
