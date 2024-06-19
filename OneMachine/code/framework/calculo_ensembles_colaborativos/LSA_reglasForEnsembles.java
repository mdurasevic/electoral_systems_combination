package framework.calculo_ensembles_colaborativos;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import framework.calculo_reglas.Regla;
import framework.calculo_reglas.local_search.GestorNS;
import framework.core.AlgoUtil;

public class LSA_reglasForEnsembles {

	// VARIABLES INTERNAS
	GestorNS gestorEstructurasVecindad;

	// PARAMETROS
	public int LSA = 1; // 0:escaladaSimple, 1:maximoGradiente
	public int iteracionesMaximas = Integer.MAX_VALUE; // sin limite
	public int vecinosMaximos = Integer.MAX_VALUE;
	public int vecinosMaximosEvaluadosFull = 50;
	public int SM = 1;
	
	// METRICAS
	public int vecinosGenerados;
	public int vecinosEvaluados;
	public int vecinosValidos;
	public int iteraciones;
	public int vecinosEvaluadosCompletamente;
	public int vecinosEvaluadosPartial;

	public LSA_reglasForEnsembles(int LSA, int iteracionesMaximas, int vecinosMaximos, int SM, int vecinosMaximosEvaluadosFull) {
		this.LSA = LSA;
		this.iteracionesMaximas = iteracionesMaximas;
		if (this.iteracionesMaximas == 0)
			this.iteracionesMaximas = Integer.MAX_VALUE;
		this.vecinosMaximos = vecinosMaximos;
		if (this.vecinosMaximos == 0)
			this.vecinosMaximos = Integer.MAX_VALUE;
		this.SM = SM;
		this.vecinosMaximosEvaluadosFull=vecinosMaximosEvaluadosFull;
		gestorEstructurasVecindad = new GestorNS();
	}

	public Ensemble_Colaborativo run(int regla, Ensemble_Colaborativo ensemble) {
		List<Regla> vecinos = new ArrayList<Regla>();
		List<Regla> validos = new ArrayList<Regla>();
		List<Ensemble_Colaborativo> ensembles = new ArrayList<Ensemble_Colaborativo>();
		int iteracionesLocales = 0;
		boolean mejora;
		Regla reglaConcreta = ensemble.get(regla);
		Ensemble_Colaborativo mejorEnsemble = ensemble;
		Ensemble_Colaborativo ensembleCandidato = ensemble.getClon();

		while (!AlgoUtil.timeOut() && iteracionesLocales < iteracionesMaximas) {

			mejora = false;
			iteracionesLocales++;
			iteraciones++;

//			vecinos = gestorEstructurasVecindad.getVecinos(reglaConcreta, vecinosMaximos);
			vecinos = gestorEstructurasVecindad.getVecinos(reglaConcreta);
			
			// 1- se calculan los vecinos
//			vecinos = gestorEstructurasVecindad.getVecinos(reglaConcreta);
			vecinosGenerados += vecinos.size(); // se cuentan los vecinos generados
			// 2- se filtran los vecinos validos
//			validos.clear();
//			for (Regla vecino : vecinos)
//				if (vecino.esValido())
//					validos.add(vecino);
//			vecinos = validos;
			vecinosValidos += vecinos.size();

			// 3- aplica la busqueda local
//			Collections.shuffle(vecinos); // ordenar de forma aleatoria		
			int n = vecinosMaximos; // evaluamos solo una parte
			if (n > vecinos.size())
				n = vecinos.size();
		
			ensembles.clear();
			for (int i = 0; i < n; i++) {
				ensembleCandidato = mejorEnsemble.getClon();
				ensembleCandidato.cambiaRegla(regla, vecinos.get(i));
				ensembles.add(ensembleCandidato);
				vecinosEvaluados++;
			}
			if (SM == 1) {
				ordenaReglasFiltro(ensembles);
				vecinosEvaluadosPartial += ensembles.size();
			}
			
			for (int i = 0; i < vecinosMaximosEvaluadosFull && i < ensembles.size() && !AlgoUtil.timeOut(); i++) {
				vecinosEvaluadosCompletamente++;
				if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(ensembles.get(i), mejorEnsemble)) {
					mejorEnsemble = ensembles.get(i);
					mejora = true;
//					System.out.println(mejorEnsemble.test());
					if (LSA == 2) // si HC nos quedamos con el primer vecino que mejore
						break;
				}
			}	
			
			// sin SM
//			for (int i = 0; i < n; i++) {
//				if (timeOut())
//					break;
//				vecinosEvaluados++;
//				vecinosEvaluadosCompletamente++;
//				ensembleCandidato = mejorEnsemble.getClon();
//				ensembleCandidato.cambiaRegla(regla, vecinos.get(i));
//				AlgoUtil.evaluadorReglas.addSeleccionable(vecinos.get(i));
//				if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(ensembleCandidato, mejorEnsemble)) {
//					mejorEnsemble = ensembleCandidato;
//					mejora = true;
//					if (LSA == 2) // si HC nos quedamos con el primer vecino que mejore
//						break;
//				}
//			}			
			// 4- se comprueba el criterio de terminación
			if (!mejora)
				break;

		} // fin
		return mejorEnsemble; // si no hay ningun vecino que mejore, se devolverá la regla inicial
	}
	
	void ordenaReglasFiltro(List<Ensemble_Colaborativo> reglas) {
		Collections.sort(reglas, new Comparator<Ensemble_Colaborativo>() {
			@Override
			public int compare(Ensemble_Colaborativo p1, Ensemble_Colaborativo p2) {
				return new Double(p1.filter()).compareTo(p2.filter());
			}
		});
	}
	
}