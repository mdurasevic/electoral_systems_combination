package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoNJobs extends NodoTerminal {

	private static final NodoNJobs INSTANCE = new NodoNJobs();

	public static NodoNJobs getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return sin_asignar.size();
	}

	@Override
	public String simbolo() {
		return "N";
	}

	@Override
	public double orden() {
		return 9;
	}
}
