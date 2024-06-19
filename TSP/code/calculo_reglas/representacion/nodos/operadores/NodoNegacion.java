package calculo_reglas.representacion.nodos.operadores;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoNegacion extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoNegacion();
	}

	@Override
	public double value(double izq, double der) {
		return -(izq);
	}

	@Override
	public boolean esUnaria() {
		return true;
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
		return true;
	}

	@Override
	public boolean simetrico() {
		return false;
	}

	@Override
	public double orden() {
		return 104;
	}
}
