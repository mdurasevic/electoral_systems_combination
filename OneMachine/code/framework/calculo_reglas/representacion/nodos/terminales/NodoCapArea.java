package framework.calculo_reglas.representacion.nodos.terminales;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoTerminal;

public class NodoCapArea extends NodoTerminal {

	private static final NodoCapArea INSTANCE = new NodoCapArea();

	public static NodoCapArea getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return (intervalo.fin - intervalo.inicio) * intervalo.cap;
	}

	@Override
	public String simbolo() {
		return "cap.area";
	}

	@Override
	public double orden() {
		return 6;
	}
	
	@Override
	public double getDimension() {
		return 2.0;
	}
}
