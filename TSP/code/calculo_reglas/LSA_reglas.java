package calculo_reglas;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import calculo_ensembles_colaborativos.GestorNS;
import calculo_reglas.AlgoUtil;
import calculo_reglas.Regla;

public class LSA_reglas {

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

	public LSA_reglas(int LSA, int iteracionesMaximas, int vecinosMaximos, int SM, int fullEvaluated) {
		this.LSA = LSA;
		this.iteracionesMaximas = iteracionesMaximas;
		if (this.iteracionesMaximas == 0)
			this.iteracionesMaximas = Integer.MAX_VALUE;
		this.vecinosMaximos = vecinosMaximos;
		if (this.vecinosMaximos == 0)
			this.vecinosMaximos = Integer.MAX_VALUE;
		this.SM = SM;
		this.vecinosMaximosEvaluadosFull = fullEvaluated;
		gestorEstructurasVecindad = new GestorNS();
	}

	public Regla run(Regla regla) {
		List<Regla> vecinos = new ArrayList<Regla>();
		List<Regla> validos = new ArrayList<Regla>();
		int iteracionesLocales = 0;
		boolean mejora;
		Regla best = regla.getClon();

		while (!AlgoUtil.timeOut() && iteracionesLocales < iteracionesMaximas) {

			mejora = false;
			iteracionesLocales++;
			iteraciones++;

			// 1- se calculan los vecinos
			vecinos = gestorEstructurasVecindad.getVecinos(best);
			vecinosGenerados += vecinos.size(); // se cuentan los vecinos generados

			// 2- se filtran los vecinos validos
			validos.clear();
			for (Regla vecino : vecinos)
				if (vecino.esValido())
					validos.add(vecino);
			vecinos = validos;
			vecinosValidos += vecinos.size();

			// 3- aplica la busqueda local
			Collections.shuffle(vecinos); // ordenar de forma aleatoria		
			int n = vecinosMaximos; // evaluamos solo una parte
			if (n > vecinos.size())
				n = vecinos.size();

			if (SM > 0) {
				ordenaReglasFiltro(vecinos);
				vecinosEvaluadosPartial += vecinos.size();
			}
			
			double cost = 0.0;
			for (int i = 0; i < vecinosMaximosEvaluadosFull && i < vecinos.size() && !AlgoUtil.timeOut(); i++) {
				if (SM != 2 || cost != vecinos.get(i).filter) { // forzamos que los evaluados tengan valores de filter
																// distintos cuando SM=2
					vecinosEvaluadosCompletamente++;
					if (AlgoUtil.mejoraTraining(vecinos.get(i), best)) {
						best = vecinos.get(i);
						mejora = true;
						if (LSA == 2) // si HC nos quedamos con el primer vecino que mejore
							break;
					}
					cost = vecinos.get(i).filter;
				}
			}	
			
	
			// 4- se comprueba el criterio de terminación
			if (!mejora)
				break;

		} // fin
		return best; // si no hay ningun vecino que mejore, se devolverá la regla inicial
	}
	
	void ordenaReglasFiltro(List<Regla> reglas) {
		Collections.sort(reglas, new Comparator<Regla>() {
			@Override
			public int compare(Regla p1, Regla p2) {
				return new Double(p1.filter()).compareTo(p2.filter());
			}
		});
	}
	
}