package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Distance from the CURRENT node
 */
public class NodoT3Dcn extends NodoTerminal {

	private static final NodoT3Dcn INSTANCE = new NodoT3Dcn();

	public static NodoT3Dcn getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.getDistanceFromCurrent();
	}

	@Override
	public String simbolo() {
		return "Dcn";
	}
	
	@Override
	public double orden() {
		return 3;
	}	
}
