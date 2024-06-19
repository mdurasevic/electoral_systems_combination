package framework.calculo_reglas.representacion.nodos.constantes;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import framework.calculo_reglas.representacion.Nodo;
import framework.calculo_reglas.representacion.nodos.NodoConstanteAbstracto;

public class NodoConstante1_0 extends NodoConstanteAbstracto {

	private static final NodoConstante1_0 INSTANCE = new NodoConstante1_0();

	public static NodoConstante1_0 getInstance() {
//		System.out.println("instanciada 1.0");
		return INSTANCE;
	}

	@Override
	public Nodo clona() {
		return INSTANCE;
	}

	@Override
	public double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo) {
		return 1.0;
	}

	@Override
	public double value(double izq, double der) {
		return 1.0;
	}

	@Override
	public double value() {
		return 1.0;
	}

	@Override
	public String simbolo() {
		return "1.0";
	}
}