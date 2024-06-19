package framework.calculo_reglas.representacion.nodos.constantes;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoConstanteAbstracto;

public class NodoConstante0_25 extends NodoConstanteAbstracto {

	private static final NodoConstante0_25 INSTANCE = new NodoConstante0_25();

	public static NodoConstante0_25 getInstance() {
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return 0.25;
	}

	@Override
	public double value(double izq, double der) {
		return 0.25;
	}

	@Override
	public double value() {
		return 0.25;
	}

	@Override
	public String simbolo() {
		return "0.25";
	}
	
}