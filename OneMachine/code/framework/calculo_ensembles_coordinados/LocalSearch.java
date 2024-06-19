package framework.calculo_ensembles_coordinados;

import java.util.HashSet;
import java.util.Set;

public class LocalSearch extends Algoritmo {

	protected boolean HC;
	protected int iteraciones;
	protected int sizeVecindario;

	public LocalSearch(Instancia instancia, int iteraciones, int sizeVecindario, boolean HC) {
		this.instancia = instancia;
		this.iteraciones = iteraciones;
		this.sizeVecindario = sizeVecindario;
		this.HC = HC;
	}

	@Override
	public Ensemble_Coordinado run() {
		return run(new Ensemble_Coordinado(iteraciones, instancia).random());
	}

	public Ensemble_Coordinado run(Ensemble_Coordinado ensemble) {
		double costeMejor = ensemble.train();
		double coste;
		for (int iteracion = 0; iteracion < iteraciones; iteracion++) {
			Ensemble_Coordinado mejorVecino = iteracion(ensemble, sizeVecindario);
			coste = mejorVecino.train();
			if (coste < costeMejor) {
				costeMejor = coste;
				ensemble = mejorVecino;
			} else
				break; // si no mejora se corta la ejecución
		}
		return ensemble;
	}

	Ensemble_Coordinado iteracion(Ensemble_Coordinado ensemble, int sizeVecindario) {
		int indice_peorRegla = ensemble.indiceReglaMenosAporta();

		Ensemble_Coordinado mejorVecino = new Ensemble_Coordinado(ensemble);

		// 3- Se seleccionan las reglas del vecindario
		// a- Si el tamaño del vecindario es mayor o igual que las reglas del Almacen
		// entonces se salta a 4-
		if (sizeVecindario >= instancia.getNreglas()) {
			// 4- Se inicia la búsqueda de vecinos, recorriendo todas las reglas
			double costeMejor = ensemble.train();
			double coste;
			for (int regla = 0; regla < instancia.getNreglas(); regla++) {
				// Si la solución actual no contiene la regla
				// Se genera un vecino y se compara con el mejor vecino
				Ensemble_Coordinado vecino = creaVecino(ensemble, indice_peorRegla, regla);
				coste = vecino.train();
				if (coste < costeMejor) {
					costeMejor = coste;
					mejorVecino = vecino;
					if (HC)
						break;
				}
			}
		}
		// b- Se seleccionan las reglas del vecindario de forma aleatoria SIN
		// repeticiones.
		else {
			Set<Integer> reglas = new HashSet<Integer>();
			while (reglas.size() < sizeVecindario) {
				int regla = (int) (Math.random() * instancia.getNreglas());
				reglas.add(regla);
			}
//			System.out.println("Tamaño vecindario:"+sizeVecindario+" real:"+reglas.size());
			// 4- Se inicia la búsqueda de vecinos, recorriendo las reglas del vecindario
			double costeMejor = ensemble.train();
			double coste;
			for (Integer regla : reglas) {
				// Si la solución actual no contiene la regla
				// Se genera un vecino y se compara con el mejor vecino
				Ensemble_Coordinado vecino = creaVecino(ensemble, indice_peorRegla, regla);
				coste = vecino.train();
				if (coste < costeMejor) {
					costeMejor = coste;
					mejorVecino = vecino;
					break;
				}
			}
		}
		return mejorVecino;
	}

	Ensemble_Coordinado creaVecino(Ensemble_Coordinado ensemble, int indice_regla, int regla) {
		Ensemble_Coordinado vecino = new Ensemble_Coordinado(ensemble);
		vecino.inserta(indice_regla, regla);
		return vecino;
	}
}
