package calculo_ensembles_colaborativos;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Scanner;
import java.util.Set;

import calculo_reglas.AlgoUtil;
import calculo_reglas.Algorithm;
import calculo_reglas.Regla;

public class GRASPensembles implements Algorithm {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"30", // tiempo
					"5", // intervalos
					"8", // di
					"8", // dm
					"5", // card cola
					"3", // initial rules
					"-1", // add rules
					"-1", // remove rules
					"0", // change trainingSet
					"0", // combination method
					"1", // elitism
					"InstancesTraining_21m", // set training
					"InstancesTraining_7m", // set filter
					"1", // SM
					"3", // vecinos maximos evaludos full
					"50" // vecinos generados
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
		AlgoUtil.cardinalidadEnsembles = Integer.parseInt(args[4]);
		// inicializacion
		int inicializacion = Integer.parseInt(args[5]);
		int incluirReglas = Integer.parseInt(args[6]);
		int borrarReglas = Integer.parseInt(args[7]);
		// conf calculo reglas
		int LSA = 1; // 1=GD
		int iteracionesMaximas = 0; // 0=inf
		// change training set
		int trainingSetChange = Integer.parseInt(args[8]);
		//
		int combinationMethod = Integer.parseInt(args[9]);
		int elitism = Integer.parseInt(args[10]);
		//
		AlgoUtil.trainingSetName = args[11];
		AlgoUtil.filterSetName = args[12];
		int SM = Integer.parseInt(args[13]);
		int vecinosMaximosEvaluadosFull = Integer.parseInt(args[14]);
		int vecinosMaximos = Integer.parseInt(args[15]);
		/*
		 * Incluirlos en el nombre
		 */
		String msg = "GRASPensemblesV2_" + AlgoUtil.minutos + "_" + AlgoUtil.intervalosMedicion + "_"
				+ AlgoUtil.profundidadMaximaInicialReglas + "_" + AlgoUtil.profundidadMaximaGlobal + "_"
				+ AlgoUtil.cardinalidadEnsembles + "_" + inicializacion + "_" + incluirReglas + "_" + borrarReglas + "_"
				+ trainingSetChange + "_" + combinationMethod + "_" + elitism + "_" + AlgoUtil.trainingSetName + "_"
				+ AlgoUtil.filterSetName + "_" + SM + "_" + vecinosMaximosEvaluadosFull + "_" + vecinosMaximos;

		GRASPensembles demo = new GRASPensembles();
		demo.incluirReglas = incluirReglas;
		demo.borrarReglas = borrarReglas;
		demo.inicializacion = inicializacion;
		demo.LSA = LSA;
		demo.limiteIteracionesLSA = iteracionesMaximas;
		demo.limiteIteracionesEnsembles = Integer.MAX_VALUE;
		demo.vecinosMaximos = vecinosMaximos;
		demo.changeTrainingSet = trainingSetChange;
		demo.lsa_reglas = new LSA_reglasForEnsembles(LSA, demo.limiteIteracionesLSA, 
				vecinosMaximos, SM, vecinosMaximosEvaluadosFull);
		AlgoUtil.combinationMethod = combinationMethod;
		demo.elitism = elitism;

		AlgoUtil.run(msg, demo);
//		AlgoUtil.cambiaConjuntoEntrenamiento();
	
		demo.cabecera();
		demo.run();
		if (AlgoUtil.intervalosMedicion == 1)
			AlgoUtil.vln(demo.getLinea());
		AlgoUtil.end();
	}

	// VARIABLES INTERNAS
	Ensemble_Colaborativo candidato, best;
	public List<Regla> reglas;
	public List<Ensemble_Colaborativo> ensemblesColaborativos;
	public LSA_reglasForEnsembles lsa_reglas;

	// metricas
	public int iteraciones;
	public int vecesConverge;

	// parametros
	public int LSA = 1;
	public int limiteIteracionesLSA = Integer.MAX_VALUE;
	public int limiteIteracionesEnsembles = Integer.MAX_VALUE;
	public int vecinosMaximos = Integer.MAX_VALUE;
	public int inicializacion = 2; // ensemble de reglas random
	public int incluirReglas = 0;
	public int borrarReglas = 1;
	public int changeTrainingSet = 0;
	public int elitism = 1;

	public GRASPensembles() {
	}

//	public GRASP(int LSA, int iteracionesLSA, int iteracionesEnsembles, int vecinosMaximos, int incluirReglas,
//			int borrarReglas) {
//		this.LSA = LSA;
//		this.limiteIteracionesLSA = iteracionesLSA;
//		this.limiteIteracionesEnsembles = iteracionesEnsembles;
//		this.vecinosMaximos = vecinosMaximos;
//		this.incluirReglas = incluirReglas;
//		this.borrarReglas = borrarReglas;
//		lsa_reglas = new LSA_reglasForEnsembles(LSA, iteracionesLSA, vecinosMaximos);
//	}

	public void run() {
//		AlgoUtil.vln(ensembleSolucion.train()+";"+ensembleSolucion.test()+";"+ensembleSolucion.representaMonticulo());
		run(inicializaEnsemble());
	}

	public Ensemble_Colaborativo run(Ensemble_Colaborativo ensemble) {
		candidato = ensemble;
		best = candidato;

		int iteracionesLocal = 0;
		int indexReglaEnsemble = 0;
		boolean mejora = false;
		while (!AlgoUtil.timeOut() && iteracionesLocal < limiteIteracionesEnsembles) {
//			System.out.println("Iteracion: "+iteracion+" train/test " + best.train() +"/"+best.test());
			if (best.get(indexReglaEnsemble) == null) {
				aÒadeRegla(indexReglaEnsemble, best);
			}
			// aplica LSA
			if (best.get(indexReglaEnsemble) != null) {
				candidato = lsa_reglas.run(indexReglaEnsemble, best);
				// si mejora se actualiza
				if (AlgoUtil.mejoraTraining(candidato, best)) {
					best = candidato;
					mejora = true;
//					System.out.println(best.test());
				}
			}
			iteracionesLocal++;
			iteraciones++;
			if (!mejora) {
				indexReglaEnsemble++;
//				best.train = false;
				if (changeTrainingSet == 1)
					AlgoUtil.cambiaConjuntoEntrenamiento(new Random().nextInt(20));
//				System.out.println(AlgoUtil.gestorInstancias.training.get(0).nombre);
//				System.out.println("Converge/iteracion " + vecesConverge + "/" + iteracion);
				if (elitism == 1)
					best = (Ensemble_Colaborativo) AlgoUtil.mejorGlobal.getClon();
				borrarRegla(best);
				vecesConverge++;
			}
			if (indexReglaEnsemble == best.reglas.length) { // volvemos a empezar con la primera regla
				indexReglaEnsemble = 0;
			}
			mejora = false;
		}
		return best;
	}

	Ensemble_Colaborativo inicializaEnsemble() {
		Ensemble_Colaborativo inicial = null;
		switch (inicializacion) {
		case 0: // una regla random
			inicial = new Ensemble_Colaborativo(AlgoUtil.cardinalidadEnsembles);
			inicial.set(0, new Regla().random());
			break;
//		case 1: // la mejor regla
//			cargaReglas("reglas");
//			inicial = new Ensemble_Colaborativo(AlgoUtil.cardinalidadEnsembles);
//			inicial.set(0, reglas.get(1711));
//			break;
		case 2: // un ensemble de reglas aleatorias
			reglas = new ArrayList<Regla>();
			for (int j = 0; j < AlgoUtil.cardinalidadEnsembles; j++)
				reglas.add(new Regla().random());
			inicial = new Ensemble_Colaborativo(reglas);
			break;
		case 3: // un ensemble de reglas heuristicas seleccionadas de forma aleatoria
			cargaReglas("best28mf0");
			List<Regla> aux = new ArrayList<Regla>();
			for (int j = 0; j < AlgoUtil.cardinalidadEnsembles; j++)
				aux.add(reglas.get((int) (new Random().nextDouble() * reglas.size())));
			inicial = new Ensemble_Colaborativo(aux);
			break;
//		case 4: // un ensemble generado por el greedy de reglas heuristicas
//			cargaReglas("reglas");
//			inicial = new Voraz_Colaborativos().run(reglas);
//			break;
//		case 5: // un ensemble generado por el greedy limitado de reglas heuristicas
//			cargaReglas("reglas");
//			inicial = new Voraz_ColaborativosLimitado(vecinosMaximos).run(reglas);
//			break;
//		case 6: // the best ensemble
//			ensemblesColaborativos = new ArrayList<Ensemble_Colaborativo>();
//			cargaEnsembles("ensembles");
//			inicial = ensemblesColaborativos.get(1014);
//			break;	
		case 7: // one previous evolved random
			ensemblesColaborativos = new ArrayList<Ensemble_Colaborativo>();
			cargaEnsembles("ensembles");
			inicial = ensemblesColaborativos.get(new Random().nextInt(ensemblesColaborativos.size()));
			break;
		}
		return inicial;
	}

	void aÒadeRegla(int i, Ensemble_Colaborativo ensemble) {
		switch (incluirReglas) {
		case 0: // una regla aleatoria
			ensemble.set(i, new Regla().random());
			break;
//		case 1: // la mejor regla
//			ensemble.set(i, seleccionaMejorRegla(AlgoUtil.getSeleccionables()));
//			break;
//		case 2: // la mejor regla seg√∫n el voraz
//			ensemble = seleccionaSiguienteVoraz(ensemble, i, AlgoUtil.getSeleccionables());
//			break;
//		case 3: // la mejor regla de un conjunto limitado (torneo)
//			ensemble.set(i, seleccionaMejorReglaLimitado(AlgoUtil.getSeleccionables()));
//			break;
//		case 4: // la mejor regla seg√∫n el voraz limitado
//			ensemble = seleccionaSiguienteVorazLimitado(ensemble, i, AlgoUtil.getSeleccionables());
//			break;
		default:
			break;
		}
	}

	void borrarRegla(Ensemble_Colaborativo ensemble) {
		switch (borrarReglas) {
		case 0: // una aleatoriamente
			ensemble.borrarAleatoriamente();
			break;
//		case 1: // la regla con peor fitness (individualmente)
//			ensemble.borrarReglaPeorFitness();
//			break;
//		case 2: // la regla que al borrarla genera el mejor ensemble
//			ensemble.borrarReglaEmpeoraMenosEnsemble();
//			break;
		default:
			break;
		}
	}

	Ensemble_Colaborativo voraz(List<Regla> reglas) {
		Ensemble_Colaborativo aux, mejor;
		mejor = new Ensemble_Colaborativo(AlgoUtil.cardinalidadEnsembles);
		mejor.set(0, reglas.get(0));
		for (int index = 0; index < AlgoUtil.cardinalidadEnsembles; index++)
			for (int i = 0; i < reglas.size() && !AlgoUtil.timeOut(); i++) {
				aux = new Ensemble_Colaborativo(mejor);
				aux.set(index, reglas.get(i));
				if (AlgoUtil.mejoraTraining(aux, mejor))
					mejor = aux;
			}
		return mejor;
	}
	
	Ensemble_Colaborativo seleccionaSiguienteVoraz(Ensemble_Colaborativo ensemble, int index, List<Regla> reglas) {
		Ensemble_Colaborativo aux, mejor;
		mejor = ensemble;
		mejor.set(index, reglas.get(0));
		for (int i = 1; i < reglas.size() && !AlgoUtil.timeOut(); i++) {
			aux = new Ensemble_Colaborativo(ensemble);
			aux.set(index, reglas.get(i));
			if (AlgoUtil.mejoraTraining(aux, ensemble))
				mejor = aux;
		}
		return mejor;
	}

	Ensemble_Colaborativo seleccionaSiguienteVorazLimitado(Ensemble_Colaborativo ensemble, int index,
			List<Regla> reglas) {
		Collections.shuffle(reglas); // aqui los mezclamos y luego iteramos
		// hasta un limite: vecinosMaximos peor podr√≠a ser otro
		Ensemble_Colaborativo aux, mejor;
		mejor = ensemble;
		mejor.set(index, reglas.get(0));
		for (int i = 0; i < vecinosMaximos && !AlgoUtil.timeOut(); i++) {
			aux = new Ensemble_Colaborativo(ensemble);
			aux.set(index, reglas.get(i));
			if (AlgoUtil.mejoraTraining(aux, ensemble))
				mejor = aux;
		}
		return mejor;
	}

	Regla seleccionaMejorRegla(List<Regla> reglas) {
		Regla mejor;
		mejor = reglas.get(0);
		for (int i = 1; i < reglas.size() && !AlgoUtil.timeOut(); i++)
			if (AlgoUtil.mejoraTraining(reglas.get(i), mejor))
				mejor = reglas.get(i);
		return mejor;
	}

	Regla seleccionaMejorReglaLimitado(List<Regla> reglas) {
		Collections.shuffle(reglas);
		Regla mejor;
		mejor = reglas.get(0);
		for (int i = 1; i < vecinosMaximos && !AlgoUtil.timeOut(); i++)
			if (AlgoUtil.mejoraTraining(reglas.get(i), mejor))
				mejor = reglas.get(i);
		return mejor;
	}

	public void cargaReglas(String fichero) {
		reglas = new ArrayList<Regla>();
		Scanner kbd;
		Set<String> strings = new HashSet<String>();
		try {
			kbd = new Scanner(new File(fichero + ".csv"));
			while (kbd.hasNextLine()) {
				String regla = kbd.nextLine();
				if (!regla.equals("")) {
					if (regla.contains(";"))
						regla = regla.split(";")[regla.split(";").length - 1];
					if (!strings.contains(regla))
						reglas.add(new Regla().generarRegla(regla));
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void cargaEnsembles(String fichero) {
		Ensemble_Colaborativo ensemble;
		List<Regla> reglas_E;
		String[] linea;
		Scanner kbd;
		Set<String> ensembles = new HashSet<String>();
		try {
			kbd = new Scanner(new File(fichero + ".csv"));
			while (kbd.hasNextLine()) {
				reglas_E = new ArrayList<Regla>();
				String aux = kbd.nextLine().split(";")[0];
				linea = aux.substring(1, aux.length() - 1).split(","); // quitamos los corchetes del inicio y el final
				for (String regla : linea)
					if (!regla.equals("")) {
						reglas_E.add(new Regla().generarRegla(regla));
					}
				ensemble = new Ensemble_Colaborativo(reglas_E);
				if (!ensembles.contains(ensemble.representa())) {
					ensemblesColaborativos.add(ensemble);
					ensembles.add(ensemble.representa());
				}
			}
			kbd.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	protected void cabecera() {
		AlgoUtil.vln(
				"Train;Test;Size;IteracionesGRASP;Generados;Validos;EvaluadosFull;EvaluadosPartial;VecinosEvaluados;IteracionesLSA;VecesConverge;VecesCambiaTraining;Regla;Monticulo;");
	}

	public String getLinea() { // cada vez que se llama se llama al voraz que calcula los ensembles
		String linea = "";
		try {
			linea += AlgoUtil.mejorGlobal.train() + ";" + AlgoUtil.mejorGlobal.test() + ";"
					+ AlgoUtil.mejorGlobal.getSize() + ";" + iteraciones + ";" + lsa_reglas.vecinosGenerados + ";"
					+ lsa_reglas.vecinosValidos + ";" + lsa_reglas.vecinosEvaluadosCompletamente + ";"
					+ lsa_reglas.vecinosEvaluadosPartial + ";" + lsa_reglas.vecinosEvaluados + ";"
					+ lsa_reglas.iteraciones + ";" + vecesConverge + ";" + AlgoUtil.vecesCambiaTraining + ";"
					+ AlgoUtil.mejorGlobal.representa() + ";" + AlgoUtil.mejorGlobal.representaMonticulo();
		} catch (Exception e) {
		}
		return linea;
	}
}