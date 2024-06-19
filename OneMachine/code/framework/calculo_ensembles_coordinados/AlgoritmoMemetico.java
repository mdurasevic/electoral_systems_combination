package framework.calculo_ensembles_coordinados;

public class AlgoritmoMemetico extends AlgoritmoGenetico {

	protected double ratioLSA;
	protected int iteraciones;
	protected int sizeVecindario;
	protected boolean HC;

	public AlgoritmoMemetico() {
	}

	public AlgoritmoMemetico(Instancia instancia, int generaciones, int sizePoblacion, double ratioCruce,
			double ratioMutacion, int cardinalidadEnsembles, double ratioLSA, int iteraciones, int sizeVecindario,
			boolean HC) {
		super(instancia, generaciones, sizePoblacion, ratioCruce, ratioMutacion, cardinalidadEnsembles);
		this.ratioLSA = ratioLSA;
		this.iteraciones = iteraciones;
		this.sizeVecindario = sizeVecindario;
		this.HC = HC;
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
//			aplicarLSA(); // pipeline
			if (verbose)
				System.out.println(i + ";" + (System.currentTimeMillis() - inicio) + ";" + ensembles.getFittest().train() + ";" + ensembles.getCosteMedio() + ";" + ensembles.getFittest().getReglas());
		}
		fin();
		return ensembles.getFittest();
	}
	
	/*
	 * Esta versión en la que se aplica LSA 
	 */
	
	
	@Override
	protected void evolucionar() {
		PoblacionEnsembles newPopulation = new PoblacionEnsembles(ensembles.size());
		ensembles.mezclarPoblacion();
		for (int i = 0; i < sizePoblacion; i = i + 2) {
			Ensemble_Coordinado padre1 = ensembles.get(i);
			Ensemble_Coordinado padre2 = ensembles.get(i + 1);
			if (Math.random() <= ratioCruce) { // SI SE CRUZAN
				// CRUZAR
				Ensemble_Coordinado hijo1 = cruzar(padre1, padre2);
				Ensemble_Coordinado hijo2 = cruzar(padre2, padre1);

				
				// MUTACION
				if (Math.random() <= ratioMutacion)
					hijo1.mutar();
				if (Math.random() <= ratioMutacion)
					hijo2.mutar();

				
				// LSA
				if (Math.random() <= ratioLSA)
					hijo1 = lsa(hijo1);
				if (Math.random() <= ratioLSA)
					hijo2 = lsa(hijo2);
				
				
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

				
				// LSA
				if (Math.random() <= ratioLSA) {
					if (mutacion1 != null)
						mutacion1 = lsa(mutacion1);
					else
						padre1 = lsa(padre1);
				}
				if (Math.random() <= ratioLSA) {
					if (mutacion2 != null)
						mutacion2 = lsa(mutacion2);
					else
						padre2 = lsa(padre2);
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
	
	void aplicarLSA() {
		for (int j = 0; j < sizePoblacion; j++)
			if (Math.random() <= ratioLSA) 
				ensembles.save(j, lsa(ensembles.get(j)));
	}
	
	Ensemble_Coordinado lsa(Ensemble_Coordinado ensemble) {
		return new LocalSearch(instancia, iteraciones, sizeVecindario, HC).run(ensemble);
	}

}