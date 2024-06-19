package calculo_reglas.representacion.nodos.operadores;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoMin0 extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoMin0();
	}

	@Override
	public double value(double izq, double der) {
		return Math.min(izq, 0);
	}

	@Override
	public boolean esUnaria() {
		return true;
	}

	@Override
	public String simbolo() {
		return "Math.min";
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
		return 106;
	}	
}
