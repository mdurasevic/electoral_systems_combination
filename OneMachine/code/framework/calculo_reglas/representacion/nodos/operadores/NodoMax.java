package framework.calculo_reglas.representacion.nodos.operadores;

import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoMax extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoMax();
	}

	@Override
	public double value(double izq, double der) {
		return Math.max(izq, der);
	}

	@Override
	public boolean esUnaria() {
		return false;
	}

	@Override
	public String simbolo() {
		return "Math.max";
	}

	@Override
	public boolean esBinariaInversa() {
		return true;
	}

	@Override
	public boolean simetrico() {
		return true;
	}

	@Override
	public boolean noAplicaRestriccionesDimension() {
		return false;
	}
	
	@Override
	public double orden() {
		return 14;
	}	
}
