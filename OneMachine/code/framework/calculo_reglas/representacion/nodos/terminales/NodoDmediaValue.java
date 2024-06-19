package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoDmediaValue extends NodoTerminal {

	private static final NodoDmediaValue INSTANCE = new NodoDmediaValue();

	public static NodoDmediaValue getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		double promedio = 0.0;
		for (Integer t : sin_asignar)
			promedio += instancia.d[t];
		return promedio / (sin_asignar.size() * 1.0);
	}

	@Override
	public String simbolo() {
		return "promedioDuedates()";
	}

	@Override
	public double orden() {
		return 9;
	}
}
