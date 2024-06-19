package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Number of nodes in the graph
 */
public class NodoT1Nn extends NodoTerminal {

	private static final NodoT1Nn INSTANCE = new NodoT1Nn();

	public static NodoT1Nn getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.instance.getSize();
	}

	@Override
	public String simbolo() {
		return "Nn";
	}

	@Override
	public double orden() {
		return 1;
	}
}
