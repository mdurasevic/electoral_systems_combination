package framework.calculo_ensembles_coordinados;

public class Voraz extends Algoritmo {

	public Voraz(Instancia instancia, int cardinalidadEnsembles) {
		this.instancia = instancia;
		this.cardinalidadEnsembles = cardinalidadEnsembles;
	}

	@Override
	public Ensemble_Coordinado run() {
		inicio();
		double costeMejor = Double.MAX_VALUE;
		Ensemble_Coordinado aux;
		boolean mejora;
		double coste;
		int best = 0;

		Ensemble_Coordinado ensemble = new Ensemble_Coordinado(cardinalidadEnsembles, instancia);

		for (int index = 0; index < cardinalidadEnsembles; index++) {
			mejora = false;
			for (int regla = 0; regla < instancia.getNreglas(); regla++) {
				aux = new Ensemble_Coordinado(ensemble);
				aux.inserta(index, regla);
				coste = aux.train();
				if (coste < costeMejor) {
					mejora = true;
					costeMejor = coste;
					best = regla;
				}
			}
			if (!mejora)
				break;
			else
				ensemble.inserta(index, best);
		}
		fin();
		return ensemble;
	}
}