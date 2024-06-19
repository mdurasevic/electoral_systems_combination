package framework.calculo_ensembles_colaborativos;

import framework.core.AlgoUtil;
import framework.core.AlgorithmAbstract;

public class RandomSearch extends AlgorithmAbstract<Ensemble_Colaborativo> {

	protected Ensemble_Colaborativo solution;

	public RandomSearch() {
	}

	public RandomSearch(String mensaje) {
		super(mensaje);
	}

	@Override
	public void run() {
		solution = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos)
				.random(AlgoUtil.evaluadorReglas.getSeleccionables());
		evaluations++;
		if (mejor == null || AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(solution, mejor))
			mejor = solution;
	}

}