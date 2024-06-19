package calculo_reglas.representacion.nodos.operadores;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoMin extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoMin();
	}

	@Override
	public double value(double izq, double der) {
		return Math.min(izq, der);
	}

	@Override
	public boolean esUnaria() {
		return false;
	}

	@Override
	public String simbolo() {
		return "Math.min";
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
		return 107;
	}
}
