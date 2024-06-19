package framework.core;

public abstract class AlgorithmAbstract<T> implements Algorithm {

	public T mejor;
	public int evaluations;

	public AlgorithmAbstract() {
	}

	public AlgorithmAbstract(String mensaje) {
		AlgoUtil.algorithm = this;
		AlgoUtil.iniciaMedicionTiempo();
		AlgoUtil.iniciaLogger(mensaje);
		AlgoUtil.iniciaEvaluadorReglas();
	}

	// ES IMPORTANTE QUE ESTE METODO SE LLAME EN LAS PARTES DEL ALGORITMO QUE MÁS TIEMPO LLEVAN.
	protected boolean timeOut() {
		return AlgoUtil.timeOut();
	}

	/**
	 * Este método se debe redefinir
	 */
	@Override
	public void imprimeLinea() {
		AlgoUtil.vln(getLinea());
	}

	public String getLinea() {
		return "redefine el getLinea()";
	}

	@Override
	public void run() {
		AlgoUtil.vln("redefine el run()");
	}

	public T getMejor() {
		return mejor;
	}
	
	public int evaluations() {
		return evaluations;
	}
}
