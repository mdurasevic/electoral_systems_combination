package framework.calculo_reglas.representacion.nodos;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;

public abstract class NodoConstanteAbstracto extends NodoAbstracto {

	@Override
	public boolean esTerminal() {
		return true;
	}

	@Override
	public boolean esConstante() {
		return true;
	}

	@Override
	public boolean noAplicaRestriccionesDimension() {
		return false;
	}

	@Override
	public double value() {
		return -1;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return -1;
	}

	@Override
	public double value(double izq, double der) {
		return -1;
	}

	@Override
	public boolean esOperacion() {
		return false;
	}

	@Override
	public boolean esUnaria() {
		return false;
	}

	@Override
	public boolean esBinariaInversa() {
		return false;
	}

	@Override
	public double getDimension() {
		return 0;
	}

	@Override
	public void setDimension(double d) {
	}

	@Override
	public boolean esNull() {
		return false;
	}

	@Override
	public boolean simetrico() {
		return false;
	}

	@Override
	public double orden() {
		return value();
	}
}
