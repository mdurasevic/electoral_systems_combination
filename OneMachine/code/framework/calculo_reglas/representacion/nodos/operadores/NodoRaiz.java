package framework.calculo_reglas.representacion.nodos.operadores;

import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoRaiz extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoRaiz();
	}

	@Override
	public double value(double izq, double der) {
		return Math.sqrt(izq);
	}

	@Override
	public boolean esUnaria() {
		return true;
	}

	@Override
	public String simbolo() {
		return "Math.sqrt";
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
		return 7;
	}	
}
