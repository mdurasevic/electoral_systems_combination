package framework.calculo_reglas.representacion.nodos.operadores;

import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoMax0 extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoMax0();
	}

	@Override
	public double value(double izq, double der) {
		return Math.max(izq, 0);
	}

	@Override
	public boolean esUnaria() {
		return true;
	}

	@Override
	public String simbolo() {
		return "Math.max";
	}

	@Override
	public boolean esBinariaInversa() {
		return false;
	}

	@Override
	public boolean noAplicaRestriccionesDimension() {
		return true;
	}

	@Override
	public boolean simetrico() {
		return false;
	}

	@Override
	public double orden() {
		return 13;
	}
}
