package framework.calculo_ensembles_coordinados;

public class AlgoritmoGenetico extends Algoritmo {

	protected int generaciones;
	protected int sizePoblacion;
	protected double ratioCruce;
	protected double ratioMutacion;

	protected PoblacionEnsembles ensembles;

	public AlgoritmoGenetico() {
	}

	public AlgoritmoGenetico(Instancia instancia, int generaciones, int sizePoblacion, double ratioCruce,
			double ratioMutacion, int cardinalidadEnsembles) {
		this.instancia = instancia;
		this.generaciones = generaciones;
		this.sizePoblacion = sizePoblacion;
		this.ratioCruce = ratioCruce;
		this.ratioMutacion = ratioMutacion;
		this.cardinalidadEnsembles = cardinalidadEnsembles;
	}

	@Override
	public Ensemble_Coordinado run() {
		inicio();
		ensembles = new PoblacionEnsembles(sizePoblacion);
		ensembles.inicializa(instancia, cardinalidadEnsembles);
		if (verbose)
			System.out.println(0 + ";" + (System.currentTimeMillis() - inicio) + ";" + ensembles.getFittest().train() + ";" + ensembles.getCosteMedio() + ";" + ensembles.getFittest().getReglas());
		for (int i = 1; i <= generaciones; i++) {
			evolucionar();
			if (verbose)
				System.out.println(i + ";" + (System.currentTimeMillis() - inicio) + ";" + ensembles.getFittest().train() + ";" + ensembles.getCosteMedio() + ";" + ensembles.getFittest().getReglas());
		}
		fin();
		return ensembles.getFittest();
	}

	protected void evolucionar() {
		PoblacionEnsembles newPopulation = new PoblacionEnsembles(ensembles.size());
		ensembles.mezclarPoblacion();
		for (int i = 0; i < sizePoblacion; i = i + 2) {
			Ensemble_Coordinado padre1 = ensembles.get(i);
			Ensemble_Coordinado padre2 = ensembles.get(i + 1);
			if (Math.random() <= ratioCruce) {
				// CRUZAR
				Ensemble_Coordinado hijo1 = cruzar(padre1, padre2);
				Ensemble_Coordinado hijo2 = cruzar(padre2, padre1);

				
				// MUTACION
				if (Math.random() <= ratioMutacion)
					hijo1.mutar();
				if (Math.random() <= ratioMutacion)
					hijo2.mutar();
				
				
				// SELECCIÓN
				if (hijo1.train() < hijo2.train()) {
					PoblacionEnsembles torneo = new PoblacionEnsembles(3);
					torneo.save(0, padre1);
					torneo.save(1, padre2);
					torneo.save(2, hijo2);
					newPopulation.save(i, hijo1);
					newPopulation.save(i + 1, torneo.getFittest());
				} else {
					PoblacionEnsembles torneo = new PoblacionEnsembles(3);
					torneo.save(0, padre1);
					torneo.save(1, padre2);
					torneo.save(2, hijo1);
					newPopulation.save(i, hijo2);
					newPopulation.save(i + 1, torneo.getFittest());
				}
			} else {
				Ensemble_Coordinado hijo1 = padre1.getClon(), hijo2 = padre2.getClon();
				Ensemble_Coordinado mutacion1 = null, mutacion2 = null;
				
				
				// MUTACION
				if (Math.random() <= ratioMutacion) {
					mutacion1 = padre1.getClon();
					mutacion1.mutar();
				}
				if (Math.random() <= ratioMutacion) {
					mutacion2 = padre2.getClon();
					mutacion2.mutar();
				}

				
				// SELECCIÓN
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
					Ensemble_Coordinado first = torneo.getFittest();
					newPopulation.save(i, first);
					torneo.remove(first);
					Ensemble_Coordinado second = torneo.getFittest();
					newPopulation.save(i + 1, second);
				}
			}
		}
		ensembles = newPopulation;
	}

	protected Ensemble_Coordinado torneo(int nTorneo) {
		PoblacionEnsembles pop = new PoblacionEnsembles(nTorneo);
		for (int i = 0; i < nTorneo; i++)
			pop.save(i, ensembles.get((int) (Math.random() * ensembles.size())));
		return pop.getFittest();
	}

	protected Ensemble_Coordinado cruzar(Ensemble_Coordinado padre1, Ensemble_Coordinado padre2) {
		padre1.mezclar();
		padre2.mezclar();
		int n = (int) (Math.random() * padre1.size());
		Ensemble_Coordinado hijo = padre2.getClon();
		for (int i = 0; i < n; i++)
			hijo.inserta(i, padre1.get(i));
		return hijo;
	}
}