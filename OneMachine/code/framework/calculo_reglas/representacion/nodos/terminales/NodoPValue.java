package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoPValue extends NodoTerminal {

	private static final NodoPValue INSTANCE = new NodoPValue();

	public static NodoPValue getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return instancia.p[tarea];
	}

	@Override
	public String simbolo() {
		return "instancia.p[tarea]";
	}

	@Override
	public double orden() {
		return 1;
	}
}
