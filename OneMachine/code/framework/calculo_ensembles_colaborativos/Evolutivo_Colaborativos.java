package framework.calculo_ensembles_colaborativos;

import framework.core.AlgoUtil;
import framework.core.AlgorithmAbstract;
import framework.core.Evolutivo;

public class Evolutivo_Colaborativos extends AlgorithmAbstract<Ensemble_Colaborativo> implements Evolutivo {

	protected Poblacion_Colaborativos pop;

	// PARAMETROS
	protected double ratioCruce = 0.8;
	protected double ratioMutacion = 0.2;
	protected int sizePop = 100;
	protected int maxEvaluations = Integer.MAX_VALUE;
	
	//
	public int generations;

	public Evolutivo_Colaborativos() {
	}
	
	public Evolutivo_Colaborativos(int maxEvaluations) {
		this.maxEvaluations = maxEvaluations;
	}

	public Evolutivo_Colaborativos(String mensaje) {
		super(mensaje);
	}

	@Override
	public void run() {
		evolucionar();
		mejor = pop.getBest();
	}

	public Ensemble_Colaborativo getMejor() {
		return pop.getBest();
	}
	
	@Override
	public void generarPoblacionInicial() {
		pop = new Poblacion_Colaborativos(sizePop);
		pop.generaRandom(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos, AlgoUtil.evaluadorReglas.getSeleccionables());
		evaluations += pop.size();
		generations++;
	}

	@Override
	public void evolucionar() {
		pop.shuffle();
		for (int i = 0; i < sizePop && !timeOut() && evaluations < maxEvaluations; i = i + 2) {
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
				if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(hijo1, hijo2)) {
					Poblacion_Colaborativos torneo = new Poblacion_Colaborativos(3);
					torneo.add(padre1);
					torneo.add(padre2);
					torneo.add(hijo2);
					pop.set(i, hijo1);
					pop.set(i + 1, torneo.getBest());
				} else {
					Poblacion_Colaborativos torneo = new Poblacion_Colaborativos(3);
					torneo.add(padre1);
					torneo.add(padre2);
					torneo.add(hijo1);
					pop.set(i, hijo2);
					pop.set(i + 1, torneo.getBest());
				}
				evaluations += 2;
			} else {
				Ensemble_Colaborativo hijo1 = padre1.getClon(), hijo2 = padre2.getClon();
				Ensemble_Colaborativo mutacion1 = null, mutacion2 = null;
				if (Math.random() <= ratioMutacion) {
					mutacion1 = padre1.getClon();
					mutar(mutacion1);
					evaluations++;
				}
				if (Math.random() <= ratioMutacion) {
					mutacion2 = padre2.getClon();
					mutar(mutacion2);
					evaluations++;
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
		generations++;
	}

	void mutar(Ensemble_Colaborativo ensemble) {
		ensemble.mezclar();
		int n = (int) (Math.random() * ensemble.reglas.length / 2);
		for (int i = 0; i < n; i++)
			ensemble.inserta(i, AlgoUtil.evaluadorReglas.getRandom());
	}

	Ensemble_Colaborativo cruzar(Ensemble_Colaborativo padre1, Ensemble_Colaborativo padre2) {
		padre1.mezclar();
		padre2.mezclar();
		int n = (int) (Math.random() * padre1.reglas.length);
		Ensemble_Colaborativo hijo = padre2.getClon();
		for (int i = 0; i < n; i++)
			hijo.inserta(i, padre1.reglas[i]);
		return hijo;
	}
}