package framework.calculo_reglas.representacion.nodos.constantes;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoConstanteAbstracto;

public class NodoConstante extends NodoConstanteAbstracto {

	private double value;

	public NodoConstante(double value) {
		this.value = value;
	}

	@Override
	public Nodo clona() {
		return new NodoConstante(value);
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return value;
	}

	@Override
	public double value(double izq, double der) {
		return value;
	}

	@Override
	public double value() {
		return value;
	}

	@Override
	public String simbolo() {
		return String.valueOf(value);
	}

}
