package calculo_ensembles_colaborativos;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class Poblacion_Colaborativos {

	protected int sizeMax;
	public List<Ensemble_Colaborativo> poblacion;
	protected Ensemble_Colaborativo mejor;

	public Poblacion_Colaborativos(int size) {
		poblacion = new ArrayList<Ensemble_Colaborativo>(size);
		sizeMax = size;
	}

	public Ensemble_Colaborativo get(int index) {
		return poblacion.get(index);
	}

	public void add(Ensemble_Colaborativo cromosoma) {
		poblacion.add(cromosoma);
		mejor = null;
	}

	public void set(int i, Ensemble_Colaborativo cromosoma) {
		poblacion.set(i, cromosoma);
		mejor = null;
	}

	public int size() {
		return poblacion.size();
	}

	public void remove(Ensemble_Colaborativo cromosoma) {
		poblacion.remove(cromosoma);
		mejor = null;
	}

	public void shuffle() {
		Collections.shuffle(poblacion);
	}

	public Ensemble_Colaborativo getRandom() {
		return poblacion.get((int) (poblacion.size() * Math.random()));
	}

	public void generaRandom(int cardinalidadEnsembles, List<Regla> seleccionables) {
		for (int i = 0; i < sizeMax; i++)
			poblacion.add(new Ensemble_Colaborativo(cardinalidadEnsembles).random(seleccionables));
		shuffle();
	}

	public Ensemble_Colaborativo getBest() {
		if (mejor == null) {
			mejor = get(0);
			for (Ensemble_Colaborativo ensemble : poblacion)
				if (AlgoUtil.mejoraTraining(ensemble, mejor))
					mejor = ensemble;
		}
		return mejor;
	}
	
	public Ensemble_Colaborativo mejorParaValidacion() {
		return mejorParaValidacion(poblacion.size());
	}
	
	public Ensemble_Colaborativo mejorParaValidacion(int n) {
		mejor = get(0);
		for (int i=0; i<n; i++) {
			poblacion.get(i).evalTrain = false;
			if (AlgoUtil.mejoraTraining(poblacion.get(i), mejor))
				mejor = poblacion.get(i);
		}
		return mejor;
	}
	
	
	public double getAvgTrain() {
		double costeMedio = 0;
		for (Ensemble_Colaborativo ensemble : poblacion)
			costeMedio += ensemble.train();
		return costeMedio / size();
	}
	
	public double getAvgNumberOfSymbols() {
		double costeMedio = 0;
		for (Ensemble_Colaborativo ensemble : poblacion)
			costeMedio += ensemble.getSize();
		return costeMedio / size();
	}

}
