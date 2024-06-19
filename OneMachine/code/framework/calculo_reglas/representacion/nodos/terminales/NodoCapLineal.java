package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoCapLineal extends NodoTerminal {

	private static final NodoCapLineal INSTANCE = new NodoCapLineal();

	public static NodoCapLineal getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return (intervalo.fin - intervalo.inicio);
	}

	@Override
	public String simbolo() {
		return "cap.lineal";
	}

	@Override
	public double orden() {
		return 5;
	}
}
