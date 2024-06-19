package framework.calculo_ensembles_colaborativos;

import java.util.HashMap;
import java.util.List;

import dominio.Instancia;
import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;

public class Ensemble_Colaborativo extends Ensemble {

	public Ensemble_Colaborativo(Ensemble ensemble) {
		super(ensemble);
	}

	public Ensemble_Colaborativo(int cardinalidadEnsembles) {
		super(cardinalidadEnsembles);
	}

	public Ensemble_Colaborativo(List<Regla> reglas) {
		super(reglas.toArray(new Regla[reglas.size()]));
	}

	@Override
	public Ensemble_Colaborativo random(List<Regla> seleccionables) {
		for (int i = 0; i < reglas.length; i++)
			reglas[i] = AlgoUtil.evaluadorReglas.getRandom(seleccionables);
		return this;
	}

	
	public void borrarAleatoriamente() {
		borrar((int) (reglas.length * Math.random()));
	}

	public void set(int i, Regla hijo1) {
		if (i >= reglas.length) {
			Regla[] copia = new Regla[reglas.length + 1];
			for (int j = 0; j < copia.length - 1; j++)
				copia[j] = reglas[j];
			reglas = copia;
		}
		reglas[i] = hijo1;
	}
	
	public void cambiaRegla(int i, Regla regla) {
		borrar(i);
		reglas[i] = regla;
	}
	
	/*
	 * RESOLVER CONJUNTO DE ENTRENAMIENTO
	 */
	public double train() {
		if (!evalTrain) {
			solutions_train = new HashMap<String, Integer>();
			train = 0;
			int solucion;
			for (Instancia instancia : AlgoUtil.gestorInstancias.training) {
				solucion = new PlanificadorEnsembles(this).planifica(instancia);
				solutions_train.put(instancia.getNombre(), solucion);
				train += solucion;
			}
			train /= AlgoUtil.gestorInstancias.training.size();
			evalTrain = true;
		}
		return train;
	}

	public int train(String instancia) {
		train();
		return solutions_train.get(instancia);
	}

	/*
	 * RESOLVER CONJUNTO DE TEST
	 */
	public double test() {
		if (!evalTest) {
			solutions_test = new HashMap<String, Integer>();
			test = 0;
			int solucion;
			for (Instancia instancia : AlgoUtil.gestorInstancias.set_test) {
				solucion = new PlanificadorEnsembles(this).planifica(instancia);
				solutions_test.put(instancia.getNombre(), solucion);
				test += solucion;
			}
			test /= AlgoUtil.gestorInstancias.set_test.size();
			evalTest = true;
		}
		return test;
	}

	public int test(String instancia) {
		test();
		return solutions_test.get(instancia);
	}

	/*
	 * RESOLVER CONJUNTO DE FILTRO
	 */
	public double filter() {
		if (!evalFilter) {
			filter = 0;
			int solucion;
			for (Instancia instancia : AlgoUtil.gestorInstancias.filtro) {
				solucion = new PlanificadorEnsembles(this).planifica(instancia);
				filter += solucion;
			}
			filter /= AlgoUtil.gestorInstancias.filtro.size();
			evalFilter = true;
		}
		return filter;
	}

	@Override
	public Ensemble_Colaborativo getClon() {
		return new Ensemble_Colaborativo(this);
	}
}
