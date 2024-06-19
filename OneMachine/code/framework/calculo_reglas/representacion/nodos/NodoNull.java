package framework.calculo_reglas.representacion.nodos;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;

public class NodoNull extends NodoAbstracto {

	private static final NodoNull INSTANCE = new NodoNull();

	public static NodoNull getInstance() {
		return INSTANCE;
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
	public boolean noAplicaRestriccionesDimension() {
		return false;
	}

	@Override
	public double value() {
		return 0;
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
	public Nodo clona() {
		return INSTANCE;
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
	public String simbolo() {
		return "NULL";
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
		return true;
	}

	@Override
	public boolean simetrico() {
		return false;
	}

	@Override
	public double orden() {
		return -1.0;
	}
}
