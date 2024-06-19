package framework.calculo_reglas.representacion.nodos;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;

public abstract class NodoOperacion extends NodoAbstracto {

	protected double dimension;

	public double getDimension() {
		return dimension;
	}

	public void setDimension(double dimension) {
		this.dimension = dimension;
	}

	public boolean esNull() {
		return false;
	}

	@Override
	public boolean esTerminal() {
		return false;
	}

	@Override
	public boolean esConstante() {
		return false;
	}

	@Override
	public boolean esOperacion() {
		return true;
	}

	@Override
	public double value() {
		return 0;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return 0;
	}

}