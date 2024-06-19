package calculo_ensembles_coordinados;

public class MA extends GA{

	public static double ratioLSA = 0.2;
	public static int limiteIteracionesLSA = 5;
	public static int limiteVecinosLSA = 100;

	public static void evolucionar() {
		PoblacionEnsembles newPopulation = new PoblacionEnsembles(poblacion.size());
		poblacion.mezclarPoblacion();
		for (int i = 0; i < popSize; i = i + 2) {
			EnsembleCoordinado padre1 = poblacion.get(i);
			EnsembleCoordinado padre2 = poblacion.get(i + 1);
			if (Math.random() <= ratioCruce) {

				// cruce
				EnsembleCoordinado hijo1 = cruzar(padre1, padre2);
				EnsembleCoordinado hijo2 = cruzar(padre2, padre1);

				// mutacion
				if (Math.random() <= ratioMutacion)
					mutar(hijo1);
				if (Math.random() <= ratioMutacion)
					mutar(hijo2);

				// LSA
				if (Math.random() <= ratioLSA)
					lsa(hijo1);
				if (Math.random() <= ratioLSA)
					lsa(hijo2);

				// selección
				if (hijo1.getCoste() < hijo2.getCoste()) {
					PoblacionEnsembles torneo = new PoblacionEnsembles(3);
					torneo.save(0, padre1);
					torneo.save(1, padre2);
					torneo.save(2, hijo2);
					newPopulation.save(i, hijo1);
					newPopulation.save(i + 1, torneo.getMejor());
				} else {
					PoblacionEnsembles torneo = new PoblacionEnsembles(3);
					torneo.save(0, padre1);
					torneo.save(1, padre2);
					torneo.save(2, hijo1);
					newPopulation.save(i, hijo2);
					newPopulation.save(i + 1, torneo.getMejor());
				}
			} else {
				EnsembleCoordinado hijo1 = padre1.getClon(), hijo2 = padre2.getClon();
				EnsembleCoordinado mutacion1 = null, mutacion2 = null;
				
				// mutacion
				if (Math.random() <= ratioMutacion) {
					mutacion1 = padre1.getClon();
					mutar(mutacion1);
				}
				if (Math.random() <= ratioMutacion) {
					mutacion2 = padre2.getClon();
					mutar(mutacion2);
				}
				
				// LSA
				if (Math.random() <= ratioLSA) {
					if (mutacion1 != null)
						lsa(mutacion1);
					else
						lsa(padre1);
				}
				if (Math.random() <= ratioLSA) {
					if (mutacion2 != null)
						lsa(mutacion2);
					else
						lsa(padre2);
				}
				
				
				if (mutacion1 == null && mutacion2 == null) {
					newPopulation.save(i, hijo1);
					newPopulation.save(i + 1, hijo2);
				} else {
					PoblacionEnsembles torneo;
					if (mutacion1 != null && mutacion2 == null) {
						torneo = new PoblacionEnsembles(3);
						torneo.save(0, padre1);
						torneo.save(1, padre2);
						torneo.save(2, mutacion1);
					} else if (mutacion1 == null && mutacion2 != null) {
						torneo = new PoblacionEnsembles(3);
						torneo.save(0, padre1);
						torneo.save(1, padre2);
						torneo.save(2, mutacion2);
					} else {
						torneo = new PoblacionEnsembles(4);
						torneo.save(0, padre1);
						torneo.save(1, padre2);
						torneo.save(2, mutacion1);
						torneo.save(3, mutacion2);
					}
					EnsembleCoordinado first = torneo.getMejor();
					newPopulation.save(i, first);
					torneo.remove(first);
					EnsembleCoordinado second = torneo.getMejor();
					newPopulation.save(i + 1, second);
				}
			}
		}
		poblacion = newPopulation;
		EnsembleCoordinado mejorPoblacion = poblacion.getMejor();
		if (mejorPoblacion.getCoste() < mejor.getCoste())
			mejor = mejorPoblacion;
	}

	static void lsa(EnsembleCoordinado ensemble) {
//		LocalSearch.busquedaLocalIterativaEscaladaSimple(ensemble, limiteIteracionesLSA, limiteVecinosLSA);
		SimpleLocalSearch.busquedaLocalIterativaMaximoGradiente(ensemble, limiteIteracionesLSA, limiteVecinosLSA);
	}
}