package calculo_reglas;

import java.util.List;
import java.util.Random;

public class GRASPreglas implements Algorithm {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"30", // tiempo
					"5", // intervalos
					"8", // di
					"8", // dm
					"0", // change trainingSet
					"InstancesTraining_21m", // set training
					"filterTSP", // set filter
					"2", // SM
					"0", // elitism
					"10" // full evaluated 
			};
		}
		/*
		 * PARAMETROS
		 */
		// tiempo
		AlgoUtil.monitoriza = true;
		AlgoUtil.minutos = Integer.parseInt(args[0]);
		AlgoUtil.intervalosMedicion = Integer.parseInt(args[1]);
		if (AlgoUtil.intervalosMedicion == 1)
			AlgoUtil.monitoriza = false;
		// size solution
		AlgoUtil.profundidadMaximaInicialReglas = Integer.parseInt(args[2]);
		AlgoUtil.profundidadMaximaGlobal = Integer.parseInt(args[3]);
		// change training set
		int trainingSetChange = Integer.parseInt(args[4]);
		//
		AlgoUtil.trainingSetName = args[5];
		AlgoUtil.filterSetName = args[6];
		int SM = Integer.parseInt(args[7]);
		int elitism = Integer.parseInt(args[8]);
		int fullEvaluated = Integer.parseInt(args[9]);

		int LSA = 1;
		
		/*
		 * Incluirlos en el nombre
		 */
		String msg = "GRASPrules_" + AlgoUtil.minutos + "_" + AlgoUtil.intervalosMedicion + "_"
				+ AlgoUtil.profundidadMaximaInicialReglas + "_" + AlgoUtil.profundidadMaximaGlobal + "_"
				+ AlgoUtil.cardinalidadEnsembles + "_" + trainingSetChange 
				+ "_" + AlgoUtil.trainingSetName + "_"
				+ AlgoUtil.filterSetName + "_" + SM + "_" + elitism + "_" + fullEvaluated; 

		GRASPreglas demo = new GRASPreglas();
		demo.limiteIteracionesEnsembles = Integer.MAX_VALUE;
		demo.changeTrainingSet = trainingSetChange;
		demo.elitism = elitism;
		demo.lsa_reglas = new LSA_reglas(LSA, demo.limiteIteracionesLSA, demo.vecinosMaximos, SM, fullEvaluated);

		AlgoUtil.run(msg, demo);
		AlgoUtil.cambiaConjuntoEntrenamiento();

		demo.cabecera();
		demo.run();
		if (AlgoUtil.intervalosMedicion == 1)
			AlgoUtil.vln(demo.getLinea());
		AlgoUtil.end();
	}

	// metricas
	public int iteraciones;
	public int vecesConverge;

	// parametros
	public int LSA = 1;
	public int limiteIteracionesLSA = Integer.MAX_VALUE;
	public int limiteIteracionesEnsembles = Integer.MAX_VALUE;
	public int vecinosMaximos = Integer.MAX_VALUE;
	public int changeTrainingSet = 0;
	private int elitism = 0;
	
	public LSA_reglas lsa_reglas;

	public GRASPreglas() {
	}

	public void run() {
		run(new Regla().random());
		System.out.println(AlgoUtil.mejorGlobal.test());
	}

	public Regla run(Regla regla) {
		Regla candidato = regla;
		Regla best = candidato;

		int iteracionesLocal = 0;
		boolean mejora = false;
		while (!AlgoUtil.timeOut() && iteracionesLocal < limiteIteracionesEnsembles) {
			//			System.out.println("Iteracion: "+iteracion+" train/test " + best.train() +"/"+best.test());
			candidato = lsa_reglas.run(best);
			// si mejora se actualiza
			if (AlgoUtil.mejoraTraining(candidato, best)) {
				best = candidato;
				mejora = true;
			}
			iteracionesLocal++;
			iteraciones++;
			if (!mejora) {
//				best.train = false;
				if (changeTrainingSet == 1)
					AlgoUtil.cambiaConjuntoEntrenamiento(new Random().nextInt(20));
				if (elitism  == 1)
					best = AlgoUtil.mejorGlobal.getClon();
				else
					best = new Regla().random();
				vecesConverge++;
			}
			mejora = false;
		}
		return best;
	}

	Regla seleccionaMejorRegla(List<Regla> reglas) {
		Regla mejor;
		mejor = reglas.get(0);
		for (int i = 1; i < reglas.size() && !AlgoUtil.timeOut(); i++)
			if (AlgoUtil.mejoraTraining(reglas.get(i), mejor))
				mejor = reglas.get(i);
		return mejor;
	}

		protected void cabecera() {
		AlgoUtil.vln(
//				"Train;Test;Size;IteracionesGRASP;Generados;Validos;EvaluadosFull;EvaluadosPartial;IteracionesLSA;VecesConverge;VecesCambiaTraining;Regla;Monticulo;");
				"Train;Size;IteracionesGRASP;Generados;Validos;EvaluadosFull;EvaluadosPartial;IteracionesLSA;VecesConverge;VecesCambiaTraining;Regla;Monticulo;");
		}

	public String getLinea() { // cada vez que se llama se llama al voraz que calcula los ensembles
//		String linea = "esta imprimo?";
		String linea = "";
		try {
			linea += AlgoUtil.mejorGlobal.train() 
//					+ ";" + AlgoUtil.mejorGlobal.test() 
					+ ";" + AlgoUtil.mejorGlobal.getSize() + ";" + iteraciones + ";" + lsa_reglas.vecinosGenerados + ";"
					+ lsa_reglas.vecinosValidos + ";" + lsa_reglas.vecinosEvaluadosCompletamente + ";"
					+ lsa_reglas.vecinosEvaluadosPartial + ";" + lsa_reglas.iteraciones
					+ ";" + vecesConverge + ";" + AlgoUtil.vecesCambiaTraining + ";" + AlgoUtil.mejorGlobal.representa()
					+ ";" + AlgoUtil.mejorGlobal.representaMonticulo();
		} catch (Exception e) {
		}
		return linea;
	}
}

//if (inicializacion == 0) 	// Regla aleatoria y la siguiente generada de forma aleatoria
//ensembleSolucion.inserta(i, new Regla().random());
//else if (inicializacion == 1) // Regla aleatoria y la siguiente la mejor de todas las generadas
//ensembleSolucion.inserta(i, seleccionaMejorRegla(AlgoUtil.evaluadorReglas.getSeleccionables()));
//else if (inicializacion == 2) // Regla aleatoria y la mejor calculada por el voraz entre todas las generadas
//ensembleSolucion = seleccionaSiguienteVoraz(ensembleSolucion, i, AlgoUtil.evaluadorReglas.getSeleccionables());
//else if (inicializacion == 3) // Regla aleatoria y la mejor calculada por el voraz entre 50 seleccionadas
//							// de forma aleatoria entre todas las generadas
//ensembleSolucion = seleccionaSiguienteVorazLimitado(ensembleSolucion, i, AlgoUtil.evaluadorReglas.getSeleccionables());
//else if (inicializacion == 4) // Regla buena y la siguiente generada de forma aleatoria
//ensembleSolucion.inserta(i, new Regla().random());
//else if (inicializacion == 5) // Regla buena y la siguiente la mejor de las generadas
//ensembleSolucion.inserta(i, seleccionaMejorRegla(AlgoUtil.evaluadorReglas.getSeleccionables()));
//else if (inicializacion == 6) // Regla buena y la mejor calculada por el voraz entre todas las generadas
//ensembleSolucion = seleccionaSiguienteVoraz(ensembleSolucion, i, AlgoUtil.evaluadorReglas.getSeleccionables());
//else if (inicializacion == 7) // Regla buena y la mejor calculada por el voraz entre 50 seleccionadas
//							// de forma aleatoria entre todas las generadas
//ensembleSolucion = seleccionaSiguienteVorazLimitado(ensembleSolucion, i, AlgoUtil.evaluadorReglas.getSeleccionables());

//ensemblesColaborativos = new ArrayList<Ensemble_Colaborativo>();
//if (inicializacion < 4) { // una regla random
//	ensembleSolucion = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos);
//	ensembleSolucion.inserta(0, new Regla().random());
//} else if (inicializacion >= 4 && inicializacion < 8) { // una regla heuristica
//	cargaReglas("reglas");
////	int best_j = 0;
////	Regla mejorAnterior = reglas.get(0);
////	for (int j = 1; j < reglas.size(); j++) {
////		if (AlgoUtil.evaluadorReglas.mejoraReal(reglas.get(j), mejorAnterior)) {
////			mejorAnterior = reglas.get(j);
////			best_j = j;
////		}
////	}
////	System.out.println(best_j);
//	ensembleSolucion = new Ensemble_Colaborativo(AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos);
////	ensembleInicial.inserta(0, reglas.get(best_j));
//	// el mejor es j= 1711
//	ensembleSolucion.inserta(0, reglas.get(1711));
//} else if (inicializacion == 8) { // Reglas aleatorias
//	for (int j = 0; j < AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos; j++)
//		reglas.add(new Regla().random());
//	ensembleSolucion = new Ensemble_Colaborativo(reglas);
//} else if (inicializacion == 9) { // Reglas seleccionadas de forma aleatoria de un conjunto de reglas buenas
//	cargaReglas("reglas");
//	List<Regla> aux = new ArrayList<Regla>();
//	for (int j = 0; j < AlgoUtil.gestorReglas.cardinalidadEnsemblesColaborativos; j++)
//		aux.add(reglas.get((int) (new Random().nextDouble() * reglas.size())));
//	ensembleSolucion = new Ensemble_Colaborativo(aux);
//} else if (inicializacion == 10) { // El ensemble calculado por el voraz partiendo de un conjunto de reglas
//	cargaReglas("reglas"); // ESTA VERSIÓN SE PODRIA CONSIDERAR LIMITANDO EL NÚMERO DE REGLAS
//	ensembleSolucion = new Voraz_Colaborativos().run(reglas);
//} else if (inicializacion == 11) { // Ensemble seleccionado de forma aleatoria de un conjunto de ensembles
//	cargaEnsembles("ensembles");
//	int random = (int) (new Random().nextDouble() * ensemblesColaborativos.size());
//	ensembleSolucion = ensemblesColaborativos.get(random);
//} else if (inicializacion == 12) { // El mejor ensemble de un conjunto de ensembles buenos
//	cargaEnsembles("ensembles");
////	int best_j = 0;
////	Ensemble_Colaborativo mejorAnterior = ensemblesColaborativos.get(0);
////	for (int j = 1; j < ensemblesColaborativos.size(); j++) {
////		if (AlgoUtil.evaluadorEnsemblesColaborativos.mejoraReal(ensemblesColaborativos.get(j), mejorAnterior)) {
////			mejorAnterior = ensemblesColaborativos.get(j);
////			best_j = j;
////		}
////	}
////	ensembleInicial = ensemblesColaborativos.get(best_j);
//	// el mejor es j= 1014
//	ensembleSolucion = ensemblesColaborativos.get(1014);
//} else if (inicializacion == 13) { // El ensemble calculado por el voraz partiendo de un conjunto de reglas
//	cargaReglas("reglas"); // ESTA VERSIÓN LIMITA EL NÚMERO DE REGLAS
//	ensembleSolucion = new Voraz_ColaborativosLimitado(vecinosMaximos).run(reglas);
//}