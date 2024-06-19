package framework.calculo_ensembles_colaborativos;

import java.util.List;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;
import framework.core.Poblacion;

public class Poblacion_Colaborativos extends Poblacion<Ensemble_Colaborativo> {

	public Poblacion_Colaborativos(int size) {
		super(size);
	}

	public void generaRandom(int cardinalidadEnsembles, List<Regla> seleccionables) {
		for (int i = 0; i < sizeMax; i++)
			poblacion.add(new Ensemble_Colaborativo(cardinalidadEnsembles).random(seleccionables));
		shuffle();
	}

	@Override
	public Ensemble_Colaborativo getBest() {
		if (mejor == null) {
			mejor = get(0);
			for (Ensemble_Colaborativo ensemble : poblacion)
				if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(ensemble, mejor))
					mejor = ensemble;
		}
		return mejor;
	}

	@Override
	public double getAvg() {
		double costeMedio = 0;
		for (Regla ensemble : poblacion)
			costeMedio += AlgoUtil.evaluadorEnsemblesColaborativos.getFullEvaluate(ensemble).train();
		return costeMedio / size();
	}

}
