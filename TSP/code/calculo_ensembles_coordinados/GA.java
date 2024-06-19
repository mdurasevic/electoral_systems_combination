package calculo_ensembles_coordinados;

import java.util.HashMap;
import java.util.Map;

import calculo_reglas.AlgoUtil;

public class GA {

	public static double ratioCruce = 0.8;
	public static double ratioMutacion = 0.2;
	public static int popSize = 100;
	public static int generaciones = 50;

	public static PoblacionEnsembles poblacion;
	public static EnsembleCoordinado mejor;
	
	public static Map<EnsembleCoordinado, Double> cache;

	public static void generaPoblacionInicial() {
		cache = new HashMap<EnsembleCoordinado, Double>();
		poblacion = new PoblacionEnsembles();
		poblacion.generaAleatoria();
		mejor = poblacion.getMejor();
	}

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
				if (Math.random() <= ratioMutacion) {
					mutacion1 = padre1.getClon();
					mutar(mutacion1);
				}
				if (Math.random() <= ratioMutacion) {
					mutacion2 = padre2.getClon();
					mutar(mutacion2);
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

	static EnsembleCoordinado torneo(int nTorneo) {
		PoblacionEnsembles pop = new PoblacionEnsembles(nTorneo);
		for (int i = 0; i < nTorneo; i++)
			pop.save(i, poblacion.get((int) (Math.random() * poblacion.size())));
		return pop.getMejor();
	}

	static void mutar(EnsembleCoordinado ensemble) {
		ensemble.mezclar();
		int n = (int) (Math.random() * ensemble.reglas.size() / 2);
		for (int i = 0; i < n; i++)
			ensemble.inserta(i, AlgoUtil.getReglaRandom());
	}

	static EnsembleCoordinado cruzar(EnsembleCoordinado padre1, EnsembleCoordinado padre2) {
		padre1.mezclar();
		padre2.mezclar();
		int n = (int) (Math.random() * padre1.reglas.size());
		EnsembleCoordinado hijo = padre2.getClon();
		for (int i = 0; i < n; i++)
			hijo.inserta(i, padre1.reglas.get(i));
		return hijo;
	}
}