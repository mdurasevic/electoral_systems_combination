package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Distance from the centroid of the nodes
 */
public class NodoT5Dc extends NodoTerminal {

	private static final NodoT5Dc INSTANCE = new NodoT5Dc();

	public static NodoT5Dc getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.distanceCentroidToCandidateCity();
	}

	@Override
	public String simbolo() {
		return "Dc";
	}

	@Override
	public double orden() {
		return 5;
	}
}
