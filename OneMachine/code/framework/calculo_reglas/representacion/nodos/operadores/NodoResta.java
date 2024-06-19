package framework.calculo_reglas.representacion.nodos.operadores;

import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoResta extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoResta();
	}

	@Override
	public double value(double izq, double der) {
		return (izq - der);
	}

	@Override
	public boolean esUnaria() {
		return false;
	}

	@Override
	public String simbolo() {
		return "-";
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
		return 6;
	}	
}
