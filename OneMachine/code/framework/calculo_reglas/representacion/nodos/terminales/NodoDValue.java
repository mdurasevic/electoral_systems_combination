package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoDValue extends NodoTerminal {

	private static final NodoDValue INSTANCE = new NodoDValue();

	public static NodoDValue getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return instancia.d[tarea];
	}

	@Override
	public String simbolo() {
		return "instancia.d[tarea]";
	}
	
	@Override
	public double orden() {
		return 4;
	}	
}
