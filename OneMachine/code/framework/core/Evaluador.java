package framework.core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import framework.calculo_reglas.Regla;

public class Evaluador {

	// cache de reglas
	public Map<Regla, Regla> evaluatedFull;
	Map<Regla, Regla> evaluatedPartial; 
	Map<Regla, Regla> mejoradosLSA;

	public List<Regla> seleccionables;

	// metricas mejor regla
	public Regla mejorGlobal;
	double coste;
	int size;
	int depth;
	public int incluyeSurrogados;

	public Evaluador() {
		evaluatedFull = new HashMap<Regla, Regla>();
		evaluatedPartial = new HashMap<Regla, Regla>();
		mejoradosLSA = new HashMap<Regla, Regla>();
		seleccionables = new ArrayList<Regla>();
		coste = Integer.MAX_VALUE;
		size = Integer.MAX_VALUE;
		depth = Integer.MAX_VALUE;
	}

	public void addSeleccionable(Regla regla) {
		seleccionables.add(regla);
	}

	public List<Regla> getSeleccionables() {
		return seleccionables;
	}

	public Regla getRandom() {
		return seleccionables.get((int) (seleccionables.size() * Math.random()));
	}

	public Regla getRandom(List<Regla> seleccionables) {
		return seleccionables.get((int) (seleccionables.size() * Math.random()));
	}

	public void addLSA(Regla a) {
		mejoradosLSA.put(a, a);
	}

	public boolean yaMejorada(Regla a) {
		return mejoradosLSA.containsKey(a);
	}

	public boolean mejoraReal(Regla a, Regla b) {
		// revisamos si previamente han sido ya comparadas o evaluadas
		a = getFullEvaluate(a);
		b = getFullEvaluate(b);

		// comprobamos si a es mejor que b
		if (a.train() < b.train()) {
			compruebaSiMejora(a);
			return true;
		} else if (a.train() == b.train()) {
			if (a.getSize() < b.getSize()) {
				compruebaSiMejora(a);
				return true;
			} else if (a.train() == b.train() && a.getProfundidad() < b.getProfundidad()) {
				compruebaSiMejora(a);
				return true;
			}
		}
		return false;
	}

	public boolean mejora(Regla a, Regla b) {
		// comprobamos si a es mejor que b
		if (a.train() < b.train()) {
			compruebaSiMejora(a);
			return true;
		} else if (a.train() == b.train()) {
			if (a.getSize() < b.getSize()) {
				compruebaSiMejora(a);
				return true;
			} else if (a.train() == b.train() && a.getProfundidad() < b.getProfundidad()) {
				compruebaSiMejora(a);
				return true;
			}
		}
		return false;
	}
	
	public Regla getFullEvaluate(Regla regla) {
		if (evaluatedFull.containsKey(regla))
			return evaluatedFull.get(regla);
		regla.train();
		evaluatedFull.put(regla, regla);
		seleccionables.add(regla); // aqui es donde se podria añadir un criba a las reglas que compondrán los
									// ensembles
		return regla;
	}

	public boolean mejoraSurrogada(Regla a, Regla b) {
		// revisamos si previamente han sido ya comparadas o evaluadas
		a = getPartialEvaluate(a);
		b = getPartialEvaluate(b);

		// comprobamos si a es mejor que b
		if (a.filter() < b.filter())
			return true;
		else if (a.filter() == b.filter())
			if (a.getSize() < b.getSize())
				return true;
			else if (a.filter() == b.filter() && a.getProfundidad() < b.getProfundidad())
				return true;

		return false;
	}

	public Regla getPartialEvaluate(Regla regla) {
//		if (incluyeSurrogados == 1)
//			if (evaluatedPartial.containsKey(regla))
//				return evaluatedPartial.get(regla);
		regla.filter();
//		if (incluyeSurrogados == 1)
//			evaluatedPartial.put(regla, regla);
		return regla;
	}

	private void compruebaSiMejora(Regla regla) {
		if (regla.train() < coste)
			actualizaMejor(regla);
		else if (regla.train() == coste) {
			if (regla.getSize() < size)
				actualizaMejor(regla);
			else if (regla.getSize() == size && regla.getProfundidad() < depth)
				actualizaMejor(regla);
		}
	}

	public void actualizaMejor(Regla regla) {
		coste = regla.train();
		size = regla.getSize();
		depth = regla.getProfundidad();
		mejorGlobal = regla;
	}
}
