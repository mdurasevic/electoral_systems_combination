package calculo_reglas.representacion.nodos.operadores;

import calculo_reglas.representacion.Nodo;
import calculo_reglas.representacion.nodos.NodoOperacion;

public class NodoDivision extends NodoOperacion {

	@Override
	public Nodo clona() {
		return new NodoDivision();
	}

	@Override
	public double value(double izq, double der) {
		double d = izq / der;
		if (Double.isNaN(d) || Double.isInfinite(d)) { // por el momento division protegida
			d = 0;
//			System.out.println("Se da!");
		}
		return d;
	}

	@Override
	public boolean esUnaria() {
		return false;
	}

	@Override
	public String simbolo() {
		return "/";
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
		return 112;
	}
}
