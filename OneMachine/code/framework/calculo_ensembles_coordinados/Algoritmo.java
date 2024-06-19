package framework.calculo_ensembles_coordinados;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;

public abstract class Algoritmo {

	protected Instancia instancia;
	protected int cardinalidadEnsembles = 10;
	protected boolean verbose = false;

	protected long inicio, fin;

	public abstract Ensemble_Coordinado run();

	public void inicio() {
		inicio = System.currentTimeMillis();
	}

	public void fin() {
		fin = System.currentTimeMillis();
	}

	public long tiempo() {
		return fin - inicio;
	}

	public void setVerbose(boolean verbose) {
		this.verbose = verbose;
	}

	public boolean getCardinalidadEnsembles() {
		return verbose;
	}

	public double test(Ensemble_Coordinado ensemble) {
		double test = 0;
		Regla regla;
		int min;
		for (dominio.Instancia i : AlgoUtil.gestorInstancias.set_test) {
			min = Integer.MAX_VALUE;
			for (Integer j : ensemble.getEnsemble()) {
				regla = instancia.seleccionables.get(j);
				if (regla != null) {
					if (regla.test(i.getNombre()) < min)
						min = regla.test(i.getNombre());
				} else
					break;
			}
			test += min;
		}
		test /= AlgoUtil.gestorInstancias.set_test.size();
		return test;
	}

}
