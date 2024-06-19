package calculo_reglas.representacion.nodos.terminales;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoConstanteAbstracto;
import domain.TSPFunction;

public class NodoConstante extends NodoConstanteAbstracto {

	private double value;

	public NodoConstante(double value) {
		this.value = value;
	}

	@Override
	public Nodo clona() {
		return new NodoConstante(value);
	}

	@Override
	public double value(TSPFunction regla) {
		return value;
	}

	@Override
	public double value(double izq, double der) {
		return value;
	}

	@Override
	public double value() {
		return value;
	}

	@Override
	public String simbolo() {
		return String.valueOf(value);
	}

}
