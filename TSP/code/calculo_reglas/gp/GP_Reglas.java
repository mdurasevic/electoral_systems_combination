package calculo_reglas.gp;

import java.util.ArrayList;
import java.util.List;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Algorithm;
import calculo_reglas.LSA_reglas;
import calculo_reglas.Regla;

public class GP_Reglas implements Algorithm {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] {
//					"100", // tiempo
//					"1", // intervalos
//					"4", // di
//					"8", // dm
//					"2", // SM
//					"1", // nCrucesGP
//					"0", // LSA reglas
//					"0", // ratio LSA
//					"0", // ME_LSA
//					"0", // iteracionesMaximas 0:inf
//					"0", // vecinosMaximos 0:inf
					"100", // generaciones
					"4", // di
					"4", // dm
					"3", // training set
					"200", // popSize
					"1", // nCrucesGP
			};
		}
		/*
		 * PARAMETROS
		 */
//		AlgoUtil.monitoriza = false;
//		// tiempo
//		AlgoUtil.minutos = Integer.parseInt(args[0]);
//		AlgoUtil.intervalosMedicion = Integer.parseInt(args[1]);
//		// size solution
//		AlgoUtil.profundidadMaximaInicialReglas = Integer.parseInt(args[2]);
//		AlgoUtil.profundidadMaximaGlobal = Integer.parseInt(args[3]);
//		// SM
//		int SM = Integer.parseInt(args[4]);
//		// conf calculo reglas
//		int nCrucesGP = Integer.parseInt(args[5]);
//		int LSA = Integer.parseInt(args[6]);
//		double ratioLSA = Double.valueOf(1.0 * Integer.parseInt(args[7]) / 100);
//		int ME_LSA = Integer.parseInt(args[8]);
//		int iteracionesMaximas = Integer.parseInt(args[9]);
//		int vecinosMaximo = Integer.parseInt(args[10]);

		int maxGeneraciones = Integer.parseInt(args[0]);
		// size solution
		AlgoUtil.profundidadMaximaInicialReglas = Integer.parseInt(args[1]);
		AlgoUtil.profundidadMaximaGlobal = Integer.parseInt(args[2]);
		// Training set
		AlgoUtil.trainingSetName = args[3];
		// conf calculo reglas
		int popSize = Integer.parseInt(args[4]);
		int nCrucesGP = Integer.parseInt(args[5]);

		String msg = "GP_TSPset2_" + maxGeneraciones + "_" + AlgoUtil.profundidadMaximaGlobal + "_" + AlgoUtil.trainingSetName + "_" + popSize + "_" + nCrucesGP;
		GP_Reglas gp = new GP_Reglas(popSize, nCrucesGP);
		AlgoUtil.run(msg, gp);

		gp.cabecera();
		gp.generarPoblacionInicial();
//		while (!AlgoUtil.timeOut()) {
		while (gp.generaciones < maxGeneraciones) {
			AlgoUtil.vln(gp.getLinea());
			gp.run();
		}
//		gp.tiempoEjecucion = AlgoUtil.tiempoEjecucion();
		AlgoUtil.vln(gp.getLinea());
		AlgoUtil.end();
	}

	// VARIABLES INTERNAS
	protected Regla mejor;
	protected Poblacion_Reglas reglas;
	protected LSA_reglas lsa;

	// PARAMETROS
	public int sizePop = 200;
	public int nCruces = 1; // si es 1 entonces no es SM-GP
	public double ratioCruce = 1.0;
	public double ratioMutacion = 0.02;
	public double ratioLSA = 0.0; // si es 0 entonces no es MGP
	public int SM = 2;
	public int fullEvaluated = 10;

	// METRICAS
	public int generaciones;
	public int tiempoEjecucion;

	public GP_Reglas(int sizePop, int nCruces) {
		this.sizePop = sizePop;
		this.nCruces = nCruces;
	}
	
	public GP_Reglas() {};
	
	public GP_Reglas(int nCruces,  double ratioLSA, int LSA, int iteracionesMaximas, int vecinosMaximos, int SM, int fullEvaluated) {
		this.nCruces = nCruces;
		this.ratioLSA = ratioLSA;
//		if (ratioLSA > 0)
		lsa = new LSA_reglas(LSA, iteracionesMaximas, vecinosMaximos, SM, fullEvaluated);
	}

	public void run() {
		evolucionar();
		mejor = reglas.getBest();
	}

	public void generarPoblacionInicial() {
		reglas = new Poblacion_Reglas(sizePop);
		reglas.generaRandom();
		mejor = reglas.getBest();
	}

	public void generarPoblacionHuristica() {
		reglas = new Poblacion_Reglas(sizePop);
		reglas.generaHeuristica();
		mejor = reglas.getBest();
	}

	public void evolucionar() {
		reglas.shuffle();
		generaciones++;
		for (int i = 0; i < sizePop && !AlgoUtil.timeOut(); i = i + 2) {
			if (AlgoUtil.timeOut())
				break;
		
			// selección por pares
			Regla padre1 = reglas.get(i);
			Regla padre2 = reglas.get(i + 1);

//			if (Math.random() <= ratioCruce) { // cruce: siempre 1.0

			// Primer hijo
			Regla hijo1;
			if (nCruces > 1)
				hijo1 = cruceFiltro(padre1, padre2);
			else
				hijo1 = cruzar(padre1, padre2);

			if (Math.random() <= ratioMutacion) // mutacion
				mutar(hijo1);

			if (Math.random() <= ratioLSA) // LSA
				hijo1 = lsa.run(hijo1);

			// Segundo hijo
			Regla hijo2;
			if (nCruces > 1)
				hijo2 = cruceFiltro(padre2, padre1);
			else
				hijo2 = cruzar(padre2, padre1);

			if (Math.random() <= ratioMutacion) // mutacion
				mutar(hijo2);

			if (Math.random() <= ratioLSA) // LSA
				hijo2 = lsa.run(hijo2);

			// torneo padres e hijos, pasa siempre un hijo
			Poblacion_Reglas tournament = new Poblacion_Reglas(3);
			tournament.add(padre1);
			tournament.add(padre2);

			// hijo1 mejor hijo2
			if (AlgoUtil.mejoraTraining(hijo1, hijo2)) {
				reglas.set(i, hijo1); // pasa hijo1
				tournament.add(hijo2); // y el mejor entre hijo2,
				reglas.set(i + 1, tournament.getBest());
			}
			// hijo2 mejor hijo1
			else {
				reglas.set(i, hijo2); // pasa hijo2
				tournament.add(hijo1); // y el mejor entre hijo1, padre1 y padre2
				reglas.set(i + 1, tournament.getBest());
			}

			// SI SE QUIERE MUCHA PRESIÓN en algun momento tendremos que reconsiderar el
			// esquema evolutivo
			// torneo padres e hijos, pasan los dos mejores
//				PoblacionReglas tournament = new PoblacionReglas(4);
//				tournament.guarda(0, padre1);
//				tournament.guarda(1, padre2);
//				tournament.guarda(2, hijo1);
//				tournament.guarda(3, hijo2);
//				tournament.ordenaPoblacionEntrenamiento();
//				newPopulation.guarda(i, tournament.getCromosoma(0));
//				newPopulation.guarda(i+1, tournament.getCromosoma(1));	
//			} else { // sin implementar si cruce no es 100%
//			}
		}
		mejor = reglas.getBest();
	}

	Regla cruceFiltro(Regla padre1, Regla padre2) {
		List<Regla> cruces = new ArrayList<Regla>();
		for (int i = 0; i < nCruces; i++)
			cruces.add(cruzar(padre1, padre2));
		Regla mejor = cruces.get(0);
		for (Regla regla : cruces)
			if (AlgoUtil.mejoraSurrogada(regla, mejor)
					&& regla.filter() != padre1.filter()
					&& regla.filter() != padre2.filter())
				mejor = regla;
		return mejor;
	}

	Regla cruzar(Regla padre1, Regla padre2) {
		int i, j, k = 0, n = 1000;
		do {
			i = padre1.posAzar();
//			j = padre2.posAzarMismaDim(padre1, i);
			j = padre2.posAzar(); // esto hace que las reglas puedan no ser dimensionalmente correctas
			k++;
		} while (j == -1 && k < n);
		if (j == -1) {
			return new Regla().random();
		}
		Regla hijo1 = new Regla(i, j, padre1, padre2);
		Regla hijo2 = new Regla(j, i, padre2, padre1);

		if (!hijo1.esValido())
			hijo1 = null;

		if (!hijo2.esValido())
			hijo2 = null;

//		// comprobamos el tamaño, si tamañoMaximo<2^profundidadMaxima -1, para compararse con el SSHE
//		if (hijo1 != null && hijo1.getSize() > Main.tamañoMaximoReglas)
//			hijo1 = null;
//		if (hijo2 != null && hijo2.getSize() > Main.tamañoMaximoReglas)
//			hijo2 = null;

		if (hijo1 == null && hijo2 == null) {
			return new Regla().random();
//			return padre1;
		} else if (hijo1 != null && hijo2 == null)
			return hijo1;
		else if (hijo1 == null && hijo2 != null) {
			return hijo2;
		} else {
//			if (cruceDistinto==0)
			return hijo1;
//			else {
//				if (AlgoUtil.evaluadorReglas.mejoraReal(hijo1, hijo2))  // con las instancias sobrecapacitadas no hay apenas diferencia, aqui si
//					return hijo1;
//				else // hijo2 mejor hijo1
//					return hijo2;
//			}
		}
	}

	Regla mutar(Regla cromosoma) {
		Regla mutacion = new Regla().random();
		int i, j, k = 0, n = 1000;
		do {
			i = cromosoma.posAzar();
			j = mutacion.posAzar();
//		j = mutacion.posAzarMismaDim(cromosoma, i);
			k++;
		} while (j != -1 && k < n);
		if (j == -1) {
			return mutacion;
		}
		Regla hijo = new Regla(i, j, cromosoma, mutacion);
		if (hijo.esValido())
			return hijo;
		else {
			return mutacion;
		}
//		if (hijo.getSize() <= Main.tamañoMaximoReglas) // para compararse con el SSHE...
//			if (hijo.esValido())
//				return hijo;
//			else
//				return mutacion;
////				return cromosoma;
//		else
//			return mutacion;
////			return cromosoma;
	}

	protected void cabecera() {
		AlgoUtil.vln("Train;AvgTrain;Size;Depth;FullLSA;PartialLSA;Tiempo;Generaciones;Regla;Monticulo;");
	}

	public String getLinea() { // cada vez que se llama se llama al voraz que calcula los ensembles
		String linea = "";
		try {
			linea += AlgoUtil.mejorGlobal.train() + ";" + reglas.getAvg() + ";"
					+ AlgoUtil.mejorGlobal.getSize() + ";" + AlgoUtil.mejorGlobal.getProfundidad() + ";" + lsa.vecinosEvaluadosCompletamente + ";" + lsa.vecinosEvaluadosPartial+ ";" 
					+ AlgoUtil.tiempoEjecucion() + ";" + generaciones + ";"  
					+ AlgoUtil.mejorGlobal.representa() + ";" + AlgoUtil.mejorGlobal.representaMonticulo() + ";";
		} catch (Exception e) {
			e.printStackTrace();
		}
		return linea;
	}

}