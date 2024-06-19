package framework.calculo_ensembles_colaborativos;

import java.util.Collections;

import framework.calculo_reglas.Regla;
import framework.core.AlgoUtil;

public class Voraz_ColaborativosLimitado extends Voraz_Colaborativos {

	public int limite = 50;

	public Voraz_ColaborativosLimitado() {
	}

	public Voraz_ColaborativosLimitado(int limite) {
		this.limite = limite;
	}

	public Voraz_ColaborativosLimitado(String mensaje, int limite) {
		super(mensaje);
		this.limite = limite;
	}

	@Override
	public void run() {
		Ensemble_Colaborativo aux;
		boolean mejora;
		Regla best = null;

		Ensemble_Colaborativo ensemble = new Ensemble_Colaborativo(
				AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos);

		for (int index = 0; index < AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos && !timeOut(); index++) {
			mejora = false;
			
			Collections.shuffle(AlgoUtil.evaluadorReglas.getSeleccionables()); // aqui los mezclamos y luego iteramos
																				// hasta un limite
			for (int i = 0; i < limite && !timeOut(); i++) {
				aux = new Ensemble_Colaborativo(ensemble);
				aux.inserta(index, AlgoUtil.evaluadorReglas.getSeleccionables().get(i));
				if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(aux, ensemble)) {
					mejora = true;
					best = AlgoUtil.evaluadorReglas.getSeleccionables().get(i);
				}
			}
			if (!mejora)
				break;
			else
				ensemble.inserta(index, best);
		}
		mejor = ensemble;
	}

}
