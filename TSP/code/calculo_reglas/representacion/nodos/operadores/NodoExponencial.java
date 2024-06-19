package calculo_reglas.representacion.nodos.operadores;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoExponencial extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoExponencial();
	}

	@Override
	public double value(double izq, double der) {
		return Math.exp(izq);
	}

	@Override
	public boolean esUnaria() {
		return true;
	}

	@Override
	public String simbolo() {
		return "Math.exp";
	}

	@Override
	public boolean esBinariaInversa() {
		return false;
	}

	@Override
	public boolean noAplicaRestriccionesDimension() {
		return false;
	}

	@Override
	public boolean simetrico() {
		return false;
	}

	@Override
	public double orden() {
		return 111;
	}
}
