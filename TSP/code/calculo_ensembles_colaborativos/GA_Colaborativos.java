package calculo_ensembles_colaborativos;

import calculo_reglas.AlgoUtil;

public class GA_Colaborativos {

	// VARIABLES INTERNAS
	public Poblacion_Colaborativos pop;
	public int generaciones = 0;
	// PARAMETROS
	protected double ratioCruce = 0.8;
	protected double ratioMutacion = 0.2;
	public int sizePop = 100;
	protected double ratioLSA = 0;

	public GA_Colaborativos() {
		pop = new Poblacion_Colaborativos(sizePop);
	}
	
	public GA_Colaborativos(int sizePop) {
		this.sizePop = sizePop;
		pop = new Poblacion_Colaborativos(sizePop);
	}

	public void generarRandomPop() {
		for (int i = 0; i < sizePop; i++)
			pop.add(new Ensemble_Colaborativo(AlgoUtil.cardinalidadEnsembles).random());
	}

	Ensemble_Colaborativo evol(int generations) {
		Ensemble_Colaborativo mejor = null;
		for (int i = 0; i < generations; i++) {
			evolucionar();
			mejor = pop.getBest();
		}
		return mejor;
	}

	Ensemble_Colaborativo evol() {
		Ensemble_Colaborativo mejor = null;
		while (!AlgoUtil.timeOut()) {
			evolucionar();
			mejor = pop.getBest();
		}
		return mejor;
	}

	public void evolucionar() {
		generaciones++;
		pop.shuffle();
		for (int i = 0; i < sizePop && !AlgoUtil.timeOut(); i = i + 2) {
//		for (int i = 0; i < sizePop; i = i + 2) {
			Ensemble_Colaborativo padre1 = pop.get(i);
			Ensemble_Colaborativo padre2 = pop.get(i + 1);
			if (Math.random() <= ratioCruce) {

				// cruce
				Ensemble_Colaborativo hijo1 = cruzar(padre1, padre2);
				Ensemble_Colaborativo hijo2 = cruzar(padre2, padre1);

				// mutacion
				if (Math.random() <= ratioMutacion)
					mutar(hijo1);
				if (Math.random() <= ratioMutacion)
					mutar(hijo2);

				// selección
				if (AlgoUtil.mejoraTraining(hijo1, hijo2)) {
					Poblacion_Colaborativos torneo = new Poblacion_Colaborativos(3);
					torneo.add(padre1);
					torneo.add(padre2);
					torneo.add(hijo2);
					pop.set(i, hijo1);
					pop.set(i + 1, (Ensemble_Colaborativo) torneo.getBest());
				} else {
					Poblacion_Colaborativos torneo = new Poblacion_Colaborativos(3);
					torneo.add(padre1);
					torneo.add(padre2);
					torneo.add(hijo1);
					pop.set(i, hijo2);
					pop.set(i + 1, (Ensemble_Colaborativo) torneo.getBest());
				}
			} else {
				Ensemble_Colaborativo hijo1 = padre1.getClon(), hijo2 = padre2.getClon();
				Ensemble_Colaborativo mutacion1 = null, mutacion2 = null;
				// mutacion
				if (Math.random() <= ratioMutacion) {
					mutacion1 = padre1.getClon();
					mutar(mutacion1);
				}
				if (Math.random() <= ratioMutacion) {
					mutacion2 = padre2.getClon();
					mutar(mutacion2);
				}

				if (mutacion1 == null && mutacion2 == null) {
					pop.set(i, hijo1);
					pop.set(i + 1, hijo2);
				} else {
					Poblacion_Colaborativos torneo;
					if (mutacion1 != null && mutacion2 == null) {
						torneo = new Poblacion_Colaborativos(3);
						torneo.add(padre1);
						torneo.add(padre2);
						torneo.add(mutacion1);
					} else if (mutacion1 == null && mutacion2 != null) {
						torneo = new Poblacion_Colaborativos(3);
						torneo.add(padre1);
						torneo.add(padre2);
						torneo.add(mutacion2);
					} else {
						torneo = new Poblacion_Colaborativos(4);
						torneo.add(padre1);
						torneo.add(padre2);
						torneo.add(mutacion1);
						torneo.add(mutacion2);
					}
					Ensemble_Colaborativo first = torneo.getBest();
					pop.set(i, first);
					torneo.remove(first);
					Ensemble_Colaborativo second = torneo.getBest();
					pop.set(i + 1, second);
				}
			}
		}
	}

	public Ensemble_Colaborativo getMejor() {
		return pop.getBest();
	}

	void mutar(Ensemble_Colaborativo ensemble) {
		ensemble.mezclar();
		int n = (int) (Math.random() * ensemble.reglas.length / 2);
		for (int i = 0; i < n; i++)
			ensemble.set(i, AlgoUtil.getReglaRandom());
	}

	Ensemble_Colaborativo cruzar(Ensemble_Colaborativo padre1, Ensemble_Colaborativo padre2) {
		padre1.mezclar(); // Ramiro
		padre2.mezclar();
		int n = (int) (Math.random() * padre1.reglas.length);
		Ensemble_Colaborativo hijo = padre2.getClon();
		for (int i = 0; i < n; i++)
			hijo.set(i, padre1.reglas[i]);
		return hijo;
	}
}