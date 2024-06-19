package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Distance from the INITIAL node
 */
public class NodoT4Din extends NodoTerminal {

	private static final NodoT4Din INSTANCE = new NodoT4Din();

	public static NodoT4Din getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.getDistanceFromInitial();
	}

	@Override
	public String simbolo() {
		return "Din";
	}
	
	@Override
	public double orden() {
		return 4;
	}	
}
