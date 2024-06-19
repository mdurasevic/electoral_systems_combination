package calculo_reglas.representacion.nodos;

import domain.TSPFunction;

public abstract class NodoTerminal extends NodoAbstracto {

	@Override
	public boolean esTerminal() {
		return true;
	}

	@Override
	public boolean esConstante() {
		return false;
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
	public double value(TSPFunction regla) {
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
		return 1;
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
}
