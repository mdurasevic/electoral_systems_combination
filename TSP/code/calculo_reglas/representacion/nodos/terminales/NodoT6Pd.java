package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Predicted distance from the initial node
 */
public class NodoT6Pd extends NodoTerminal {

	private static final NodoT6Pd INSTANCE = new NodoT6Pd();

	public static NodoT6Pd getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.predictedDistance();
	}

	@Override
	public String simbolo() {
		return "Pd";
	}

	@Override
	public double orden() {
		return 6;
	}
}
