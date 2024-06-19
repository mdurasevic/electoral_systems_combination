package framework.calculo_reglas.representacion.nodos.operadores;

import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoLoge extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoLoge();
	}

	@Override
	public double value(double izq, double der) {
		return Math.log(izq);
	}

	@Override
	public boolean esUnaria() {
		return true;
	}

	@Override
	public String simbolo() {
		return "Math.log";
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
		return 15;
	}
}
