package framework.calculo_reglas.local_search;

import framework.calculo_reglas.Regla;

public class GRASP_reglas extends LSA_reglas {

	// METRICAS
	public LSA_reglas lsa;

	public GRASP_reglas() {
		lsa = new LSA_reglas();
	}

	public GRASP_reglas(String mensaje) {
		super(mensaje);
		lsa = new LSA_reglas();
	}

	public GRASP_reglas(int LSA, int ME_LSA, int vecinosMaximos) {
		this.LSA = LSA;
		this.ME_LSA = ME_LSA;
		this.vecinosMaximos = vecinosMaximos;
		lsa = new LSA_reglas(LSA, ME_LSA, iteracionesMaximas, vecinosMaximos);
	}

	@Override
	public void run() {
//		while (!AlgoUtil.timeOut())
		lsa.run(new Regla().random());
	}
}