package calculo_ensembles_coordinados;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class SimpleLocalSearch {

	static List<Regla> vecindarioReglas = AlgoUtil.seleccionables;

	public static EnsembleCoordinado busquedaLocalIterativaEscaladaSimple(EnsembleCoordinado ensemble, int sizeVecindario) {
		double costeMejor = ensemble.getCoste();
		double coste;
		while (true) {
			int indice_peorRegla = ensemble.indiceReglaMenosAporta();
			EnsembleCoordinado mejorVecino = busquedaLocalEscaladaSimple(ensemble, indice_peorRegla, sizeVecindario);
			coste = mejorVecino.getCoste();
			if (coste < costeMejor) {
				costeMejor = coste;
				ensemble = mejorVecino;
			} else
				break; // si no mejora se corta la ejecución
		}
		return ensemble;
	}

	public static EnsembleCoordinado busquedaLocalIterativaMaximoGradiente(EnsembleCoordinado ensemble, int sizeVecindario) {
		double costeMejor = ensemble.getCoste();
		double coste;
		while (true) {
			int indice_peorRegla = ensemble.indiceReglaMenosAporta();
			EnsembleCoordinado mejorVecino = busquedaLocalMaximoGradiente(ensemble, indice_peorRegla, sizeVecindario);
			coste = mejorVecino.getCoste();
			if (coste < costeMejor) {
				costeMejor = coste;
				ensemble = mejorVecino;
			} else
				break; // si no mejora se corta la ejecución
		}
		return ensemble;
	}

	public static EnsembleCoordinado busquedaLocalIterativaEscaladaSimple(EnsembleCoordinado ensemble, int iteraciones,
			int sizeVecindario) {
		double costeMejor = ensemble.getCoste();
		double coste;
		for (int iteracion = 0; iteracion < iteraciones; iteracion++) {
			int indice_peorRegla = ensemble.indiceReglaMenosAporta();
			EnsembleCoordinado mejorVecino = busquedaLocalEscaladaSimple(ensemble, indice_peorRegla, sizeVecindario);
			coste = mejorVecino.getCoste();
			if (coste < costeMejor) {
				costeMejor = coste;
				ensemble = mejorVecino;
			} else
				break; // si no mejora se corta la ejecución
		}
		return ensemble;
	}

	public static EnsembleCoordinado busquedaLocalIterativaMaximoGradiente(EnsembleCoordinado ensemble, int iteraciones,
			int sizeVecindario) {
		double costeMejor = ensemble.getCoste();
		double coste;
		for (int iteracion = 0; iteracion < iteraciones; iteracion++) {
			int indice_peorRegla = ensemble.indiceReglaMenosAporta();
			EnsembleCoordinado mejorVecino = busquedaLocalMaximoGradiente(ensemble, indice_peorRegla, sizeVecindario);
			coste = mejorVecino.getCoste();
			if (coste < costeMejor) {
				costeMejor = coste;
				ensemble = mejorVecino;
			} else
				break; // si no mejora se corta la ejecución
		}
		return ensemble;
	}

	static EnsembleCoordinado busquedaLocalEscaladaSimple(EnsembleCoordinado ensemble, int indice_peorRegla, int sizeVecindario) {
		// 1- copiamos en el mejorVecino las reglas del ensemble original
		EnsembleCoordinado mejorVecino = new EnsembleCoordinado(ensemble);

		// 2- para cada cada instancia buscamos su mejor solución con las reglas del
		// ensemble
		double[] solucionesNuevoEnsemble = new double[AlgoUtil.set_train.size()];
		for (int instancia = 0; instancia < solucionesNuevoEnsemble.length; instancia++)
			solucionesNuevoEnsemble[instancia] = ensemble.getCoste(AlgoUtil.set_train.get(instancia));

		// 3- Se seleccionan las reglas del vecindario
		// a- Si el tamaño del vecindario es mayor o igual que las reglas del Almacen
		// entonces se salta a 4-
		if (sizeVecindario >= vecindarioReglas.size()) {
			// 4- Se inicia la búsqueda de vecinos, recorriendo todas las reglas
			double costeMejor = ensemble.getCoste();
			double coste;
			for (int regla = 0; regla < vecindarioReglas.size(); regla++) {
				// Si la solución actual no contiene la regla
				// Se genera un vecino y se compara con el mejor vecino
				EnsembleCoordinado vecino = creaVecino(ensemble, indice_peorRegla, regla);
				coste = vecino.getCoste();
				if (coste < costeMejor) {
					costeMejor = coste;
					mejorVecino = vecino;
					break;
				}
			}
		}
		// b- Se seleccionan las reglas del vecindario de forma aleatoria SIN
		// repeticiones.
		else {
			Set<Integer> reglas = new HashSet<Integer>();
			while (reglas.size() < sizeVecindario) {
				int regla = (int) (Math.random() * vecindarioReglas.size());
				reglas.add(regla);
			}
//			System.out.println("Tamaño vecindario:"+sizeVecindario+" real:"+reglas.size());
			// 4- Se inicia la búsqueda de vecinos, recorriendo las reglas del vecindario
			double costeMejor = ensemble.getCoste();
			double coste;
			for (Integer regla : reglas) {
				// Si la solución actual no contiene la regla
				// Se genera un vecino y se compara con el mejor vecino
				EnsembleCoordinado vecino = creaVecino(ensemble, indice_peorRegla, regla);
				coste = vecino.getCoste();
				if (coste < costeMejor) {
					costeMejor = coste;
					mejorVecino = vecino;
					break;
				}
			}
		}
		return mejorVecino;
	}

	static EnsembleCoordinado busquedaLocalMaximoGradiente(EnsembleCoordinado ensemble, int indice_peorRegla, int sizeVecindario) {
		// 1- copiamos en el mejorVecino las reglas del ensemble original
		EnsembleCoordinado mejorVecino = new EnsembleCoordinado(ensemble);

		// 2- para cada cada instancia buscamos su mejor solución con las reglas del
		// ensemble
		double[] solucionesNuevoEnsemble = new double[AlgoUtil.set_train.size()];
		for (int instancia = 0; instancia < solucionesNuevoEnsemble.length; instancia++)
			solucionesNuevoEnsemble[instancia] = ensemble.getCoste(AlgoUtil.set_train.get(instancia));

		// 3- Se seleccionan las reglas del vecindario
		// a- Si el tamaño del vecindario es mayor o igual que las reglas del Almacen
		// entonces se salta a 4-
		if (sizeVecindario >= vecindarioReglas.size()) {
			// 4- Se inicia la búsqueda de vecinos, recorriendo todas las reglas
			double costeMejor = ensemble.getCoste();
			double coste;
			for (int regla = 0; regla < vecindarioReglas.size(); regla++) {
				// Si la solución actual no contiene la regla
				// Se genera un vecino y se compara con el mejor vecino
				EnsembleCoordinado vecino = creaVecino(ensemble, indice_peorRegla, regla);
				coste = vecino.getCoste();
				if (coste < costeMejor) {
					costeMejor = coste;
					mejorVecino = vecino;
//					break; // FUERZA UN M�?XIMO GRADIENTE!
				}
			}
		}
		// b- Se seleccionan las reglas del vecindario de forma aleatoria SIN
		// repeticiones.
		else {
			Set<Integer> reglas = new HashSet<Integer>();
			while (reglas.size() < sizeVecindario) {
				int regla = (int) (Math.random() * vecindarioReglas.size());
				reglas.add(regla);
			}
			// 4- Se inicia la búsqueda de vecinos, recorriendo las reglas del vecindario
			double costeMejor = ensemble.getCoste();
			double coste;
			for (Integer regla : reglas) {
				// Si la solución actual no contiene la regla
				// Se genera un vecino y se compara con el mejor vecino
				EnsembleCoordinado vecino = creaVecino(ensemble, indice_peorRegla, regla);
				coste = vecino.getCoste();
				if (coste < costeMejor) {
					costeMejor = coste;
					mejorVecino = vecino;
//					break; // FUERZA UN M�?XIMO GRADIENTE!
				}
			}
		}
		return mejorVecino;
	}

	static EnsembleCoordinado creaVecino(EnsembleCoordinado ensemble, int indice_regla, int regla) {
		EnsembleCoordinado vecino = new EnsembleCoordinado(ensemble);
		vecino.inserta(indice_regla, vecindarioReglas.get(regla));
		return vecino;
	}

	static EnsembleCoordinado ensembleRandom() {
		return new EnsembleCoordinado();
	}
}
