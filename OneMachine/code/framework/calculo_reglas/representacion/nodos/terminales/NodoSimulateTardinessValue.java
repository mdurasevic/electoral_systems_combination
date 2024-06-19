package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoSimulateTardinessValue extends NodoTerminal {

	private static final NodoSimulateTardinessValue INSTANCE = new NodoSimulateTardinessValue();

	public static NodoSimulateTardinessValue getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return (int) Math.max(0, instancia.p[tarea] - (instancia.d[tarea] - intervalo.inicio));
	}

	@Override
	public String simbolo() {
		return "simulateTardiness[tarea]";
	}
	
	@Override
	public double orden() {
		return 8;
	}	
}
