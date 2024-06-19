package framework.calculo_ensembles_colaborativos;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;
import framework.core.AlgorithmAbstract;

public class Voraz_Colaborativos extends AlgorithmAbstract<Ensemble_Colaborativo> {

	public Voraz_Colaborativos() {
	}

	public Voraz_Colaborativos(String mensaje) {
		super(mensaje);
	}

	@Override
	public void run() {
		Ensemble_Colaborativo aux;
		boolean mejora;
		Regla best = null;

		Ensemble_Colaborativo ensemble = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos);

		for (int index = 0; index < AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos && !timeOut(); index++) {
			mejora = false;
			for (int i = 0; i < AlgoUtil.evaluadorReglas.getSeleccionables().size() && !timeOut(); i++) {
				aux = new Ensemble_Colaborativo(ensemble);
				aux.inserta(index, AlgoUtil.evaluadorReglas.getSeleccionables().get(i));
				if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(aux, ensemble)) {
					mejora = true;
					best = AlgoUtil.evaluadorReglas.getSeleccionables().get(i);
				}
			}
			if (!mejora)
				break;
			else
				ensemble.inserta(index, best);
		}
		mejor = ensemble;
	}

}
