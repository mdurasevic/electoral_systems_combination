package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoTerminal;
import domain.TSPFunction;

/**
 * Distance left estimation
 */
public class NodoT7Dle extends NodoTerminal {

	private static final NodoT7Dle INSTANCE = new NodoT7Dle();

	public static NodoT7Dle getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(TSPFunction regla) {
		return regla.distanceLeftEstimation();
	}

	@Override
	public String simbolo() {
		return "Dle";
	}
	
	@Override
	public double orden() {
		return 7;
	}	
}
