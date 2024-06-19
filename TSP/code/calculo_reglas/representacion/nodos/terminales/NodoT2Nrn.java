package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Number of remaining nodes to visit
 */
public class NodoT2Nrn extends NodoTerminal {

	private static final NodoT2Nrn INSTANCE = new NodoT2Nrn();

	public static NodoT2Nrn getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.noVisited.size();
	}

	@Override
	public String simbolo() {
		return "Nrn";
	}

	@Override
	public double orden() {
		return 2;
	}
}
