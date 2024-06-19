package calculo_reglas.representacion;

import domain.TSPFunction;

public interface Nodo {

	boolean esTerminal();

	boolean esConstante();

	boolean noAplicaRestriccionesDimension();

	double value();
	
	double value(TSPFunction function);

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
