package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoInicioIntervaloValue extends NodoTerminal {

	private static final NodoInicioIntervaloValue INSTANCE = new NodoInicioIntervaloValue();

	public static NodoInicioIntervaloValue getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return intervalo.inicio;
	}

	@Override
	public String simbolo() {
		return "intervalo.inicio";
	}
	
	@Override
	public double orden() {
		return 3;
	}	
}
