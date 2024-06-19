package framework.calculo_reglas.representacion;

import java.util.List;

import dominio.Instancia;
import dominio.Intervalo;
import dominio.Planificador;

public interface Nodo {

	boolean esTerminal();

	boolean esConstante();

	boolean noAplicaRestriccionesDimension();

	double value();
	
	double value(Instancia instancia, List<Integer> sin_asignar, int tarea, Intervalo intervalo);

	double value(double izq, double der);

	Nodo clona();

	boolean esOperacion();

	boolean esUnaria();

	boolean esBinariaInversa();

	String simbolo();

	double getDimension();

	void setDimension(double d);

	boolean esNull();

	boolean simetrico();

	double orden();

}
