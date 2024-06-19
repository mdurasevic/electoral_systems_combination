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

public class RandomSearch implements Algorithm {

	public static void main(String[] args) {
		if (args.length == 0) {
			args = new String[] { 
					"10", // tiempo
					"1", // intervalos
					"8", // di
					"8", // dm
					"3", // card cola
					"0", // combination method
					"InstancesTraining_35m" // set training
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
		int combinationMethod = Integer.parseInt(args[5]);
		AlgoUtil.trainingSetName = args[6];
		/*
		 * Incluirlos en el nombre
		 */
		String msg = "RandomSearchensembles_" + AlgoUtil.minutos + "_" + AlgoUtil.intervalosMedicion + "_"
				+ AlgoUtil.profundidadMaximaInicialReglas + "_" + AlgoUtil.profundidadMaximaGlobal + "_"
				+ AlgoUtil.cardinalidadEnsembles + "_" + combinationMethod + "_" + AlgoUtil.trainingSetName;

		RandomSearch demo = new RandomSearch();

		AlgoUtil.run(msg, demo);
		AlgoUtil.cambiaConjuntoEntrenamiento();

		boolean c = true;
		double bestTrain = Double.MAX_VALUE;
		double bestTest = Double.MAX_VALUE;
		cargaReglas("best28mf0");
		int iterations = 0;
		while (!AlgoUtil.timeOut()) {
			iterations++;
			List<Regla> reglas = new ArrayList<Regla>();
			for (int j = 0; j < AlgoUtil.cardinalidadEnsembles; j++)
				reglas.add(todas.get(new Random().nextInt(todas.size())));
			Ensemble_Colaborativo inicial = new Ensemble_Colaborativo(reglas);
			double train = inicial.train();
//			if (train < bestTrain) {
//				bestTrain = train;
//				System.out.println(
//						bestTrain + " " + bestTest + " " + iterations + " " + 1.0 * AlgoUtil.tiempoEjecucion() / 60);
//			}
//			double test = inicial.test();
//			if (test < bestTest) {
//				bestTest = test;
//				System.out.println(bestTrain + " " + bestTest + " " + iterations);
//			}
		}
		System.out.println(iterations);

	}

	// VARIABLES INTERNAS
	Ensemble_Colaborativo candidato, best;
	public static List<Regla> todas;
	public List<Ensemble_Colaborativo> ensemblesColaborativos;

	public RandomSearch() {
	}

	public static void cargaReglas(String fichero) {
		todas = new ArrayList<Regla>();
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
						todas.add(new Regla().generarRegla(regla));
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

	@Override
	public String getLinea() {
		return "no implementado";
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		
	}

}