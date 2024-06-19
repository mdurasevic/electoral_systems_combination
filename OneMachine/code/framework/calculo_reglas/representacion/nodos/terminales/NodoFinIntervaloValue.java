package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoFinIntervaloValue extends NodoTerminal {

	private static final NodoFinIntervaloValue INSTANCE = new NodoFinIntervaloValue();

	public static NodoFinIntervaloValue getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return intervalo.fin;
	}

	@Override
	public String simbolo() {
		return "intervalo.fin";
	}
	
	@Override
	public double orden() {
		return 7;
	}	
}
